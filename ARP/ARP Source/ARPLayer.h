
#if !defined(AFX_ARPLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_ARPLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "EthernetLayer.h"

class CARPLayer
: public CBaseLayer
{
private:
	inline void		ResetHeader( );

public:
	BOOL			Receive( unsigned char* ppayload ) ;
	BOOL			Send( unsigned char* ppayload, int nlength );

	CARPLayer( char* pName );
	virtual ~CARPLayer();

	// Set Type, code , Address
	void	SetHardType(unsigned short hardtype);
	void	SetProtType(unsigned short prottype);
	void	SetOpcode(unsigned short op);
	void	SetSenderEtherAddress(unsigned char* enet);
	void	SetSenderIPAddress(unsigned char* ip);
	void	SetTargetEtherAddress(unsigned char* enet);
	void	SetTargetIPAddress(unsigned char* ip);
	
	// Get Address
	unsigned char*	GetSenderEtherAddress();
	unsigned char*	GetSenderIPAddress();
	unsigned char*	GetTargetEtherAddress();
	unsigned char*	GetTargetIPAddress();
	
	// ARP Cache Table Struct : Linked List
	int			CacheAddItem(unsigned char* ppayload);
	void		CacheDeleteItem(int index);	
	void		CacheDeleteByTimer(int tID);
	void		CacheAllDelete();		
	int			CacheUpdate(unsigned char* ppayload); 
	BOOL		CacheSearchExist(IP_ADDR ip);
	CACHE_ENTRY GetSelectCacheEntry(IP_ADDR ip);

	// Proxy Table Entry Struct : Linked List
	void	ProxyAddItem(PROXY_ENTRY proxy);
	void	ProxyDeleteItem(int index);	
	BOOL	ProxySearchExist(IP_ADDR ip);

	// ARP Header Struct
	typedef struct _ARP_HEADER {
		unsigned short	arp_hardtype;
		unsigned short	arp_prottype;
		unsigned char	arp_hardsize;
		unsigned char	arp_protsize;
		unsigned short	arp_op;
		ETHERNET_ADDR	arp_srcenet;	// Sender MAC
		IP_ADDR			arp_srcip;		// Sender IP
		ETHERNET_ADDR	arp_dstenet;	// Target MAC
		IP_ADDR			arp_dstip;		// Target IP
	} ARP_HEADER, *PARP_HEADER ;

protected:
	ARP_HEADER	m_sHeader ;

public:
	LPARP_NODE cache_head;
	LPPARP_NODE proxy_head;

protected:
	CEthernetLayer* m_Ether;
};

#endif // !defined(AFX_ARPLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
