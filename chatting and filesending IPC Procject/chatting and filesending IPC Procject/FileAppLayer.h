// FileAppLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "IPLayer.h"
#include "TCPLayer.h"
#include "IPCAppDlg.h"

static BOOL bGoLoop;
static BOOL bMSGCheck;
static BOOL bMSGFail;
static BOOL bFILE;
static int send_fileTotlen;

class CFileAppLayer 
	: public CBaseLayer
{
private:
	inline void		ResetHeader( );
	CObject* mp_Dlg ;

public:
	unsigned char	*m_ppayload;
	int				m_length;
	unsigned char	*m_FilePath;
	int				receive_fileTotlen;

	BOOL			Receive( unsigned char* ppayload );
	BOOL			Send( unsigned char* filePath );

	static UINT		FileThread( LPVOID pParam );	// FileApp 쓰레드

	CFileAppLayer( char* pName );
	virtual ~CFileAppLayer();

	typedef struct _FILE_APP {
		unsigned long	fapp_totlen ;					// 총 길이
		unsigned short	fapp_type ;						// 데이터 타입
		unsigned char	fapp_msg_type ;					// 메시지 종류
		unsigned char	unused;							// 사용 안함
		unsigned long	fapp_seq_num ;					// fragmentation 순서
		unsigned char	fapp_data[ APP_DATA_SIZE ] ;
	} FILE_APP, *LPFILE_APP ;

protected:
	FILE_APP	m_sHeader ;

	enum {			DATA_TYPE_BEGIN = 0x00,	// 전송 시작 부분
		DATA_TYPE_CONT = 0x01,	// 전송 중간 부분
		DATA_TYPE_END =  0x02 };// 전송   끝 부분

	enum {			MSG_TYPE_FRAG = 0x00,	// 단편화된 메시지
		MSG_TYPE_CHECK = 0x01,	// 수신받은 데이터에 대한 확인 메시지
		MSG_TYPE_FAIL = 0x02 };	// 잘못된 데이터를 받았을 때 보내는 메시지
};




