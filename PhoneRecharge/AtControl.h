#pragma once

class CAtControl
{
public:
	int AtInit();
	int AtDial(char* ServiceNum);//拨充值服务号号
	int AtHangup();
	int SendKeying(char* KeyingValue);//发送按键值
	
	CAtControl(void);
	~CAtControl(void);
};
