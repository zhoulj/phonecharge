#include "stdafx.h"
#include "LogFile.h"

CLogFile::CLogFile(const char *szFileName)//�趨��־�ļ���
{
	_szFileName = NULL;
	_hFile = INVALID_HANDLE_VALUE;
	::InitializeCriticalSection(&_csLock);
	SetFileName(szFileName);
}

CLogFile::~CLogFile()
{
	::DeleteCriticalSection(&_csLock);
	Close();
	if(_szFileName)
		delete []_szFileName;
}

//���ļ��� ָ�뵽�ļ�β
bool CLogFile::OpenFile()
{
	if(IsOpen())
		return true;
	if(!_szFileName)
		return false;
	_hFile =  CreateFile(_szFileName,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!IsOpen() && GetLastError() == 2)//�򿪲��ɹ��� ����Ϊ�ļ������ڣ� �����ļ�
	_hFile =  CreateFile(_szFileName,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(IsOpen())
		SetFilePointer(_hFile, 0, NULL, FILE_END);
	return IsOpen();
}

DWORD CLogFile::Write(LPCVOID lpBuffer, DWORD dwLength)
{
	DWORD dwWriteLength = 0;
	if(IsOpen())
		WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);

	return dwWriteLength;
}

//д��־, ������չ�޸�
void CLogFile::WriteLog( LPCVOID lpBuffer, DWORD dwLength)
{
	time_t now;
	char temp[21];
	DWORD dwWriteLength;
	if(IsOpen())
	{
		time(&now);
		strftime(temp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
		WriteFile(_hFile, "\xd\xa#-----------------------------", 32, &dwWriteLength, NULL);
		WriteFile(_hFile, temp, 19, &dwWriteLength, NULL);
		WriteFile(_hFile, "-----------------------------#\xd\xa", 32, &dwWriteLength, NULL);
		WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);
		WriteFile(_hFile, "\xd\xa", 2, &dwWriteLength, NULL);
		FlushFileBuffers(_hFile);
	}
}


const char * CLogFile::GetFileName()
{
	return _szFileName;
}
//�޸��ļ����� ͬʱ�ر���һ����־�ļ�
void CLogFile::SetFileName(const char *szName)
{
	assert(szName);
	if(_szFileName)
		delete []_szFileName;
	Close();
	_szFileName = new char[strlen(szName) + 1];
	assert(_szFileName);
	strcpy(_szFileName, szName);
}

bool CLogFile::IsOpen()
{
	return _hFile != INVALID_HANDLE_VALUE;
}

void CLogFile::Close()
{
	if(IsOpen())
	{
		CloseHandle(_hFile);
		_hFile = INVALID_HANDLE_VALUE;
	}
}

void CLogFile::Log(LPCVOID lpBuffer, DWORD dwLength)//׷����־����
{
	assert(lpBuffer);
	__try
	{
		Lock();
		if(!OpenFile())
			return;
		WriteLog(lpBuffer, dwLength);
	}
	__finally
	{
		Unlock();
	}
}

void CLogFile::Log(const char *szText)
{
	Log((void *)szText, strlen(szText));
}


//CLogFileEx Code Part
CLogFileEx::CLogFileEx(const char *szPath , LOG_TYPE iType )
{
	_szPath = NULL;
	SetPath(szPath);
	_iType = iType;
	memset(_szLastDate, 0, 9);
}
CLogFileEx::~CLogFileEx()
{
	if(_szPath)
		delete []_szPath;
}

const char * CLogFileEx::GetPath()
{
	return _szPath;
}

void CLogFileEx::Log(LPCVOID lpBuffer, DWORD dwLength)
{
	assert(lpBuffer);
	char temp[10];
	static const char format[3][10] = {"%Y", "%Y-%m", "%Y%m%d"};
	__try
	{
		Lock();
		time_t now = time(NULL);
		strftime(temp, 9, format[_iType], localtime(&now));
		if(strcmp(_szLastDate, temp) != 0)//�����ļ���
		{
			strcat(strcpy(_szFileName, _szPath), "\\");
			strcat(strcat(_szFileName, temp), ".log");
			strcpy(_szLastDate, temp);
			Close();
		}
		if(!OpenFile())
			return;
		WriteLog(lpBuffer, dwLength);
	}
	__finally
	{
		Unlock();
	}
}

void CLogFileEx::Log(const char *szText)
{
	Log(szText, strlen(szText));
}

