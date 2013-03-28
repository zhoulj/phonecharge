#include "StdAfx.h"
#include "stdio.h"
#include "QISR.h"
#include <string>
#include "windows.h"
#include "./include/qisr.h"
#include <conio.h>
using namespace std;

#pragma comment(lib,"./lib/msc.lib")


const int BUFFER_NUM = 4096;
const int MAX_KEYWORD_LEN = 4096;
//static CLogFileEx *g_pLogFile = NULL;

CQISR::CQISR(void)
{
  memset(exID,0,128);
  m_pLogFileEx = CLogFileEx::GetInstance(); 
}

CQISR::~CQISR(void)
{
}

/*******************************************************************
| �������ƣ�����ʶ���ʼ��                                         |
| �������ܣ���ʼ������ʶ�����                                     |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  0�ɹ���������ֵʧ��                                    |
| ˵������                                                         |
|******************************************************************/
int CQISR::SRInit()
{
  int nRet = MSP_SUCCESS;
  //appid ��������Ķ�
  nRet = QISRInit("appid=5142e191");    
  if(nRet != MSP_SUCCESS)
  {
    m_pLogFileEx->Log("SR---QISRInit with errorCode: %d \n",nRet);    
    return nRet;
  }  
  return 0;
}

/*******************************************************************
| �������ƣ�����ʶ��                                               |
| �������ܣ��Խ��յ������ļ����ϴ���������������ʶ��             |
|            ����ʶ��Ľ���ı�����                                |
| ���������wavfile����ʶ��pcm16λwave�ļ�                         |
| ���������SpeechText�����ı���ʶ����                           |
| ����ֵ��  0�ɹ���-1������ֵʧ�ܣ�������ֵ�����msp_errors.h      |
| ˵������                                                         |
|******************************************************************/
int CQISR::FileSpeechRecognition(char* wavfile,char* SpeechText)
{
  int ret = MSP_SUCCESS;
  int i = 0;
  FILE* fp = NULL;
  char buff[BUFFER_NUM];
  string strTextSR = "";
  int len;
  int status = MSP_AUDIO_SAMPLE_CONTINUE, ep_status = -1, rec_status = -1, rslt_status = -1;
  const char* param = "rst=plain,sub=asr,ssm=1,aue=speex,auf=audio/L16;rate=16000";//ע��sub=asr
  const char* sess_id = QISRSessionBegin(exID, param, &ret);//���﷨ID����QISRSessionBegin
  if ( MSP_SUCCESS != ret )
  {    
    m_pLogFileEx->Log("SR---QISRSessionBegin err %d\n",ret); 
    return ret;
  }
  
  //��Ҫʶ��������ļ�
  fp = fopen(wavfile , "rb");
  if ( NULL == fp )
  {    
    m_pLogFileEx->Log("SR---failed to open speech recognition wave file,please check the file.\n",ret); 
    QISRSessionEnd(sess_id, "normal");
    return -1;
  }
  //��ȡ�����ļ��������͵�����������ʶ�𣬷���ʶ��Ĵ������������Ϣ    
  while ( !feof(fp) )
  {
    len = fread(buff, 1, BUFFER_NUM, fp);

    ret = QISRAudioWrite(sess_id, buff, len, status, &ep_status, &rec_status);
    if ( ret != MSP_SUCCESS )
    {      
      m_pLogFileEx->Log("SR---QISRAudioWrite err %d\n", ret);
      break;
    }

    if ( rec_status == MSP_REC_STATUS_SUCCESS )
    {
      const char* result = QISRGetResult(sess_id, &rslt_status, 0, &ret);
      if (ret != MSP_SUCCESS )
      {
        m_pLogFileEx->Log("SR---error code: %d\n", ret);
        break;
      }
      else if( rslt_status == MSP_REC_STATUS_NO_MATCH )
        m_pLogFileEx->Log("SR---get result nomatch\n");
      else
      {
        if ( result != NULL )
          m_pLogFileEx->Log("SR---get result");//[%d/%d]:len:%d\n %s\n", ret, rslt_status,strlen(result), result);
      }
    }
    Sleep(120);
  }

  if (ret == MSP_SUCCESS)
  {	
    status = MSP_AUDIO_SAMPLE_LAST;
    ret = QISRAudioWrite(sess_id, buff, 1, status, &ep_status, &rec_status);
    if ( ret != MSP_SUCCESS )
       m_pLogFileEx->Log("SR---QISRAudioWrite write last audio err %d\n", ret);
  }

  if (ret == MSP_SUCCESS)
  {	
    char asr_result[1024] = "";
    int pos_of_result = 0;
    int loop_count = 0;
    do 
    {
      const char* result = QISRGetResult(sess_id, &rslt_status, 0, &ret);
      if ( ret != 0 )
      {
        m_pLogFileEx->Log("SR---QISRGetResult err %d\n", ret);
        break;
      }

      if( rslt_status == MSP_REC_STATUS_NO_MATCH )
      {       
        m_pLogFileEx->Log("SR---get result nomatch\n");
      }
      else if ( result != NULL )
      {
        //m_pLogFileEx->Log("SR---[%d]:get result[%d/%d]: %s\n", (loop_count), ret, rslt_status, result);       
        strcpy(asr_result+pos_of_result,result);
        pos_of_result += strlen(result);
        strTextSR = result;        
        strTextSR = strTextSR.substr(strTextSR.find("input=") + 6,strTextSR.length());
        strcpy(SpeechText,strTextSR.c_str());      
      }
      else
      {
        //m_pLogFileEx->Log("SR---[%d]:get result[%d/%d]\n",(loop_count), ret, rslt_status);       
      }
      Sleep(500);
    } while (rslt_status != MSP_REC_STATUS_COMPLETE && loop_count++ < 30);
    if (strcmp(asr_result,"")==0)
    {
      m_pLogFileEx->Log("SR---speech recognition wave file no result\n"); 
      QISRSessionEnd(sess_id, NULL);
      fclose(fp); 
      return MSP_ERROR_TIME_OUT;
    }
  }

  QISRSessionEnd(sess_id, NULL);
  m_pLogFileEx->Log("SR---speech recognition QISRSessionEnd Success.\n"); 
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
  //�ύ�﷨�ؼ������ݣ��ɹ�������������﷨ID
  const char* testID = QISRUploadData(sessionID, "contact", UserData, len, "dtt=keylist", &ret);
  if(ret != MSP_SUCCESS)
  {
    printf("QISRUploadData with errorCode: %d \n", ret);
    return ret;
  }
  memcpy((void*)exID, testID, strlen(testID));
  //m_pLogFileEx->Log("SR---Get New exID: \"%s\" \n", exID);  

  QISRSessionEnd(sessionID, "normal");
  return 0;
}

/*******************************************************************
| �������ƣ���õ�ǰ�﷨ID                                         |
| �������ܣ���õ�ǰ�﷨ID                                         |
| �����������                                                     |
| �����������                                                     |
| ����ֵ��  ���ص�ǰϵͳ�õ��﷨ID                                 |
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
| ����ֵ��  0�ɹ���-1ʧ��                                        |
| ˵������                                                         |
|******************************************************************/
int CQISR::SetExID(char strexID[128])
{
  if (strexID != NULL)
  {
    strcpy(exID,strexID);    
    return 0;
  }  
  return -1;
}

