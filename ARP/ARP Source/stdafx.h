

#if !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
#define AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.

/* �߰� header */
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����

#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

/* �߰� ��ũ�� */
#define MAX_LAYER_NUMBER		0xff

#define ETHER_MAX_SIZE			1514
#define ETHER_HEADER_SIZE		14
#define ETHER_MAX_DATA_SIZE		( ETHER_MAX_SIZE - ETHER_HEADER_SIZE )

#define ARP_HARDTYPE			0x0001	// Ethernet frame
#define ARP_PROTOTYPE_IP		0x0800
#define ARP_PROTOTYPE_ARP		0x0806
#define ARP_PROTOTYPE_RARP		0x0835
#define ARP_OPCODE_REQUEST		0x0001
#define ARP_OPCODE_REPLY		0x0002
#define ARP_HEADER_SIZE			28

#define IP_HEADER_SIZE			( sizeof(unsigned short) * 4 +	\
								  sizeof(unsigned char) * 12  )
#define IP_DATA_SIZE			( ETHER_MAX_SIZE - (	ARP_HEADER_SIZE + \
														IP_HEADER_SIZE  + \
														ETHER_HEADER_SIZE ))

#define CACHE_INCOMPLETE		0x00
#define CACHE_COMPLETE			0x01
#define CACHE_TTL				10

#define TIMER_REPLYTIME			180000
#define TIMER_VALIDTIME			1200000

#define NI_COUNT_NIC			10

#define PM_DEVICE_NAME_LENGTH	20


// Ethernet Address
typedef struct _ETHERNET_ADDR
{
	union {
		struct { unsigned char e0, e1, e2, e3, e4, e5; } s_un_byte ;
		unsigned char s_ether_addr[6] ;
	} S_un ;

#define addr_e0 S_un.s_un_byte.e0
#define addr_e1 S_un.s_un_byte.e1
#define addr_e2 S_un.s_un_byte.e2
#define addr_e3 S_un.s_un_byte.e3
#define addr_e4 S_un.s_un_byte.e4
#define addr_e5 S_un.s_un_byte.e5
#define addrs_e S_un.s_ether_addr
} ETHERNET_ADDR, *LPETHERNET_ADDR ;

// IP Address
typedef struct _IP_ADDR
{
	union {
		struct { unsigned char i0, i1, i2, i3; } s_un_byte ;
		unsigned char s_ip_addr[4] ;
	} S_un ;

#define addr_i0 S_un.s_un_byte.i0
#define addr_i1 S_un.s_un_byte.i1
#define addr_i2 S_un.s_un_byte.i2
#define addr_i3 S_un.s_un_byte.i3
#define addrs_i S_un.s_ip_addr
} IP_ADDR, *LPIP_ADDR ;

// CACHE Entry
typedef struct _CACHE_ENTRY
{
	IP_ADDR			cache_ipaddr;
	ETHERNET_ADDR	cache_enetaddr;
	unsigned char	cache_type;
	unsigned short	cache_ttl;
}CACHE_ENTRY, *LPCACHE_ENTRY;

// Linked List
typedef struct _ARP_NODE
{
	CACHE_ENTRY			key;
	struct _ARP_NODE	*next;
}ARP_NODE, *LPARP_NODE;

// PROXY Entry
typedef struct _PROXY_ENTRY
{
	IP_ADDR			proxy_ipaddr;
	ETHERNET_ADDR	proxy_enetaddr;
	unsigned char	proxy_devname[PM_DEVICE_NAME_LENGTH];
}PROXY_ENTRY, *LPPROXY_ENTRY;

// Linked List
typedef struct _PARP_NODE
{
	PROXY_ENTRY			key;
	struct _PARP_NODE	*next;
}PARP_NODE, *LPPARP_NODE;

#endif // !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)