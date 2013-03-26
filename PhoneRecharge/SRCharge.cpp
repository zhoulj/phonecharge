/*******************************************************************
| Copyright (c) 2013,�������¿Ƽ����޹�˾                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| �ļ����ƣ�SRCharge.cpp                                           |
| �ļ���ʶ��                                                       |
| ժҪ��    ���ļ���������SRCharge������һЩ��Ա                   |
| ����˵������ɳ�ֵ���̿��ƣ�����CAtControl��CRecord��CQISR����� |
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
#include <iostream>
#include <string>
#include <Windows.h> 
#include "SRCharge.h"
#include "QISR.h"


using namespace std;

// ���ǵ���������һ��ʾ����
static CIFSR *g_pclSr = NULL;
static CQISR *g_pQISR = NULL;

CSRCharge::CSRCharge(void)
{
  g_pQISR = new CQISR();
}

CSRCharge::~CSRCharge(void)
{
  delete g_pQISR;
}
int CSRCharge::SRChargeInit()
{  
  //����¼��ʵ��������¼������
  m_pPlayer = CSoundBase::GetInstance();
  waveFormat.bits = BITS_16;
  waveFormat.channel = CHANNEL_SINGLE;
  waveFormat.samples = SAMPLES_16000;
	return 0;
}
/*CIniWriter(char* szFileName)
{
  memset(m_szFileName, 0x00, 255);
  memcpy(m_szFileName, szFileName, strlen(szFileName));
  char szValue[255];
  sprintf(szValue, "%d", iValue);
  WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}*/
int CSRCharge::Recharge(char* strPhoneNum,char* strCardPassword)
{  
  string strTemp = "";
  const char* result = NULL;
  char cResult[500],cTemp[3];
  int iResult = 0;

  //¼��wave�ļ�
 // if(m_pPlayer->Record(TEXT("Recharge.wav"),&waveFormat) == FALSE)
  //{
   // MessageBox(NULL,TEXT("FAILED"),TEXT(""),MB_OK);
 // }

  //����ʶ��
  iResult = g_pQISR->SRInit();
  if (iResult != 0)
    return iResult;
  iResult = g_pQISR->SetExID("30764386a1c7321e34b1b079692d8a69");
  if (iResult != 0)
    return iResult;
  iResult = g_pQISR->FileSpeechRecognition("����ɽ����.wav",cResult);  
  if (iResult != 0)
  {
    //itoa(iResult,cTemp,10);    
    //MessageBox(0,cTemp,"TEST",MB_OK);
    return iResult;
  }
  MessageBox(0,cResult,"TEST",MB_OK);
  
  return 0;
}
