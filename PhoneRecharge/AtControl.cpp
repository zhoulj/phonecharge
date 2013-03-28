#include "StdAfx.h"
#include "AtControl.h"

CAtControl::CAtControl(void)
{
}

CAtControl::~CAtControl(void)
{
  if (INVALID_HANDLE_VALUE != m_hRecvEvent)
  {
    CloseHandle(m_hRecvEvent);
    m_hRecvEvent = INVALID_HANDLE_VALUE;
  }
}
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

int CAtControl::AtDial(char* ServiceNum)//拨充值服务号号
{
  char strSendData[100];
  memset(strSendData, 0, 100);
  strcpy(strSendData, "ATD");
  strcat(strSendData, ServiceNum);
  strcat(strSendData, ";\r\n");

  m_Comm.SendData(strSendData, strlen(strSendData) * sizeof(char) + 1);
  DWORD dwRet = WaitForSingleObject(m_hRecvEvent, 10000);
  if (dwRet == WAIT_OBJECT_0)
  {
    // 串口输入数据成功
    ResetEvent(m_hRecvEvent);
    return 0;
  }
  else if(dwRet == WAIT_TIMEOUT)
  {
    // time lost
    return -1;
  }
  else
  {
    if (g_nFlag == -1)
    {
      return -2;
    }
  }
  return 0;
}

int CAtControl::AtHangup()  // 挂断
{
  m_Comm.SendData("ATH", strlen("ATH") * sizeof(char) + 1);
  DWORD dwRet = WaitForSingleObject(m_hRecvEvent, 10000);
  if (dwRet == WAIT_OBJECT_0)
  {
    // 串口输入数据成功
    ResetEvent(m_hRecvEvent);
    return 0;
  }
  else if(dwRet == WAIT_TIMEOUT)
  {
    // time lost
    return -1;
  }
  else
  {
    if (g_nFlag == -1)
    {
      return -2;
    }
  }
  return 0;
}

int CAtControl::SendKeying(char* KeyingValue)//发送按键值
{
  return 0;
}
