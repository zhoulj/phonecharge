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
//static CIFSR *g_pclSr = NULL;
static CQISR *g_pQISR = NULL;
static CAtControl *g_pclAtCtrl = NULL;

CSRCharge::CSRCharge(void)
{
  g_pQISR = new CQISR();
}

CSRCharge::~CSRCharge(void)
{
  if(g_pQISR)
    delete g_pQISR;
  if(g_pclAtCtrl)
    delete g_pclAtCtrl;
}
int CSRCharge::SRChargeInit()
{  
  //����¼��ʵ��������¼������
  m_pPlayer = CSoundBase::GetInstance();
  waveFormat.bits = BITS_16;
  waveFormat.channel = CHANNEL_SINGLE;
  waveFormat.samples = SAMPLES_16000;
  //��ʼ��Atָ���豸
  g_pclAtCtrl = new CAtControl();
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
  char cResult[500],cTemp[3];
  int nResult = 0;


  //At��ʼ��
  nResult = g_pclAtCtrl->AtInit();
  if (nResult != 0)
    return nResult;  

  //����ʶ��
  nResult = g_pQISR->SRInit();
  if (nResult != 0)
    return nResult;
  nResult = g_pQISR->SetExID("30764386a1c7321e34b1b079692d8a69");
  if (nResult != 0)
    return nResult;

  //13800138000��ֵ���̴���
  nResult = g_pclAtCtrl->AtDial("13800138000");
  if (nResult != 0)
    return nResult; 
  
  //¼��wave�ļ�
  if (m_pPlayer->Record(TEXT("RechargeSpeech.wav"),&waveFormat) == FALSE)
  {
    m_pLogFileEx->Log("Rec---Record RechargeSpeech wave file fail.");    
  }
  else
  {  
    nResult = g_pQISR->FileSpeechRecognition("����ɽ����.wav",cResult);  
    if (nResult != 0)
    {
      //itoa(iResult,cTemp,10);    
      //MessageBox(0,cTemp,"TEST",MB_OK);
      return nResult;
    }
    MessageBox(0,cResult,"TEST",MB_OK);
  }
   
  return 0;
}
