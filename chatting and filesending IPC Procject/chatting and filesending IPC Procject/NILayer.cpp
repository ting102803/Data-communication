#include "stdafx.h"
#include "ipc.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define IPTOSBUFFERS 12

CNILayer::CNILayer( char *pName, LPADAPTER *pAdapterObject, int iNumAdapter )
	: CBaseLayer( pName )
{
	m_AdapterObject = NULL;
	m_iNumAdapter = iNumAdapter; //��ġ��ȣ
	m_thrdSwitch = TRUE;
	SetAdapterList(NULL);
}

CNILayer::~CNILayer()
{
}

void CNILayer::PacketStartDriver()//��Ʈ��ũ ������ ���� �Լ�
{
	char errbuf[PCAP_ERRBUF_SIZE];//������ ������ ����

	//��Ʈ��ũ ��ġ ��ȣ�� �߸��Ǿ�����
	if(m_iNumAdapter < 0 ){
		AfxMessageBox("Not exist NICard");
		return;
	}
	// packet capture descriptor(PCD) ����, ��Ŷ ĸ�� ����
	m_AdapterObject = pcap_open_live(m_pAdapterList[m_iNumAdapter]->name,1500,PCAP_OPENFLAG_PROMISCUOUS,2000,errbuf);
	if(!m_AdapterObject){ // PCD�� �߸� ������ ���
		AfxMessageBox(errbuf); //���� ���
		return;
	}
	AfxBeginThread(ReadingThread, this); // ��Ŷ ���� ������ ����
}

pcap_if_t *CNILayer::GetAdapterObject(int iIndex) // ��Ʈ��ũ ��ġ ����
{
	return m_pAdapterList[iIndex];
}

char *CNILayer::GetIPAddress(int iIndex) //ip �ּ� ����
{
	return ipAddr[iIndex];
}


char *iptos(u_long in) //IP�� ���ڿ� �������� ��ȯ�ϴ� �Լ�  
{
    static char output[IPTOSBUFFERS][3*4+3+1];// ip�ּҸ� ������ �迭
    static short number;
    u_char *p;
 
    p = (u_char *)&in; 
    number = (number + 1 == IPTOSBUFFERS ? 0 : number + 1);
    sprintf(output[number], "%d.%d.%d.%d", p[3], p[2], p[1], p[0]);	//�������� ��� �ǹǷ� �������� ����
    return output[number];
}

void CNILayer::SetAdapterNumber(int iNum) //��Ʈ��ũ ��ġ ��ȣ ����
{
	m_iNumAdapter = iNum;
}

void CNILayer::SetAdapterList(LPADAPTER *plist) //��Ʈ��ũ ��ġ ����Ʈ ����
{
	pcap_if_t *alldevs; 
	pcap_if_t *d;
	pcap_addr_t *a;
	int i=0;
	int j=0;
	
	char errbuf[PCAP_ERRBUF_SIZE];

	for(int j=0;j<NI_COUNT_NIC;j++) //��Ʈ��ũ ��ġ ����Ʈ �ʱ�ȭ
	{
		m_pAdapterList[j] = NULL;
	}

	if(pcap_findalldevs(&alldevs, errbuf) == -1) //��Ʈ��ũ ��ġ ���� ������
	{
		AfxMessageBox("Not exist NICard");
		return;
	}
	if(!alldevs) // ����̽��� �ϳ��� ���� �� ����
	{
		AfxMessageBox("Not exist NICard");
		return;
	}

	for(d=alldevs; d; d=d->next) // ��Ʈ��ũ ��ġ ����Ʈ �Է�
	{
		m_pAdapterList[i] = d;

		 for(a=d->addresses;a;a=a->next) {
			
			ipAddr[i] = iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr);
		 }
		 i++;
	}
}

BOOL CNILayer::Send(unsigned char *ppayload, int nlength) //��Ŷ�� ������ �Լ�
{
	if(pcap_sendpacket(m_AdapterObject,ppayload,nlength))
	{
		AfxMessageBox("��Ŷ ���� ����");
		return FALSE;
	}
	return TRUE;
}

BOOL CNILayer::Receive( unsigned char* ppayload ) //��Ŷ�� �޴� �Լ�
{
	BOOL bSuccess = FALSE;

	bSuccess = mp_aUpperLayer[0]->Receive(ppayload);// ���� �������� payload �ø�
	return bSuccess;
}

UINT CNILayer::ReadingThread(LPVOID pParam) //��Ŷ�� �д� ������
{
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	int result;
	
	CNILayer *pNI = (CNILayer *)pParam;  
	
	while(pNI->m_thrdSwitch) // ������ư�� ������� true 
	{
		result = pcap_next_ex(pNI->m_AdapterObject,&header,&pkt_data);// ��Ŷ �о����
		
		if(result==0){
		
		}
		else if(result==1){
		
			pNI->Receive((u_char *)pkt_data);// ���� �������� payload �ø� 
		}
		else if(result<0){
		
		}
	}

	return 0;
}
