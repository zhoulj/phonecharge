#pragma once

class CIFSR
{
public:
	CIFSR(void);
	virtual ~CIFSR(void);
public:
 /*******************************************************************
  | 函数名称：语音识别初始化                                         |
  | 函数功能：初始化语音识别对象                                     |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  无                                                     |
  | 说明：无                                                         |
  |******************************************************************/
  virtual int SRInit() = 0;
 /*******************************************************************
  | 函数名称：语音识别                                               |
  | 函数功能：对接收的语音文件，上传服务器进行语音识别，             |
  |            并将识别的结果文本返回                                |
  | 输入参数：wavfile语音识别pcm16位wave文件，SpeechText返回识别结果 |
  | 输出参数：无                                                     |
  | 返回值：  无                                                     |
  | 说明：无                                                         |
  |******************************************************************/
	virtual int FileSpeechRecognition(char* wavfile,char* SpeechText) = 0 ;

	//memory recognition
  virtual int MemorySpeechRecognition(char* wavBuffer, UINT uLen, char* &SpeechText) = 0;

	//Close interface
	virtual void SRClose() = 0;

};
