// ChatAppLayer.cpp: implementation of the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ipc.h"
#include "ChatAppLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatAppLayer::CChatAppLayer( char* pName ) 
: CBaseLayer( pName ), 
  mp_Dlg( NULL )
{
	ResetHeader( ) ;//헤더를 사용하기전에 리셋한다
}

CChatAppLayer::~CChatAppLayer()
{

}

void CChatAppLayer::SetSourceAddress(unsigned int src_addr)
{
	m_sHeader.app_srcaddr = src_addr ;
}

void CChatAppLayer::SetDestinAddress(unsigned int dst_addr)
{
	m_sHeader.app_dstaddr = dst_addr ;
}

void CChatAppLayer::ResetHeader( )
{
	m_sHeader.app_srcaddr = 0x00000000 ;
	m_sHeader.app_dstaddr = 0x00000000 ;
	m_sHeader.app_length  = 0x0000 ;
	m_sHeader.app_type    = 0x00 ;
	memset( m_sHeader.app_data, 0, APP_DATA_SIZE ) ;//헤더에서 사용되는 값들을 미리 초기화한다
//최대 비트값에 따라 초기화가 다르다
}

unsigned int CChatAppLayer::GetSourceAddress()
{
	return m_sHeader.app_srcaddr ;
}

unsigned int CChatAppLayer::GetDestinAddress()
{
	return m_sHeader.app_dstaddr ;
}

BOOL CChatAppLayer::Send(unsigned char *ppayload, int nlength)
{
	m_sHeader.app_length = (unsigned short) nlength ;

	BOOL bSuccess = FALSE ;//앞으로 메시지의 전송 성공여부를 나타내는 boolean 변수이다
//////////////////////// fill the blank ///////////////////////////////
	memcpy(m_sHeader.app_data, ppayload, nlength > APP_DATA_SIZE ? APP_DATA_SIZE : nlength);  
	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader, nlength + APP_HEADER_SIZE);

	return bSuccess ;
}

BOOL CChatAppLayer::Receive( unsigned char* ppayload )
{
	PCHAT_APP_HEADER app_hdr = (PCHAT_APP_HEADER) ppayload ;

	if ( app_hdr->app_dstaddr == m_sHeader.app_srcaddr || 
	   ( app_hdr->app_srcaddr != m_sHeader.app_srcaddr &&
		 app_hdr->app_dstaddr == (unsigned int) 0xff ) )//주소가 제대로입력했는지 한번더 확인,브로드캐스팅인지 확인
	{
//////////////////////// fill the blank ///////////////////////////////
unsigned char GetBuff[APP_DATA_SIZE];
		memset( GetBuff, '\0', APP_DATA_SIZE);
		memcpy( GetBuff, app_hdr->app_data, app_hdr->app_length > APP_DATA_SIZE? APP_DATA_SIZE : app_hdr->app_length);
		CString Msg;

		if(app_hdr->app_dstaddr==(unsigned int)0xff)
		{Msg.Format("[%d:BROADCAST] %s", app_hdr->app_srcaddr,(char*) GetBuff);}
		else
		{Msg.Format("[%d:%d] %s", app_hdr->app_srcaddr, app_hdr->app_dstaddr, (char*) GetBuff);}

		mp_aUpperLayer[0]->Receive((unsigned char*) Msg.GetBuffer(0));
		return TRUE ;//현재 Layer에있는 data값이 메시지텍스트 이기때문에 이부분을 읽어서  IPCAppDlg로 보낸다
	}
	else
		return FALSE ;
}


