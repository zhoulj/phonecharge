#pragma once

#include "com.h"    // ���봮��ͨѶ������

class CAtControl
{
public:
	int AtInit();
	int AtDial(char* ServiceNum);//����ֵ����ź�
	int AtHangup();
	int SendKeying(char* KeyingValue);//���Ͱ���ֵ
	
	CAtControl(void);
	~CAtControl(void);
private:
  Ccom m_Comm;
  HANDLE m_hRecvEvent;
};
