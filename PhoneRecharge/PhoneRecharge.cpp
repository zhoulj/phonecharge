// PhoneRecharge.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "PhoneRecharge.h"
#include "SRCharge.h"
#include "AtControl.h"
#include "QISR.h"
#include "MicrosoftSR.h"
#include "LogFile.h"

// ���ǵ���������һ��ʾ�� 
//PHONERECHARGE_API int nPhoneRecharge=0;

// ���ǵ���������һ��ʾ����
static CIFSR *g_pclSr = NULL;
static CAtControl *g_pclAtCtrl = NULL;
static CSRCharge *g_pclSRcharge = NULL;


//PHONERECHARGE_API int fnPhoneRecharge(void)
//{
  //if(g_pclSr == NULL)
    //g_pclSr = new QISR();
	//return 42;
//}

int phoneRecharge(char* strPhoneNum,char* strCardPassword)  
{  	
  int result=0;
  g_pclSRcharge = new CSRCharge();
  g_pclSRcharge->SRChargeInit();
  result = g_pclSRcharge->Recharge(strCardPassword, strPhoneNum);
	return result;  
}  
// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� PhoneRecharge.h
CPhoneRecharge::CPhoneRecharge()
{
	return;
}
