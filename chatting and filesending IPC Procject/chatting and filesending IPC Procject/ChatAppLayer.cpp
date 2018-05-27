

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

void CChatAppLayer::ResetHeader( ) // ChatApp ��� �ʱ�ȭ
{
   m_sHeader.capp_totlen  = 0x0000 ;
   m_sHeader.capp_type    = 0x00 ;

   memset( m_sHeader.capp_data, 0, APP_DATA_SIZE ) ;
}

BOOL CChatAppLayer::Send(unsigned char *ppayload, int nlength)
{
   // ppayload : �Է��� ���ڿ� , nlength : ������ ����
   m_ppayload = ppayload;
   m_length = nlength;

   if(nlength <= APP_DATA_SIZE){ //������ ���̰� app_data_size ���� �������
      m_sHeader.capp_totlen = nlength;
      memcpy(m_sHeader.capp_data,ppayload,nlength); // GetBuff�� data ����
      mp_UnderLayer->Send((unsigned char*) &m_sHeader,nlength+APP_HEADER_SIZE);//���� �������� �ѱ��
   }else{
      AfxBeginThread(ChatThread,this);//Ŭ��� ������ �̿��Ѵ�.
   }
   
   return TRUE;
}
BOOL CChatAppLayer::Receive( unsigned char* ppayload )
{
   // ppayload�� ChatApp ��� ����ü�� �ִ´�.
   LPCHAT_APP capp_hdr = (LPCHAT_APP) ppayload ;
   int length = capp_hdr->capp_totlen-1;
   static unsigned char *GetBuff; // �����͸� ���� GetBuff�� �����Ѵ�.
   
   if(length <= APP_DATA_SIZE){
      GetBuff = (unsigned char *)malloc(length);//���� ����
      memset(GetBuff,0,length);  // GetBuff�� �ʱ�ȭ���ش�.
      memcpy(GetBuff,capp_hdr->capp_data,length); // GetBuff�� data ����
      GetBuff[length] = '\0';

      mp_aUpperLayer[0]->Receive((unsigned char*) GetBuff); // ���� �������� ������ �ø�

      return TRUE;
   }
   if(capp_hdr->capp_type == DATA_TYPE_BEGIN)//������ ù�κ��϶� 
   {      
      GetBuff = (unsigned char *)malloc((length)); //���۸� �����Ѵ�.
      memset(GetBuff,0,(length)); //���ۿ� �����͸� ����.
   }
   else if(capp_hdr->capp_type == DATA_TYPE_CONT) // ������ �߰��κ��϶�

   {
      strncat((char *)GetBuff,(char *)capp_hdr->capp_data,strlen((char *)capp_hdr->capp_data));
      GetBuff[strlen((char *)GetBuff)] = '\0';
   }
   else if(capp_hdr->capp_type == DATA_TYPE_END) //������ ������ �κ�
   {
      memcpy(GetBuff,GetBuff,(length)); //���ۿ� length ���̸�ŭ�� ���۸� �����Ѵ�
      GetBuff[(length)] = '\0';

      mp_aUpperLayer[0]->Receive((unsigned char*) GetBuff);// ���� �������� ������ �ø�
      free(GetBuff);// ���� ����
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

   if( Capp->m_length < APP_DATA_SIZE ) //capp ���̰� app_data_size ���� �������
      seq_tot_num = 1;
   else //Ŭ���
      seq_tot_num = (Capp->m_length/APP_DATA_SIZE)+1;

   for(int i=0;i<=seq_tot_num+1;i++)
   {
      if(seq_tot_num==1){ //�������
         data_length = Capp->m_length; 
      }
      else{
         if(i==seq_tot_num)
            data_length = Capp->m_length%APP_DATA_SIZE;
         else
            data_length = APP_DATA_SIZE;
      }

     memset(Capp->m_sHeader.capp_data, 0, data_length);

      if(i==0) // ó���κ�
      {
         Capp->m_sHeader.capp_totlen = Capp->m_length;
         Capp->m_sHeader.capp_type = DATA_TYPE_BEGIN;
         memset(Capp->m_sHeader.capp_data,0,data_length);
         data_length = 0;
      }
      else if(i!=0 && i<=seq_tot_num)  // �߰��κ�
      {
         data_index = data_length;
         Capp->m_sHeader.capp_type = DATA_TYPE_CONT;
         Capp->m_sHeader.capp_seq_num = i-1;

         CString str = Capp->m_ppayload;
         str = str.Mid(temp,temp+data_index);

         memcpy(Capp->m_sHeader.capp_data,str,data_length);
         temp += data_index;
      }
      else // �������κ� 
      {
         Capp->m_sHeader.capp_type = DATA_TYPE_END;
         memset(Capp->m_ppayload,0,data_length);
         data_length = 0;
      }
      ((CTCPLayer*) (Capp->GetUnderLayer()))->SetDestinPort(TCP_PORT_CHAT);//tcp layer �� �������� TCPLayer Port�� ����
       bSuccess = Capp->mp_UnderLayer->Send((unsigned char*) &Capp->m_sHeader,data_length+APP_HEADER_SIZE);
   }

   return bSuccess;
}