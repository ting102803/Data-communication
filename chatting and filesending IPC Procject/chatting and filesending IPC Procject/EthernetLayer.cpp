
#include "stdafx.h"
#include "ipc.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CEthernetLayer::CEthernetLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( ) ;
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()//�����ּҿ� �����ּ��� ���ּҰ����� �ʱ�ȭ
{
	memset( m_sHeader.enet_dstaddr.S_un.s_ether_addr, 0, 6 ) ;
	m_sHeader.enet_dstaddr.S_un.s_un_byte.e0=0x00;
	m_sHeader.enet_dstaddr.S_un.s_un_byte.e1=0x00;
	m_sHeader.enet_dstaddr.S_un.s_un_byte.e2=0x00;
	m_sHeader.enet_dstaddr.S_un.s_un_byte.e3=0x00;
	m_sHeader.enet_dstaddr.S_un.s_un_byte.e4=0x00;
	m_sHeader.enet_dstaddr.S_un.s_un_byte.e5=0x00;

	memset( m_sHeader.enet_srcaddr.S_un.s_ether_addr, 0, 6 ) ;
	m_sHeader.enet_srcaddr.S_un.s_un_byte.e0=0x00;
	m_sHeader.enet_srcaddr.S_un.s_un_byte.e1=0x00;
	m_sHeader.enet_srcaddr.S_un.s_un_byte.e2=0x00;
	m_sHeader.enet_srcaddr.S_un.s_un_byte.e3=0x00;
	m_sHeader.enet_srcaddr.S_un.s_un_byte.e4=0x00;
	m_sHeader.enet_srcaddr.S_un.s_un_byte.e5=0x00;

	memset( m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE ) ;
	m_sHeader.enet_type = 0x3412 ; // 0x0800
}

unsigned char* CEthernetLayer::GetEnetDstAddress() 
{
	return m_sHeader.enet_srcaddr.addrs;
}

unsigned char* CEthernetLayer::GetEnetSrcAddress() 
{
	return m_sHeader.enet_dstaddr.addrs;
}

void CEthernetLayer::SetEnetSrcAddress(unsigned char *pAddress)
{
	memcpy( m_sHeader.enet_srcaddr.addrs, pAddress, 6 ) ;
}

void CEthernetLayer::SetEnetDstAddress(unsigned char *pAddress)
{

	memcpy( m_sHeader.enet_dstaddr.addrs, pAddress, 6 ) ;
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.enet_data, ppayload, nlength ) ;

	BOOL bSuccess = FALSE ;
	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader,nlength+ETHER_HEADER_SIZE);

	return bSuccess ;
}//send�Լ��� �������̾��� send�Լ��� ����� �߰��ϰ� ȣ���Ѵ�.
	
BOOL CEthernetLayer::Receive( unsigned char* ppayload )
{
	// ���� �������� ���� payload�� ���� ������ header������ �°� ����.
	LPETHERNET pFrame = (LPETHERNET) ppayload ;

	BOOL bSuccess = FALSE ;

	if(ntohs(pFrame->enet_type) == 0x1234&&(memcmp((const char*)pFrame->enet_dstaddr.S_un.s_ether_addr,(const char*)m_sHeader.enet_srcaddr.S_un.s_ether_addr,6) == 0)
		&&(memcmp((const char*)pFrame->enet_srcaddr.S_un.s_ether_addr,(const char*)m_sHeader.enet_dstaddr.S_un.s_ether_addr,6) == 0)
		&&(memcmp((const char*)pFrame->enet_srcaddr.S_un.s_ether_addr,(const char*)m_sHeader.enet_srcaddr.S_un.s_ether_addr,6) != 0)
		){ // Ethernet Frametype �˻� // ntoths�� ���ε���� ��Ʋ�ε��
		bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*) pFrame->enet_data);
	}
	return bSuccess ;
}
