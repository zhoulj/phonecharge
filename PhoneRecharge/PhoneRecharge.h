// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PHONERECHARGE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PHONERECHARGE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma once


#ifdef PHONERECHARGE_EXPORTS
#define PHONERECHARGE_API __declspec(dllexport)
#else
#define PHONERECHARGE_API __declspec(dllimport)
#endif


//extern "C" PHONERECHARGE_API int nPhoneRecharge;
//extern "C" PHONERECHARGE_API int fnPhoneRecharge(void);
extern "C" _declspec(dllexport) int phoneRecharge(char* strPhoneNum,char* strCardPassword);  



