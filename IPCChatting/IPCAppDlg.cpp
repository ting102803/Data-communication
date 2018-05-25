// IPCAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ipc.h"
#include "IPCAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg dialog

CIPCAppDlg::CIPCAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPCAppDlg::IDD, pParent), 
	  CBaseLayer( "ChatDlg" ),
	  m_bSendReady( FALSE ),
	  m_nAckReady( -1 ) 
{
	//{{AFX_DATA_INIT(CIPCAppDlg)
	m_unDstAddr = 0;
	m_unSrcAddr = 0;
	m_stMessage = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_LayerMgr.AddLayer( new CChatAppLayer( "ChatApp" ) ) ;
	m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ;
	m_LayerMgr.AddLayer( new CFileLayer( "File" ) ) ;
	m_LayerMgr.AddLayer( this ) ;

//////////////////////// fill the blank ///////////////////////////////
	m_LayerMgr.ConnectLayers("File ( *Ethernet  ( *ChatApp ( *ChatDlg ) )  )" );
	m_ChatApp = (CChatAppLayer*) mp_UnderLayer ;
}

void CIPCAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPCAppDlg)
	DDX_Control(pDX, IDC_LIST_CHAT, m_ListChat);
	DDX_Text(pDX, IDC_EDIT_DST, m_unDstAddr);
	DDX_Text(pDX, IDC_EDIT_SRC, m_unSrcAddr);
	DDX_Text(pDX, IDC_EDIT_MSG, m_stMessage);
	//}}AFX_DATA_MAP
}

// 레지스트리에 등록하기 위한 변수
UINT nRegSendMsg ;
UINT nRegAckMsg ;

BEGIN_MESSAGE_MAP(CIPCAppDlg, CDialog)
	//{{AFX_MSG_MAP(CIPCAppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnSendMessage)
	ON_BN_CLICKED(IDC_BUTTON_ADDR, OnButtonAddrSet)
	ON_BN_CLICKED(IDC_CHECK_TOALL, OnCheckBroadcast)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE( nRegSendMsg, OnRegSendMsg )
//////////////////////// fill the blank ///////////////////////////////
	ON_REGISTERED_MESSAGE( nRegAckMsg, OnRegAckMsg )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg message handlers

BOOL CIPCAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetRegstryMessage( ) ;
	SetDlgState( IPC_INITIALIZING ) ;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIPCAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( nID == SC_CLOSE )
	{
		if ( MessageBox( "Are you sure ?", 
						 "Question", 
						 MB_YESNO | MB_ICONQUESTION ) 
			 == IDNO )
			return ;
		else EndofProcess( ) ;
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}//프로그램을 종료할때 나오는 메세지들을 정의해놓은 함수이다

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIPCAppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIPCAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CIPCAppDlg::OnSendMessage() //센드 메세지를 눌렀을 경우
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE ) ;

	if ( !m_stMessage.IsEmpty() )
	{
		SetTimer( 1, 2000, NULL ) ;//2초후 온타이머가 실행된다
		m_nAckReady = 0 ;//타이머의 실행유무를 나타내기위한 변수

		SendData( ) ;
		m_stMessage = "" ;

		(CEdit*) GetDlgItem( IDC_EDIT_MSG )->SetFocus( ) ;

//////////////////////// fill the blank ///////////////////////////////
		::SendMessage(HWND_BROADCAST, nRegSendMsg, 0, 0);

	}

	UpdateData( FALSE ) ;
}

void CIPCAppDlg::OnButtonAddrSet() //프로그램에서 설정을 눌렀을경우
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE ) ;

	if ( !m_unDstAddr || 
		 !m_unSrcAddr  )
	{
		MessageBox( "주소를 설정 오류발생", 
					"경고", 
					MB_OK | MB_ICONSTOP ) ;

		return ;
	}

	if ( m_bSendReady ){
		SetDlgState( IPC_ADDR_RESET ) ;
		SetDlgState( IPC_INITIALIZING ) ;
	}
	else{
		m_ChatApp->SetSourceAddress( m_unSrcAddr ) ;
		m_ChatApp->SetDestinAddress( m_unDstAddr ) ;

		SetDlgState( IPC_ADDR_SET ) ;
		SetDlgState( IPC_READYTOSEND ) ;
	}

	m_bSendReady = !m_bSendReady ;
}

void CIPCAppDlg::OnCheckBroadcast() //브로드캐스팅기능을 눌렀을경우
{
	// TODO: Add your control notification handler code here
	CButton* pChkButton = (CButton*) GetDlgItem( IDC_CHECK_TOALL ) ;

	if ( pChkButton->GetCheck( ) )	SetDlgState( IPC_BROADCASTMODE ) ;
	else							SetDlgState( IPC_UNICASTMODE ) ;
}

void CIPCAppDlg::SetRegstryMessage()
{
	nRegSendMsg = RegisterWindowMessage( "Send IPC Message" ) ;
//////////////////////// fill the blank ///////////////////////////////
	nRegAckMsg = RegisterWindowMessage( "Ack IPC Message" ) ;
}

void CIPCAppDlg::SendData()//전송을 눌렀을경우
{
	CString MsgHeader ; 
	if ( m_unDstAddr == (unsigned int)0xff )
		MsgHeader.Format( "[%d:BROADCAST] ", m_unSrcAddr ) ;
	else
		MsgHeader.Format( "[%d:%d] ", m_unSrcAddr, m_unDstAddr ) ;//일반인지 브로드캐스팅인지에따라 포맷을 설정

	m_ListChat.AddString( MsgHeader + m_stMessage ) ;//메세지를 추가

//////////////////////// fill the blank ///////////////////////////////**
	int nlength = m_stMessage.GetLength();
	unsigned char* ppayload = new unsigned char[nlength+1];
	memcpy(ppayload,(unsigned char*)(LPCTSTR)m_stMessage, nlength);
	ppayload[nlength]='\0';
	m_ChatApp->Send(ppayload, m_stMessage.GetLength());
}//ppayload로 변경후에 하위 계층으로 전송

BOOL CIPCAppDlg::Receive(unsigned char *ppayload)
{
	if ( m_nAckReady == -1 )//타이머가 작동안하고 메세지를 잘 주고받았을 경우
	{
//////////////////////// fill the blank ///////////////////////////////
		::SendMessage(HWND_BROADCAST, nRegAckMsg, 0, 0);
	}

	m_ListChat.AddString( (char*) ppayload ) ;//받은 데이터를 출력한다
	return TRUE ;
}

BOOL CIPCAppDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( pMsg->message )
	{
	case WM_KEYDOWN :
		switch( pMsg->wParam )
		{
		case VK_RETURN : 
			if ( ::GetDlgCtrlID( ::GetFocus( ) ) == IDC_EDIT_MSG ) 
				OnSendMessage( ) ;					return FALSE ;
		case VK_ESCAPE : return FALSE ;
		}
		break ;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CIPCAppDlg::SetDlgState(int state)
{
	UpdateData( TRUE ) ;

	CButton*	pChkButton = (CButton*) GetDlgItem( IDC_CHECK_TOALL ) ;

	CButton*	pSendButton = (CButton*) GetDlgItem( IDC_BUTTON_SEND ) ;
	CButton*	pSetAddrButton = (CButton*) GetDlgItem( IDC_BUTTON_ADDR ) ;
	CEdit*		pMsgEdit = (CEdit*) GetDlgItem( IDC_EDIT_MSG ) ;
	CEdit*		pSrcEdit = (CEdit*) GetDlgItem( IDC_EDIT_SRC ) ;
	CEdit*		pDstEdit = (CEdit*) GetDlgItem( IDC_EDIT_DST ) ;

	switch( state )
	{
	case IPC_INITIALIZING : 
		pSendButton->EnableWindow( FALSE ) ;
		pMsgEdit->EnableWindow( FALSE ) ;
		m_ListChat.EnableWindow( FALSE ) ;
		break ;
	case IPC_READYTOSEND :	
		pSendButton->EnableWindow( TRUE ) ;
		pMsgEdit->EnableWindow( TRUE ) ;
		m_ListChat.EnableWindow( TRUE ) ;
		break ;
	case IPC_WAITFORACK :	break ;
	case IPC_ERROR :		break ;
	case IPC_UNICASTMODE :
		m_unDstAddr = 0x0 ;
		pDstEdit->EnableWindow( TRUE ) ;
		break ;
	case IPC_BROADCASTMODE :	
		m_unDstAddr = 0xff ;
		pDstEdit->EnableWindow( FALSE ) ;
		break ;
	case IPC_ADDR_SET :		
		pSetAddrButton->SetWindowText( "재설정(&R)" ) ; 
		pSrcEdit->EnableWindow( FALSE ) ;
		pDstEdit->EnableWindow( FALSE ) ;
		pChkButton->EnableWindow( FALSE ) ;
		break ;
	case IPC_ADDR_RESET :	
		pSetAddrButton->SetWindowText( "설정(&O)" ) ; 
		pSrcEdit->EnableWindow( TRUE ) ;
		if ( !pChkButton->GetCheck( ) )
			pDstEdit->EnableWindow( TRUE ) ;
		pChkButton->EnableWindow( TRUE ) ;
		break ;
	}

	UpdateData( FALSE ) ;
}

void CIPCAppDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer( ) ;
}

LRESULT CIPCAppDlg::OnRegSendMsg(WPARAM wParam, LPARAM lParam)
{
//////////////////////// fill the blank ///////////////////////////////**
	if( m_nAckReady )
		if(m_LayerMgr.GetLayer("File")->Receive()){
			::SendMessage( HWND_BROADCAST, nRegAckMsg, 0, 0 );}

	return 0 ;
}

LRESULT CIPCAppDlg::OnRegAckMsg(WPARAM wParam, LPARAM lParam) 
{
	if ( !m_nAckReady ){
		m_nAckReady = -1 ;
		KillTimer( 1 ) ;
	}//메세지를 보낼땐 0으로 초기화된상태라서 실행한다면 -1로 변경후 타이머를 종료

	return 0 ;
}

void CIPCAppDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	m_ListChat.AddString( ">> The last message was time-out.." ) ;
	m_nAckReady = -1 ;
	KillTimer( 1 ) ;
	//타이머가 2초시간이 다지면 타임아웃메세지를 출력하고 타이머를 종료
	CDialog::OnTimer(nIDEvent);
}
