#pragma once

class CAtControl
{
public:
	int AtInit();
	int AtDial(char* ServiceNum);//����ֵ����ź�
	int AtHangup();
	int SendKeying(char* KeyingValue);//���Ͱ���ֵ
	
	CAtControl(void);
	~CAtControl(void);
};
