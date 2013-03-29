// com.cpp: implementation of the Ccom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "com.h"
#include <stdio.h>
//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif


CReadThread *CReadThread::self=NULL;
CWriteThread *CWriteThread::self=NULL;
//////////////////////////////////////////////////////////////////////
// CALLBACK
//////////////////////////////////////////////////////////////////////
DWORD WINAPI WriteProc( LPVOID lpParameter)
{
	CWriteThread::self->WriteExecute();
	return 0;
}
DWORD WINAPI ReadProc(LPVOID lpParameter)
{
	CReadThread::self->ReadExecute();
	return 0;
}
//////////////////////////////////////////////////////////////////////
// CReadThread
//////////////////////////////////////////////////////////////////////
void CReadThread::ReadExecute()
{
	char szInputBuffer[INPUTBUFFERSIZE-1];
	DWORD nNumberOfBytesRead;
	DWORD dwHandleSignaled;
	DWORD dwEvtMask=0;
	DWORD lasterrnum;
	// Needed for overlapped I/O (ReadFile)
	OVERLAPPED overlappedRead;
	// Needed for overlapped Comm Event handling.
	OVERLAPPED overlappedCommEvent;
	memset(&overlappedRead,0,sizeof(overlappedRead));
	memset(&overlappedCommEvent,0,sizeof(overlappedCommEvent));
	
	// Lets put an event in the Read overlapped structure.
	overlappedRead.hEvent = CreateEvent( NULL, true, true, NULL);
	if (overlappedRead.hEvent == 0) 
	{
		PostHangupCall();
		goto EndReadThread;
	}
	
	// And an event for the CommEvent overlapped structure.
	overlappedCommEvent.hEvent = CreateEvent( NULL, true, true, NULL);
	if (overlappedCommEvent.hEvent == 0) 
	{
		PostHangupCall();
		goto EndReadThread;
	}
	
	// We will be waiting on these objects.
	HANDLE HandlesToWaitFor[3];
	HandlesToWaitFor[0] = hCloseEvent;
	HandlesToWaitFor[1] = overlappedCommEvent.hEvent;
	HandlesToWaitFor[2] = overlappedRead.hEvent;
	
	// Setup CommEvent handling.
	
	// Set the comm mask so we receive error signals.
	if (! SetCommMask(hCommFile, EV_ERR | EV_RLSD | EV_RING )) 
	{
		PostHangupCall();
		goto EndReadThread;
	}
	
	// Start waiting for CommEvents (Errors)
	if (! SetupCommEvent( &overlappedCommEvent,  &dwEvtMask )) 
        goto EndReadThread;
	
	// Start waiting for Read events.
	if (! SetupReadEvent( &overlappedRead,szInputBuffer, INPUTBUFFERSIZE,nNumberOfBytesRead )) 
        goto EndReadThread;
	
	// Keep looping until we break out.
	while (true)
	{
		// Wait until some event occurs (data to read; error; stopping).
		dwHandleSignaled = WaitForMultipleObjects(3, HandlesToWaitFor,false, INFINITE);
		
		// Which event occured?
		switch(dwHandleSignaled)
		{
		case WAIT_OBJECT_0:     // Signal to  the thread.
			// Time to return;.
			goto EndReadThread;
			break;
		case WAIT_OBJECT_0 + 1: // CommEvent signaled.
			// Handle the CommEvent.
			if (! HandleCommEvent( &overlappedCommEvent, &dwEvtMask, true ) )
				goto EndReadThread;
			
			// Start waiting for the next CommEvent.
			if (! SetupCommEvent( &overlappedCommEvent, &dwEvtMask ) )
				goto EndReadThread;
			break;
		case WAIT_OBJECT_0 + 2: // Read Event signaled.
			// Get the new data!
			if (! HandleReadEvent( &overlappedRead,szInputBuffer,INPUTBUFFERSIZE,nNumberOfBytesRead )) 
				goto EndReadThread;
			
			// Wait for more new data.
			if (! SetupReadEvent( &overlappedRead,szInputBuffer, INPUTBUFFERSIZE,nNumberOfBytesRead ) )
				goto EndReadThread; 
			break;
		case WAIT_FAILED:       // Wait failed.  Shouldn't happen.
			lasterrnum=GetLastError();
			PostHangupCall();
			goto EndReadThread;
			break;
		default:    // This case should never occur.
			PostHangupCall();
			goto EndReadThread;
			break;
			
		}//switch
	}//while true
	
	// Time to clean up Read Thread.
EndReadThread:
	PurgeComm( hCommFile, PURGE_RXABORT + PURGE_RXCLEAR );
	CloseHandle( overlappedRead.hEvent );
	CloseHandle( overlappedCommEvent.hEvent );
}

CReadThread::CReadThread()
{
	self=this;	
	hReadThread=CreateThread(NULL,0,&ReadProc,NULL,CREATE_SUSPENDED,&threadID);
  printf("read thread id: %d\n", threadID);
}
CReadThread::~CReadThread()
{
}
void CReadThread::Resume()
{
	ResumeThread(hReadThread);
}
void CReadThread::Suspend()
{
	SuspendThread(hReadThread);
}
void CReadThread::SetPriority(int nPriority)
{
	SetThreadPriority(hReadThread,nPriority);
}
void CReadThread::Terminate()
{
	CloseHandle(hReadThread);
}
bool CReadThread::SetupReadEvent(OVERLAPPED * lpOverlappedRead, char * lpszInputBuffer, DWORD dwSizeofBuffer, DWORD &lpnNumberOfBytesRead)
{
	DWORD dwLastError;
StartSetupReadEvent:
	// Make sure the CloseEvent hasn't been signaled yet.
	// Check is needed because this function is potentially recursive.
	if (WAIT_TIMEOUT != WaitForSingleObject(hCloseEvent,0) )
        return false;
	
	// Start the overlapped ReadFile.
	if (ReadFile(hCommFile,lpszInputBuffer, dwSizeofBuffer,&lpnNumberOfBytesRead, lpOverlappedRead ) )
	{
		// This would only happen if there was data waiting to be read.
		// Handle the data.
		if (! HandleReadData( lpszInputBuffer, lpnNumberOfBytesRead ) )
			return false;
		
		// Start waiting for more data.
		goto StartSetupReadEvent;
	}
	// ReadFile failed.  Expected because of overlapped I/O.
	dwLastError = GetLastError();
	
	// LastError was ERROR_IO_PENDING, as expected.
	if (dwLastError == ERROR_IO_PENDING )
	{
		return true;
	}
	// Its possible for this error to occur if the
	// service provider has closed the port.  Time to }.
	if (dwLastError == ERROR_INVALID_HANDLE) 
        return false;
	// Unexpected error come here. No idea what could cause this to happen.
	PostHangupCall();
	return false;
}

bool CReadThread::HandleReadData(char * lpszInputBuffer, DWORD dwSizeofBuffer)
{
	char * lpszPostedBytes;
	
	// If we got data and didn't just time out empty...
	if (dwSizeofBuffer != 0) 
	{
		// Do something with the bytes read.
		
		//lpszPostedBytes=(char *)malloc(dwSizeofBuffer+1);
    lpszPostedBytes=new char[dwSizeofBuffer+1];
		if (lpszPostedBytes == NULL)
		{
			// Out of memory	
			PostHangupCall();
			return false;
		}
		memcpy(lpszPostedBytes,lpszInputBuffer,dwSizeofBuffer);
        lpszPostedBytes[dwSizeofBuffer] = '\0';
		
		return ReceiveData( lpszPostedBytes, dwSizeofBuffer );
	}
	return false;
}
bool CReadThread::HandleReadEvent(OVERLAPPED * lpOverlappedRead, char * lpszInputBuffer, DWORD dwSizeofBuffer, DWORD &lpnNumberOfBytesRead)
{
	DWORD dwLastError;
	
	if (GetOverlappedResult( hCommFile,lpOverlappedRead, &lpnNumberOfBytesRead, false ) )
	{
		return HandleReadData( lpszInputBuffer, lpnNumberOfBytesRead );
	}
	
	// Error in GetOverlappedResult; handle it.
	dwLastError = GetLastError();
	// Its possible for this error to occur if the
	// service provider has closed the port.  Time to }.
	if (dwLastError == ERROR_INVALID_HANDLE) 
        return false;
	
	// Unexpected error come here. No idea what could cause this to happen.
	PostHangupCall();
	return false;
}

bool CReadThread::SetupCommEvent(OVERLAPPED * lpOverlappedCommEvent, LPDWORD lpfdwEvtMask)
{
	DWORD dwLastError;
StartSetupCommEvent:
	
	// Make sure the CloseEvent hasn't been signaled yet.
	// Check is needed because this function is potentially recursive.
	if (WAIT_TIMEOUT != WaitForSingleObject( hCloseEvent,0 ) )
        return false;
	
	// Start waiting for Comm Errors.
	if (WaitCommEvent( hCommFile, lpfdwEvtMask, lpOverlappedCommEvent ) )
	{
		// This could happen if there was an error waiting on the
		// comm port.  Lets try and handle it.
		
		if (! HandleCommEvent( NULL, lpfdwEvtMask, false ) )
		{
			return false;
		}		
		// What could cause infinite recursion at this point?
		goto StartSetupCommEvent;
	}
	
	// We expect ERROR_IO_PENDING returned from WaitCommEvent
	// because we are waiting with an overlapped structure.
	
	dwLastError = GetLastError();
	
	// LastError was ERROR_IO_PENDING, as expected.
	if (dwLastError == ERROR_IO_PENDING )
	{
		return true;
	}
	
	// Its possible for this error to occur if the
	// service provider has closed the port.  Time to }.
	if (dwLastError == ERROR_INVALID_HANDLE )
        return false;
	
	// Unexpected error. No idea what could cause this to happen.
	PostHangupCall();
	return false;
}
bool CReadThread::HandleCommEvent(OVERLAPPED * lpOverlappedCommEvent, LPDWORD lpfdwEvtMask, bool fRetrieveEvent)
{
	DWORD dwDummy;
	DWORD dwErrors;
	DWORD dwLastError;
	DWORD dwModemEvent;
	// If this fails, it could be because the file was closed (and I/O is
	// finished) | because the overlapped I/O is still in progress.  In
	// either case (| any others) its a bug and return false.
	if (fRetrieveEvent) 
	{
		if (! GetOverlappedResult( hCommFile,lpOverlappedCommEvent, &dwDummy, false ) )
		{
			dwLastError = GetLastError();
			
			// Its possible for this error to occur if the
			// service provider has closed the port.  Time to }.
			if (dwLastError == ERROR_INVALID_HANDLE) 
				return false;
			PostHangupCall();
			return false;
		}
	}
	// Was the event an error?
	if ((*lpfdwEvtMask & EV_ERR) != 0 )
	{
		// Which error was it?
		if (! ClearCommError( hCommFile, &dwErrors, NULL ) )
		{
			dwLastError = GetLastError();
			
			// Its possible for this error to occur if the
			// service provider has closed the port.  Time to }.
			if (dwLastError == ERROR_INVALID_HANDLE )
				return false;
			
			PostHangupCall();
			return false;
		}
		// Its possible that multiple errors occured and were handled
		// in the last ClearCommError.  Because all errors were signaled
		// individually, but cleared all at once, pending comm events
		// can yield EV_ERR while dwErrors equals 0.  Ignore this event.
		
		if (! ReceiveError( dwErrors ) )
			return false;
		
		return true;
	}
	
	dwModemEvent = 0;
	
	if ((*lpfdwEvtMask & EV_RLSD) != 0) 
        dwModemEvent = ME_RLSD;
	if ((*lpfdwEvtMask & EV_RING) != 0) 
        dwModemEvent = dwModemEvent | ME_RING;
	
	if (dwModemEvent != 0) 
	{
		if (! ModemStateChange( dwModemEvent )) 
		{
			return false;
		}		
		return true;
	}
	
	if (((*lpfdwEvtMask & EV_ERR)==0) && (dwModemEvent==0)) 
	{
		// Should not have gotten here.
		PostHangupCall();
	}
	return false;
}

bool CReadThread::ReceiveData(char * lpNewString, DWORD dwSizeofNewString)
{
  // At状态
  Ccom::m_nAtRet = 0;
  int i,j=0;
  char sp[512];
  memset(sp, 0, 512);
  for (i = 0; *(lpNewString + i) != '\0'; i++) {
    if (*(lpNewString + i) == 10 || *(lpNewString + i) == 13)
      continue;
    sp[j++]=*(lpNewString + i);
  }
  sp[j] = 0;
  int nRet = strlen(sp);
  if(nRet < 2)
    return true;
  if (NULL != strstr(sp, "OK"))
  {
    Ccom::m_nAtRet = 1;
    SetEvent(hRecvEvent);
    return true;
  }
  else if (NULL != strstr(sp, "ERROR"))
  {
    Ccom::m_nAtRet = -1;
    SetEvent(hRecvEvent);
    return false;
  }
  else if (NULL != strstr(sp, "NO CARRIER"))
  {
    Ccom::m_nAtRet = -2;
    SetEvent(hRecvEvent);
    return false;
  }
  return true;
}

bool CReadThread::ReceiveError(DWORD EvtMask)
{
	if (! PostMessage( hComm32Window, PWM_RECEIVEERROR, 0, (LPARAM)EvtMask ) )
        PostHangupCall();
	else
		return true;
	return false;
}
bool CReadThread::ModemStateChange(DWORD ModemEvent)
{
	if (! PostMessage( hComm32Window, PWM_MODEMSTATECHANGE, 0, LPARAM(ModemEvent) ) )
        PostHangupCall();
	else
        return true;
	return false;
}

void CReadThread::PostHangupCall()
{
	PostMessage( hComm32Window, PWM_REQUESTHANGUP, 0, 0 );
}
//////////////////////////////////////////////////////////////////////
// CWriteThread
//////////////////////////////////////////////////////////////////////
CWriteThread::CWriteThread()
{
	self=this;	
	hWriteThread=CreateThread(NULL,0,&WriteProc,NULL,CREATE_SUSPENDED,&threadID);
  printf("write thread id: %d\n", threadID);
}
CWriteThread::~CWriteThread()
{
}
void CWriteThread::Terminate()
{
	CloseHandle(hWriteThread);
}
void CWriteThread::Resume()
{
	ResumeThread(hWriteThread);
}
void CWriteThread::Suspend()
{
	SuspendThread(hWriteThread);
}
void CWriteThread::SetPriority(int nPriority)
{
	SetThreadPriority(hWriteThread,nPriority);
}
bool CWriteThread::HandleWriteData(OVERLAPPED * lpOverlappedWrite, char * pDataToWrite, DWORD dwNumberOfBytesToWrite)
{
	DWORD dwLastError;		
	DWORD dwNumberOfBytesWritten;
	DWORD dwWhereToStartWriting;
	DWORD dwHandleSignaled;
	HANDLE HandlesToWaitFor[2];
	dwNumberOfBytesWritten = 0;
	dwWhereToStartWriting = 0; // Start at the {ning.
	
	HandlesToWaitFor[0] = hCloseEvent;
	HandlesToWaitFor[1] = (*lpOverlappedWrite).hEvent;
	
	// Keep looping until all characters have been written.
	do
	{      // Start the overlapped I/O.
		if (! WriteFile( hCommFile,&pDataToWrite[dwWhereToStartWriting],dwNumberOfBytesToWrite, &dwNumberOfBytesWritten,lpOverlappedWrite ))
		{
			// WriteFile failed.  Expected; lets handle it.
			dwLastError = GetLastError();
			
			// Its possible for this error to occur if the
			// service provider has closed the port.  Time to }.
			if (dwLastError == ERROR_INVALID_HANDLE)
				return false;
			
			// Unexpected error.  No idea what.
			if (dwLastError != ERROR_IO_PENDING )
			{
				PostHangupCall();
				return false;
			}
			
			// This is the expected ERROR_IO_PENDING case.
			
			// Wait for either overlapped I/O completion,
			// or for the CloseEvent to get signaled.
			dwHandleSignaled = WaitForMultipleObjects(2, HandlesToWaitFor,false, INFINITE);
			switch(dwHandleSignaled)
			{
			case WAIT_OBJECT_0:     // CloseEvent signaled!
				// Time to return;.
				return false;
				break;
				
			case WAIT_OBJECT_0 + 1: // Wait finished.
				// Time to get the results of the WriteFile
				if (! GetOverlappedResult(hCommFile,lpOverlappedWrite,&dwNumberOfBytesWritten, true))
				{
					dwLastError = GetLastError();
					
					// Its possible for this error to occur if the
					// service provider has closed the port.
					if (dwLastError == ERROR_INVALID_HANDLE)
						return false;
					
					// No idea what could cause another error.
					PostHangupCall();
					return false;
				}
				break;
				
			case WAIT_FAILED: // Wait failed.  Shouldn't happen.
				PostHangupCall();
				return false;
				
			default: // This case should never occur.
				PostHangupCall();
				return false;
				break;
			} //case
		}// {WriteFile failure}
		
		// Some data was written.  Make sure it all got written.
		dwNumberOfBytesToWrite-=dwNumberOfBytesWritten;
		dwWhereToStartWriting+=dwNumberOfBytesWritten;
	}
	while (dwNumberOfBytesToWrite > 0);  // Write the whole thing!
	// Wrote the whole string.
	return true;
}

void CWriteThread::PostHangupCall()
{
	PostMessage( hComm32Window, PWM_REQUESTHANGUP, 0, 0 );
}
void CWriteThread::WriteExecute()
{
	MSG msg;
	DWORD dwHandleSignaled;
	OVERLAPPED overlappedWrite;
	bool CompleteOneWriteRequire;
	// Needed for overlapped I/O.
	ZeroMemory(&overlappedWrite,sizeof(overlappedWrite));
	   
	overlappedWrite.hEvent = CreateEvent( NULL, true, true, NULL );
	if (overlappedWrite.hEvent == 0)
	{
		PostHangupCall();
		goto EndWriteThread;
	}
	   
	CompleteOneWriteRequire = true;
	   
	   // This is the main loop.  Loop until we break out.
	while (true)
	{
		if (! PeekMessage(&msg, 0, 0, 0, PM_REMOVE ) )
		{
			// If there are no messages pending, wait for a message or
			// the CloseEvent.
			
			*pFSendDataEmpty = true;
			if (CompleteOneWriteRequire )
			{
				if (! PostMessage( hComm32Window, PWM_SENDDATAEMPTY, 0, 0 ) )
				{
					PostHangupCall();
					goto EndWriteThread;
				}
			}			
			CompleteOneWriteRequire = false;			
			dwHandleSignaled = MsgWaitForMultipleObjects(1, &hCloseEvent, false,
				INFINITE, QS_ALLINPUT);
			switch(dwHandleSignaled)
			{
			case WAIT_OBJECT_0:     // CloseEvent signaled!
				// Time to exit.
				goto EndWriteThread;
				break;				
			case WAIT_OBJECT_0 + 1: // New message was received.
				// Get the message that woke us up by looping again.
				continue;
				break;				
			case WAIT_FAILED:       // Wait failed.  Shouldn't happen.
				PostHangupCall();
				goto EndWriteThread;
				break;
			default:                // This case should never occur.
				PostHangupCall();
				goto EndWriteThread;
				break;
			}
		}		
		// Make sure the CloseEvent isn't signaled while retrieving messages.
		if (WAIT_TIMEOUT != WaitForSingleObject(hCloseEvent,0) )
			goto EndWriteThread;		
		// Process the message.
		// This could happen if a dialog is created on this thread.
		// This doesn't occur in this sample, but might if modified.
		if (msg.hwnd != 0 )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
    }		
    printf("%d\n", msg.message);
		// Handle the message.
		switch(msg.message)
		{			
		case PWM_COMMWRITE:  // New string to write to Comm port.
			{
				// Write the string to the comm port.  HandleWriteData
				// does ! return until the whole string has been written,
				// an error occurs or until the CloseEvent is signaled.
				if (! HandleWriteData( &overlappedWrite,
					(char *)(msg.lParam), DWORD(msg.wParam) ) )
				{
					// If it failed, either we got a signal to } or there
					// really was a failure.
					
					LocalFree( HLOCAL(msg.lParam) );
					goto EndWriteThread;
				}
				
				CompleteOneWriteRequire = true;
				// Data was sent in a LocalAlloc()d buffer.  Must free it.
				LocalFree( HLOCAL(msg.lParam) );
			}
		}
	} //{main loop}
	   
	   // Thats the }.  Now clean up.
EndWriteThread:  
	PurgeComm(hCommFile, PURGE_TXABORT + PURGE_TXCLEAR);
	*pFSendDataEmpty = true;
	CloseHandle(overlappedWrite.hEvent);
	//{TWriteThread.Execute}
}
//////////////////////////////////////////////////////////////////////
// Ccom
//////////////////////////////////////////////////////////////////////

int Ccom::m_nAtRet = 0;

Ccom::Ccom()
{

}
Ccom::Ccom(HANDLE hRecv, HWND hWnd)
{
	ReadThread = NULL;
	WriteThread = NULL;
	hCommFile = 0;
	hCloseEvent = 0;
	FSendDataEmpty = true;
	
	FCommName = "COM5";
	FBaudRate = 9600;
	FParityCheck = false;
	FOutx_CtsFlow = false;
	FOutx_DsrFlow = false;
	FDtrControl = DtrEnable;
	FDsrSensitivity = false;
	FTxContinueOnXoff = true;
	FOutx_XonXoffFlow = true;
	FInx_XonXoffFlow = true;
	FReplaceWhenParityError = false;
	FIgnoreNullChar = false;
	FRtsControl = RtsEnable;
	FXonLimit = 500;
	FXoffLimit = 500;
	FByteSize = _8;
	FParity = None;
	FStopBits = _1;
	FXonChar = 0x11;      // Ctrl-Q
	FXoffChar = 0x13;     // Ctrl-S
	FReplacedChar = 0;
	FReadIntervalTimeout         = 50;
	FReadTotalTimeoutMultiplier  = 0;
	FReadTotalTimeoutConstant    = 0;
	FWriteTotalTimeoutMultiplier = 0;
	FWriteTotalTimeoutConstant   = 0;
	
	FHWnd=hWnd;
  m_hRecv = hRecv;
	Opened=false;
}

Ccom::~Ccom()
{
	
}
bool	Ccom::StartComm()
{
	HANDLE hNewCommFile;
	// Are we already doing comm?
  if (hCommFile != 0) 
	{
		//AfxMessageBox("串口已被占用!");
    printf("串口已被占用!\n");
		return false;
	}
	TCHAR strCommName[200];
  memset(strCommName, 0, 200);
  _stprintf_s(strCommName, _T("%s%s"), L"\\\\.\\", L"COM5");
  hNewCommFile = CreateFile(strCommName,
    GENERIC_READ | GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
    0 );
	
  if (hNewCommFile == INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox("打开串口失败!");
    printf("打开串口失败!");
		return false;
	}
	
	// Is this a valid comm handle?
	if (GetFileType( hNewCommFile ) != FILE_TYPE_CHAR) 
	{
		CloseHandle( hNewCommFile );
		//AfxMessageBox("File handle is ! a comm handle");
    printf("File handle is ! a comm handle");
		return false;
	}
	
	if (!SetupComm( hNewCommFile, 4096, 4096 )) 
	{
		CloseHandle( hCommFile );
		//AfxMessageBox("Cannot setup comm buffer");
		printf("Cannot setup comm buffer");
    return false;
	}
	
	// It is ok to continue.
	
	hCommFile = hNewCommFile;
	
	// purge any information in the buffer
	
	PurgeComm( hCommFile, PURGE_TXABORT | PURGE_RXABORT |
		PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	FSendDataEmpty = true;
	
	// Setting the time-out value
	_SetCommTimeout();
	
	// Querying  setting the comm port configurations.
	_SetCommState();

	// Create the event that will signal the threads to close.
	hCloseEvent = CreateEvent( NULL, true, false, NULL );
	
	if (hCloseEvent == 0) 
	{
		CloseHandle( hCommFile );
		hCommFile = 0;
		//AfxMessageBox("Unable to create event");
    printf("Unable to create event");
		return false;
	}
	
	// Create the Read thread.
	try
	{
		ReadThread = new CReadThread();
	}
	catch(...)
	{
		ReadThread = NULL;
		CloseHandle( hCloseEvent );
		CloseHandle( hCommFile );
		hCommFile = 0;
		//AfxMessageBox("Unable to create read thread");
    printf("Unable to create read thread");
		return false;
	}
	ReadThread->hCommFile = hCommFile;
	ReadThread->hCloseEvent = hCloseEvent;
	ReadThread->hComm32Window = FHWnd;
  ReadThread->hRecvEvent = m_hRecv;
	
	// Comm threads should have a higher base priority than the UI thread.
	// If they don't,  any temporary priority boost the UI thread gains
	// could cause the COMM threads to loose data.
	
	ReadThread->SetPriority(THREAD_PRIORITY_HIGHEST);
	
/*	// Create the Write thread.
	try
	{
		WriteThread =new CWriteThread();
	}
	catch(...)
	{
		CloseReadThread();
		WriteThread = NULL;
		CloseHandle( hCloseEvent );
		CloseHandle( hCommFile );
		hCommFile = 0;
		//AfxMessageBox("Unable to create write thread");
    printf("Unable to create write thread");
		return false;
	}
	WriteThread->hCommFile = hCommFile;
	WriteThread->hCloseEvent = hCloseEvent;
	WriteThread->hComm32Window = FHWnd;
	WriteThread->pFSendDataEmpty = &FSendDataEmpty;
	
	WriteThread->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	

	WriteThread->Resume();
	Opened=true;
  */
  ReadThread->Resume();
	return true;
	// Everything was created ok.  Ready to go!
}

// 往串口里面写入数据
bool Ccom::SendData(char * pDataToWrite, WORD dwSizeofDataToWrite)
{
  DWORD dwBytesWritten = 0L;
  OVERLAPPED ol;
  ZeroMemory(&ol, sizeof(ol));
  ol.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
  BOOL bRet = WriteFile(hCommFile, pDataToWrite, dwSizeofDataToWrite, &dwBytesWritten, &ol);
  //写入数据失败
  if (bRet == 0 || dwBytesWritten == 0)
  {
    if(GetLastError() != ERROR_IO_PENDING)
    {
      printf("写入数据失败！\n");
      return false;
    }
  }
  return true;
}


void Ccom::StopComm()
{
	// No need to continue if we're ! communicating.
	if (hCommFile == 0 )
        return;
	
	// Close the threads.
	CloseReadThread();
	CloseWriteThread();
	
	// ! needed anymore.
	CloseHandle( hCloseEvent );
	
	// Now close the comm port handle.
	CloseHandle( hCommFile );
	hCommFile = 0;
	Opened=false;
}// {TComm.StopComm}
bool Ccom::WriteCommData(char * pDataToWrite, WORD dwSizeofDataToWrite)
{
	char *Buffer;
	if ((WriteThread != NULL) && (dwSizeofDataToWrite != 0))
	{
		//Buffer =(char *)malloc(dwSizeofDataToWrite+1);
    Buffer = new char[dwSizeofDataToWrite+1];
    memset(Buffer, 0, dwSizeofDataToWrite+1);
		memcpy(Buffer,pDataToWrite,dwSizeofDataToWrite);
    int ret;
		if (ret = PostThreadMessage(WriteThread->threadID, PWM_COMMWRITE,WPARAM(dwSizeofDataToWrite), LPARAM(Buffer) ))
		{
			FSendDataEmpty = false;
			return true;
		}
    if(GetLastError() == ERROR_INVALID_THREAD_ID)
      printf("xiancheng error\n");
	}
	return false;
} //{TComm.WriteCommData}
DWORD   Ccom::GetModemState()
{
	DWORD dwModemState;
	if (! GetCommModemStatus( hCommFile, &dwModemState ))
		return 0;
	else
		return dwModemState;
}
bool Ccom::IsOpened()
{
	return Opened;
}
void Ccom::CloseReadThread()
{
	// If it exists...
	if (ReadThread !=NULL)
	{
		// Signal the event to close the worker threads.
		SetEvent( hCloseEvent );
		
		// Purge all outstanding reads
		PurgeComm( hCommFile, PURGE_RXABORT + PURGE_RXCLEAR );
		
		// Wait 10 seconds for it to exit.  Shouldn't happen.
		if (WaitForSingleObject(ReadThread->hReadThread, 10000) == WAIT_TIMEOUT)
			ReadThread->Terminate();
		ReadThread = NULL;
	}
}// {TComm.CloseReadThread}
void Ccom::CloseWriteThread()
{
	// If it exists...
	if (WriteThread != NULL)
	{
		// Signal the event to close the worker threads.
		SetEvent(hCloseEvent);
		
		// Purge all outstanding writes.
		PurgeComm(hCommFile, PURGE_TXABORT + PURGE_TXCLEAR);
		FSendDataEmpty = true;
		
		// Wait 10 seconds for it to exit.  Shouldn't happen.
		if (WaitForSingleObject( WriteThread->hWriteThread, 10000 ) == WAIT_TIMEOUT)
			WriteThread->Terminate();
		WriteThread = NULL;
	}
}// {TComm.CloseWriteThread}
void Ccom::_SetCommState()
{
	DCB dcb;
	COMMPROP commprop;
	DWORD fdwEvtMask;
	
	// Configure the comm settings.
	// NOTE: Most Comm settings can be set through TAPI, but this means that
	//       the CommFile will have to be passed to this component.
	
	GetCommState( hCommFile, &dcb );
	GetCommProperties( hCommFile, &commprop );
	GetCommMask( hCommFile, &fdwEvtMask );
	
	// fAbortOnError is the only DCB dependancy in TapiComm.
	// Can't guarentee that the SP will set this to what we expect.
	//{dcb.fAbortOnError = false; ! VALID}
	
	dcb.BaudRate = FBaudRate;
	
	dcb.XonLim = FXonLimit;
	dcb.XoffLim = FXoffLimit;
	
	dcb.ByteSize = FByteSize;
	dcb.Parity = FParity;
	dcb.StopBits = FStopBits;
	
	dcb.XonChar = FXonChar;
	dcb.XoffChar = FXoffChar;
	
	dcb.ErrorChar = FReplacedChar;
	
	SetCommState( hCommFile, &dcb );
}

void Ccom::_SetCommTimeout()
{
	COMMTIMEOUTS commtimeouts;	
	GetCommTimeouts( hCommFile, &commtimeouts );	
	// The CommTimeout numbers will very likely change if you are
	// coding to meet some kind of specification where
	// you need to reply within a certain amount of time after
	// recieving the last byte.  However,  If 1/4th of a second
	// goes by between recieving two characters, its a good
	// indication that the transmitting } has finished, even
	// assuming a 1200 baud modem.
	if (FBaudRate>=19200)
	{
		FReadIntervalTimeout         = 20;
	}
	else if (FBaudRate>=9600 && FBaudRate<19200)
	{
		FReadIntervalTimeout         = 30;
	}
	else if (FBaudRate>=4800 && FBaudRate<9600)
	{
		FReadIntervalTimeout         = 50;
	}
	commtimeouts.ReadIntervalTimeout         = FReadIntervalTimeout;
	commtimeouts.ReadTotalTimeoutMultiplier  = FReadTotalTimeoutMultiplier;
	commtimeouts.ReadTotalTimeoutConstant    = FReadTotalTimeoutConstant;
	commtimeouts.WriteTotalTimeoutMultiplier = FWriteTotalTimeoutMultiplier;
	commtimeouts.WriteTotalTimeoutConstant   = FWriteTotalTimeoutConstant;
	
	SetCommTimeouts( hCommFile, &commtimeouts );
}
void Ccom::SetBaudRate(DWORD Rate)
{
	if (Rate == FBaudRate )
        return;
	
	FBaudRate = Rate;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetParityCheck(bool b )
{
	if (b == FParityCheck )
        return;
	
	FParityCheck = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetOutx_CtsFlow(bool b)
{
	if (b == FOutx_CtsFlow )
        return;
	
	FOutx_CtsFlow = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetOutx_DsrFlow( bool b )
{
	if (b == FOutx_DsrFlow )
        return;
	
	FOutx_DsrFlow = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetDtrControl( DtrControl c )
{
	if (c == FDtrControl )
        return;
	
	FDtrControl = c;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetDsrSensitivity( bool b )
{
	if (b == FDsrSensitivity )
        return;
	
	FDsrSensitivity = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetTxContinueOnXoff( bool b )
{
	if (b == FTxContinueOnXoff )
        return;
	
	FTxContinueOnXoff = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetOutx_XonXoffFlow( bool b )
{
	if (b == FOutx_XonXoffFlow )
        return;
	
	FOutx_XonXoffFlow = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetInx_XonXoffFlow( bool b )
{
	if (b == FInx_XonXoffFlow )
        return;
	
	FInx_XonXoffFlow = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetReplaceWhenParityError( bool b )
{
	if (b == FReplaceWhenParityError )
        return;
	
	FReplaceWhenParityError = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetIgnoreNullChar( bool b )
{
	if (b == FIgnoreNullChar )
        return;
	
	FIgnoreNullChar = b;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetRtsControl( RtsControl c )
{
	if (c == FRtsControl )
        return;
	
	FRtsControl = c;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetXonLimit( WORD Limit )
{
	if (Limit == FXonLimit )
        return;
	
	FXonLimit = Limit;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetXoffLimit( WORD Limit )
{
	if (Limit == FXoffLimit )
        return;
	
	FXoffLimit = Limit;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetByteSize(ByteSize Size)
{
	if (Size == FByteSize )
        return;
	
	FByteSize = Size;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetParity(Parity p)
{
	if (p == FParity )
        return;
	
	FParity = p;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetStopBits(StopBits Bits)
{
	if (Bits == FStopBits )
        return;
	
	FStopBits = Bits;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetXonChar(char c )
{
	if (c == FXonChar )
        return;
	
	FXonChar = c;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetXoffChar(char  c )
{
	if (c == FXoffChar )
        return;
	
	FXoffChar = c;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetReplacedChar(char c)
{
	if (c == FReplacedChar )
        return;
	
	FReplacedChar = c;
	
	if (hCommFile != 0 )
        _SetCommState();
}

void Ccom::SetReadIntervalTimeout( DWORD v)
{
	if (v == FReadIntervalTimeout )
        return;
	
	FReadIntervalTimeout = v;
	
	if (hCommFile != 0 )
        _SetCommTimeout();
}

void Ccom::SetReadTotalTimeoutMultiplier( DWORD v )
{
	if (v == FReadTotalTimeoutMultiplier )
        return;
	
	FReadTotalTimeoutMultiplier = v;
	
	if (hCommFile != 0 )
        _SetCommTimeout();
}

void Ccom::SetReadTotalTimeoutConstant( DWORD v )
{
	if (v == FReadTotalTimeoutConstant )
        return;
	
	FReadTotalTimeoutConstant = v;
	
	if (hCommFile != 0 )
        _SetCommTimeout();
}

void Ccom::SetWriteTotalTimeoutMultiplier(DWORD v )
{
	if (v == FWriteTotalTimeoutMultiplier )
        return;
	
	FWriteTotalTimeoutMultiplier = v;
	
	if (hCommFile != 0 )
        _SetCommTimeout();
}

void Ccom::SetWriteTotalTimeoutConstant( DWORD v )
{
	if (v == FWriteTotalTimeoutConstant )
        return;
	
	FWriteTotalTimeoutConstant = v;
	
	if (hCommFile != 0 )
        _SetCommTimeout();
}

void Ccom::SetCommName(LPCTSTR lpCommName)
{
	
	FCommName = (char*)lpCommName;
}

char* Ccom::GetCommName()
{
	return FCommName;
}
