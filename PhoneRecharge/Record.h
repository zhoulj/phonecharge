#pragma once

class CRecord
{
public:
	int RecordInit();//l¼����ʼ��
	int StartRecord(char* RecFile);//�õ�Ҫ¼�Ƶ��ļ�������ʼ¼��
	int StopRecord();//¼��ֹͣ������¼���ļ�
	CRecord(void);
	~CRecord(void);
};
