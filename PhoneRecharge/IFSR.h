#pragma once

class CIFSR
{
public:
	CIFSR(void);
	virtual ~CIFSR(void);
public:
 /*******************************************************************
  | �������ƣ�����ʶ���ʼ��                                         |
  | �������ܣ���ʼ������ʶ�����                                     |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  ��                                                     |
  | ˵������                                                         |
  |******************************************************************/
  virtual int SRInit() = 0;
 /*******************************************************************
  | �������ƣ�����ʶ��                                               |
  | �������ܣ��Խ��յ������ļ����ϴ���������������ʶ��             |
  |            ����ʶ��Ľ���ı�����                                |
  | ���������wavfile����ʶ��pcm16λwave�ļ���SpeechText����ʶ���� |
  | �����������                                                     |
  | ����ֵ��  ��                                                     |
  | ˵������                                                         |
  |******************************************************************/
	virtual int FileSpeechRecognition(char* wavfile,char* SpeechText) = 0 ;

	//memory recognition
  virtual int MemorySpeechRecognition(char* wavBuffer, UINT uLen, char* &SpeechText) = 0;

	//Close interface
	virtual void SRClose() = 0;

};
