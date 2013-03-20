/*******************************************************************
| Copyright (c) 2012,长春市威致科技有限公司                      |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| 文件名称：PSerialPort.cpp                                        |
| 文件标识：                                                       |
| 摘要：    本文件定义了类PSerialPort及它的一些成员                |
| 其它说明：无                                                     |
| 当前版本：0.9.2                                                  |
| 作    者：                                                       |
| 完成日期：                                                       |
|----------------------------------------------------------------- |
| 修改记录1：                                                      |
| 修改日期：                                                       |
| 版 本 号：                                                       |
| 修 改 人：                                                       |
| 修改原因：                                                       |
| 修改内容：                                                       |
| 修改代码位置：                                                   |
|------------------------------------------------------------------|
| 修改记录2：…                                                    |
|******************************************************************/

#include "stdafx.h"
#include "PSerialPort.h"
//#pragma message( "Pentium processor build" )

#pragma warning(disable : 4244)


/*******************************************************************
| 函数名称：构造函数                                               |
| 函数功能：初始化CPSerialPort对象                                 |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  无                                                     |
| 说明：无                                                         |
|******************************************************************/
CPSerialPort::CPSerialPort(void):m_hComm(INVALID_HANDLE_VALUE),//串口句柄
                                 m_hReadThread(NULL),//读串口线程handle 
                                 m_bReceiving(FALSE),//
                                 m_nBufferSize(256), //缓冲大小
                                 m_dwUserData(NULL), //参数
                                 wDataCount(0),
                                 wPos(0),
                                 wStart(0)
{
  if (strComData[0] != '\0')
  {
    //清零
    ZeroMemory(strComData, 4096 * sizeof(char));
  }

  //初始化临界区
  InitializeCriticalSection(&m_BufferSection);
}

/*******************************************************************
| 函数名称：析构函数                                               |
| 函数功能：释放CPSerialPort对象的资源                             |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  无                                                     |
| 说明：无                                                         |
|******************************************************************/
CPSerialPort::~CPSerialPort(void)
{
  try
  {
    //关闭串口设备
    ClosePort();
  }
  catch(...)
  {
    return; //异常
  }
  
  //串口句柄
  m_hComm = INVALID_HANDLE_VALUE;
  
  //读串口线程handle
  m_hReadThread = NULL;
  
  //是否持续接收标志,初始化为无效
  m_bReceiving = FALSE;
  
  try
  {
    //释放临界区资源
    DeleteCriticalSection(&m_BufferSection);
  }
  catch(...)
  {
    return; //异常
  }
}

/*******************************************************************
| 函数名称：ReadPortThread                                         |
| 函数功能：接收数据线程函数                                       |
| 输入参数：LPVOID lpParameter,函数指针                            |
| 输出参数：无                                                     |
| 返回值：  成功返回0，失败返回1                                   |
| 说明：无                                                         |
|******************************************************************/
DWORD WINAPI CPSerialPort::ReadPortThread(LPVOID lpParameter)
{
  //局部变量
  CPSerialPort* pSerial = (CPSerialPort*)lpParameter;//串口通讯对象
  BOOL fReadState = FALSE;                             //打开串口标志
  BYTE* pbuf = NULL;                                    //字符串指针
  DWORD dwLength = 0L;                                  //实际读取到数据的长度
  
  //分配空间
  try
  {
    pbuf = new BYTE[pSerial->m_nBufferSize];
  }
  catch(...)
  {
    return 1;  //分配空间失败 
  }


  //判断,串口读取操作
  while ((pSerial->m_hComm != INVALID_HANDLE_VALUE)
         && (pSerial->m_bReceiving))
  {
    //ZeroMemory(pbuf, pSerial->m_nBufferSize);
    //读取串口数据
    fReadState = ReadFile(pSerial->m_hComm,
      pbuf,
      pSerial->m_nBufferSize,
      &dwLength,
      NULL);

    //从串口读取数据失败
    if (fReadState == FALSE)
    {
      Sleep(1);
    }
    else
    {
      //成功读取到串口的数据
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

      //回送数据
      if (pSerial->m_lpDataArriveProc != NULL)
      {
        //调用m_lpDataArriveProc(),回送数据
        pSerial->m_lpDataArriveProc((char*)pbuf, dwLength, pSerial->m_dwUserData);
      }

    }
  }

  //释放动态申请的内存空间
  if (pbuf != NULL)
  {
    delete [] pbuf;

    pbuf = NULL;
  }

#ifdef _DEBUG
  TRACE1(_T("PSerialPort %s\n"), _T("Success"));
#endif

  //返回
  return 0;
}

/*******************************************************************
| 函数名称：OpenPort                                               |
| 函数功能：打开串口                                               |
| 输入参数：LPCTSTR Port,端口号                                    |
|           int Baudrate,波特率                                    |
|           int DataBits,数据位                                    |
|           int StopBits,停止位                                    |
|           int Parity,校验位                                      |
|           LPDataArriveProc proc,函数指针                         |
|           DWORD userdata                                         |
| 输出参数：无                                                     |
| 返回值：  WORD型,成功返回0，失败返回1                            |
| 说明：无                                                         |
|******************************************************************/
BOOL CPSerialPort::OpenPort(LPCTSTR Port,
                            signed int BaudRate,
                            signed int DataBits,
                            signed int StopBits,
                            signed int Parity,
                            LPDataArriveProc proc,
                            void* userdata)
{
  //函数地址
  m_lpDataArriveProc = proc;
  //使用的数据
  m_dwUserData = userdata;
  
  //判断,打开串口
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    m_hComm = CreateFile(Port, GENERIC_READ | GENERIC_WRITE,
      0, 0, OPEN_EXISTING, 0, 0);

    //打开串口失败
    if (m_hComm == INVALID_HANDLE_VALUE )
    {
      return FALSE;
    }

    //获取串口的当前状态 
    GetCommState(m_hComm, &dcb);
    //串口设备各状态值 
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

    //设置串口通讯设备状态参数
    SetCommMask(m_hComm, EV_RXCHAR);
    SetupComm(m_hComm, 16384, 16384);  

    //检验串口设置是否成功
    if (SetCommState(m_hComm, &dcb) == FALSE)
    {
      //无法按当前参数配置端口，请检查参数！"));
      PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
      //关闭串口
      ClosePort();
      //返回失败 
      return FALSE;
    }

    //设置串口通讯超时参数
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

    //判断超时参数是否设置成功
    if (SetCommTimeouts(m_hComm, &CommTimeOuts) == FALSE)
    {
      //无法设置超时参数;
      PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);

      //关闭串口
      ClosePort();

      //返回失败 
      return FALSE;
    }

    //清除打开串口过程中所占用的资源
    PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);

    //返回失败
    return TRUE;
  }
  
  //失败
  return FALSE;
}

/*******************************************************************
| 函数名称：关闭串口                                               |
| 函数功能：关闭已经打开的串口设备                                 |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  成功返回TRUE，失败返回FALSE                            |
| 说明：无                                                         |
|******************************************************************/
BOOL CPSerialPort::ClosePort(void)
{
  //关闭接收
  Deactivate();
  
  //判断串口句柄是否有效
  if (m_hComm != INVALID_HANDLE_VALUE)
  {
    //设置串口状态
    SetCommMask(m_hComm, 0);
    //释放串口设备所占的资源
    PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
    //关闭串口设备 
    CloseHandle(m_hComm);
    //置串口设备句柄无效
    m_hComm = INVALID_HANDLE_VALUE;
    //返回成功
    return TRUE;
  }
  
  //成功
  return TRUE;
}

/*******************************************************************
| 函数名称：GetPortThreadPriority                                  |
| 函数功能：返回工作线程的优先级                                   |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  成功返回TRUE，失败返回FALSE                            |
| 说明：无                                                         |
|******************************************************************/
BOOL CPSerialPort::GetPortThreadPriority(int &nPriority)
{
  //判断串口句柄是否有效
  if ((m_hComm == INVALID_HANDLE_VALUE) || (m_hReadThread == NULL))
  {
    //串口设备无效,返回1
    return FALSE;
  }

  //持续接收标志无效
  if (m_bReceiving == FALSE)
  {
    //返回失败
    return FALSE;
  }

#ifdef _WINCE
  nPriority = CeGetThreadPriority(m_hReadThread);
#else
  nPriority = GetThreadPriority(m_hReadThread);
#endif

  //返回成功
  return TRUE;
}

/*******************************************************************
| 函数名称：SetPortThreadPriority                                  |
| 函数功能：设置工作线程的优先级                                   |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  成功返回TRUE，失败返回FALSE                            |
| 说明：无                                                         |
|******************************************************************/
BOOL CPSerialPort::SetPortThreadPriority(signed int nPriority)
{
  //返回值
  BOOL bRet = FALSE;

  //判断串口句柄是否有效
  if ((m_hComm == INVALID_HANDLE_VALUE) || (m_hReadThread == NULL))
  {
    //串口设备无效,返回1
    return FALSE;
  }

  //持续接收标志无效
  if (m_bReceiving == FALSE)
  {
    //返回失败
    return FALSE;
  }

  //判断优先级是否大于255
  if (nPriority > 255)
  {
    //将优先级置为255
    nPriority = 255;
  }

  //判断优先级是否小于0
  if (nPriority < 0)
  {
    //将优先级置为0
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
| 函数名称：Activate                                               |
| 函数功能：激活与关闭持续接收                                     |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  成功返回TRUE，失败返回FALSE                            |
| 说明：无                                                         |
|******************************************************************/
BOOL CPSerialPort::Activate(void)
{
  //判断串口句柄是否有效
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    //串口设备无效,返回1
    return 1;
  }

  //接收标志有效 ,接收数据
  if (m_bReceiving == FALSE)
  {
    //释放串口设备所占的资源
    PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);

    //置接收标志有效
    m_bReceiving = TRUE;

    //创建读取数据的线程
    m_hReadThread = CreateThread(NULL, 0, ReadPortThread, this, 0, NULL);
  }

  //判断线程创建是否成功
  if (m_hReadThread != NULL)
  {
    //设置线程的优先级
    //SetThreadPriority(m_hReadThread, THREAD_PRIORITY_HIGHEST);

    //线程创建成功
    return 0;
  }
  else//线程创建失败
  {
    //置接收标志无效
    m_bReceiving = FALSE;
    //返回FALSE
    return 1;
  }
}

/*******************************************************************
| 函数名称：Deactivate                                             |
| 函数功能：停止接收数据的线程                                     |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  成功返回TRUE，失败返回FALSE                            |
| 说明：无                                                         |
|******************************************************************/
BOOL CPSerialPort::Deactivate(void)
{
  //判断串口句柄是否有效,无效返回FALSE
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  //停止接收线程
  if (m_bReceiving == TRUE)
  {
    //置接收标志为无效
    m_bReceiving = FALSE;
    //等待线程
    WaitForSingleObject(m_hReadThread, 500);
    //关闭读取串口数据的线程
    CloseHandle(m_hReadThread);
    //置串口读取线程句柄为空
    m_hReadThread = NULL;

    //返回TRUE
    return TRUE;
  }
  
  //返回FALSE
  return FALSE;
}

/*******************************************************************
| 函数名称：IsActive                                               |
| 函数功能：判断是否接收数据中                                     |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  成功返回0，失败返回1                                   |
| 说明：无                                                         |
|******************************************************************/
BOOL CPSerialPort::IsActive(void)
{
  //返回接收当前的接收数据标志
  return m_bReceiving;
}

/*******************************************************************
| 函数名称：WritePort                                              |
| 函数功能：发送数据                                               |
| 输入参数：char *data,字符指针                                    |
|           int length,字符长度                                    |
| 输出参数：无                                                     |
| 返回值：  成功返回0，失败返回1                                   |
| 说明：无                                                         |
|******************************************************************/
DWORD CPSerialPort::WritePort(char *data, signed int length)
{
  //判断串口句柄是否有效,无效则返回0
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    return 1;
  }

  //局部变量
  BOOL fWriteState = 0;       //写文件返回值
  DWORD dwBytesWritten = 0L;  //写入数据的长度

  //向串口写入数据
  fWriteState = WriteFile(m_hComm, 
                          data, 
                          (DWORD)length * sizeof(char), 
                          &dwBytesWritten, 
                          NULL);

  //写入数据失败
  if (fWriteState == 0)
  {
    return 2;
  }

  //成功
  return 0;
}


/*******************************************************************
| 函数名称：ReadPort                                               |
| 函数功能：读取一定长度的数据                                     |
| 输入参数：无                                                     |
| 输出参数：char *data,字符指针                                    |
|           int length,字符长度                                    |
| 返回值：  成功返回0，失败返回1                                   |
| 说明：无                                                         |
|******************************************************************/
DWORD CPSerialPort::ReadPort(char *data,signed int length)
{
  //局部变量
  BOOL fReadState = FALSE;    //读文件标志
  DWORD dwLength = 0L;        //读到数据的实际长度
  DWORD dwBytesRead = 0L;     //
  signed int iTimeOutCount = 0;
  char* pbuf = NULL;          //存放字符的空间
  
  while (m_hComm != INVALID_HANDLE_VALUE)
  {
    //空间分配
    try
    {
      pbuf = new char[length];
    }
    catch(...)
    {
      //申请空间失败
      return 1;
    }
    
    //从串口读取数据
    fReadState = ReadFile(m_hComm, data, length, &dwLength, NULL);
    
    //判断是否读取成功
    if (fReadState == FALSE)
    {
      //读取失败,跳出循环
      break;
    }
    else
    {
      dwBytesRead += dwLength;
      data += dwLength;  
    }

    if (dwBytesRead == (DWORD)length)
    {
      //跳出循环
      break;
    }

    if (dwLength != 0)
    {
      //将iTimeOutCount置为0
      iTimeOutCount = 0;
    }
    else
    {
      iTimeOutCount++;
      //等待
      Sleep(5);
    }

    if (iTimeOutCount == 5)
    {
      //跳出循环
      break;
    }
  }
  
  //释放动态申请的内存空间
  if (pbuf != NULL)
  {
    delete[] pbuf;

    pbuf = NULL;
  }
  
  return dwBytesRead;
}

/*******************************************************************
| 函数名称：SaveComData                                            |
| 函数功能：保存串口数据到队列中                                   |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  成功返回0，失败返回1                                   |
| 说明：无                                                         |
|******************************************************************/
WORD CPSerialPort::SaveComData(const char* strdata, DWORD wLength)
{
  //循环变量
  DWORD i = 0L;   
  
  //参数检测
  if (strdata == NULL)
  {
    return 1;
  }

  //进入临界区  
  EnterCriticalSection(&m_BufferSection);

  //保存数据到缓冲区
  for (i = 0; i < wLength; i++)
  {
    //首先判断是否已经存满4K的数据
    if (wDataCount < BUFFERSIZE)
    {
       strComData[wPos] = strdata[i];
       wPos++;
       wDataCount++;
    } 
    else
    {
       //到了缓冲区的最后
       if (wPos >= BUFFERSIZE)
       {
         //位置计数器置为0
         wPos = 0;
       }
       strComData[wPos] = strdata[i];
       wPos++;
    }
  }
  
  wDataCount = wDataCount;
  
  //离开临界区
  LeaveCriticalSection(&m_BufferSection);  
  
  //返回
  return 0;
}

/*******************************************************************
| 函数名称：ReadBuffer                                             |
| 函数功能：从buffer读取数据                                       |
| 输入参数：char *strData,字符指针                                 |
|           WORD wLength,字符长度                                  |
| 输出参数：无                                                     |
| 返回值：  成功返回0，失败返回1                                   |
| 说明：无                                                         |
|******************************************************************/
WORD CPSerialPort::ReadBuffer(char* strData, WORD &wLength)
{
  //参数检测
  if (strData == NULL)
  {
    return 1;
  }

  //检测数组缓冲区中是否有数据
  if (wDataCount == 0)
  {
    strData = NULL;
    wLength = 0;
    return 2;
  }

  //进入临界区
  EnterCriticalSection(&m_BufferSection);

  //数组未发生尾到头的循环移动
  if (wStart < wPos)
  {
    //输入缓冲大于数组长度,拷贝全部的数据
    if (wDataCount <= wLength)
    {
      //输出字符长度
      wLength = wDataCount;

      //拷贝全部有效数据 
      memcpy(strData, &strComData[wStart], wDataCount * sizeof(char));

      //清零数组
      ZeroMemory(&strComData[wStart], wDataCount * sizeof(char));

      //修改数组参数
      wStart = 0;
      wPos = 0;
      wDataCount = 0;
    }
    else
    {
      //拷贝数据
      memcpy(strData, &strComData[wStart], wLength * sizeof(char));

      //清零已经拷贝的数据
      ZeroMemory(&strComData[wStart], wLength * sizeof(char));

      //修改数组有效数据起始位置
      wStart = wStart + wLength;

      //数组有效数据长度
      wDataCount = wDataCount - wLength;

      //输出字符长度
      wLength = wLength;
    }
  }
  else//数组发生尾到头的移动
  {
    WORD lenght = 0;

    //计算wStart到数组尾的字符个数
    lenght = BUFFERSIZE - wStart;
    
    //输入缓冲大于数组长度,拷贝全部的数据
    if (wDataCount <= wLength)
    { 
      //输出字符长度
      wLength = wDataCount;

      //拷贝wStart到数组尾的数据
      memcpy(strData, &strComData[wStart], lenght * sizeof(char));

      //清零数组中的该段数据
      ZeroMemory(&strComData[wStart], lenght * sizeof(char));

      //拷贝数组头到wPos的数据
      memcpy(&strData[lenght], strComData, 
             (wDataCount - lenght) * sizeof(char));

      //清零数组中的该段数据
      ZeroMemory(strComData, (wDataCount - lenght) * sizeof(char));

      //设置数组缓冲区的参数
      wStart = 0;
      wPos = 0;
      wDataCount = 0;
    }
    else
    {
      //尾段数据大于输入缓冲
      if (lenght >= wLength)
      {
        //拷贝wStart到数组尾的数据
        memcpy(strData, &strComData[wStart], wLength * sizeof(char));
        //清零数组中的该段数据
        ZeroMemory(&strComData[wStart], wLength * sizeof(char));
        //修改有效数据的起始位置
        wStart = wStart + wLength;
      }
      else
      {
        //拷贝wStart到数组尾的数据
        memcpy(strData, &strComData[wStart], lenght * sizeof(char));
        //清零数组中的该段数据
        ZeroMemory(&strComData[wStart], lenght * sizeof(char));
        //拷贝数组头到wPos的数据
        memcpy(&strData[lenght], 
               strComData, 
               (wLength - lenght) * sizeof(char));
        //清零数组中的该段数据
        ZeroMemory(strComData, (wLength - lenght) * sizeof(char));
        //修改数组有效数据的起始位置
        wStart = wLength - lenght;
      }
      //修改缓冲数组的有效数据长度
      wDataCount = wDataCount - wLength;
      //输出字符长度
      wLength = wLength;
    }
  }

  //离开临界区
  LeaveCriticalSection(&m_BufferSection);

  //返回
  return 0;
}
