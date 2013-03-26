#pragma once
/*
Log File Library(WIN98/NT/2000)

Compile by： BC++ 5; C++ BUILDER; VC++; VC.NET;

copyright(c) usergy 2005-8-9

*/
/*
//这个代码用工业现场24X7值守的程序纪录各种信息， 简单易用;
//一般用一个全局日志对象， 有临界排斥可以多线程安全使用。
//有两个类
class LogFile;//用户定义日志文件名
class LogFileEx;//有日志文件名自动生成功能 , 可分年月日频率生成文件名, 可指定日志存放的目录

LogFile gLog("My.Log");
gLog.Log("test", 4);//记录日志
gLog.Log("系统启动");

LogFileEx gLog(".", LogFileEx :: YEAR);//一年生成一个日志文件
LogFileEx gLog(".\\Log", LogFileEx :: MONTH);//一月生成一个日志文件
LogFileEx gLog(".\\Log", LogFileEx :: DAY);//一天生成一个日志文件
//注意日志所属目录创建失败会自动退出， 请注意目录的合法性， 文件生成频率看情况掌握
//24小时运行的程序可以每天生成一个日志文件， 以免内容过多
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
	//打开文件， 指针到文件尾
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

	//设定日志文件名
	CLogFile(const char *szFileName = "Log.log");
	virtual ~CLogFile();

	const char * GetFileName();
	void SetFileName(const char *szName);

	bool IsOpen();
	void Close();
	//追加日志内容
	void Log(LPCVOID lpBuffer, DWORD dwLength);
	void Log(const char *szText);
private://屏蔽函数
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
	CLogFileEx(const char *szPath = ".", LOG_TYPE iType = MONTH);
	~CLogFileEx();
	const char * GetPath();
	void Log(LPCVOID lpBuffer, DWORD dwLength);
	void Log(const char *szText);
private://屏蔽函数
	CLogFileEx(const CLogFileEx&);
	CLogFileEx&operator = (const CLogFileEx&);
};
#endif
