// PhoneRecharge.cpp : ���� DLL Ӧ�ó���ĵ���������

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
static CSRCharge *g_pclSRcharge = NULL;


//PHONERECHARGE_API int fnPhoneRecharge(void)
//{
	//return 42;
//}
/*******************************************************************
| �������ƣ�dll�ӿں���                                            |
| �������ܣ�dll�����ṩ�Ľӿں��������ղ������г�ֵ                |
| ���������strPhoneNum ��ֵ�绰�ţ�strCardPassword��ֵ������      |
| �����������                                                     |
| ����ֵ��  0�ɹ���ֵ�ɹ���������ֵ��ֵʧ��                                    |
| ˵������                                                         |
|******************************************************************/
int phoneRecharge(char* strPhoneNum,char* strCardPassword)  
{  	
  int nResult=0;
  if (g_pclSRcharge == NULL)
    g_pclSRcharge = new CSRCharge();
  g_pclSRcharge->SRChargeInit();
  nResult = g_pclSRcharge->Recharge(strCardPassword,strPhoneNum);
	return nResult;  
}  
// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� PhoneRecharge.h
CPhoneRecharge::CPhoneRecharge()
{
  if (g_pclSRcharge != NULL)
    delete g_pclSRcharge;
	return;
}
