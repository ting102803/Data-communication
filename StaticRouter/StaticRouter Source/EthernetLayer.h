#pragma once
#include "baselayer.h"

class CEthernetLayer : public CBaseLayer  
{
private:
   inline void      ResetHeader( );         

public:
   BOOL         Receive( unsigned char* ppayload,int dev_num ) ;      
   BOOL         Send( unsigned char* ppayload, int nlength,int dev_num );

   //mac1
   BOOL         Send( unsigned char* ppayload, int nlength ,unsigned short type,int dev_num);
   void         SetDestinAddress( unsigned char* pAddress);
   void         SetSourceAddress( unsigned char* pAddress,int dev_num );
   unsigned char*   GetDestinAddress();
   unsigned char*   GetSourceAddress(int dev_num);

   //mac2

   void SetType(unsigned short type);

   CEthernetLayer( char* pName );
   virtual ~CEthernetLayer();

   typedef struct _ETHERNET_ADDR
   {
      union   {
         struct { unsigned char e0, e1, e2, e3, e4, e5; } s_un_byte;
         unsigned char s_ether_addr[6];
      } S_un;

   #define addr0 S_un.s_un_byte.e0
   #define addr1 S_un.s_un_byte.e1
   #define addr2 S_un.s_un_byte.e2
   #define addr3 S_un.s_un_byte.e3
   #define addr4 S_un.s_un_byte.e4
   #define addr5 S_un.s_un_byte.e5
   #define addr_ethernet S_un.s_ether_addr

   } ETHERNET_ADDR, *LPETHERNET_ADDR;   

   typedef struct _ETHERNET
   {
      ETHERNET_ADDR Ethernet_dstAddr;      
      ETHERNET_ADDR Ethernet_srcAddr;      
      unsigned short Ethernet_type;   
      unsigned char Ethernet_data[ ETHERNET_MAX_DATA ]; 
   } EthernetHeader, *PEthernetHeader;         
   unsigned char dev_1_mac_addr[6];
   unsigned char dev_2_mac_addr[6];

   EthernetHeader   Ethernet_Header;            
};