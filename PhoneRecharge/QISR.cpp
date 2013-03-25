#include "StdAfx.h"
#include "stdio.h"
#include "QISR.h"
#include <string>
#include "windows.h"
#include "./include/qisr.h"
#include <conio.h>
#include "LogFile.h"


#pragma comment(lib,"./lib/msc.lib")


const int BUFFER_NUM = 4096;
const int MAX_KEYWORD_LEN = 4096;
static CLogFileEx *g_pLogFile = NULL;

CQISR::CQISR(void)
{
  exID[128]=NULL;
  g_pLogFile = new CLogFileEx();
  
//  LogFileEx gLog(".\\Log", LogFileEx :: MONTH);
  //g_pLogFile->GetFileName()
}

CQISR::~CQISR(void)
{
  if(g_pLogFile != NULL)
    delete g_pLogFile;
}

/*******************************************************************
| �������ƣ�����ʶ���ʼ��                                         |
| �������ܣ���ʼ������ʶ�����                                     |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  ��                                                     |
| ˵������                                                         |
|******************************************************************/
void CQISR::SRInit()
{
  int ret = MSP_SUCCESS;
  //appid ��������Ķ�
  ret = QISRInit("appid=5142e191");
  if(ret != MSP_SUCCESS)
  {
    printf("QISRInit with errorCode: %d \n", ret);
    return;
  }
  return;
}

/*******************************************************************
| �������ƣ�����ʶ��                                               |
| �������ܣ��Խ��յ������ļ����ϴ���������������ʶ��             |
|            ����ʶ��Ľ���ı�����                                |
| ���������wavfile����ʶ��pcm16λwave�ļ���SpeechText����ʶ���� |
| �����������                                                     |
| ����ֵ��  ��                                                     |
| ˵������                                                         |
|******************************************************************/
int CQISR::FileSpeechRecognition(char* wavfile,char* SpeechText)
{
  int ret = MSP_SUCCESS;
  int i = 0;
  FILE* fp = NULL;
  char buff[BUFFER_NUM];
  int len;
  int status = MSP_AUDIO_SAMPLE_CONTINUE, ep_status = -1, rec_status = -1, rslt_status = -1;

  const char* param = "rst=plain,sub=asr,ssm=1,aue=speex,auf=audio/L16;rate=16000";//ע��sub=asr
  const char* sess_id = QISRSessionBegin(exID, param, &ret);//���﷨ID����QISRSessionBegin
  if ( MSP_SUCCESS != ret )
  {
    printf("QISRSessionBegin err %d\n", ret);	
    return ret;
  }

  fp = fopen(wavfile , "rb");
  if ( NULL == fp )
  {
    printf("failed to open file,please check the file.\n");
    QISRSessionEnd(sess_id, "normal");
    return -1;
  }

  printf("writing audio...\n");
  while ( !feof(fp) )
  {
    len = fread(buff, 1, BUFFER_NUM, fp);

    ret = QISRAudioWrite(sess_id, buff, len, status, &ep_status, &rec_status);
    if ( ret != MSP_SUCCESS )
    {
      printf("\nQISRAudioWrite err %d\n", ret);
      break;
    }

    if ( rec_status == MSP_REC_STATUS_SUCCESS )
    {
      const char* result = QISRGetResult(sess_id, &rslt_status, 0, &ret);
      if (ret != MSP_SUCCESS )
      {
        printf("error code: %d\n", ret);
        break;
      }
      else if( rslt_status == MSP_REC_STATUS_NO_MATCH )
        printf("get result nomatch\n");
      else
      {
        if ( result != NULL )
          printf("get result[%d/%d]:len:%d\n %s\n", ret, rslt_status,strlen(result), result);
      }
    }
    printf(".");
    Sleep(120);
  }
  printf("\n");

  if (ret == MSP_SUCCESS)
  {	
    status = MSP_AUDIO_SAMPLE_LAST;
    ret = QISRAudioWrite(sess_id, buff, 1, status, &ep_status, &rec_status);
    if ( ret != MSP_SUCCESS )
      printf("QISRAudioWrite write last audio err %d\n", ret);
  }

  if (ret == MSP_SUCCESS)
  {	
    printf("get reuslt\n");
    char asr_result[1024] = "";
    int pos_of_result = 0;
    int loop_count = 0;
    do 
    {
      const char* result = QISRGetResult(sess_id, &rslt_status, 0, &ret);
      if ( ret != 0 )
      {
        printf("QISRGetResult err %d\n", ret);
        break;
      }

      if( rslt_status == MSP_REC_STATUS_NO_MATCH )
      {
        printf("get result nomatch\n");
      }
      else if ( result != NULL )
      {
        printf("[%d]:get result[%d/%d]: %s\n", (loop_count), ret, rslt_status, result);
        strcpy(asr_result+pos_of_result,result);
        pos_of_result += strlen(result);
      }
      else
      {
        printf("[%d]:get result[%d/%d]\n",(loop_count), ret, rslt_status);
        //memcpy(SpeechText,rslt_status,strlen(rslt_status));      
        //MessageBox(0,rslt_status,"TEST",MB_OK);
      }
      Sleep(500);
    } while (rslt_status != MSP_REC_STATUS_COMPLETE && loop_count++ < 30);
    if (strcmp(asr_result,"")==0)
    {
      printf("no result\n");
    }
    else
    {
      printf("Have resultv is \n.",asr_result);
    }

  }

  QISRSessionEnd(sess_id, NULL);
  printf("QISRSessionEnd.\n");
  fclose(fp); 
  return 0;
}

//memory recognition
int CQISR::MemorySpeechRecognition(char* wavBuffer, UINT uLen, char* &SpeechText)
{
  return 0;  
}

//Close interface
void CQISR::SRClose()
{

}

/*******************************************************************
| �������ƣ��ϴ��﷨                                               |
| �������ܣ��ϴ������﷨key������ȡ�﷨ID������Ҫʶ�������        |
| ���������asr_keywords_utf8���﷨�ı��ö��ŷָ���utf8��ʽ        |
| �����������                                                     |
| ����ֵ��  0�ɹ�������ʧ��                                        |
| ˵������                                                         |
|******************************************************************/
int  CQISR::SendSyntax(char * asr_keywords_utf8)
{
  int ret = MSP_SUCCESS;
  const char * sessionID = NULL;
  sessionID = QISRSessionBegin(NULL, "ssm=1,sub=asr", &ret);
  if(ret != MSP_SUCCESS)
  {
    printf("QISRSessionBegin with errorCode: %d \n", ret);
    return ret;
  }

  char UserData[MAX_KEYWORD_LEN];
  memset(UserData, 0, MAX_KEYWORD_LEN);
  FILE* fp = fopen("asr_keywords_utf8.txt", "rb");//�ؼ����б��ļ�������utf8��ʽ
  if (fp == NULL)
  {
    printf("keyword file cannot open\n");
    return -1;
  }
  int len = fread(UserData, 1, MAX_KEYWORD_LEN, fp);
  UserData[len] = 0;
  fclose(fp);
  const char* testID = QISRUploadData(sessionID, "contact", UserData, len, "dtt=keylist", &ret);
  if(ret != MSP_SUCCESS)
  {
    printf("QISRUploadData with errorCode: %d \n", ret);
    return ret;
  }
  memcpy((void*)exID, testID, strlen(testID));
  printf("exID: \"%s\" \n", exID);//����õ�exID�������Ļ��

  QISRSessionEnd(sessionID, "normal");
  return 0;
}

/*******************************************************************
| �������ƣ���õ�ǰ�﷨ID                                         |
| �������ܣ���õ�ǰ�﷨ID                                         |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  ��                                                     |
| ˵������                                                         |
|******************************************************************/
const char* CQISR::getExID(void)
{
    return exID;
}

/*******************************************************************
| �������ƣ������﷨ID                                             |
| �������ܣ��������ļ��л���﷨ID���������﷨ID��������           |
| ���������exID���ֺ���ĸ�ĳ��ַ���                               |
| �����������                                                     |
| ����ֵ��  0�ɹ�������ʧ��                                        |
| ˵������                                                         |
|******************************************************************/
int CQISR::SetExID(char strexID[128])
{
  if (strexID != NULL)
  {
    strcpy(exID,strexID);
    //memcpy((void*)exID, strexID, strlen(strexID));
    return 0;
  }  
  return -1;
}

