
#if _MSC_VER > 1000
#pragma once
#endif 
                                                         
#include "stdafx.h"
class CBaseLayer  
{
public:
   char*         GetLayerName( );
   CBaseLayer*      GetUnderLayer( );   
   CBaseLayer*      GetUpperLayer( int nindex );
   void         SetUnderUpperLayer( CBaseLayer* pUULayer = NULL );
   void         SetUpperUnderLayer( CBaseLayer* pUULayer = NULL );
   void         SetUnderLayer( CBaseLayer* pUnderLayer = NULL ) ;
   void         SetUpperLayer( CBaseLayer* pUpperLayer = NULL ) ;

   CBaseLayer( char* pName = NULL );   
   virtual ~CBaseLayer();            

   virtual   BOOL   Send( unsigned char* ppayload, int nlength,int dev_num)    { return FALSE ; }

   virtual   BOOL   Receive( unsigned char* ppayload ,int dev_num)    { return FALSE ; } 
   virtual   BOOL   Receive( )    { return FALSE ; }

   static HANDLE File;
   static HANDLE Transfer;
   static HANDLE Sending;

protected:
   char*         m_pLayerName;                     
   CBaseLayer*      mp_UnderLayer;                     
   CBaseLayer*      mp_aUpperLayer[ MAX_LAYER_NUMBER ];      
   int            m_nUpperLayerCount;                  
};
