#include "StdAfx.h"
#include "NILayer.h"
CNILayer::CNILayer( char* pName ) : CBaseLayer( pName )
{

}

CNILayer::~CNILayer() { }
//Dlg���� ���õ� Device�� �������� ���ؼ� Dlg�������� �� �Լ��� ȣ���
void CNILayer::SetDevice(pcap_if_t *DeviceList,int dev_num)
{
   char ebuf[30];
   this->DeviceList[dev_num-1] = DeviceList;
   this->Device[dev_num-1] = pcap_open_live(this->DeviceList[dev_num-1]->name , 1500 , 1 , 1 , ebuf);
}

BOOL CNILayer::Send(unsigned char* ppayload, int nlength,int dev_num)
{
   // ��Ŷ�� ������ �Լ��� ppayload�� �������� packet�̵ǰ� ũ��� nlength�� �ȴ�.
   if(Device != NULL && pcap_sendpacket(Device[dev_num-1] , ppayload , nlength ) == 0) {
      return TRUE;
   } else {
      AfxMessageBox( "sendpacket error!", MB_OK | MB_ICONSTOP ) ;   // ���н� �޼���â ���
      return FALSE;
   }
}//���õ� Device�� ������ ��Ŷ�� ũ�Ⱑ 0�� �ƴϰ� �����Ұ�� ��Ŷ�� �����Ѵ�.

BOOL CNILayer::Receive(int dev_num)
{
   int result;
   result = pcap_next_ex(Device[dev_num-1],&pkt_header[dev_num-1],(const unsigned char **)&pkt_data[dev_num-1]);
   if(result == 1){
      mp_aUpperLayer[0]->Receive(pkt_data[dev_num-1],dev_num);   // ���� �������� �����Ѵ�.
      return TRUE;
   }
   mutex.Unlock();
   return FALSE;
} // Dlg ���� ���� �ҷ����� �Լ�! �����带 �����Ѵ�.
void CNILayer::StartReadThread()
{
   pThread_1 = AfxBeginThread(WaitPacket_1 , this);
   pThread_2 = AfxBeginThread(WaitPacket_2 , this);
   if(pThread_1 == NULL || pThread_2 == NULL) {
      AfxMessageBox("Read ������ ���� ����");
   }
}
// �����带 ������� ��Ŷ�� �޾��� ��� ���� ����(Ethernet)���� ������ ���� Receive �Լ��� ȣ���ؼ� ��Ŷ�� �޴´�.
// pThread �� ���� ���� 1,2 ���� �����带 ���������� ������� ������ ��Ŷ�� �������ִ�.
//�� ������ �� �ϳ��� ������� ���� ��� ������ ������ �����ߴ� �޽����� �����
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