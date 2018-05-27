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
	m_iNumAdapter = iNumAdapter; //장치번호
	m_thrdSwitch = TRUE;
	SetAdapterList(NULL);
}

CNILayer::~CNILayer()
{
}

void CNILayer::PacketStartDriver()//네트워크 연결을 여는 함수
{
	char errbuf[PCAP_ERRBUF_SIZE];//에러를 저장할 변수

	//네트워크 장치 번호가 잘못되었을때
	if(m_iNumAdapter < 0 ){
		AfxMessageBox("Not exist NICard");
		return;
	}
	// packet capture descriptor(PCD) 생성, 패킷 캡쳐 시작
	m_AdapterObject = pcap_open_live(m_pAdapterList[m_iNumAdapter]->name,1500,PCAP_OPENFLAG_PROMISCUOUS,2000,errbuf);
	if(!m_AdapterObject){ // PCD가 잘못 생성된 경우
		AfxMessageBox(errbuf); //에러 출력
		return;
	}
	AfxBeginThread(ReadingThread, this); // 패킷 리딩 스레드 시작
}

pcap_if_t *CNILayer::GetAdapterObject(int iIndex) // 네트워크 장치 리턴
{
	return m_pAdapterList[iIndex];
}

char *CNILayer::GetIPAddress(int iIndex) //ip 주소 리턴
{
	return ipAddr[iIndex];
}


char *iptos(u_long in) //IP를 문자열 형식으로 변환하는 함수  
{
    static char output[IPTOSBUFFERS][3*4+3+1];// ip주소를 저장할 배열
    static short number;
    u_char *p;
 
    p = (u_char *)&in; 
    number = (number + 1 == IPTOSBUFFERS ? 0 : number + 1);
    sprintf(output[number], "%d.%d.%d.%d", p[3], p[2], p[1], p[0]);	//역순으로 출력 되므로 역순으로 조합
    return output[number];
}

void CNILayer::SetAdapterNumber(int iNum) //네트워크 장치 번호 설정
{
	m_iNumAdapter = iNum;
}

void CNILayer::SetAdapterList(LPADAPTER *plist) //네트워크 장치 리스트 생성
{
	pcap_if_t *alldevs; 
	pcap_if_t *d;
	pcap_addr_t *a;
	int i=0;
	int j=0;
	
	char errbuf[PCAP_ERRBUF_SIZE];

	for(int j=0;j<NI_COUNT_NIC;j++) //네트워크 장치 리스트 초기화
	{
		m_pAdapterList[j] = NULL;
	}

	if(pcap_findalldevs(&alldevs, errbuf) == -1) //네트워크 장치 정보 가져옴
	{
		AfxMessageBox("Not exist NICard");
		return;
	}
	if(!alldevs) // 디바이스가 하나도 없을 때 에러
	{
		AfxMessageBox("Not exist NICard");
		return;
	}

	for(d=alldevs; d; d=d->next) // 네트워크 장치 리스트 입력
	{
		m_pAdapterList[i] = d;

		 for(a=d->addresses;a;a=a->next) {
			
			ipAddr[i] = iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr);
		 }
		 i++;
	}
}

BOOL CNILayer::Send(unsigned char *ppayload, int nlength) //패킷을 보내는 함수
{
	if(pcap_sendpacket(m_AdapterObject,ppayload,nlength))
	{
		AfxMessageBox("패킷 전송 실패");
		return FALSE;
	}
	return TRUE;
}

BOOL CNILayer::Receive( unsigned char* ppayload ) //패킷을 받는 함수
{
	BOOL bSuccess = FALSE;

	bSuccess = mp_aUpperLayer[0]->Receive(ppayload);// 상위 계층으로 payload 올림
	return bSuccess;
}

UINT CNILayer::ReadingThread(LPVOID pParam) //패킷을 읽는 쓰레드
{
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	int result;
	
	CNILayer *pNI = (CNILayer *)pParam;  
	
	while(pNI->m_thrdSwitch) // 설정버튼을 누른경우 true 
	{
		result = pcap_next_ex(pNI->m_AdapterObject,&header,&pkt_data);// 패킷 읽어오기
		
		if(result==0){
		
		}
		else if(result==1){
		
			pNI->Receive((u_char *)pkt_data);// 상위 계층으로 payload 올림 
		}
		else if(result<0){
		
		}
	}

	return 0;
}
