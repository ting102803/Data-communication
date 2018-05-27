//
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
   m_bSendReady(FALSE)
{
   //자신의 Mac주소를 직접 설정함
   m_unSrcEnetAddr = "80-FA-5B-29-82-F1";
   m_unDstEnetAddr = "B4-B5-2F-87-BD-B9";
   //보내려는 message내용과 file을 초기화시킴
   m_stMessage = _T("");
   m_filePath = _T("");

   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   //LayerManager에 각 layer를 추가 함
   m_LayerMgr.AddLayer( this ) ;   
   m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ;
   m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ;
   m_LayerMgr.AddLayer( new CIPLayer( "IP" ) );
   m_LayerMgr.AddLayer( new CTCPLayer( "TCP" ) );
   m_LayerMgr.AddLayer( new CChatAppLayer( "ChatApp" ) ) ;
   m_LayerMgr.AddLayer( new CFileAppLayer( "FileApp" ) ) ;
   //////////////////////// fill the blank ///////////////////////////////
   // 레이어를 연결한다. (레이어 생성) layer의 계층을 연결해준다.
   m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *IP ( *TCP ( *FileApp ( *ChatDlg ) *ChatApp ( *ChatDlg ) ) ) ) )" );
   ///////////////////////////////////////////////////////////////////////
   //생성된 layer를 받아옴
   m_ChatApp = (CChatAppLayer*)mp_UnderLayer ;
   m_TCP = (CTCPLayer *)m_LayerMgr.GetLayer("TCP");
   m_IP = (CIPLayer *)m_LayerMgr.GetLayer("IP");
   m_NI = (CNILayer *)m_LayerMgr.GetLayer("NI");
}

void CIPCAppDlg::DoDataExchange(CDataExchange* pDX)
{   //생성된 다이얼로그와 컨트롤 의 멤버 변수를 연결한다.
   CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_LIST_CHAT, m_ListChat);
   DDX_Text(pDX, IDC_EDIT_DST, m_unDstEnetAddr);
   DDX_Text(pDX, IDC_EDIT_SRC, m_unSrcEnetAddr);
   DDX_Text(pDX, IDC_EDIT_MSG, m_stMessage);

   DDX_Control(pDX, IDC_EDIT_SRCIP, m_unSrcIPAddr);
   DDX_Control(pDX, IDC_EDIT_DSTIP, m_unDstIPAddr);
   DDX_Text(pDX, IDC_EDIT_FilePath, m_filePath);

   DDX_Control(pDX,IDC_PROGRESS, m_ProgressCtrl);
   DDX_Control(pDX, IDC_COMBO_ENETADDR, m_ComboEnetName);
}

BEGIN_MESSAGE_MAP(CIPCAppDlg, CDialog)
   //이벤트 처리를 테이블 형태로 만듦
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_BUTTON_SEND, OnSendMessage)
   ON_BN_CLICKED(IDC_BUTTON_ADDR, OnButtonAddrSet)

   ON_BN_CLICKED(IDC_BUTTON_FILE, OnAddFile)
   ON_BN_CLICKED(IDC_BUTTON_FILESEND, OnSendFile)

   ON_CBN_SELCHANGE(IDC_COMBO_ENETADDR, OnComboEnetAddr)
   ON_LBN_SELCHANGE(IDC_LIST_CHAT, &CIPCAppDlg::OnLbnSelchangeListChat)
END_MESSAGE_MAP()

BOOL CIPCAppDlg::OnInitDialog()
{  //다이얼 로그의 객체를 초기화하는 함수
   CDialog::OnInitDialog();
   //시스템 메뉴에 정보 메뉴를 추가한다.
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

   //대화 상자에 아이콘을 설정한다.
   SetIcon(m_hIcon, TRUE);         // Set big icon
   SetIcon(m_hIcon, FALSE);      // Set small icon
   //초기화
   SetRegstryMessage( ) ;
   SetDlgState(IPC_INITIALIZING);
   SetDlgState(CFT_COMBO_SET);

   return TRUE;  //포커스를 컨트롤에 설정하지 않으면 true를 반환한다.
}

void CIPCAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{   //닫기 버튼을 눌렀을때의 반응들로 종료여부를 한번더 묻는다.
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
}

void CIPCAppDlg::OnPaint() 
{   //클라이언트에서 아이콘과 관련된 함수
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting
      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // 클라이언트 사각형에서 아이콘을 가운데로 정렬 
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;
      // 아이콘을 그림
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

//드래그 할때 커서
HCURSOR CIPCAppDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

void CIPCAppDlg::OnSendMessage() 
{
   //send버튼을 누를 경우 발생되는 함수로 SendData()를 이용하여 데이터를 전송한다.
   UpdateData( TRUE ) ;
   if ( !m_stMessage.IsEmpty() )
   {
      SendData( ) ;//실제로 data를 전송한다.
      m_stMessage = "" ;//전송후 입력칸을 초기화 한다.
     //커서를 입력칸으로 설정한다.
      (CEdit*) GetDlgItem( IDC_EDIT_MSG )->SetFocus( );
   }
   UpdateData( FALSE ) ;
}

void CIPCAppDlg::OnButtonAddrSet()  
{   //설정 버튼을 누를 경우 발생하는 이벤트이다.
   UpdateData( TRUE ) ;
   //ip주소를 저장하기 위한 변수를 생성한다.
   unsigned char src_ip[4];
   unsigned char dst_ip[4];
   if ( !m_unDstEnetAddr ||!m_unSrcEnetAddr  )
   {   //잘못된 주소가 입력될경우 경고창을 띄운다.
      MessageBox( "주소를 설정 오류발생","경고",MB_OK | MB_ICONSTOP ) ;
      return ;
   }
   unsigned char srcMAC[12], dstMAC[12];
   if ( m_bSendReady ){ 
      //기존의 상태가 sendReady상태일경우 주소를 재설정하기 위한 상태로 변경한다.
      SetDlgState( IPC_ADDR_RESET ) ;
      SetDlgState( IPC_INITIALIZING ) ;
   }
   else{
      //입력된 ip주소를 받아와 저장한다.
      m_unSrcIPAddr.GetAddress(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);
      m_unDstIPAddr.GetAddress(dst_ip[0],dst_ip[1],dst_ip[2],dst_ip[3]);
      //입력된 ip를 이용하여 iplayer의 ip를 설정한다.
      m_IP->SetSrcIPAddress(src_ip);
      m_IP->SetDstIPAddress(dst_ip);
      //미리 설정한 이더넷 주소를 얻어온다.
      sscanf(m_unSrcEnetAddr, "%02X-%02X-%02X-%02X-%02X-%02X", &srcMAC[0], &srcMAC[1], &srcMAC[2], &srcMAC[3], &srcMAC[4], &srcMAC[5]);
      sscanf(m_unDstEnetAddr, "%02X-%02X-%02X-%02X-%02X-%02X", &dstMAC[0], &dstMAC[1], &dstMAC[2], &dstMAC[3], &dstMAC[4], &dstMAC[5]);

      ((CEthernetLayer *)m_LayerMgr.GetLayer("Ethernet"))->SetEnetSrcAddress(srcMAC); 
      ((CEthernetLayer *)m_LayerMgr.GetLayer("Ethernet"))->SetEnetDstAddress(dstMAC);

      int nIndex = m_ComboEnetName.GetCurSel();
      m_NI->SetAdapterNumber(nIndex);
     //패킷 시작
      m_NI->PacketStartDriver();
     //Dlg의 상태를 변경한다.
      SetDlgState( IPC_ADDR_SET ) ;
      SetDlgState( IPC_READYTOSEND ) ;      
   }
   //설정 또는 재설정시 상태를 지금상테와 반대로 한다.
   m_bSendReady = !m_bSendReady ;
}


void CIPCAppDlg::SetRegstryMessage()
{
}

void CIPCAppDlg::SendData()
{   //실제로 data를 전송하는 함수
   CString MsgHeader ; //메시지의 앞부분을 설정한다.
   MsgHeader.Format( "[나:너] " ) ;
   //header와 실제 전송하려는 메시지를 합쳐 m_ListChat에 추가한다.
   m_ListChat.AddString( MsgHeader + m_stMessage ) ;
   // 입력한 메시지를 파일로 저장
   int nlength = m_stMessage.GetLength();
   unsigned char* ppayload = new unsigned char[nlength+1];
   memcpy(ppayload,(unsigned char*)(LPCTSTR)m_stMessage,nlength);
   ppayload[nlength] = '\0';
   // TCPLayer Port를 설정
   m_TCP->SetDestinPort(TCP_PORT_CHAT);
   // 보낼 data와 메시지 길이에 1을 더하여 Send함수로 넘겨준다.
   m_ChatApp->Send(ppayload,m_stMessage.GetLength()+1);
   //ListChat의 가로 스크롤을 추가한다.
   EditScroll(m_ListChat,( MsgHeader + m_stMessage ));
}

BOOL CIPCAppDlg::Receive(unsigned char *ppayload)
{   //메세지를 입력받는 부분
   CString Msg;//입력받은 메세지를 저장하는 변수
   int len_ppayload = strlen((char *)ppayload); // ppayload의 길이
   // GetBuff 동적할당 후 초기화
   unsigned char *GetBuff = (unsigned char *)malloc(len_ppayload);
   memset(GetBuff,0,len_ppayload);
   memcpy(GetBuff,ppayload,len_ppayload);
   GetBuff[len_ppayload] = '\0';
   // App Header를 분석하여, 리스트 창에 뿌려줄 내용의 메시지를 구성한다.
   // 보내는 쪽 또는 받는 쪽과 GetBuff에 저장된 메시지 내용을 합친다.
   // 출력 형식을 설정한다.
   Msg.Format("[너:나] %s",(char *)GetBuff);
   //m_ListChat에 받은 문자를 저장한다.
   m_ListChat.AddString( (char*) Msg.GetBuffer(0) );
   //ListChat의 가로 스크롤을 추가한다.
   EditScroll(m_ListChat,Msg);
   return TRUE ;
}

void EditScroll(CListBox& listbox, CString message){
   //입력되는 문자를 모두 볼수 있도록 가로 스크롤을 추가한다.
   CSize size;
   CDC *pDC = listbox.GetDC();
    // 현재 리스트박스의 폰트를 얻어와 DC에 적용시킨다.
    CFont* pOld = pDC->SelectObject(listbox.GetFont());
   size = pDC->GetTextExtent(message);
    size.cx += 3;
    pDC->SelectObject(pOld);
    listbox.ReleaseDC(pDC);
    // 구한 문자열의 Pixel 단위를 넘긴다.
   if (listbox.GetHorizontalExtent() < size.cx)
    {   //이전 listbox보다 클경우 재설정한다.
      listbox.SetHorizontalExtent(size.cx);
    }
}

BOOL CIPCAppDlg::PreTranslateMessage(MSG* pMsg) 
{
   switch( pMsg->message )
   {
   case WM_KEYDOWN :
      switch( pMsg->wParam )
      {
      case VK_RETURN : 
         if ( ::GetDlgCtrlID( ::GetFocus( ) ) == IDC_EDIT_MSG ) 
            OnSendMessage( ) ;               return FALSE ;
      case VK_ESCAPE : return FALSE ;
      }
      break ;
   }

   return CDialog::PreTranslateMessage(pMsg);
}

void CIPCAppDlg::SetDlgState(int state) // 다이얼로그 초기화 부분
{
   UpdateData( TRUE ) ;
   int i;
   CString device_description;
   //각 리소스 id에 해당하는 객체를 설정한다.
   CButton*   pChkButton = (CButton*) GetDlgItem( IDC_CHECK_TOALL ) ;
   CButton*   pSendButton = (CButton*) GetDlgItem( IDC_BUTTON_SEND ) ;
   CButton*   pSetAddrButton = (CButton*) GetDlgItem( IDC_BUTTON_ADDR ) ;
   CButton*   pFileSearchButton = (CButton*) GetDlgItem( IDC_BUTTON_FILE ) ;
   CButton*   pFileSendButton = (CButton*) GetDlgItem( IDC_BUTTON_FILESEND ) ;

   CEdit*      pMsgEdit = (CEdit*) GetDlgItem( IDC_EDIT_MSG ) ;
   CEdit*      pSrcEdit = (CEdit*) GetDlgItem( IDC_EDIT_SRC ) ;
   CEdit*      pDstEdit = (CEdit*) GetDlgItem( IDC_EDIT_DST ) ;
   CEdit*      pSrcIPEdit = (CEdit*) GetDlgItem( IDC_EDIT_SRCIP ) ;
   CEdit*      pDstIPEdit = (CEdit*) GetDlgItem( IDC_EDIT_DSTIP ) ;
   CEdit*      pFilePathEdit = (CEdit*) GetDlgItem( IDC_EDIT_FilePath ) ;

   CComboBox*   pEnetNameCombo = (CComboBox*)GetDlgItem(IDC_COMBO_ENETADDR);
   //각 상태에 따른 다이얼로그의 사용여부를 설정한다.
   switch( state )
   {
   case IPC_INITIALIZING : // 첫 화면 세팅
      pSendButton->EnableWindow( FALSE ) ;
      pMsgEdit->EnableWindow( FALSE ) ;
      m_ListChat.EnableWindow( FALSE ) ;
      pFilePathEdit->EnableWindow( FALSE );
      pFileSearchButton->EnableWindow( FALSE );
      pFileSendButton->EnableWindow( FALSE );
      break ;
   case IPC_READYTOSEND : // Send(S)버튼을 눌렀을 때 세팅
      pSendButton->EnableWindow( TRUE ) ;
      pMsgEdit->EnableWindow( TRUE ) ;
      m_ListChat.EnableWindow( TRUE ) ;
      pFilePathEdit->EnableWindow( TRUE );
      pFileSearchButton->EnableWindow( TRUE );
      pFileSendButton->EnableWindow( TRUE );
      break ;
   case IPC_WAITFORACK :   break ;
   case IPC_ERROR :      break ;
   case IPC_ADDR_SET :   // 설정(&O)버튼을 눌렀을 때
      pSetAddrButton->SetWindowText( "재설정(&R)" ) ; 
      pSrcEdit->EnableWindow( FALSE ) ;
      pDstEdit->EnableWindow( FALSE ) ;
      pSrcIPEdit->EnableWindow( FALSE );
      pDstIPEdit->EnableWindow( FALSE );
      pEnetNameCombo->EnableWindow( FALSE );
      m_NI->m_thrdSwitch = TRUE;
      break ;
   case IPC_ADDR_RESET : // 재설정(&R)버튼을 눌렀을 때
      pSetAddrButton->SetWindowText( "설정(&O)" ) ; 
      pSrcEdit->EnableWindow( TRUE ) ;
      pDstEdit->EnableWindow( TRUE ) ;
      pSrcIPEdit->EnableWindow( TRUE );
      pDstIPEdit->EnableWindow( TRUE );
      pEnetNameCombo->EnableWindow( TRUE );
      m_NI->m_thrdSwitch = FALSE;
      break ;
   case CFT_COMBO_SET :
      for(i=0;i<NI_COUNT_NIC;i++){
         if(!m_NI->GetAdapterObject(i))
            break;
         device_description = m_NI->GetAdapterObject(i)->description;
         device_description.Trim();
         pEnetNameCombo->AddString(device_description);
         pEnetNameCombo->SetCurSel(0);
      }
      m_unSrcIPAddr.SetAddress(inet_addr(m_NI->GetIPAddress(0)));
      break;
   }
   UpdateData( FALSE ) ;
}

void CIPCAppDlg::EndofProcess()
{
   m_LayerMgr.DeAllocLayer( ) ;
}

void CIPCAppDlg::OnAddFile()
{   //File버튼을 누를시 발생하는 함수
   UpdateData(TRUE);
   // 전송할 파일을 찾아서 경로를 추가.
   CFileDialog dlg( true, "*.*", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, "All Files(*.*)|*.*|", NULL );
   if( dlg.DoModal() == IDOK )
   {
      m_filePath = dlg.GetPathName();
   }
   UpdateData(FALSE);
}


void CIPCAppDlg::OnSendFile() 
{   //FileSend버튼을 누를 경우 발생하는 함수
   //UpdateData(TRUE);
   // TCP Port를 설정
   m_TCP->SetDestinPort(TCP_PORT_FILE);
   // 파일경로를 FileAppLayer로 넘겨준다.
   ((CFileAppLayer*)GetUnderLayer())->Send( (unsigned char*)(LPCTSTR)m_filePath );
   //m_filePath="";//파일경로칸을초기화한다.
   //(CEdit*) GetDlgItem( IDC_EDIT_FilePath )->SetFocus( );//입력커서를 파일경로 입력칸으로 설정
   //UpdateData(FALSE);
}

void CIPCAppDlg::OnComboEnetAddr()
{
   UpdateData(TRUE);

   int nIndex = m_ComboEnetName.GetCurSel();   
   m_NI->GetAdapterObject(nIndex)->name;
   m_unSrcIPAddr.SetAddress(inet_addr(m_NI->GetIPAddress(nIndex)));

   UpdateData(FALSE);
}

void CIPCAppDlg::OnOffFileButton(BOOL bBool)
{   //File버튼에 변수를 설정하고 사용 여부를 설정한다.
   CButton* pFileSendButton = (CButton*) GetDlgItem( IDC_BUTTON_FILESEND ) ;
   CButton* pFileSearchButton = (CButton*) GetDlgItem( IDC_BUTTON_FILE ) ;
   pFileSendButton->EnableWindow(bBool);
   pFileSearchButton->EnableWindow(bBool);
}

void CIPCAppDlg::OnLbnSelchangeListChat()
{
}