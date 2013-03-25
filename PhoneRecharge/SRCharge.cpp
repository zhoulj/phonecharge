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
#include "SRCharge.h"
#include "QISR.h"
#include "stdio.h"

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
	return 0;
}
int CSRCharge::Recharge(char* strPhoneNum,char* strCardPassword)
{  
  const char* result = NULL;
  char cresult[500];
  //string strfilename = "";
  g_pQISR->SRInit();
  g_pQISR->SetExID("30764386a1c7321e34b1b079692d8a69");
  g_pQISR->FileSpeechRecognition("����ɽ����.wav",cresult);
  if (cresult != NULL)
  {
   // printf("�����ֵ�����룺s%,��ֵ�绰�ţ�s%.",cresult);
    MessageBox(0,cresult,"TEST",MB_OK);
  }
  return 0;
}
