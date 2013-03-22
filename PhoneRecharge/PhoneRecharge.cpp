// PhoneRecharge.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "PhoneRecharge.h"
#include "SRCharge.h"
#include "AtControl.h"
#include "QISR.h"
#include "MicrosoftSR.h"


// 这是导出变量的一个示例
PHONERECHARGE_API int nPhoneRecharge=0;

// 这是导出函数的一个示例。
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
// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 PhoneRecharge.h
CPhoneRecharge::CPhoneRecharge()
{
	return;
}
