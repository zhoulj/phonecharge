/*******************************************************************
| Copyright (c) 2012,长春市威致科技有限公司                        |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| 文件名称：PSerialPort.h                                          |
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
#pragma once

#pragma warning(disable : 4018)

//#include <iostream>
#include <queue>

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//定义函数别名
typedef void (*LPDataArriveProc)(char *data, DWORD length, void* userdata);

#define BUFFERSIZE 4096

//与串口通信的类
class CPSerialPort  
{ 
public:

  /*******************************************************************
  | 函数名称：构造函数                                               |
  | 函数功能：初始化CPSerialPort对象                                 |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  无                                                     |
  | 说明：无                                                         |
  |******************************************************************/
  CPSerialPort(void);

  /*******************************************************************
  | 函数名称：析构函数                                               |
  | 函数功能：释放CPSerialPort对象的资源                             |                 
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  无                                                     |
  | 说明：无                                                         |
  |******************************************************************/
  virtual ~CPSerialPort(void);

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
  BOOL OpenPort(LPCTSTR Port,
                  signed int Baudrate,
                  signed int DataBits,
                  signed int StopBits,
                  signed int Parity,
                  LPDataArriveProc proc=NULL,
                  void* userdata = NULL);
  
  /*******************************************************************
  | 函数名称：关闭串口                                               |
  | 函数功能：关闭已经打开的串口设备                                 |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  成功返回TRUE，失败返回FALSE                            |
  | 说明：无                                                         |
  |******************************************************************/  
  BOOL ClosePort(void); 
  
  /*******************************************************************
  | 函数名称：GetPortThreadPriority                                  |
  | 函数功能：返回工作线程的优先级                                   |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  成功返回TRUE，失败返回FALSE                            |
  | 说明：无                                                         |
  |******************************************************************/
  BOOL GetPortThreadPriority(int &nPriority);

  /*******************************************************************
  | 函数名称：SetPortThreadPriority                                  |
  | 函数功能：设置工作线程的优先级                                   |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  成功返回TRUE，失败返回FALSE                            |
  | 说明：无                                                         |
  |******************************************************************/
  BOOL SetPortThreadPriority(signed int nPriority);

  /*******************************************************************
  | 函数名称：Activate                                               |
  | 函数功能：激活与关闭持续接收                                     |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  成功返回TRUE，失败返回FALSE                            |
  | 说明：无                                                         |
  |******************************************************************/
  BOOL Activate(void);

  /*******************************************************************
  | 函数名称：Deactivate                                             |
  | 函数功能：停止接收数据的线程                                     |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  成功返回TRUE，失败返回FALSE                            |
  | 说明：无                                                         |
  |******************************************************************/
  BOOL Deactivate(void);

  /*******************************************************************
  | 函数名称：IsActive                                               |
  | 函数功能：判断是否接收数据中                                     |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  成功返回0，失败返回1                                   |
  | 说明：无                                                         |
  |******************************************************************/
  BOOL IsActive(void);
  
  /*******************************************************************
  | 函数名称：ReadPortThread                                         |
  | 函数功能：接收数据线程函数                                       |
  | 输入参数：LPVOID lpParameter,函数指针                            |
  | 输出参数：无                                                     |
  | 返回值：  成功返回0，失败返回1                                   |
  | 说明：无                                                         |
  |******************************************************************/
  static DWORD WINAPI ReadPortThread(LPVOID lpParameter);
  
  /*******************************************************************
  | 函数名称：ReadPort                                               |
  | 函数功能：读取一定长度的数据                                     |
  | 输入参数：无                                                     |
  | 输出参数：char *data,字符指针                                    |
  |           int length,字符长度                                    |
  | 返回值：  成功返回0，失败返回1                                   |
  | 说明：无                                                         |
  |******************************************************************/
  DWORD ReadPort(char *data,signed int length); 
  
  /*******************************************************************
  | 函数名称：WritePort                                              |
  | 函数功能：发送数据                                               |
  | 输入参数：char *data,字符指针                                    |
  |           int length,字符长度                                    |
  | 输出参数：无                                                     |
  | 返回值：  成功返回0，失败返回1                                   |
  | 说明：无                                                         |
  |******************************************************************/  
    DWORD WritePort(char *data,signed int length); 
  
  /*******************************************************************
  | 函数名称：SaveComData                                            |
  | 函数功能：保存串口数据到队列中                                   |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  成功返回0，失败返回1                                   |
  | 说明：无                                                         |
  |******************************************************************/
    WORD SaveComData(const char* strdata, DWORD wLength);

  /*******************************************************************
  | 函数名称：ReadBuffer                                             |
  | 函数功能：从buffer读取数据                                       |
  | 输入参数：char *strData,字符指针                                 |
  |           WORD wLength,字符长度                                  |
  | 输出参数：无                                                     |
  | 返回值：  成功返回0，失败返回1                                   |
  | 说明：无                                                         |
  |******************************************************************/
    WORD ReadBuffer(char* strData, WORD &wLength);

private:

  //串口设备handle
  HANDLE m_hComm; 

  //读串口线程handle
  HANDLE m_hReadThread; 

  //是否持续接收标志
  BOOL m_bReceiving;

  //缓冲大小
  signed int m_nBufferSize; 
  
  //定义函数
  LPDataArriveProc m_lpDataArriveProc;

  //参数
  void* m_dwUserData;

  //串口设置
  DCB dcb;

  //连接串口设置的TimeOut
  COMMTIMEOUTS CommTimeOuts;  

  char strComData[4096];  //Com口数据
  WORD wDataCount;  //当前字符个数
  WORD wPos;  //位置计数器,在0-4095之间循环
  WORD wStart;  //数据开始位置

  //字符数组操作的临界区
  CRITICAL_SECTION m_BufferSection;  
};

