// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PHONERECHARGE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PHONERECHARGE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma  once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PHONERECHARGE_EXPORTS
#define PHONERECHARGE_API __declspec(dllexport)
#else
#define PHONERECHARGE_API __declspec(dllimport)
#endif

// �����Ǵ� PhoneRecharge.dll ������
class PHONERECHARGE_API CPhoneRecharge {
public:
	CPhoneRecharge(void);
	// TODO: �ڴ�������ķ�����
};

extern PHONERECHARGE_API int nPhoneRecharge;

PHONERECHARGE_API int fnPhoneRecharge(void);


#ifdef __cplusplus
 }
#endif
