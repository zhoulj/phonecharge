/*******************************************************************
| Copyright (c) 2013,长春威致科技有限公司                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| 文件名称：QISR.h                                                 |
| 文件标识：                                                       |
| 摘要：    本文件定义了类QISR及它的一些成员                       |
| 其它说明：无                                                     |
| 当前版本：0.0.1                                                  |
| 作    者：                                                       |
| 完成日期：                                                       |
|----------------------------------------------------------------- |
| 修改记录1：                                                      |
| 修改日期：                                                       |
| 版 本 号：                                                       |
| 修 改 人：                                                       |
| 修改原因：                                                       |
| 修改内容：                                                       |
| 修改代码位置：                                                   |
|                                                                  |
|******************************************************************/
#pragma once
#include <string.h>;
class CQISR
{

public:
	void initQISR();
	int  SendSyntax(char * asr_keywords_utf8);//上传语法并获取语法ID
	const char* getExID(void);//返回语法ID
	int SetExID(char exID[128]);
	int SpeechRecognition(char* wavfile,char* &SpeechText);//语音识别wav文件并返回文本
	CQISR(void);
	~CQISR(void);
private:
	char exID[128];//语法ID

};
