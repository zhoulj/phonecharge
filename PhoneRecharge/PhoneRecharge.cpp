// PhoneRecharge.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "PhoneRecharge.h"
#include "SRCharge.h"
#include "AtControl.h"
#include "QISR.h"
#include "MicrosoftSR.h"


// ���ǵ���������һ��ʾ��
PHONERECHARGE_API int nPhoneRecharge=0;

// ���ǵ���������һ��ʾ����
static CIFSR *g_pclSr = NULL;
static CAtControl *g_pclAtCtrl = NULL;

PHONERECHARGE_API int fnPhoneRecharge(void)
{
  //if(g_pclSr == NULL)
    //g_pclSr = new QISR();
	return 42;
}

_declspec(dllexport) int phoneRecharge(char* CardPassword,char* PhoneNum)  
{  	
	return 0;  
}  
// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� PhoneRecharge.h
CPhoneRecharge::CPhoneRecharge()
{
	return;
}
