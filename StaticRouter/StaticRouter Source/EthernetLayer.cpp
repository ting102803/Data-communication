#include "StdAfx.h"
#include "EthernetLayer.h"

CEthernetLayer::CEthernetLayer( char* pName )		// ������
: CBaseLayer( pName )
{
	ResetHeader();
}

CEthernetLayer::~CEthernetLayer()					// �Ҹ���
{
}
// Header�� �ʱ�ȭ �ϴ� �Լ�.
void CEthernetLayer::ResetHeader()
{
	// destination address initialize
	memset( Ethernet_Header.Ethernet_srcAddr.addr_ethernet , 0 , 6 );
	// source address initialize
	memset( Ethernet_Header.Ethernet_dstAddr.addr_ethernet , 0 , 6 );
	// data type initialize
	Ethernet_Header.Ethernet_type = 0x0800;
	// data initialize
	memset( Ethernet_Header.Ethernet_data , 0 , ETHERNET_MAX_DATA );
}
// �۽����� ethernet �ּҸ� ������ �Լ�.
unsigned char* CEthernetLayer::GetSourceAddress(int dev_num)
{
	if(dev_num == 1)
		return this->dev_1_mac_addr;
	return this->dev_2_mac_addr;
	// �ҽ� �ּҸ� ��� �Լ��̴�.
	// ������� m_sHeader�� �ҽ��ּҰ� ��� .enet_srcaddr�� �����Ѵ�.
}
// �������� ethernet �ּҸ� ������ �Լ�.
unsigned char* CEthernetLayer::GetDestinAddress()
{	
	return Ethernet_Header.Ethernet_dstAddr.addr_ethernet;
	//������ �ּҸ� ��� �Լ� �̴�.
	// �ɹ����� m_sHeader�� �������ּҰ� ��� .enet_dstaddr�� �����Ѵ�.

}
// �۽����� ethernet �ּҸ� �����ϴ� �Լ�.
void CEthernetLayer::SetSourceAddress(unsigned char *pAddress,int dev_num)
{
	// pAddress�� �Ѿ�� ���ڸ� memcpy �Ѵ�.
	if(dev_num == 1){
		memcpy(dev_1_mac_addr,pAddress,6);
	}
	else{
		memcpy(dev_2_mac_addr,pAddress,6);
	}
	memcpy( Ethernet_Header.Ethernet_srcAddr.addr_ethernet , pAddress, 6 ) ;
}
// �������� ethernet �ּҸ� �����ϴ� �Լ�.
void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy( Ethernet_Header.Ethernet_dstAddr.addr_ethernet , pAddress, 6 ) ;
	// �ɹ� ������ ������ �ּҸ� �ִ� enet_dstaddr�� 
	//���ڷ� ���� paddress�� ���� �����Ѵ�.
}

// �׷��� �Ű������� �Ѿ���� ppayload�� ������ test packet�� �Է��� "Group 6 test packet" �̶�� ���ڿ� �Դϴ�.
BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength,int dev_num)
{
	memcpy(Ethernet_Header.Ethernet_data , ppayload, nlength);
	BOOL bSuccess=mp_UnderLayer->Send((unsigned char*)&Ethernet_Header, nlength + ETHERNET_HEADER_SIZE,dev_num);
	// NILayer�� data�� ������ �κ�.
	return bSuccess ;
}

// NILayer���� data�� ������ demultiplexing �ϰ� ���� �������� �����ִ� �������� ���� �ϸ� �ɰͰ����ϴ�.
BOOL CEthernetLayer::Receive( unsigned char* ppayload ,int dev_num)
{
	PEthernetHeader pFrame = (PEthernetHeader)ppayload;
	char Broad[6];
	memset(Broad,0xff,6);
	EthernetHeader Ethernet_Header;
	if(!memcmp(&pFrame->Ethernet_srcAddr,GetSourceAddress(dev_num),6)){//�ڽſ��� ���� ��Ŷ
		return FALSE;
	}
	else if((!memcmp(&pFrame->Ethernet_dstAddr,GetSourceAddress(dev_num),6)) || (!memcmp(&pFrame->Ethernet_dstAddr,Broad,6))){
		//Broad Cast or �ڽ� Mac�ּ�
		if(pFrame->Ethernet_type == arp_type){ //arp_type�� ���
			GetUpperLayer(1)->Receive((unsigned char *)pFrame->Ethernet_data,dev_num);
		}
		else if(pFrame->Ethernet_type == ip_type){ //ip_type�� ���
			GetUpperLayer(0)->Receive((unsigned char *)pFrame->Ethernet_data,dev_num);
		}
	}
	return FALSE;
}

void CEthernetLayer::SetType(unsigned short type){
	Ethernet_Header.Ethernet_type = type;
}

BOOL CEthernetLayer::Send( unsigned char* ppayload, int nlength ,unsigned short type,int dev_num){
	char Broad[6];
	memset(Broad,0xff,6);
	if(dev_num == 1){
		memcpy(&Ethernet_Header.Ethernet_srcAddr,dev_1_mac_addr,6);
	}
	else{
		memcpy(&Ethernet_Header.Ethernet_srcAddr,dev_2_mac_addr,6);
	}
	memcpy(&Ethernet_Header.Ethernet_data, ppayload, nlength);	// data �κ��� ���� �Ѵ�.
	Ethernet_Header.Ethernet_type = type;
	if(type == arp_type){
			memcpy(&Ethernet_Header.Ethernet_dstAddr,Broad,6);
	}

	// NILayer�� data�� ������ �κ�.
	return GetUnderLayer()->Send((unsigned char *)&Ethernet_Header,nlength + ETHERNET_HEADER_SIZE,dev_num);
}