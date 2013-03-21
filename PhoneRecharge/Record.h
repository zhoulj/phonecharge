#pragma once

class CRecord
{
public:
	int RecordInit();//l录音初始化
	int StartRecord(char* RecFile);//得到要录制的文件名并开始录音
	int StopRecord();//录音停止并保存录音文件
	CRecord(void);
	~CRecord(void);
};
