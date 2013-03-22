/*******************************************************************
| Copyright (c) 2013,长春威致科技有限公司                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| 文件名称：SRCharge.cpp                                             |
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
#include "SRCharge.h"
#include "stdio.h"

CSRCharge::CSRCharge(void)
{
}

CSRCharge::~CSRCharge(void)
{
}
int CSRCharge::SRChargeInit()
{
	return 0;
}
int CSRCharge::Recharge(char* strPhoneNum,char* strCardPassword)
{
  printf("输出充值卡密码：s%,充值电话号：s%.");
  return 0;
}
