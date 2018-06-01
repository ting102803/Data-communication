#include "StdAfx.h"
#include "NILayer.h"
CNILayer::CNILayer( char* pName ) : CBaseLayer( pName )
{

}

CNILayer::~CNILayer() { }
//Dlg에서 선택된 Device를 가져오기 위해서 Dlg계층에서 이 함수가 호출됨
void CNILayer::SetDevice(pcap_if_t *DeviceList,int dev_num)
{
   char ebuf[30];
   this->DeviceList[dev_num-1] = DeviceList;
   this->Device[dev_num-1] = pcap_open_live(this->DeviceList[dev_num-1]->name , 1500 , 1 , 1 , ebuf);
}

BOOL CNILayer::Send(unsigned char* ppayload, int nlength,int dev_num)
{
   // 패킷을 보내는 함수로 ppayload가 보내지는 packet이되고 크기는 nlength이 된다.
   if(Device != NULL && pcap_sendpacket(Device[dev_num-1] , ppayload , nlength ) == 0) {
      return TRUE;
   } else {
      AfxMessageBox( "sendpacket error!", MB_OK | MB_ICONSTOP ) ;   // 실패시 메세지창 출력
      return FALSE;
   }
}//선택된 Device의 보내는 패킷이 크기가 0이 아니고 존재할경우 패킷을 전송한다.

BOOL CNILayer::Receive(int dev_num)
{
   int result;
   result = pcap_next_ex(Device[dev_num-1],&pkt_header[dev_num-1],(const unsigned char **)&pkt_data[dev_num-1]);
   if(result == 1){
      mp_aUpperLayer[0]->Receive(pkt_data[dev_num-1],dev_num);   // 상위 계층으로 전달한다.
      return TRUE;
   }
   mutex.Unlock();
   return FALSE;
} // Dlg 에서 먼저 불려지는 함수! 쓰레드를 실행한다.
void CNILayer::StartReadThread()
{
   pThread_1 = AfxBeginThread(WaitPacket_1 , this);
   pThread_2 = AfxBeginThread(WaitPacket_2 , this);
   if(pThread_1 == NULL || pThread_2 == NULL) {
      AfxMessageBox("Read 쓰레드 생성 실패");
   }
}
// 쓰레드를 실행시켜 패킷을 받았을 경우 상위 계층(Ethernet)으로 보내기 위해 Receive 함수를 호출해서 패킷을 받는다.
// pThread 를 통해 각각 1,2 번의 쓰레드를 병렬적으로 실행시켜 각각의 패킷을 받을수있다.
//두 쓰레드 중 하나라도 실행되지 않을 경우 쓰래드 생성이 실패했단 메시지를 출력함
unsigned int CNILayer::WaitPacket_1(LPVOID pParam) 
{
   CNILayer *temp_NILayer = (CNILayer*)pParam;
   int result;
   while(1){
      temp_NILayer->Receive(1);
   }
   return 0;
}

unsigned int CNILayer::WaitPacket_2(LPVOID pParam){
   CNILayer *temp_NILayer = (CNILayer*)pParam;
   int result;
   while(1){
      temp_NILayer->Receive(2);
   }
   return 0;
}