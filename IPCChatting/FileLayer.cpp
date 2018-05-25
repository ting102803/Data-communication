// FileLayer.cpp: implementation of the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ipc.h"
#include "FileLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileLayer::CFileLayer( char* pName )
: CBaseLayer( pName )
{
}

CFileLayer::~CFileLayer()
{
	TRY
	{
//////////////////////// fill the blank ///////////////////////////////
		CFile::Remove("IpcBuff.txt");
	}
	CATCH( CFileException, e )
	{
		#ifdef _DEBUG
			afxDump << "File cannot be removed\n";
		#endif
	}
	END_CATCH
}

BOOL CFileLayer::Send(unsigned char *ppayload, int nlength)
{
	TRY
	{
		CFile m_FileDes( "IpcBuff.txt", 
						 CFile::modeCreate | CFile::modeWrite ) ;
//////////////////////// fill the blank ///////////////////////////////
		m_FileDes.Write(ppayload, nlength);
		m_FileDes.Close();
	}
	CATCH( CFileException, e )
	{//텍스트파일로 ppayload를 저장한다
#ifdef _DEBUG
		  afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
		return FALSE ;
	}
	END_CATCH

	return TRUE ;
}

BOOL CFileLayer::Receive( )
{
	TRY
	{
		CFile m_FileDes( "IpcBuff.txt", CFile::modeRead ) ;

//////////////////////// fill the blank ///////////////////////////////********
		int nlength = ETHER_HEADER_SIZE+ETHER_MAX_DATA_SIZE;
		unsigned char* ppayload = new unsigned char[nlength+1];

		m_FileDes.Read(ppayload,nlength);
		ppayload[nlength]='\0';

		if(!mp_aUpperLayer[0]->Receive(ppayload)){
			m_FileDes.Close();
			return FALSE;}

		m_FileDes.Close( ) ;
	}
	CATCH( CFileException, e )
	{//텍스트를 읽어서 ETHERNET을 위한 형변환을 해준뒤 올려보낸다
#ifdef _DEBUG
		  afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
		  return FALSE ;
	}
	END_CATCH

	return TRUE ;
}
