/*******************************************************************
| Copyright (c) 2013,长春威致科技有限公司                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| 文件名称：AtControl.cpp                                          |
| 文件标识：                                                       |
| 摘要：    本文件定义了类AtControl及它的一些成员                  |
| 其它说明：初始化Com口，完成拨号、挂断、发送按键等功能            |
| 当前版本：                                                       |
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
#include "StdAfx.h"
#include "AtControl.h"

/*******************************************************************
| 函数名称：构造函数                                               |
| 函数功能：初始化CAtControl对象                                   |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  无                                                     |
| 说明：无                                                         |
|******************************************************************/
CAtControl::CAtControl(void)
{
}

/*******************************************************************
| 函数名称：析构函数                                               |
| 函数功能：释放CAtControl对象的资源                               |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  无                                                     |
| 说明：无                                                         |
|******************************************************************/
CAtControl::~CAtControl(void)
{
  if (INVALID_HANDLE_VALUE != m_hRecvEvent)
  {
    CloseHandle(m_hRecvEvent);
    m_hRecvEvent = INVALID_HANDLE_VALUE;
  }
  m_Comm.StopComm();
}

/*******************************************************************
| 函数名称：Com口初始化                                            |
| 函数功能：初始化Com口，及其相关参数                              |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  0成功，其他失败                                        |
| 说明：无                                                         |
|******************************************************************/
int CAtControl::AtInit()
{
  m_hRecvEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (NULL == m_hRecvEvent)
  {
    return -1;
  }
  m_Comm = Ccom(m_hRecvEvent, NULL);
  if(!m_Comm.StartComm())
  {
    return -2;
  }
  return 0;
}

/*******************************************************************
| 函数名称：拨号                                                   |
| 函数功能：通过Com口At指令拨号                                    |
| 输入参数：要播的号码                                             |
| 输出参数：无                                                     |
| 返回值：  0成功，其他失败                                        |
| 说明：无                                                         |
|******************************************************************/
int CAtControl::AtDial(char* ServiceNum)//拨充值服务号号
{
  // 发送拨号指令
  char strSendData[100];
  memset(strSendData, 0, 100);
  strcpy(strSendData, "ATD");
  strcat(strSendData, ServiceNum);
  strcat(strSendData, ";\r\n");
  m_Comm.SendData(strSendData, strlen(strSendData) * sizeof(char) + 1);
  // 等待返回结果
  DWORD dwRet = WaitForSingleObject(m_hRecvEvent, 10000);
  if(dwRet == WAIT_TIMEOUT)
  {
    // time lost
    ResetEvent(m_hRecvEvent);
    return -1;
  }
  else
  {
    if (Ccom::m_nAtRet == 1)
    {
      ResetEvent(m_hRecvEvent);
      return 0;
    }
    else if (Ccom::m_nAtRet == -1)
    {
      ResetEvent(m_hRecvEvent);
      return -2;
    }
    else if (Ccom::m_nAtRet == -2)
    {
      ResetEvent(m_hRecvEvent);
      return -3;
    }
  }
}

/*******************************************************************
| 函数名称：挂断                                                   |
| 函数功能：通过Com口At指令挂断电话                                |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  0成功，其他失败                                        |
| 说明：无                                                         |
|******************************************************************/
int CAtControl::AtHangup()  // 挂断
{
  // 发送挂断指令
  m_Comm.SendData("ATH\r\n", strlen("ATH\r\n") * sizeof(char) + 1);
  // 等待返回结果
  DWORD dwRet = WaitForSingleObject(m_hRecvEvent, 10000);
  if(dwRet == WAIT_TIMEOUT)
  {
    // time lost
    ResetEvent(m_hRecvEvent);
    return -1;
  }
  else
  {
    if (Ccom::m_nAtRet == 1)
    {
      ResetEvent(m_hRecvEvent);
      return 0;
    }
    else if (Ccom::m_nAtRet == -1)
    {
      ResetEvent(m_hRecvEvent);
      return -2;
    }
    else if (Ccom::m_nAtRet == -2)
    {
      ResetEvent(m_hRecvEvent);
      return -3;
    }
  }
}

/*******************************************************************
| 函数名称：发送按键值                                             |
| 函数功能：通过At指令向Com口发送按键值                            |
| 输入参数：KeyingValue，发送按键值，可以是多个键值如电话号        |
| 输出参数：无                                                     |
| 返回值：  0成功，其他失败                                        |
| 说明：无                                                         |
|******************************************************************/
int CAtControl::SendKeying(char* KeyingValue)//发送按键值
{
  // 发送按键消息指令
  char strSendData[100];
  memset(strSendData, 0, 100);
  strcpy(strSendData, "AT+VTS=");
  strcat(strSendData, KeyingValue);
  strcat(strSendData, "\r\n");

  m_Comm.SendData(strSendData, strlen(strSendData) * sizeof(char) + 1);

  // 等待返回结果
  DWORD dwRet = WaitForSingleObject(m_hRecvEvent, 10000);
  if(dwRet == WAIT_TIMEOUT)
  {
    // time lost
    ResetEvent(m_hRecvEvent);
    return -1;
  }
  else
  {
    if (Ccom::m_nAtRet == 1)
    {
      ResetEvent(m_hRecvEvent);
      return 0;
    }
    else if (Ccom::m_nAtRet == -1)
    {
      ResetEvent(m_hRecvEvent);
      return -2;
    }
    else if (Ccom::m_nAtRet == -2)
    {
      ResetEvent(m_hRecvEvent);
      return -3;
    }
  }
}

/*******************************************************************
| 函数名称：话筒模式                                               |
| 函数功能：通过At指令向Com口发送命令改变听筒模式                  |
| 输入参数：无                                                     |
| 输出参数：无                                                     |
| 返回值：  0成功，其他失败                                        |
| 说明：无                                                         |
|******************************************************************/
int CAtControl::MicroPhone()
{
  // 发送按键消息指令
  char strSendData[100];
  memset(strSendData, 0, 100);
  strcpy(strSendData, "AT+SPEAKER=1");
  strcat(strSendData, KeyingValue);
  strcat(strSendData, "\r\n");

  m_Comm.SendData(strSendData, strlen(strSendData) * sizeof(char) + 1);

  // 等待返回结果
  DWORD dwRet = WaitForSingleObject(m_hRecvEvent, 10000);
  if(dwRet == WAIT_TIMEOUT)
  {
    // time lost
    ResetEvent(m_hRecvEvent);
    return -1;
  }
  else
  {
    if (Ccom::m_nAtRet == 1)
    {
      ResetEvent(m_hRecvEvent);
      return 0;
    }
    else if (Ccom::m_nAtRet == -1)
    {
      ResetEvent(m_hRecvEvent);
      return -2;
    }
    else if (Ccom::m_nAtRet == -2)
    {
      ResetEvent(m_hRecvEvent);
      return -3;
    }
  }
}
