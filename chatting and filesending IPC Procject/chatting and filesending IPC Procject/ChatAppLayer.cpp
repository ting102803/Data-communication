

#include "stdafx.h"
#include "ipc.h"
#include "ChatAppLayer.h"
#include "TCPLayer.h"      
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CChatAppLayer::CChatAppLayer( char* pName ) 
   : CBaseLayer( pName ), 
   mp_Dlg( NULL )
{
   ResetHeader( ) ;
}

CChatAppLayer::~CChatAppLayer()
{
}

void CChatAppLayer::ResetHeader( ) // ChatApp 헤더 초기화
{
   m_sHeader.capp_totlen  = 0x0000 ;
   m_sHeader.capp_type    = 0x00 ;

   memset( m_sHeader.capp_data, 0, APP_DATA_SIZE ) ;
}

BOOL CChatAppLayer::Send(unsigned char *ppayload, int nlength)
{
   // ppayload : 입력한 문자열 , nlength : 문자의 길이
   m_ppayload = ppayload;
   m_length = nlength;

   if(nlength <= APP_DATA_SIZE){ //문자의 길이가 app_data_size 보다 작을경우
      m_sHeader.capp_totlen = nlength;
      memcpy(m_sHeader.capp_data,ppayload,nlength); // GetBuff에 data 복사
      mp_UnderLayer->Send((unsigned char*) &m_sHeader,nlength+APP_HEADER_SIZE);//하위 계층으로 넘긴다
   }else{
      AfxBeginThread(ChatThread,this);//클경우 쓰레드 이용한다.
   }
   
   return TRUE;
}
BOOL CChatAppLayer::Receive( unsigned char* ppayload )
{
   // ppayload를 ChatApp 헤더 구조체로 넣는다.
   LPCHAT_APP capp_hdr = (LPCHAT_APP) ppayload ;
   int length = capp_hdr->capp_totlen-1;
   static unsigned char *GetBuff; // 데이터를 쌓을 GetBuff를 선언한다.
   
   if(length <= APP_DATA_SIZE){
      GetBuff = (unsigned char *)malloc(length);//버퍼 생성
      memset(GetBuff,0,length);  // GetBuff를 초기화해준다.
      memcpy(GetBuff,capp_hdr->capp_data,length); // GetBuff에 data 복사
      GetBuff[length] = '\0';

      mp_aUpperLayer[0]->Receive((unsigned char*) GetBuff); // 상위 계층으로 데이터 올림

      return TRUE;
   }
   if(capp_hdr->capp_type == DATA_TYPE_BEGIN)//데이터 첫부분일때 
   {      
      GetBuff = (unsigned char *)malloc((length)); //버퍼를 생성한다.
      memset(GetBuff,0,(length)); //버퍼에 데이터를 쓴다.
   }
   else if(capp_hdr->capp_type == DATA_TYPE_CONT) // 데이터 중간부분일때

   {
      strncat((char *)GetBuff,(char *)capp_hdr->capp_data,strlen((char *)capp_hdr->capp_data));
      GetBuff[strlen((char *)GetBuff)] = '\0';
   }
   else if(capp_hdr->capp_type == DATA_TYPE_END) //데이터 마지막 부분
   {
      memcpy(GetBuff,GetBuff,(length)); //버퍼에 length 길이만큼의 버퍼를 복사한다
      GetBuff[(length)] = '\0';

      mp_aUpperLayer[0]->Receive((unsigned char*) GetBuff);// 상위 계층으로 데이터 올림
      free(GetBuff);// 버퍼 해제
   }
   else
      return FALSE;

   return TRUE ;
}

UINT CChatAppLayer::ChatThread( LPVOID pParam )
{
   
   BOOL bSuccess = FALSE;
   CChatAppLayer *Capp = (CChatAppLayer *)pParam;
   int data_length = APP_DATA_SIZE;
   int seq_tot_num;
   int data_index;   
   int temp = 0;

   if( Capp->m_length < APP_DATA_SIZE ) //capp 길이가 app_data_size 보다 작을경우
      seq_tot_num = 1;
   else //클경우
      seq_tot_num = (Capp->m_length/APP_DATA_SIZE)+1;

   for(int i=0;i<=seq_tot_num+1;i++)
   {
      if(seq_tot_num==1){ //작을경우
         data_length = Capp->m_length; 
      }
      else{
         if(i==seq_tot_num)
            data_length = Capp->m_length%APP_DATA_SIZE;
         else
            data_length = APP_DATA_SIZE;
      }

     memset(Capp->m_sHeader.capp_data, 0, data_length);

      if(i==0) // 처음부분
      {
         Capp->m_sHeader.capp_totlen = Capp->m_length;
         Capp->m_sHeader.capp_type = DATA_TYPE_BEGIN;
         memset(Capp->m_sHeader.capp_data,0,data_length);
         data_length = 0;
      }
      else if(i!=0 && i<=seq_tot_num)  // 중간부분
      {
         data_index = data_length;
         Capp->m_sHeader.capp_type = DATA_TYPE_CONT;
         Capp->m_sHeader.capp_seq_num = i-1;

         CString str = Capp->m_ppayload;
         str = str.Mid(temp,temp+data_index);

         memcpy(Capp->m_sHeader.capp_data,str,data_length);
         temp += data_index;
      }
      else // 마지막부분 
      {
         Capp->m_sHeader.capp_type = DATA_TYPE_END;
         memset(Capp->m_ppayload,0,data_length);
         data_length = 0;
      }
      ((CTCPLayer*) (Capp->GetUnderLayer()))->SetDestinPort(TCP_PORT_CHAT);//tcp layer 로 내려보내 TCPLayer Port를 설정
       bSuccess = Capp->mp_UnderLayer->Send((unsigned char*) &Capp->m_sHeader,data_length+APP_HEADER_SIZE);
   }

   return bSuccess;
}