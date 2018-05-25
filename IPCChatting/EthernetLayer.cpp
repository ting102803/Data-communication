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
}//Ethernetlayer�� ���缭 ����� ����

unsigned char* CEthernetLayer::GetSourceAddress()//�۽��ּҸ� �����ϴ� �Լ�
{
	return m_sHeader.enet_srcaddr ;
}

unsigned char* CEthernetLayer::GetDestinAddress()//�����ּҸ� �����ϴ� �Լ�
{
//////////////////////// fill the blank ///////////////////////////////
	return m_sHeader.enet_dstaddr;
}

void CEthernetLayer::SetSourceAddress(unsigned char *pAddress)//�۽��ּҸ� ����� �����ϴ� �Լ�
{
//////////////////////// fill the blank ///////////////////////////////
	memcpy( m_sHeader.enet_srcaddr, pAddress, 6 ) ;
}

void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)//�����ּҸ� ����� �����ϴ� �Լ�
{
	memcpy( m_sHeader.enet_dstaddr, pAddress, 6 ) ;
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memcpy( m_sHeader.enet_data, ppayload, nlength ) ;

	BOOL bSuccess = FALSE ;//�۽� ���� ������ ���� ����
//////////////////////// fill the blank ///////////////////////////////
	bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader, nlength + ETHER_HEADER_SIZE);
	//���������� send�Լ��� ������� ���ڸ� �������� �����Ѵ�
	return bSuccess ;
}

BOOL CEthernetLayer::Receive( unsigned char* ppayload )
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload ;
	//���� ppayload�� ����ȯ�ؼ� ����������� ����Ҽ��ֵ��� ����
	BOOL bSuccess = FALSE ;
//////////////////////// fill the blank ///////////////////////////////
	bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)pFrame->enet_data);

	return bSuccess ;
}
