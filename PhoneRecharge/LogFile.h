#pragma once
/*
Log File Library(WIN98/NT/2000)

Compile by�� BC++ 5; C++ BUILDER; VC++; VC.NET;

copyright(c) usergy 2005-8-9

*/
/*
//��������ù�ҵ�ֳ�24X7ֵ�صĳ����¼������Ϣ�� ������;
//һ����һ��ȫ����־���� ���ٽ��ų���Զ��̰߳�ȫʹ�á�
//��������
class LogFile;//�û�������־�ļ���
class LogFileEx;//����־�ļ����Զ����ɹ��� , �ɷ�������Ƶ�������ļ���, ��ָ����־��ŵ�Ŀ¼

LogFile gLog("My.Log");
gLog.Log("test", 4);//��¼��־
gLog.Log("ϵͳ����");

LogFileEx gLog(".", LogFileEx :: YEAR);//һ������һ����־�ļ�
LogFileEx gLog(".\\Log", LogFileEx :: MONTH);//һ������һ����־�ļ�
LogFileEx gLog(".\\Log", LogFileEx :: DAY);//һ������һ����־�ļ�
//ע����־����Ŀ¼����ʧ�ܻ��Զ��˳��� ��ע��Ŀ¼�ĺϷ��ԣ� �ļ�����Ƶ�ʿ��������
//24Сʱ���еĳ������ÿ������һ����־�ļ��� �������ݹ���
*/

#ifndef _LOGFILE_H
#define _LOGFILE_H

#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>



class CLogFile
{
protected:
	CRITICAL_SECTION _csLock;
	char * _szFileName;
	HANDLE _hFile;
	//���ļ��� ָ�뵽�ļ�β
	bool OpenFile();
	DWORD Write(LPCVOID lpBuffer, DWORD dwLength);
	virtual void WriteLog( LPCVOID lpBuffer, DWORD dwLength);
	void Lock()
	{
		::EnterCriticalSection(&_csLock);
	}
	void Unlock()
	{
		::LeaveCriticalSection(&_csLock);
	}
public:

	//�趨��־�ļ���
	CLogFile(const char *szFileName = "Log.log");
	virtual ~CLogFile();

	const char * GetFileName();
	void SetFileName(const char *szName);

	bool IsOpen();
	void Close();
	//׷����־����
	void Log(LPCVOID lpBuffer, DWORD dwLength);
	void Log(const char *szText);
private://���κ���
	CLogFile(const CLogFile&);
	CLogFile&operator = (const CLogFile&);
};


class CLogFileEx : public CLogFile
{
protected:
	char *_szPath;
	char _szLastDate[9];
	int _iType;

	void SetPath(const char * szPath)
	{
		assert(szPath);
		WIN32_FIND_DATA wfd;
		char temp[MAX_PATH + 5] = {0};
		if(FindFirstFile(szPath, &wfd) == INVALID_HANDLE_VALUE && CreateDirectory(szPath, NULL) == 0)
		{
			strcat(strcpy(temp, szPath), "Create Fail. Exit Now! Error ID :");
			ltoa(GetLastError(), temp + strlen(temp), 10);
			MessageBox(NULL, temp, "Class LogFileEx", MB_OK);
			return;
		}
		else
		{
			GetFullPathName(szPath, MAX_PATH, temp, NULL);
			_szPath = new char[strlen(temp) + 1];
			assert(_szPath);  
			strcpy(_szPath, temp);
		}
	}
public:
	enum LOG_TYPE{YEAR = 0, MONTH = 1, DAY = 2};
	~CLogFileEx();
	const char * GetPath();
	void Log(LPCVOID lpBuffer, DWORD dwLength);
	void Log(const char *szText);

  //���徲̬  
  static CLogFileEx * GetInstance()
  {
    if (m_pInstance == NULL) 
      m_pInstance = new CLogFileEx();
    return m_pInstance; 
  }
private://���κ���
	CLogFileEx(const char *szPath = ".", LOG_TYPE iType = MONTH);
	CLogFileEx(const CLogFileEx&);
	CLogFileEx&operator = (const CLogFileEx&);


  //���徲̬��ʵ������
  static  CLogFileEx *m_pInstance;
  class CGarbo // ����Ψһ��������������������ɾ��CSingleton��ʵ�� 
  {
  public:
    ~CGarbo()
    { 
      if (CLogFileEx::m_pInstance)
        delete CLogFileEx::m_pInstance;
    }
  };
  //�ͷ��ڴ�
  static CGarbo Garbo; // ����һ����̬��Ա���ڳ������ʱ��ϵͳ�������������������ע���������������������Ҫ����Ӧcpp�ļ��жԾ�̬��Ա���ж��塣

};
#endif
