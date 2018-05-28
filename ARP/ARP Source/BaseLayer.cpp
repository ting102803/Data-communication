

#include "stdafx.h"
#include "ARP.h"
#include "BaseLayer.h"

CBaseLayer::CBaseLayer( char* pName ) 
: m_nUpperLayerCount( 0 ), 
  mp_UnderLayer( NULL )
{
	m_pLayerName = pName ;
}

CBaseLayer::~CBaseLayer()
{

}

void CBaseLayer::SetUnderUpperLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer )
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderUpperLayer] The variable , 'pUULayer' is NULL" ) ;
#endif
		return ;			
	}

	this->mp_UnderLayer = pUULayer;
	pUULayer->SetUpperLayer(this);
}

void CBaseLayer::SetUpperUnderLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperUnderLayer] The variable , 'pUULayer' is NULL" ) ;
#endif
		return ;			
	}

	SetUpperLayer(pUULayer);	
	pUULayer->SetUnderLayer(this);
}

void CBaseLayer::SetUpperLayer(CBaseLayer *pUpperLayer )
{
	if ( !pUpperLayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperLayer] The variable , 'pUpperLayer' is NULL" ) ;
#endif
		return ;			
	}

	
	this->mp_aUpperLayer[ m_nUpperLayerCount++ ] = pUpperLayer ;
}

void CBaseLayer::SetUnderLayer(CBaseLayer *pUnderLayer)
{
	if ( !pUnderLayer )
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderLayer] The variable , 'pUnderLayer' is NULL\n" ) ;
#endif
		return ;			
	}

	this->mp_UnderLayer = pUnderLayer ;
}

CBaseLayer* CBaseLayer::GetUpperLayer(int nindex)
{
	if ( nindex < 0						||
		 nindex > m_nUpperLayerCount	||
		 m_nUpperLayerCount < 0			)
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUpperLayer] There is no UpperLayer in Array..\n" ) ;
#endif 
		return NULL ;
	}

	return mp_aUpperLayer[ nindex ] ;
}

CBaseLayer* CBaseLayer::GetUnderLayer()
{
	if ( !mp_UnderLayer )
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUnderLayer] There is not a UnerLayer..\n" ) ;
#endif 
		return NULL ;
	}

	return mp_UnderLayer ;
}

char* CBaseLayer::GetLayerName()
{
	return m_pLayerName ; 
}

