// PhoneRecharge.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "PhoneRecharge.h"
#include "SRCharge.h"


// ���ǵ���������һ��ʾ��
PHONERECHARGE_API int nPhoneRecharge=0;

// ���ǵ���������һ��ʾ����
PHONERECHARGE_API int fnPhoneRecharge(void)
{
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
