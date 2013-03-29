/*******************************************************************
| Copyright (c) 2013,�������¿Ƽ����޹�˾                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| �ļ����ƣ�AtControl.cpp                                          |
| �ļ���ʶ��                                                       |
| ժҪ��    ���ļ���������AtControl������һЩ��Ա                  |
| ����˵������ʼ��Com�ڣ���ɲ��š��Ҷϡ����Ͱ����ȹ���            |
| ��ǰ�汾��                                                       |
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
#include "StdAfx.h"
#include "AtControl.h"

/*******************************************************************
| �������ƣ����캯��                                               |
| �������ܣ���ʼ��CAtControl����                                   |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  ��                                                     |
| ˵������                                                         |
|******************************************************************/
CAtControl::CAtControl(void)
{
}

/*******************************************************************
| �������ƣ���������                                               |
| �������ܣ��ͷ�CAtControl�������Դ                               |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  ��                                                     |
| ˵������                                                         |
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
| �������ƣ�Com�ڳ�ʼ��                                            |
| �������ܣ���ʼ��Com�ڣ�������ز���                              |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  0�ɹ�������ʧ��                                        |
| ˵������                                                         |
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
| �������ƣ�����                                                   |
| �������ܣ�ͨ��Com��Atָ���                                    |
| ���������Ҫ���ĺ���                                             |
| �����������                                                     |
| ����ֵ��  0�ɹ�������ʧ��                                        |
| ˵������                                                         |
|******************************************************************/
int CAtControl::AtDial(char* ServiceNum)//����ֵ����ź�
{
  // ���Ͳ���ָ��
  char strSendData[100];
  memset(strSendData, 0, 100);
  strcpy(strSendData, "ATD");
  strcat(strSendData, ServiceNum);
  strcat(strSendData, ";\r\n");
  m_Comm.SendData(strSendData, strlen(strSendData) * sizeof(char) + 1);
  // �ȴ����ؽ��
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
| �������ƣ��Ҷ�                                                   |
| �������ܣ�ͨ��Com��Atָ��Ҷϵ绰                                |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  0�ɹ�������ʧ��                                        |
| ˵������                                                         |
|******************************************************************/
int CAtControl::AtHangup()  // �Ҷ�
{
  // ���͹Ҷ�ָ��
  m_Comm.SendData("ATH\r\n", strlen("ATH\r\n") * sizeof(char) + 1);
  // �ȴ����ؽ��
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
| �������ƣ����Ͱ���ֵ                                             |
| �������ܣ�ͨ��Atָ����Com�ڷ��Ͱ���ֵ                            |
| ���������KeyingValue�����Ͱ���ֵ�������Ƕ����ֵ��绰��        |
| �����������                                                     |
| ����ֵ��  0�ɹ�������ʧ��                                        |
| ˵������                                                         |
|******************************************************************/
int CAtControl::SendKeying(char* KeyingValue)//���Ͱ���ֵ
{
  // ���Ͱ�����Ϣָ��
  char strSendData[100];
  memset(strSendData, 0, 100);
  strcpy(strSendData, "AT+VTS=");
  strcat(strSendData, KeyingValue);
  strcat(strSendData, "\r\n");

  m_Comm.SendData(strSendData, strlen(strSendData) * sizeof(char) + 1);

  // �ȴ����ؽ��
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
