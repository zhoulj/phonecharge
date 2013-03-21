/*******************************************************************
| Copyright (c) 2013,�������¿Ƽ����޹�˾                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| �ļ����ƣ�QISR.h                                                 |
| �ļ���ʶ��                                                       |
| ժҪ��    ���ļ���������QISR������һЩ��Ա                       |
| ����˵������                                                     |
| ��ǰ�汾��0.0.1                                                  |
| ��    �ߣ�                                                       |
| ������ڣ�                                                       |
|----------------------------------------------------------------- |
| �޸ļ�¼1��                                                      |
| �޸����ڣ�                                                       |
| �� �� �ţ�                                                       |
| �� �� �ˣ�                                                       |
| �޸�ԭ��                                                       |
| �޸����ݣ�                                                       |
| �޸Ĵ���λ�ã�                                                   |
|                                                                  |
|******************************************************************/
#pragma once
#include <string.h>;
class CQISR
{

public:
	void initQISR();
	int  SendSyntax(char * asr_keywords_utf8);//�ϴ��﷨����ȡ�﷨ID
	const char* getExID(void);//�����﷨ID
	int SetExID(char exID[128]);
	int SpeechRecognition(char* wavfile,char* &SpeechText);//����ʶ��wav�ļ��������ı�
	CQISR(void);
	~CQISR(void);
private:
	char exID[128];//�﷨ID

};
