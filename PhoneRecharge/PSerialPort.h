/*******************************************************************
| Copyright (c) 2012,�����з����Ƽ����޹�˾                      |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| �ļ����ƣ�PSerialPort.h                                          |
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
#pragma once

#pragma warning(disable : 4018)

//#include <iostream>
#include <queue>

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���庯������
typedef void (*LPDataArriveProc)(char *data, DWORD length, void* userdata);

#define BUFFERSIZE 4096

//�봮��ͨ�ŵ���
class CPSerialPort  
{ 
public:

  /*******************************************************************
  | �������ƣ����캯��                                               |
  | �������ܣ���ʼ��CPSerialPort����                                 |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  ��                                                     |
  | ˵������                                                         |
  |******************************************************************/
  CPSerialPort(void);

  /*******************************************************************
  | �������ƣ���������                                               |
  | �������ܣ��ͷ�CPSerialPort�������Դ                             |                 
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  ��                                                     |
  | ˵������                                                         |
  |******************************************************************/
  virtual ~CPSerialPort(void);

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
  BOOL OpenPort(LPCTSTR Port,
                  signed int Baudrate,
                  signed int DataBits,
                  signed int StopBits,
                  signed int Parity,
                  LPDataArriveProc proc=NULL,
                  void* userdata = NULL);
  
  /*******************************************************************
  | �������ƣ��رմ���                                               |
  | �������ܣ��ر��Ѿ��򿪵Ĵ����豸                                 |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
  | ˵������                                                         |
  |******************************************************************/  
  BOOL ClosePort(void); 
  
  /*******************************************************************
  | �������ƣ�GetPortThreadPriority                                  |
  | �������ܣ����ع����̵߳����ȼ�                                   |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
  | ˵������                                                         |
  |******************************************************************/
  BOOL GetPortThreadPriority(int &nPriority);

  /*******************************************************************
  | �������ƣ�SetPortThreadPriority                                  |
  | �������ܣ����ù����̵߳����ȼ�                                   |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
  | ˵������                                                         |
  |******************************************************************/
  BOOL SetPortThreadPriority(signed int nPriority);

  /*******************************************************************
  | �������ƣ�Activate                                               |
  | �������ܣ�������رճ�������                                     |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
  | ˵������                                                         |
  |******************************************************************/
  BOOL Activate(void);

  /*******************************************************************
  | �������ƣ�Deactivate                                             |
  | �������ܣ�ֹͣ�������ݵ��߳�                                     |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����TRUE��ʧ�ܷ���FALSE                            |
  | ˵������                                                         |
  |******************************************************************/
  BOOL Deactivate(void);

  /*******************************************************************
  | �������ƣ�IsActive                                               |
  | �������ܣ��ж��Ƿ����������                                     |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
  | ˵������                                                         |
  |******************************************************************/
  BOOL IsActive(void);
  
  /*******************************************************************
  | �������ƣ�ReadPortThread                                         |
  | �������ܣ����������̺߳���                                       |
  | ���������LPVOID lpParameter,����ָ��                            |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
  | ˵������                                                         |
  |******************************************************************/
  static DWORD WINAPI ReadPortThread(LPVOID lpParameter);
  
  /*******************************************************************
  | �������ƣ�ReadPort                                               |
  | �������ܣ���ȡһ�����ȵ�����                                     |
  | �����������                                                     |
  | ���������char *data,�ַ�ָ��                                    |
  |           int length,�ַ�����                                    |
  | ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
  | ˵������                                                         |
  |******************************************************************/
  DWORD ReadPort(char *data,signed int length); 
  
  /*******************************************************************
  | �������ƣ�WritePort                                              |
  | �������ܣ���������                                               |
  | ���������char *data,�ַ�ָ��                                    |
  |           int length,�ַ�����                                    |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
  | ˵������                                                         |
  |******************************************************************/  
    DWORD WritePort(char *data,signed int length); 
  
  /*******************************************************************
  | �������ƣ�SaveComData                                            |
  | �������ܣ����洮�����ݵ�������                                   |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
  | ˵������                                                         |
  |******************************************************************/
    WORD SaveComData(const char* strdata, DWORD wLength);

  /*******************************************************************
  | �������ƣ�ReadBuffer                                             |
  | �������ܣ���buffer��ȡ����                                       |
  | ���������char *strData,�ַ�ָ��                                 |
  |           WORD wLength,�ַ�����                                  |
  | �����������                                                     |
  | ����ֵ��  �ɹ�����0��ʧ�ܷ���1                                   |
  | ˵������                                                         |
  |******************************************************************/
    WORD ReadBuffer(char* strData, WORD &wLength);

private:

  //�����豸handle
  HANDLE m_hComm; 

  //�������߳�handle
  HANDLE m_hReadThread; 

  //�Ƿ�������ձ�־
  BOOL m_bReceiving;

  //�����С
  signed int m_nBufferSize; 
  
  //���庯��
  LPDataArriveProc m_lpDataArriveProc;

  //����
  void* m_dwUserData;

  //��������
  DCB dcb;

  //���Ӵ������õ�TimeOut
  COMMTIMEOUTS CommTimeOuts;  

  char strComData[4096];  //Com������
  WORD wDataCount;  //��ǰ�ַ�����
  WORD wPos;  //λ�ü�����,��0-4095֮��ѭ��
  WORD wStart;  //���ݿ�ʼλ��

  //�ַ�����������ٽ���
  CRITICAL_SECTION m_BufferSection;  
};
