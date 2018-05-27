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

	static UINT		FileThread( LPVOID pParam );	// FileApp ������

	CFileAppLayer( char* pName );
	virtual ~CFileAppLayer();

	typedef struct _FILE_APP {
		unsigned long	fapp_totlen ;					// �� ����
		unsigned short	fapp_type ;						// ������ Ÿ��
		unsigned char	fapp_msg_type ;					// �޽��� ����
		unsigned char	unused;							// ��� ����
		unsigned long	fapp_seq_num ;					// fragmentation ����
		unsigned char	fapp_data[ APP_DATA_SIZE ] ;
	} FILE_APP, *LPFILE_APP ;

protected:
	FILE_APP	m_sHeader ;

	enum {			DATA_TYPE_BEGIN = 0x00,	// ���� ���� �κ�
		DATA_TYPE_CONT = 0x01,	// ���� �߰� �κ�
		DATA_TYPE_END =  0x02 };// ����   �� �κ�

	enum {			MSG_TYPE_FRAG = 0x00,	// ����ȭ�� �޽���
		MSG_TYPE_CHECK = 0x01,	// ���Ź��� �����Ϳ� ���� Ȯ�� �޽���
		MSG_TYPE_FAIL = 0x02 };	// �߸��� �����͸� �޾��� �� ������ �޽���
};




