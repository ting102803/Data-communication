
#include "stdafx.h"
#include "ARP.h"
#include "ARPLayer.h"



CARPLayer::CARPLayer( char* pName )
: CBaseLayer( pName )
{
   ResetHeader( ) ;

   //캐쉬Table을 업데이트 하기 위해 쓰이는 헤드 노드
   cache_head = NULL; 
   proxy_head = NULL;
}

CARPLayer::~CARPLayer()
{
}

void CARPLayer::ResetHeader()
{
   m_sHeader.arp_hardtype = 0x0000;
   m_sHeader.arp_prottype = 0x0000;
   m_sHeader.arp_hardsize = 0x06;
   m_sHeader.arp_protsize = 0x04;
   m_sHeader.arp_op = 0x0000;
   memset(m_sHeader.arp_srcenet.addrs_e, 0, 6);
   memset(m_sHeader.arp_srcip.addrs_i, 0, 4);
   memset(m_sHeader.arp_dstenet.addrs_e, 0, 6);
   memset(m_sHeader.arp_dstip.addrs_i, 0, 4);
}//ARP 헤더 초기화

void CARPLayer::SetHardType(unsigned short hardtype)
{
   m_sHeader.arp_hardtype = hardtype;
}
void CARPLayer::SetProtType(unsigned short prottype)
{
   m_sHeader.arp_prottype = prottype;
}

void CARPLayer::SetOpcode(unsigned short op)
{
   m_sHeader.arp_op = op;
}
void CARPLayer::SetSenderEtherAddress(unsigned char* enet)
{
   ETHERNET_ADDR s_ether;
   s_ether.addr_e0 = enet[0];
   s_ether.addr_e1 = enet[1];
   s_ether.addr_e2 = enet[2];
   s_ether.addr_e3 = enet[3];
   s_ether.addr_e4 = enet[4];
   s_ether.addr_e5 = enet[5];

   memcpy(m_sHeader.arp_srcenet.addrs_e, s_ether.addrs_e, 6);
 }
void CARPLayer::SetSenderIPAddress(unsigned char* ip)
{
   IP_ADDR s_ip;
   s_ip.addr_i0 = ip[0];
   s_ip.addr_i1 = ip[1];
   s_ip.addr_i2 = ip[2];
   s_ip.addr_i3 = ip[3];

   memcpy(m_sHeader.arp_srcip.addrs_i, s_ip.addrs_i, 4);
}
void CARPLayer::SetTargetEtherAddress(unsigned char* enet)
{
   ETHERNET_ADDR t_ether;
   t_ether.addr_e0 = enet[0];
   t_ether.addr_e1 = enet[1];
   t_ether.addr_e2 = enet[2];
   t_ether.addr_e3 = enet[3];
   t_ether.addr_e4 = enet[4];
   t_ether.addr_e5 = enet[5];

   memcpy(m_sHeader.arp_dstenet.addrs_e, t_ether.addrs_e, 6);
}
void CARPLayer::SetTargetIPAddress(unsigned char* ip)
{
   IP_ADDR t_ip;
   t_ip.addr_i0 = ip[0];
   t_ip.addr_i1 = ip[1];
   t_ip.addr_i2 = ip[2];
   t_ip.addr_i3 = ip[3];

   memcpy(m_sHeader.arp_dstip.addrs_i, t_ip.addrs_i, 4);
}
//ARP를 보낼때 필요한 비트 SET하는 함수들


unsigned char* CARPLayer::GetSenderEtherAddress()
{
   return m_sHeader.arp_srcenet.addrs_e;
}
unsigned char* CARPLayer::GetSenderIPAddress()
{
   return m_sHeader.arp_srcip.addrs_i;
}
unsigned char* CARPLayer::GetTargetEtherAddress()
{
   return m_sHeader.arp_dstenet.addrs_e;
}
unsigned char* CARPLayer::GetTargetIPAddress()
{
   return m_sHeader.arp_dstip.addrs_i;
}

//받은 ARP 헤더들을 리턴시키는 함수
BOOL CARPLayer::Send(unsigned char *ppayload, int nlength)
{
   BOOL bSuccess = FALSE ;
   bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
   return bSuccess ;
}
   // 헤더를 하위 레이어로 보내는 함수
BOOL CARPLayer::Receive( unsigned char* ppayload )
{
   // 하위 Layer에서 받은 ppayload를 ARP Layer Header구조에 캐스팅해서 읽음
   PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
   BOOL bSuccess = FALSE ;
   
   // 하위 Layer인 Ethernet Layer 객체 생성
   m_Ether = (CEthernetLayer *)GetUnderLayer();

   int index;

   // 아래의 4가지 조건을 만족하는 경우는 "ARP" 
   // ARP 메시지의 Destination IP와 내 IP가 같음
   // ARP 메시지의 Source IP와 자신 IP가 다름 
   // ARP 메시지의 Destination IP와 source IP가 다름
   // Proxy Cache Entry에 없음
   if( ( memcmp((char *)pFrame->arp_dstip.addrs_i,(char*)m_sHeader.arp_srcip.addrs_i,4) == 0 ) &&
      ( memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)m_sHeader.arp_srcip.addrs_i,4)      ) &&
      ( memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)pFrame->arp_dstip.addrs_i,4)        ) &&
      ( ! ProxySearchExist(pFrame->arp_dstip) ) )
   {
      if(ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST) // 상대방에게 요청 받은 경우
      {
         m_Ether->SetEnetDstAddress(pFrame->arp_srcenet.addrs_e);
         m_Ether->SetEnetSrcAddress(m_sHeader.arp_srcenet.addrs_e);
         SetHardType(ntohs(ARP_HARDTYPE));
         SetProtType(ntohs(ARP_PROTOTYPE_IP));
         SetOpcode(ntohs(ARP_OPCODE_REPLY));
         SetSenderEtherAddress(m_sHeader.arp_srcenet.addrs_e);
         SetSenderIPAddress(m_sHeader.arp_srcip.addrs_i);
         SetTargetEtherAddress(pFrame->arp_srcenet.addrs_e);
         SetTargetIPAddress(pFrame->arp_srcip.addrs_i);
         
     
         index = CacheAddItem(ppayload);
       // Cache Table에 추가 하고, 상위 Layer로 전달하여 Receive
         bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)index);
         mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
      }     // 헤더를 하위 레이어로 전달
      else if(ntohs(pFrame->arp_op) == ARP_OPCODE_REPLY) // 상대방에게 응답 받은 경우
      {
         index = CacheUpdate(ppayload);
         bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)index);
       // Cache Table을 Update 해주고, 상위 Layer로 전달하여 Receive
      }
   }

   // "Gratitous ARP"
   // Entry에 등록된 IP주소에 맥주소를 변경하는것이다.
   // ARP 메시지의 Source IP와 Destination과 같음.
    else if(memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)pFrame->arp_dstip.addrs_i,4) == 0)
   {
      // ARP 메시지의 opcode가 Request(2) 이고, Source IP와 내 IP가 같으면
      // opcode를 Reply(1)로 바꿔서, MAC주소를 입력한 MAC주소로 변경하여 하위 Layer에 전달하여 전송한다
      if( ( ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST ) &&
         ( memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)m_sHeader.arp_srcip.addrs_i,4) == 0 ) )
      {
         SetOpcode(ntohs(ARP_OPCODE_REPLY));
         SetSenderEtherAddress(m_sHeader.arp_srcenet.addrs_e);
         mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
      }
      // ARP 메시지의 opcode가 Request(2) 이고, 이미 ARP Cache Table에 존재한다면
      // Cache Table을 Update 해주고, 상위 Layer로 전달하여 Receive
      else if( ( ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST ) &&
             ( CacheSearchExist(pFrame->arp_dstip) ) )
      {
         index = CacheUpdate(ppayload);
         bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*) index);
      }
   }
   // 위의 두 조건이 모두 만족하지 않는다면 "Proxy ARP"
   else
   {
      // ARP 메시지의 opcode가 Request(2) 이고, Proxy ARP Entry에 존재한다면
      if( ( ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST ) &&
          ( ProxySearchExist(pFrame->arp_dstip) ) )
      {
         // 상대가 요청한 IP 주소와 MAC 주소를 등록후 보낸다.
         m_Ether->SetEnetDstAddress(pFrame->arp_srcenet.addrs_e);
         m_Ether->SetEnetSrcAddress(m_sHeader.arp_srcenet.addrs_e);
         SetHardType(ntohs(ARP_HARDTYPE));
         SetProtType(ntohs(ARP_PROTOTYPE_IP));
         SetOpcode(ntohs(ARP_OPCODE_REPLY));
         // Sender의 MAC 주소와 IP 주소는 Proxy ARP에서 지정
         SetTargetEtherAddress(pFrame->arp_srcenet.addrs_e);
         SetTargetIPAddress(pFrame->arp_srcip.addrs_i);
         
         // ARP Cache Table에 Item 추가
         CacheAddItem(ppayload);

         // 상위 Layer에 전달하여 Receive (256은 Byte Ordering 해서 0x100 -> 0x001)
         bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*) 256);

         mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
      }
   }

   return bSuccess;
}

// Cash 테이블
int CARPLayer::CacheAddItem(unsigned char* ppayload)
{
   CACHE_ENTRY cache;
   LPARP_NODE currNode;
   LPARP_NODE newNode = (LPARP_NODE)malloc(sizeof(ARP_NODE));

   if(ppayload == NULL)
   {
      // Cache entry Node 생성
      cache.cache_enetaddr = m_sHeader.arp_dstenet;
      cache.cache_ipaddr = m_sHeader.arp_dstip;
      cache.cache_type = CACHE_INCOMPLETE;
      cache.cache_ttl = CACHE_TTL;
   }
   else
   {
      PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
      cache.cache_enetaddr = pFrame->arp_srcenet;
      cache.cache_ipaddr = pFrame->arp_srcip;
      cache.cache_type = CACHE_COMPLETE;
      cache.cache_ttl = CACHE_TTL;
   }   
   newNode->key = cache;
   newNode->next = NULL;

   // IP 중복이있는지 검사한다.
   currNode = cache_head;
   while(currNode!=NULL)
   {
      if(!memcmp(currNode->key.cache_ipaddr.addrs_i,newNode->key.cache_ipaddr.addrs_i,4))
      {
         return -1;
      }
      currNode = currNode->next;
   }
   if(!memcmp(m_sHeader.arp_dstip.addrs_i,m_sHeader.arp_srcip.addrs_i,4))
   {
      return -1;
   }

   // Cache entry에 추가
   currNode = cache_head;
   if(cache_head == NULL)
   {
      newNode->next = cache_head;
      cache_head = newNode;
   }
   else
   {
      while(currNode->next!=NULL)
         currNode = currNode->next;
      newNode->next = currNode->next;
      currNode->next = newNode;
   }

   return newNode->key.cache_ipaddr.addr_i3;
}

void CARPLayer::CacheDeleteItem(int index)
{
   LPARP_NODE currNode;
   LPARP_NODE tempNode;

   int nodeIndex = 0;

   // Cache entry에서 삭제
   currNode = cache_head;
   if(index==0)//인덱스가 0이면
   {
      tempNode = cache_head;
      cache_head = cache_head->next;//cache를 next 로옮겨서 cache 를 null로만든다
      free(tempNode);
   }
   else
   {
      index--;
      while(currNode->next!=NULL){
         if(index == nodeIndex)
            break;
         currNode = currNode->next;
         nodeIndex++;
      }
      tempNode = currNode->next;
      currNode->next = tempNode->next;
      free(tempNode);
   }
}

void CARPLayer::CacheAllDelete()
{
   // Cache entry 초기화
   LPARP_NODE tempNode;
   while(cache_head!=NULL)
   {
      tempNode = cache_head;
      cache_head = cache_head->next;
      free(tempNode);
   }
}

void CARPLayer::CacheDeleteByTimer(int tID)
{
   // Cache entry에서 삭제

   LPARP_NODE currNode;
   LPARP_NODE tempNode;

   int ipIndex = 0;

   currNode = cache_head;
   if(currNode==NULL)
      return;

   if(currNode->next==NULL)
   {
      tempNode = cache_head;
      cache_head = cache_head->next;//cache를 next 로옮겨서 cache 를 null로 만든다
      free(tempNode);
   }
   else
   {
      while(currNode->next!=NULL){
         if(currNode == cache_head && currNode->key.cache_ipaddr.addr_i3==tID)
         {
            tempNode = currNode;
            currNode = tempNode->next;
            free(tempNode);
            cache_head = currNode;
            break;
         }
         if(currNode->next->key.cache_ipaddr.addr_i3==tID)
         {
            tempNode = currNode->next;
            currNode->next = tempNode->next;
            free(tempNode);
            break;
         }      
         currNode = currNode->next;
      }
   }
}

int CARPLayer::CacheUpdate(unsigned char* ppayload)
{
   // Cache table 갱신
   // Cache table의 Mac address를 갱신하고, 타이머 번호를 return해준다.
   PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
   LPARP_NODE currNode;

   currNode = cache_head;
   while(currNode!=NULL)
   {
      if(!memcmp(currNode->key.cache_ipaddr.addrs_i,pFrame->arp_srcip.addrs_i,4))
      {
         memcpy(currNode->key.cache_enetaddr.addrs_e,pFrame->arp_srcenet.addrs_e,6);
         currNode->key.cache_type = CACHE_COMPLETE;
         break;
      }
      currNode = currNode->next;
   }
   if(currNode==NULL) // 들어온 ip주소가 Cache table에 없는 경우 : 나에게 ARP를 요청한 경우
      return pFrame->arp_srcip.addr_i3+255;
   return currNode->key.cache_ipaddr.addr_i3;
}

BOOL CARPLayer::CacheSearchExist(IP_ADDR ip)
{
   // Cache table에 찾고자 하는 ip가 존재하는지 검사
   LPARP_NODE currNode;
   
   currNode = cache_head;
   while(currNode!=NULL)
   {
      if(!memcmp(currNode->key.cache_ipaddr.addrs_i,ip.addrs_i,4))
      {
         return TRUE;
      }
      currNode = currNode->next;
   }
   return FALSE;
   // 찾으면 TRUE, 못찾으면 FALSE Return 한다
}

CACHE_ENTRY CARPLayer::GetSelectCacheEntry(IP_ADDR ip)
{
   // Cache table에 있는 etnry를 return.
   LPARP_NODE currNode;

   currNode = cache_head;
   while(currNode!=NULL)
   {
      if(!memcmp(currNode->key.cache_ipaddr.addrs_i,ip.addrs_i,4))
      {
         if(currNode->key.cache_type == CACHE_COMPLETE)
         {
            return currNode->key;
         }
      }
      currNode = currNode->next;
   }
   memset(currNode->key.cache_enetaddr.addrs_e,0,6);
   return currNode->key;
}

// Proxy Table
void CARPLayer::ProxyAddItem(PROXY_ENTRY proxy)
{
   // Proxy table에 추가
   LPPARP_NODE currNode;
   LPPARP_NODE newNode = (LPPARP_NODE)malloc(sizeof(PARP_NODE));

   newNode->key = proxy;
   newNode->next = NULL;

   // IP 중복 검사
   currNode = proxy_head;
   while(currNode!=NULL)
   {
      if(!memcmp(currNode->key.proxy_ipaddr.addrs_i,newNode->key.proxy_ipaddr.addrs_i,4))
      {
         return;
      }
      currNode = currNode->next;
   }

   // Proxy entry에 추가
   currNode = proxy_head;
   if(proxy_head == NULL)
   {
      newNode->next = proxy_head;
      proxy_head = newNode;
   }
   else
   {
      while(currNode->next!=NULL)
         currNode = currNode->next;
      newNode->next = currNode->next;
      currNode->next = newNode;
   }
}

void CARPLayer::ProxyDeleteItem(int index)
{
   LPPARP_NODE currNode;
   LPPARP_NODE tempNode;

   int nodeIndex = 0;   
   
   // Proxy entry에서 삭제
   currNode = proxy_head;
   if(index==0)
   {
      tempNode = proxy_head;
      proxy_head = proxy_head->next;
      free(tempNode);
   }
   else
   {
      index--;
      while(currNode->next!=NULL){
         if(index == nodeIndex)
            break;
         currNode = currNode->next;
         nodeIndex++;
      }
      tempNode = currNode->next;
      currNode->next = tempNode->next;
      free(tempNode);
   }
}

BOOL CARPLayer::ProxySearchExist(IP_ADDR ip)
{
   // Proxy table에 찾고자 하는 ip가 있는지 검사
   
   LPPARP_NODE currNode;
   
   currNode = proxy_head;
   while(currNode!=NULL)
   {
      if(!memcmp(currNode->key.proxy_ipaddr.addrs_i,ip.addrs_i,4))
      {
         // 요청 받은 ip로 proxy table에서 값을 찾아 ethernet과 ip 주소를 설정
         SetSenderEtherAddress(currNode->key.proxy_enetaddr.addrs_e);
         SetSenderIPAddress(currNode->key.proxy_ipaddr.addrs_i);
         return TRUE;
      }
      currNode = currNode->next;
   }
   return FALSE;
   // 찾으면 TRUE, 못찾으면 FALSE return한다.
}