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
//static CIFSR *g_pclSr = NULL;
static CQISR *g_pQISR = NULL;

CSRCharge::CSRCharge(void)
{
  g_pQISR = NULL;
  g_pclAtCtrl = NULL;
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
  if (g_pQISR == NULL)
  {
    g_pQISR = new CQISR();
  }  
  if (g_pclAtCtrl == NULL)
  {  
    g_pclAtCtrl = new CAtControl();
  }

  //创建录音实例并设置录音参数
  m_pPlayer = CSoundBase::GetInstance();
  waveFormat.bits = BITS_16;
  waveFormat.channel = CHANNEL_SINGLE;
  waveFormat.samples = SAMPLES_16000;
  //初始化At指令设备

	return 0;
}

//
// 
//
int CSRCharge::Recharge(char* strPhoneNum,char* strCardPassword)
{  
  char cResult[500],cTemp[3];
  int nResult = 0;


  //At初始化
  nResult = g_pclAtCtrl->AtInit();
  if (nResult != 0)
    return nResult;  

  //语音识别
  nResult = g_pQISR->SRInit();
  if (nResult != 0)
    return nResult;
  nResult = g_pQISR->SetExID("30764386a1c7321e34b1b079692d8a69");
  if (nResult != 0)
    return nResult;

  //13800138000充值流程处理
  nResult = g_pclAtCtrl->AtDial("13800138000");
  if (nResult != 0)
    return nResult; 
  
  //录制wave文件
  if (m_pPlayer->Record(TEXT("RechargeSpeech.wav"),&waveFormat) == FALSE)
  {
    m_pLogFileEx->Log("Rec---Record RechargeSpeech wave file fail.");    
  }
  else
  {  
    nResult = g_pQISR->FileSpeechRecognition("阿里山龙胆.wav",cResult);  
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
