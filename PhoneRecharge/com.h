// com.h: interface for the Ccom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COM_H__B6972783_9F49_4703_BCC9_64A01236F7C9__INCLUDED_)
#define AFX_COM_H__B6972783_9F49_4703_BCC9_64A01236F7C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PWM_GOTCOMMDATA WM_USER+1// 0x401;//WM_USER+1
#define PWM_RECEIVEERROR WM_USER+2
#define PWM_REQUESTHANGUP WM_USER+3
#define PWM_MODEMSTATECHANGE WM_USER+4
#define PWM_SENDDATAEMPTY WM_USER+5
static const BYTE ME_CTS = 0x1;
static const BYTE ME_DSR = 0x2;
static const BYTE ME_RING = 0x4;
static const BYTE ME_RLSD = 0x8;
#define PWM_COMMWRITE WM_USER+6
static const WORD INPUTBUFFERSIZE=0x800;
enum Parity {None, Odd, Even, Mark, Space};
enum StopBits { _1, _1_5, _2 };
enum ByteSize { _5=5, _6, _7, _8 };
enum DtrControl { DtrEnable, DtrDisable, DtrHandshake };
enum RtsControl { RtsEnable, RtsDisable, RtsHandshake, RtsTransmissionAvailable };

class CReadThread
{
public:
	CReadThread();
	DWORD threadID;
	virtual ~CReadThread();	
	static CReadThread *self;
	HANDLE hReadThread;
  HANDLE hRecvEvent;

	void ReadExecute();
	HANDLE hCommFile;
	HANDLE hCloseEvent;
	HWND hComm32Window;
	bool	SetupCommEvent(OVERLAPPED * lpOverlappedCommEvent, LPDWORD lpfdwEvtMask);
	bool	SetupReadEvent(OVERLAPPED * lpOverlappedRead, char * lpszInputBuffer, DWORD dwSizeofBuffer, DWORD &lpnNumberOfBytesRead);
	bool	HandleCommEvent(OVERLAPPED * lpOverlappedCommEvent, LPDWORD lpfdwEvtMask, bool fRetrieveEvent);
	bool	HandleReadEvent(OVERLAPPED * lpOverlappedRead, char * lpszInputBuffer, DWORD dwSizeofBuffer, DWORD &lpnNumberOfBytesRead);
	bool	HandleReadData(char * lpszInputBuffer, DWORD dwSizeofBuffer);
	bool	ReceiveData(char * lpNewString, DWORD dwSizeofNewString);
	bool	ReceiveError(DWORD EvtMask);
	bool	ModemStateChange(DWORD ModemEvent);
	void	PostHangupCall();
	void	Suspend();
	void	Resume();
	void	SetPriority(int nPriority);
	void	Terminate();
};
class  CWriteThread
{	
protected:
	bool   HandleWriteData(OVERLAPPED * lpOverlappedWrite, char * pDataToWrite, DWORD dwNumberOfBytesToWrite);
public:
	void   WriteExecute();
	static CWriteThread *self;
	HANDLE hWriteThread;
	HANDLE hCommFile;
	HANDLE hCloseEvent;
	HWND hComm32Window;
	bool	*pFSendDataEmpty;
	void	PostHangupCall();
	void	Suspend();
	void	Resume();
	void	SetPriority(int nPriority);
	void	Terminate();
public:
	CWriteThread();
	virtual ~CWriteThread();
	DWORD threadID;
};
class Ccom  
{
public:
  Ccom();
	Ccom(HANDLE hRecv, HWND hWnd);//¸¸´°¿Ú¾ä±ú
	virtual ~Ccom();
private:

	CReadThread* ReadThread;
	CWriteThread* WriteThread;
	HANDLE hCommFile;
	HANDLE hCloseEvent;
  HANDLE m_hRecv;
	HWND FHWnd;
	bool FSendDataEmpty;
	char* FCommName;
	DWORD FBaudRate;
	bool Opened;
	bool FParityCheck;
	bool FOutx_CtsFlow;
	bool FOutx_DsrFlow;
	DtrControl FDtrControl;
	bool FDsrSensitivity;
	bool FTxContinueOnXoff;
	bool FOutx_XonXoffFlow;
	bool FInx_XonXoffFlow;
	bool FReplaceWhenParityError;
	bool FIgnoreNullChar;
	RtsControl FRtsControl;
	WORD FXonLimit;
	WORD FXoffLimit;
	ByteSize FByteSize;
	Parity FParity;
	StopBits FStopBits;
	char FXonChar;
	char FXoffChar;
	char FReplacedChar;
	DWORD FReadIntervalTimeout;
	DWORD FReadTotalTimeoutMultiplier;
	DWORD FReadTotalTimeoutConstant;
	DWORD FWriteTotalTimeoutMultiplier;
	DWORD FWriteTotalTimeoutConstant;
	
	void   SetParityCheck(bool b);
	void   SetOutx_CtsFlow(bool b);
	void   SetOutx_DsrFlow(bool b);
	void   SetDtrControl(DtrControl c);
	void   SetDsrSensitivity(bool b);
	void   SetTxContinueOnXoff(bool b);
	void   SetOutx_XonXoffFlow(bool b);
	void   SetInx_XonXoffFlow(bool b);
	void   SetReplaceWhenParityError(bool b);
	void   SetIgnoreNullChar(bool b);
	void   SetRtsControl(RtsControl c);
	void   SetXonLimit(WORD Limit);
	void   SetXoffLimit(WORD Limit);
	void   SetByteSize(ByteSize Size);
	void   SetParity(Parity p);
	void   SetStopBits(StopBits Bits);
	void   SetXonChar(char c);
	void   SetXoffChar(char c);
	void   SetReplacedChar(char c);
	void   SetReadIntervalTimeout(DWORD v);
	void   SetReadTotalTimeoutMultiplier(DWORD v);
	void   SetReadTotalTimeoutConstant(DWORD v);
	void   SetWriteTotalTimeoutMultiplier(DWORD v);
	void   SetWriteTotalTimeoutConstant(DWORD v);
	void   _SetCommState();
	void   _SetCommTimeout();
	
protected:
	void   CloseReadThread();
	void   CloseWriteThread();
	void   ReceiveData(char * Buffer, WORD BufferLength);
	void   ReceiveError(DWORD EvtMask);
	void   ModemStateChange(DWORD ModemEvent);
	void   RequestHangup();
	void   _SendDataEmpty();
	
public:
	char* GetCommName();
	void SetCommName(LPCTSTR lpCommName);
	void   SetBaudRate(DWORD Rate);
	bool   StartComm();
	void   StopComm();
	bool   WriteCommData(char * pDataToWrite, WORD dwSizeofDataToWrite);
	DWORD   GetModemState();
	bool	IsOpened();

  bool SendData(char * pDataToWrite, WORD dwSizeofDataToWrite);
  static int m_nAtRet;  // 0:Init  1:OK  -1:ERROR  -2:NO CARRIER
};

#endif // !defined(AFX_COM_H__B6972783_9F49_4703_BCC9_64A01236F7C9__INCLUDED_)
