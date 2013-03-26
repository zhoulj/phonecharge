/*******************************************************************
| Copyright (c) 2013,长春威致科技有限公司                        |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| 文件名称：SoundBase.h                                               |
| 文件标识：                                                       |
| 摘要：    本文件定义了类CSoundBase及它的一些成员                    |
| 其它说明：语音识别需要录制的wave文件，从声卡获取语音数据                                                   |
| 当前版本：                                                       |
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
|------------------------------------------------------------------|
| 修改记录2：…                                                    |
|******************************************************************/
// SoundBase.h: interface for the CSoundBase class.
//
//////////////////////////////////////////////////////////////////////
#ifndef SOUNDBASE_H
#define SOUNDBASE_H

#pragma   comment(lib, "winmm.lib ") 
#include "mmsystem.h"
//------------------------------------------------------------------------------
//Macro define
#define MAX_SAVEPATH_LENGTH			500	//The length of saved path

//------------------------------------------------------------------------------
//Value type

enum ChannelType
{
	CHANNEL_SINGLE,
	CHANNEL_DOUBLE
};

enum SamplesPerSecType
{    
	SAMPLES_11025,
  SAMPLES_16000,
	SAMPLES_22050,
	SAMPLES_44100
};

enum BitsPerSampleType
{
	BITS_8,
	BITS_16
};
//---------------------------------------------------------------------------
//Struct

//Wave format data
typedef struct
{
	ChannelType channel;
	SamplesPerSecType samples;
	BitsPerSampleType bits;
}WAVEFORMAT_SETTING,*PWAVEFORMAT_SETTING;
//------------------------------------------------------------------------------
class CSoundBase  
{
public:
	void StopPlaying();
  /*******************************************************************
  | 函数名称：StopRecording停止录音                                               |
  | 函数功能：停止录音并保存录音文件                                 |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  无                                                     |
  | 说明：    无                                                     |
  |******************************************************************/
	void StopRecording();
	void StopAll();
	static CSoundBase* GetInstance();
	BOOL Play(const TCHAR *pszPath = NULL);
  /*******************************************************************
  | 函数名称：Record录音                                               |
  | 函数功能：得到要录制的文件名并开始录音                           |
  | 输入参数：pszPath录音的文件名                 |
  |           PWAVEFORMAT_SETTING                 |
  |           pWaveFormat                         |
  | 输出参数：无                                                     |
  | 返回值：  无                                                     |
  | 说明：   无                                                      |
  |******************************************************************/
	BOOL Record(TCHAR *pszPath,const PWAVEFORMAT_SETTING pWaveFormat = NULL);

	virtual ~CSoundBase();
protected:
	void OnWIM_OPEN(WPARAM wParam, LPARAM lParam);
	void OnWIM_DATA(WPARAM wParam, LPARAM lParam);
	void OnWIM_CLOSE(WPARAM wParam, LPARAM lParam);
	BOOL WriteWaveFileHeader(TCHAR *pszFilename, const PWAVEFORMATEX pWFX, DWORD dwBufferSize,BOOL bCover);
	static void CALLBACK WaveInProc(HWAVEIN hWi,UINT uMsg, DWORD dwInstance,  DWORD dwParam1, DWORD dwParam2);
	CSoundBase();
	void SetRecordWaveFormat(const PWAVEFORMAT_SETTING pWaveFormat);
	
	static CSoundBase *m_pInstance;
	WAVEFORMATEX m_WaveFormatEx;
	BOOL m_bRecording;
	HANDLE m_hSaveFile;
	HWAVEIN m_hWaveIn;
	PBYTE m_pBuffer1;
	PBYTE m_pBuffer2;
	PWAVEHDR m_pWaveHdr1;
	PWAVEHDR m_pWaveHdr2;
	DWORD m_dwDataLength; //The length of the data
	TCHAR m_szSavePath[MAX_SAVEPATH_LENGTH]; //The path to save

};

#endif // SOUNDBASE_H
