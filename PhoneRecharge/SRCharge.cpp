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
#include "SRCharge.h"
#include "QISR.h"
#include "stdio.h"

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
	return 0;
}
int CSRCharge::Recharge(char* strPhoneNum,char* strCardPassword)
{  
  const char* result = NULL;
  char cresult[500];
  //string strfilename = "";
  g_pQISR->SRInit();
  g_pQISR->SetExID("30764386a1c7321e34b1b079692d8a69");
  g_pQISR->FileSpeechRecognition("阿里山龙胆.wav",cresult);
  if (cresult != NULL)
  {
   // printf("输出充值卡密码：s%,充值电话号：s%.",cresult);
    MessageBox(0,cresult,"TEST",MB_OK);
  }
  return 0;
}
