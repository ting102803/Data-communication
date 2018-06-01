

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN           
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS     


#define _AFX_ALL_WARNINGS
#define HAVE_REMOTE
#define VC_EXTRALEAN		
#include <afxwin.h>         
#include <afxext.h>         
#include <afxdisp.h>        
#include <pcap.h>			// 추가한 부분

#include "Packet32.h"		// 추가한 부분
#include <String.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     
#endif 

#define MAX_LAYER_NUMBER		0xff	

#define ETHERNET_MAX_SIZE			1514
#define ETHERNET_HEADER_SIZE		14
#define ETHERNET_MAX_DATA			( ETHERNET_MAX_SIZE - ETHERNET_HEADER_SIZE )

#define IP_HEADER_SIZE				20
#define IP_MAX_DATA					( ETHERNET_MAX_DATA - IP_HEADER_SIZE )

#define TCP_HEADER_SIZE				20
#define TCP_MAX_DATA				( IP_MAX_DATA - TCP_HEADER_SIZE )

#define complete					0x00
#define incomplete					0x01

#define request						htons(0x01)
#define reply						htons(0x02)

#define ARP_MESSAGE_SIZE			28

#define delete_timer				0x01 // 1초마다 삭제 타이머
#define wait_timer					0x02 // 2초 기다리는 타이머
#define gratuitous_timer			0x03 //gratuitous timer

#define ip_type						htons(0x0800)
#define arp_type					htons(0x0806)


#ifdef _UNICODE

#endif


