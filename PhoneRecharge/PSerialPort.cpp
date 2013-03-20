/*******************************************************************
| Copyright (c) 2012,���������¿Ƽ����޹�˾                      |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| �ļ����ƣ�PSerialPort.cpp                                        |
| �ļ���ʶ��                                                       |
| ժҪ��    ���ļ���������PSerialPort������һЩ��Ա                |
| ����˵������                                                     |
| ��ǰ�汾��0.9.2                                                  |
| ��    �ߣ�                                                       |
| ������ڣ�                                                       |
|----------------------------------------------------------------- |
| �޸ļ�¼1��                                                      |
| �޸����ڣ�                                                       |
| �� �� �ţ�                                                       |
| �� �� �ˣ�                                                       |
| �޸�ԭ��                                                       |
| �޸����ݣ�                                                       |
| �޸Ĵ���λ�ã�                                                   |
|------------------------------------------------------------------|
| �޸ļ�¼2����                                                    |
|******************************************************************/

#include "stdafx.h"
#include "PSerialPort.h"
//#pragma message( "Pentium processor build" )

#pragma warning(disable : 4244)


/*******************************************************************
| �������ƣ����캯��                                               |
| �������ܣ���ʼ��CPSerialPort����                                 |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  ��                                                     |
| ˵������                                                         |
|******************************************************************/
CPSerialPort::CPSerialPort(void):m_hComm(INVALID_HANDLE_VALUE),//���ھ��
                                 m_hReadThread(NULL),//�������߳�handle 
                                 m_bReceiving(FALSE),//
                                 m_nBufferSize(256), //�����С
                                 m_dwUserData(NULL), //����
                                 wDataCount(0),
                                 wPos(0),
                                 wStart(0)
{
  if (strComData[0] != '\0')
  {
    //����
    ZeroMemory(strComData, 4096 * sizeof(char));
  }

  //��ʼ���ٽ���
  InitializeCriticalSection(&m_BufferSection);
}

/*******************************************************************
| �������ƣ���������                                               |
| �������ܣ��ͷ�CPSerialPort�������Դ                             |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  ��                                                     |
| ˵������                                                         |
|******************************************************************/
CPSerialPort::~CPSerialPort(void)
{
  try
  {
    //�رմ����豸
    ClosePort();
  }
  catch(...)
  {
    return; //�쳣
  }
  
  //���ھ��
  m_hComm = INVALID_HANDLE_VALUE;
  
  //�������߳�handle
  m_hReadThread = NULL;
  
  //�Ƿ�������ձ�־,��ʼ��Ϊ��Ч
  m_bReceiving = FALSE;
  
  try
  {
    //�ͷ��ٽ�����Դ
    DeleteCriticalSection(&m_BufferSection);
  }
  catch(...)
  {
    return; //�쳣
  }
}

/*******************************************************************
| �������ƣ�ReadPortThread                                         |
| �������ܣ����������̺߳���                                       |
| ���������LPVOID lpParameter,����ָ��                            |
| �����������                                                     |
| ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
| ˵������                                                         |
|******************************************************************/
DWORD WINAPI CPSerialPort::ReadPortThread(LPVOID lpParameter)
{
  //�ֲ�����
  CPSerialPort* pSerial = (CPSerialPort*)lpParameter;//����ͨѶ����
  BOOL fReadState = FALSE;                             //�򿪴��ڱ�־
  BYTE* pbuf = NULL;                                    //�ַ���ָ��
  DWORD dwLength = 0L;                                  //ʵ�ʶ�ȡ�����ݵĳ���
  
  //����ռ�
  try
  {
    pbuf = new BYTE[pSerial->m_nBufferSize];
  }
  catch(...)
  {
    return 1;  //����ռ�ʧ�� 
  }


  //�ж�,���ڶ�ȡ����
  while ((pSerial->m_hComm != INVALID_HANDLE_VALUE)
         && (pSerial->m_bReceiving))
  {
    //ZeroMemory(pbuf, pSerial->m_nBufferSize);
    //��ȡ��������
    fReadState = ReadFile(pSerial->m_hComm,
      pbuf,
      pSerial->m_nBufferSize,
      &dwLength,
      NULL);

    //�Ӵ��ڶ�ȡ����ʧ��
    if (fReadState == FALSE)
    {
      Sleep(1);
    }
    else
    {
      //�ɹ���ȡ�����ڵ�����
#ifdef RELDEBUG_COM
      if(dwLength > 0)
      {
        if(dwLength > 6)
          RETAILMSG(1, (_T("-------  CPSerialPort::ReadPortThread From Com Read Data 0x%x 0x%x 0x%x 0x%x;!\r\n"),
              pbuf[0], pbuf[1], pbuf[2], pbuf[5])); 
        else
          RETAILMSG(1, (_T("-------  CPSerialPort::ReadPortThread From Com Read Data < 4 Bytes;!\r\n"))); 
      }
#endif

      //��������
      if (pSerial->m_lpDataArriveProc != NULL)
      {
        //����m_lpDataArriveProc(),��������
        pSerial->m_lpDataArriveProc((char*)pbuf, dwLength, pSerial->m_dwUserData);
      }

    }
  }

  //�ͷŶ�̬������ڴ�ռ�
  if (pbuf != NULL)
  {
    delete [] pbuf;

    pbuf = NULL;
  }

#ifdef _DEBUG
  OutputDebugString(_T("PSerialPort Success\n"));
#endif

  //����
  return 0;
}

/*******************************************************************
| �������ƣ�OpenPort                                               |
| �������ܣ��򿪴���                                               |
| ���������LPCTSTR Port,�˿ں�                                    |
|           int Baudrate,������                                    |
|           int DataBits,����λ                                    |
|           int StopBits,ֹͣλ                                    |
|           int Parity,У��λ                                      |
|           LPDataArriveProc proc,����ָ��                         |
|           DWORD userdata                                         |
| �����������                                                     |
| ����ֵ��  WORD��,�ɹ�����0��ʧ�ܷ���1                            |
| ˵������                                                         |
|******************************************************************/
BOOL CPSerialPort::OpenPort(LPCTSTR Port,
                            signed int BaudRate,
                            signed int DataBits,
                            signed int StopBits,
                            signed int Parity,
                            LPDataArriveProc proc,
                            void* userdata)
{
  //������ַ
  m_lpDataArriveProc = proc;
  //ʹ�õ�����
  m_dwUserData = userdata;
  
  //�ж�,�򿪴���
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    m_hComm = CreateFile(Port, GENERIC_READ | GENERIC_WRITE,
      0, 0, OPEN_EXISTING, 0, 0);

    //�򿪴���ʧ��
    if (m_hComm == INVALID_HANDLE_VALUE )
    {
      return FALSE;
    }

    //��ȡ���ڵĵ�ǰ״̬ 
    GetCommState(m_hComm, &dcb);
    //�����豸��״ֵ̬ 
    dcb.BaudRate = (DWORD)BaudRate;
    dcb.ByteSize = (BYTE)DataBits;
    dcb.Parity = (BYTE)Parity;
    dcb.StopBits = (BYTE)StopBits;
    dcb.fParity = FALSE;
    dcb.fBinary = TRUE;
    dcb.fDtrControl = 0;
    dcb.fRtsControl = 0;
    dcb.fOutX = 0;
    dcb.fInX = 0;
    dcb.fTXContinueOnXoff = 0;

    //���ô���ͨѶ�豸״̬����
    SetCommMask(m_hComm, EV_RXCHAR);
    SetupComm(m_hComm, 16384, 16384);  

    //���鴮�������Ƿ�ɹ�
    if (SetCommState(m_hComm, &dcb) == FALSE)
    {
      //�޷�����ǰ�������ö˿ڣ����������"));
      PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
      //�رմ���
      ClosePort();
      //����ʧ�� 
      return FALSE;
    }

    //���ô���ͨѶ��ʱ����
    GetCommTimeouts(m_hComm, &CommTimeOuts);
    CommTimeOuts.ReadIntervalTimeout = 10;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
    CommTimeOuts.ReadTotalTimeoutConstant = 10;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
    CommTimeOuts.WriteTotalTimeoutConstant = 0;  
    //CommTimeOuts.ReadIntervalTimeout=100;
    //CommTimeOuts.ReadTotalTimeoutMultiplier=1;
    //CommTimeOuts.ReadTotalTimeoutConstant=100;
    //CommTimeOuts.WriteTotalTimeoutMultiplier=0;
    //CommTimeOuts.WriteTotalTimeoutConstant=0;    

    //�жϳ�ʱ�����Ƿ����óɹ�
    if (SetCommTimeouts(m_hComm, &CommTimeOuts) == FALSE)
    {
      //�޷����ó�ʱ����;
      PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);

      //�رմ���
      ClosePort();

      //����ʧ�� 
      return FALSE;
    }

    //����򿪴��ڹ�������ռ�õ���Դ
    PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);

    //����ʧ��
    return TRUE;
  }
  
  //ʧ��
  return FALSE;
}

/*******************************************************************
| �������ƣ��رմ���                                               |
| �������ܣ��ر��Ѿ��򿪵Ĵ����豸                                 |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
| ˵������                                                         |
|******************************************************************/
BOOL CPSerialPort::ClosePort(void)
{
  //�رս���
  Deactivate();
  
  //�жϴ��ھ���Ƿ���Ч
  if (m_hComm != INVALID_HANDLE_VALUE)
  {
    //���ô���״̬
    SetCommMask(m_hComm, 0);
    //�ͷŴ����豸��ռ����Դ
    PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
    //�رմ����豸 
    CloseHandle(m_hComm);
    //�ô����豸�����Ч
    m_hComm = INVALID_HANDLE_VALUE;
    //���سɹ�
    return TRUE;
  }
  
  //�ɹ�
  return TRUE;
}

/*******************************************************************
| �������ƣ�GetPortThreadPriority                                  |
| �������ܣ����ع����̵߳����ȼ�                                   |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
| ˵������                                                         |
|******************************************************************/
BOOL CPSerialPort::GetPortThreadPriority(int &nPriority)
{
  //�жϴ��ھ���Ƿ���Ч
  if ((m_hComm == INVALID_HANDLE_VALUE) || (m_hReadThread == NULL))
  {
    //�����豸��Ч,����1
    return FALSE;
  }

  //�������ձ�־��Ч
  if (m_bReceiving == FALSE)
  {
    //����ʧ��
    return FALSE;
  }

#ifdef _WINCE
  nPriority = CeGetThreadPriority(m_hReadThread);
#else
  nPriority = GetThreadPriority(m_hReadThread);
#endif

  //���سɹ�
  return TRUE;
}

/*******************************************************************
| �������ƣ�SetPortThreadPriority                                  |
| �������ܣ����ù����̵߳����ȼ�                                   |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
| ˵������                                                         |
|******************************************************************/
BOOL CPSerialPort::SetPortThreadPriority(signed int nPriority)
{
  //����ֵ
  BOOL bRet = FALSE;

  //�жϴ��ھ���Ƿ���Ч
  if ((m_hComm == INVALID_HANDLE_VALUE) || (m_hReadThread == NULL))
  {
    //�����豸��Ч,����1
    return FALSE;
  }

  //�������ձ�־��Ч
  if (m_bReceiving == FALSE)
  {
    //����ʧ��
    return FALSE;
  }

  //�ж����ȼ��Ƿ����255
  if (nPriority > 255)
  {
    //�����ȼ���Ϊ255
    nPriority = 255;
  }

  //�ж����ȼ��Ƿ�С��0
  if (nPriority < 0)
  {
    //�����ȼ���Ϊ0
    nPriority = 0;
  }

#ifdef _WINCE
  bRet = CeSetThreadPriority(m_hReadThread, nPriority);
#else
  bRet = SetThreadPriority(m_hReadThread, nPriority);
#endif

  return bRet;
}

/*******************************************************************
| �������ƣ�Activate                                               |
| �������ܣ�������رճ�������                                     |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
| ˵������                                                         |
|******************************************************************/
BOOL CPSerialPort::Activate(void)
{
  //�жϴ��ھ���Ƿ���Ч
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    //�����豸��Ч,����1
    return 1;
  }

  //���ձ�־��Ч ,��������
  if (m_bReceiving == FALSE)
  {
    //�ͷŴ����豸��ռ����Դ
    PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);

    //�ý��ձ�־��Ч
    m_bReceiving = TRUE;

    //������ȡ���ݵ��߳�
    m_hReadThread = CreateThread(NULL, 0, ReadPortThread, this, 0, NULL);
  }

  //�ж��̴߳����Ƿ�ɹ�
  if (m_hReadThread != NULL)
  {
    //�����̵߳����ȼ�
    //SetThreadPriority(m_hReadThread, THREAD_PRIORITY_HIGHEST);

    //�̴߳����ɹ�
    return 0;
  }
  else//�̴߳���ʧ��
  {
    //�ý��ձ�־��Ч
    m_bReceiving = FALSE;
    //����FALSE
    return 1;
  }
}

/*******************************************************************
| �������ƣ�Deactivate                                             |
| �������ܣ�ֹͣ�������ݵ��߳�                                     |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
| ˵������                                                         |
|******************************************************************/
BOOL CPSerialPort::Deactivate(void)
{
  //�жϴ��ھ���Ƿ���Ч,��Ч����FALSE
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  //ֹͣ�����߳�
  if (m_bReceiving == TRUE)
  {
    //�ý��ձ�־Ϊ��Ч
    m_bReceiving = FALSE;
    //�ȴ��߳�
    WaitForSingleObject(m_hReadThread, 500);
    //�رն�ȡ�������ݵ��߳�
    CloseHandle(m_hReadThread);
    //�ô��ڶ�ȡ�߳̾��Ϊ��
    m_hReadThread = NULL;

    //����TRUE
    return TRUE;
  }
  
  //����FALSE
  return FALSE;
}

/*******************************************************************
| �������ƣ�IsActive                                               |
| �������ܣ��ж��Ƿ����������                                     |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
| ˵������                                                         |
|******************************************************************/
BOOL CPSerialPort::IsActive(void)
{
  //���ؽ��յ�ǰ�Ľ������ݱ�־
  return m_bReceiving;
}

/*******************************************************************
| �������ƣ�WritePort                                              |
| �������ܣ���������                                               |
| ���������char *data,�ַ�ָ��                                    |
|           int length,�ַ�����                                    |
| �����������                                                     |
| ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
| ˵������                                                         |
|******************************************************************/
DWORD CPSerialPort::WritePort(char *data, signed int length)
{
  //�жϴ��ھ���Ƿ���Ч,��Ч�򷵻�0
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    return 1;
  }

  //�ֲ�����
  BOOL fWriteState = 0;       //д�ļ�����ֵ
  DWORD dwBytesWritten = 0L;  //д�����ݵĳ���

  //�򴮿�д������
  fWriteState = WriteFile(m_hComm, 
                          data, 
                          (DWORD)length * sizeof(char), 
                          &dwBytesWritten, 
                          NULL);

  //д������ʧ��
  if (fWriteState == 0)
  {
    return 2;
  }

  //�ɹ�
  return 0;
}


/*******************************************************************
| �������ƣ�ReadPort                                               |
| �������ܣ���ȡһ�����ȵ�����                                     |
| �����������                                                     |
| ���������char *data,�ַ�ָ��                                    |
|           int length,�ַ�����                                    |
| ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
| ˵������                                                         |
|******************************************************************/
DWORD CPSerialPort::ReadPort(char *data,signed int length)
{
  //�ֲ�����
  BOOL fReadState = FALSE;    //���ļ���־
  DWORD dwLength = 0L;        //�������ݵ�ʵ�ʳ���
  DWORD dwBytesRead = 0L;     //
  signed int iTimeOutCount = 0;
  char* pbuf = NULL;          //����ַ��Ŀռ�
  
  while (m_hComm != INVALID_HANDLE_VALUE)
  {
    //�ռ����
    try
    {
      pbuf = new char[length];
    }
    catch(...)
    {
      //����ռ�ʧ��
      return 1;
    }
    
    //�Ӵ��ڶ�ȡ����
    fReadState = ReadFile(m_hComm, data, length, &dwLength, NULL);
    
    //�ж��Ƿ��ȡ�ɹ�
    if (fReadState == FALSE)
    {
      //��ȡʧ��,����ѭ��
      break;
    }
    else
    {
      dwBytesRead += dwLength;
      data += dwLength;  
    }

    if (dwBytesRead == (DWORD)length)
    {
      //����ѭ��
      break;
    }

    if (dwLength != 0)
    {
      //��iTimeOutCount��Ϊ0
      iTimeOutCount = 0;
    }
    else
    {
      iTimeOutCount++;
      //�ȴ�
      Sleep(5);
    }

    if (iTimeOutCount == 5)
    {
      //����ѭ��
      break;
    }
  }
  
  //�ͷŶ�̬������ڴ�ռ�
  if (pbuf != NULL)
  {
    delete[] pbuf;

    pbuf = NULL;
  }
  
  return dwBytesRead;
}

/*******************************************************************
| �������ƣ�SaveComData                                            |
| �������ܣ����洮�����ݵ�������                                   |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
| ˵������                                                         |
|******************************************************************/
WORD CPSerialPort::SaveComData(const char* strdata, DWORD wLength)
{
  //ѭ������
  DWORD i = 0L;   
  
  //�������
  if (strdata == NULL)
  {
    return 1;
  }

  //�����ٽ���  
  EnterCriticalSection(&m_BufferSection);

  //�������ݵ�������
  for (i = 0; i < wLength; i++)
  {
    //�����ж��Ƿ��Ѿ�����4K������
    if (wDataCount < BUFFERSIZE)
    {
       strComData[wPos] = strdata[i];
       wPos++;
       wDataCount++;
    } 
    else
    {
       //���˻����������
       if (wPos >= BUFFERSIZE)
       {
         //λ�ü�������Ϊ0
         wPos = 0;
       }
       strComData[wPos] = strdata[i];
       wPos++;
    }
  }
  
  wDataCount = wDataCount;
  
  //�뿪�ٽ���
  LeaveCriticalSection(&m_BufferSection);  
  
  //����
  return 0;
}

/*******************************************************************
| �������ƣ�ReadBuffer                                             |
| �������ܣ���buffer��ȡ����                                       |
| ���������char *strData,�ַ�ָ��                                 |
|           WORD wLength,�ַ�����                                  |
| �����������                                                     |
| ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
| ˵������                                                         |
|******************************************************************/
WORD CPSerialPort:: ReadBuffer(char* strData, WORD &wLength)
{
  //�������
  if (strData == NULL)
  {
    return 1;
  }

  //������黺�������Ƿ�������
  if (wDataCount == 0)
  {
    strData = NULL;
    wLength = 0;
    return 2;
  }

  //�����ٽ���
  EnterCriticalSection(&m_BufferSection);

  //����δ����β��ͷ��ѭ���ƶ�
  if (wStart < wPos)
  {
    //���뻺��������鳤��,����ȫ��������
    if (wDataCount <= wLength)
    {
      //����ַ�����
      wLength = wDataCount;

      //����ȫ����Ч���� 
      memcpy(strData, &strComData[wStart], wDataCount * sizeof(char));

      //��������
      ZeroMemory(&strComData[wStart], wDataCount * sizeof(char));

      //�޸��������
      wStart = 0;
      wPos = 0;
      wDataCount = 0;
    }
    else
    {
      //��������
      memcpy(strData, &strComData[wStart], wLength * sizeof(char));

      //�����Ѿ�����������
      ZeroMemory(&strComData[wStart], wLength * sizeof(char));

      //�޸�������Ч������ʼλ��
      wStart = wStart + wLength;

      //������Ч���ݳ���
      wDataCount = wDataCount - wLength;

      //����ַ�����
      wLength = wLength;
    }
  }
  else//���鷢��β��ͷ���ƶ�
  {
    WORD lenght = 0;

    //����wStart������β���ַ�����
    lenght = BUFFERSIZE - wStart;
    
    //���뻺��������鳤��,����ȫ��������
    if (wDataCount <= wLength)
    { 
      //����ַ�����
      wLength = wDataCount;

      //����wStart������β������
      memcpy(strData, &strComData[wStart], lenght * sizeof(char));

      //���������еĸö�����
      ZeroMemory(&strComData[wStart], lenght * sizeof(char));

      //��������ͷ��wPos������
      memcpy(&strData[lenght], strComData, 
             (wDataCount - lenght) * sizeof(char));

      //���������еĸö�����
      ZeroMemory(strComData, (wDataCount - lenght) * sizeof(char));

      //�������黺�����Ĳ���
      wStart = 0;
      wPos = 0;
      wDataCount = 0;
    }
    else
    {
      //β�����ݴ������뻺��
      if (lenght >= wLength)
      {
        //����wStart������β������
        memcpy(strData, &strComData[wStart], wLength * sizeof(char));
        //���������еĸö�����
        ZeroMemory(&strComData[wStart], wLength * sizeof(char));
        //�޸���Ч���ݵ���ʼλ��
        wStart = wStart + wLength;
      }
      else
      {
        //����wStart������β������
        memcpy(strData, &strComData[wStart], lenght * sizeof(char));
        //���������еĸö�����
        ZeroMemory(&strComData[wStart], lenght * sizeof(char));
        //��������ͷ��wPos������
        memcpy(&strData[lenght], 
               strComData, 
               (wLength - lenght) * sizeof(char));
        //���������еĸö�����
        ZeroMemory(strComData, (wLength - lenght) * sizeof(char));
        //�޸�������Ч���ݵ���ʼλ��
        wStart = wLength - lenght;
      }
      //�޸Ļ����������Ч���ݳ���
      wDataCount = wDataCount - wLength;
      //����ַ�����
      wLength = wLength;
    }
  }

  //�뿪�ٽ���
  LeaveCriticalSection(&m_BufferSection);

  //����
  return 0;
}
