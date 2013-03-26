/*******************************************************************
| Copyright (c) 2013,长春威致科技有限公司                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| 文件名称：SRCharge.cpp                                           |
| 文件标识：                                                       |
| 摘要：    本文件定义了类SRCharge及它的一些成员                   |
| 其它说明：完成充值流程控制，调用CAtControl、CRecord、CQISR类完成 |
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
#include <iostream>
#include <string>
#include <Windows.h> 
#include "SRCharge.h"
#include "QISR.h"


using namespace std;

// 这是导出函数的一个示例。
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
  //创建录音实例并设置录音参数
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

  //录制wave文件
 // if(m_pPlayer->Record(TEXT("Recharge.wav"),&waveFormat) == FALSE)
  //{
   // MessageBox(NULL,TEXT("FAILED"),TEXT(""),MB_OK);
 // }

  //语音识别
  iResult = g_pQISR->SRInit();
  if (iResult != 0)
    return iResult;
  iResult = g_pQISR->SetExID("30764386a1c7321e34b1b079692d8a69");
  if (iResult != 0)
    return iResult;
  iResult = g_pQISR->FileSpeechRecognition("阿里山龙胆.wav",cResult);  
  if (iResult != 0)
  {
    //itoa(iResult,cTemp,10);    
    //MessageBox(0,cTemp,"TEST",MB_OK);
    return iResult;
  }
  MessageBox(0,cResult,"TEST",MB_OK);
  
  return 0;
}
