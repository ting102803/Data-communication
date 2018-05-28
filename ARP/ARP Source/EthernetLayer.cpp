
#include "stdafx.h"
#include "ARP.h"
#include "EthernetLayer.h"


CEthernetLayer::CEthernetLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( ) ;
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset( m_sHeader.enet_dstaddr.addrs, 0, 6 ) ;
	memset( m_sHeader.enet_srcaddr.addrs, 0, 6 ) ;
	memset( m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE ) ;
	m_sHeader.enet_frametype = ntohs(ARP_PROTOTYPE_ARP);
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
	ETHERNET_ADDR src_ether;
	src_ether.addr0 = pAddress[0];
	src_ether.addr1 = pAddress[1];
	src_ether.addr2 = pAddress[2];
	src_ether.addr3 = pAddress[3];
	src_ether.addr4 = pAddress[4];
	src_ether.addr5 = pAddress[5];

	memcpy( m_sHeader.enet_srcaddr.addrs, src_ether.addrs, 6 ) ;
}

void CEthernetLayer::SetEnetDstAddress(unsigned char *pAddress)
{
	ETHERNET_ADDR dst_ether;
	dst_ether.addr0 = pAddress[0];
	dst_ether.addr1 = pAddress[1];
	dst_ether.addr2 = pAddress[2];
	dst_ether.addr3 = pAddress[3];
	dst_ether.addr4 = pAddress[4];
	dst_ether.addr5 = pAddress[5];

	memcpy( m_sHeader.enet_dstaddr.addrs, dst_ether.addrs, 6 ) ;
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.enet_data, ppayload, nlength ) ;

	BOOL bSuccess = FALSE ;
	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader,nlength+ETHER_HEADER_SIZE);

	return bSuccess ;
}
	
BOOL CEthernetLayer::Receive( unsigned char* ppayload )
{
	
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;
	unsigned char broadcast[6];
	memset(broadcast,0xff,6);

	//���ּҰ� �ǹٸ��� Ȯ��
	if( ( memcmp((char *)pFrame->enet_dstaddr.addrs,(char *)m_sHeader.enet_srcaddr.addrs,6) == 0 &&
		  memcmp((char *)pFrame->enet_srcaddr.addrs,(char *)m_sHeader.enet_srcaddr.addrs,6) != 0 ) ||
		  memcmp((char *)pFrame->enet_dstaddr.addrs,(char *)broadcast,6) == 0 )
	{
		bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*) pFrame->enet_data);
	}
	return bSuccess ;
}
