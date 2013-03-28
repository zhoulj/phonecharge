// PhoneRecharge.cpp : 定义 DLL 应用程序的导出函数。

#include "stdafx.h"
#include "PhoneRecharge.h"
#include "SRCharge.h"
//#include "AtControl.h"
//#include "QISR.h"
#include "MicrosoftSR.h"
#include "LogFile.h"

// 这是导出变量的一个示例 
//PHONERECHARGE_API int nPhoneRecharge=0;

// 这是导出函数的一个示例。
static CSRCharge *g_pclSRcharge = NULL;


//PHONERECHARGE_API int fnPhoneRecharge(void)
//{
	//return 42;
//}
/*******************************************************************
| 函数名称：dll接口函数                                            |
| 函数功能：dll对外提供的接口函数，接收参数进行充值                |
| 输入参数：strPhoneNum 充值电话号，strCardPassword充值卡密码      |
| 输出参数：无                                                     |
| 返回值：  0成功充值成功，其他数值充值失败                                    |
| 说明：无                                                         |
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
// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 PhoneRecharge.h
CPhoneRecharge::CPhoneRecharge()
{
  if (g_pclSRcharge != NULL)
    delete g_pclSRcharge;
	return;
}
