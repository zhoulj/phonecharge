#pragma once

class CMicrosoftSR
{
public:
   /*******************************************************************
  | �������ƣ�����ʶ���ʼ��                                         |
  | �������ܣ���ʼ������ʶ�����                                     |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  ��                                                     |
  | ˵������                                                         |
  |******************************************************************/
  void SRInit();
 /*******************************************************************
  | �������ƣ�����ʶ��                                               |
  | �������ܣ��Խ��յ������ļ����ϴ���������������ʶ��             |
  |            ����ʶ��Ľ���ı�����                                |
  | ���������wavfile����ʶ��pcm16λwave�ļ���SpeechText����ʶ���� |
  | �����������                                                     |
  | ����ֵ��  ��                                                     |
  | ˵������                                                         |
  |******************************************************************/
	int FileSpeechRecognition(char* wavfile,char* &SpeechText);

	//memory recognition
  int MemorySpeechRecognition(char* wavBuffer, UINT uLen, char* &SpeechText);

	//Close interface
	void SRClose();


	CMicrosoftSR(void);
	virtual ~CMicrosoftSR(void);
protected:

private:

};
