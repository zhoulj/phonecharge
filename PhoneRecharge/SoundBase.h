/*******************************************************************
| Copyright (c) 2013,�������¿Ƽ����޹�˾                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| �ļ����ƣ�SoundBase.h                                            |
| �ļ���ʶ��                                                       |
| ժҪ��    ���ļ���������CSoundBase������һЩ��Ա                 |
| ����˵��������ʶ����Ҫ¼�Ƶ�wave�ļ�����������ȡ��������         |
| ��ǰ�汾��                                                       |
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
|------------------------------------------------------------------|
| �޸ļ�¼2����                                                    |
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
  | �������ƣ�StopRecordingֹͣ¼��                                  |
  | �������ܣ�ֹͣ¼��������¼���ļ�                                 |
  | �����������                                                     |
  | �����������                                                     |
  | ����ֵ��  ��                                                     |
  | ˵����    ��                                                     |
  |******************************************************************/
	void StopRecording();
	void StopAll();
	static CSoundBase* GetInstance();
	BOOL Play(const TCHAR *pszPath = NULL);
  /*******************************************************************
  | �������ƣ�Record¼��                                             |
  | �������ܣ��õ�Ҫ¼�Ƶ��ļ�������ʼ¼��                           |
  | ���������pszPath¼�����ļ���                                    |
  |           PWAVEFORMAT_SETTING                                    |
  |           pWaveFormat                                            |
  | �����������                                                     |
  | ����ֵ��  ��                                                     |
  | ˵����   ��                                                      |
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
