#include "StdAfx.h"
#include "AtControl.h"

CAtControl::CAtControl(void)
{
}

CAtControl::~CAtControl(void)
{
}
int AtInit()
{
  return 0;
}

int AtDial(char* ServiceNum)//拨充值服务号号
{
  return 0;
}

int AtHangup()
{
  return 0;
}

int SendKeying(char* KeyingValue)//发送按键值
{
  return 0;
}
