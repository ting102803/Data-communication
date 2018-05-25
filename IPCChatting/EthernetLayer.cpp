// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ipc.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
	memset( m_sHeader.enet_dstaddr, 0, 6 ) ;
	memset( m_sHeader.enet_srcaddr, 0, 6 ) ;
	memset( m_sHeader.enet_data, ETHER_MAX_DATA_SIZE, 6 ) ;
	m_sHeader.enet_type = 0 ;
}//Ethernetlayer에 맞춰서 헤더를 리셋

unsigned char* CEthernetLayer::GetSourceAddress()//송신주소를 리턴하는 함수
{
	return m_sHeader.enet_srcaddr ;
}

unsigned char* CEthernetLayer::GetDestinAddress()//도착주소를 리턴하는 함수
{
//////////////////////// fill the blank ///////////////////////////////
	return m_sHeader.enet_dstaddr;
}

void CEthernetLayer::SetSourceAddress(unsigned char *pAddress)//송신주소를 헤더에 설정하는 함수
{
//////////////////////// fill the blank ///////////////////////////////
	memcpy( m_sHeader.enet_srcaddr, pAddress, 6 ) ;
}

void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)//수신주소를 헤더에 설정하는 함수
{
	memcpy( m_sHeader.enet_dstaddr, pAddress, 6 ) ;
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.enet_data, ppayload, nlength ) ;

	BOOL bSuccess = FALSE ;//송신 성공 유무를 묻는 변수
//////////////////////// fill the blank ///////////////////////////////
	bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader, nlength + ETHER_HEADER_SIZE);
	//하위계층으 send함수를 헤더값을 인자를 가지도록 실행한다
	return bSuccess ;
}

BOOL CEthernetLayer::Receive( unsigned char* ppayload )
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload ;
	//받은 ppayload를 형변환해서 현재계층에서 사용할수있도록 변경
	BOOL bSuccess = FALSE ;
//////////////////////// fill the blank ///////////////////////////////
	bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)pFrame->enet_data);

	return bSuccess ;
}
