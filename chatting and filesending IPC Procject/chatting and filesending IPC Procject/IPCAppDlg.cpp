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
   //�ڽ��� Mac�ּҸ� ���� ������
   m_unSrcEnetAddr = "80-FA-5B-29-82-F1";
   m_unDstEnetAddr = "B4-B5-2F-87-BD-B9";
   //�������� message����� file�� �ʱ�ȭ��Ŵ
   m_stMessage = _T("");
   m_filePath = _T("");

   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   //LayerManager�� �� layer�� �߰� ��
   m_LayerMgr.AddLayer( this ) ;   
   m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ;
   m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ;
   m_LayerMgr.AddLayer( new CIPLayer( "IP" ) );
   m_LayerMgr.AddLayer( new CTCPLayer( "TCP" ) );
   m_LayerMgr.AddLayer( new CChatAppLayer( "ChatApp" ) ) ;
   m_LayerMgr.AddLayer( new CFileAppLayer( "FileApp" ) ) ;
   //////////////////////// fill the blank ///////////////////////////////
   // ���̾ �����Ѵ�. (���̾� ����) layer�� ������ �������ش�.
   m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *IP ( *TCP ( *FileApp ( *ChatDlg ) *ChatApp ( *ChatDlg ) ) ) ) )" );
   ///////////////////////////////////////////////////////////////////////
   //������ layer�� �޾ƿ�
   m_ChatApp = (CChatAppLayer*)mp_UnderLayer ;
   m_TCP = (CTCPLayer *)m_LayerMgr.GetLayer("TCP");
   m_IP = (CIPLayer *)m_LayerMgr.GetLayer("IP");
   m_NI = (CNILayer *)m_LayerMgr.GetLayer("NI");
}

void CIPCAppDlg::DoDataExchange(CDataExchange* pDX)
{   //������ ���̾�α׿� ��Ʈ�� �� ��� ������ �����Ѵ�.
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
   //�̺�Ʈ ó���� ���̺� ���·� ����
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
{  //���̾� �α��� ��ü�� �ʱ�ȭ�ϴ� �Լ�
   CDialog::OnInitDialog();
   //�ý��� �޴��� ���� �޴��� �߰��Ѵ�.
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

   //��ȭ ���ڿ� �������� �����Ѵ�.
   SetIcon(m_hIcon, TRUE);         // Set big icon
   SetIcon(m_hIcon, FALSE);      // Set small icon
   //�ʱ�ȭ
   SetRegstryMessage( ) ;
   SetDlgState(IPC_INITIALIZING);
   SetDlgState(CFT_COMBO_SET);

   return TRUE;  //��Ŀ���� ��Ʈ�ѿ� �������� ������ true�� ��ȯ�Ѵ�.
}

void CIPCAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{   //�ݱ� ��ư�� ���������� ������� ���Ῡ�θ� �ѹ��� ���´�.
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
{   //Ŭ���̾�Ʈ���� �����ܰ� ���õ� �Լ�
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting
      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // Ŭ���̾�Ʈ �簢������ �������� ����� ���� 
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;
      // �������� �׸�
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

//�巡�� �Ҷ� Ŀ��
HCURSOR CIPCAppDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

void CIPCAppDlg::OnSendMessage() 
{
   //send��ư�� ���� ��� �߻��Ǵ� �Լ��� SendData()�� �̿��Ͽ� �����͸� �����Ѵ�.
   UpdateData( TRUE ) ;
   if ( !m_stMessage.IsEmpty() )
   {
      SendData( ) ;//������ data�� �����Ѵ�.
      m_stMessage = "" ;//������ �Է�ĭ�� �ʱ�ȭ �Ѵ�.
     //Ŀ���� �Է�ĭ���� �����Ѵ�.
      (CEdit*) GetDlgItem( IDC_EDIT_MSG )->SetFocus( );
   }
   UpdateData( FALSE ) ;
}

void CIPCAppDlg::OnButtonAddrSet()  
{   //���� ��ư�� ���� ��� �߻��ϴ� �̺�Ʈ�̴�.
   UpdateData( TRUE ) ;
   //ip�ּҸ� �����ϱ� ���� ������ �����Ѵ�.
   unsigned char src_ip[4];
   unsigned char dst_ip[4];
   if ( !m_unDstEnetAddr ||!m_unSrcEnetAddr  )
   {   //�߸��� �ּҰ� �Էµɰ�� ���â�� ����.
      MessageBox( "�ּҸ� ���� �����߻�","���",MB_OK | MB_ICONSTOP ) ;
      return ;
   }
   unsigned char srcMAC[12], dstMAC[12];
   if ( m_bSendReady ){ 
      //������ ���°� sendReady�����ϰ�� �ּҸ� �缳���ϱ� ���� ���·� �����Ѵ�.
      SetDlgState( IPC_ADDR_RESET ) ;
      SetDlgState( IPC_INITIALIZING ) ;
   }
   else{
      //�Էµ� ip�ּҸ� �޾ƿ� �����Ѵ�.
      m_unSrcIPAddr.GetAddress(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);
      m_unDstIPAddr.GetAddress(dst_ip[0],dst_ip[1],dst_ip[2],dst_ip[3]);
      //�Էµ� ip�� �̿��Ͽ� iplayer�� ip�� �����Ѵ�.
      m_IP->SetSrcIPAddress(src_ip);
      m_IP->SetDstIPAddress(dst_ip);
      //�̸� ������ �̴��� �ּҸ� ���´�.
      sscanf(m_unSrcEnetAddr, "%02X-%02X-%02X-%02X-%02X-%02X", &srcMAC[0], &srcMAC[1], &srcMAC[2], &srcMAC[3], &srcMAC[4], &srcMAC[5]);
      sscanf(m_unDstEnetAddr, "%02X-%02X-%02X-%02X-%02X-%02X", &dstMAC[0], &dstMAC[1], &dstMAC[2], &dstMAC[3], &dstMAC[4], &dstMAC[5]);

      ((CEthernetLayer *)m_LayerMgr.GetLayer("Ethernet"))->SetEnetSrcAddress(srcMAC); 
      ((CEthernetLayer *)m_LayerMgr.GetLayer("Ethernet"))->SetEnetDstAddress(dstMAC);

      int nIndex = m_ComboEnetName.GetCurSel();
      m_NI->SetAdapterNumber(nIndex);
     //��Ŷ ����
      m_NI->PacketStartDriver();
     //Dlg�� ���¸� �����Ѵ�.
      SetDlgState( IPC_ADDR_SET ) ;
      SetDlgState( IPC_READYTOSEND ) ;      
   }
   //���� �Ǵ� �缳���� ���¸� ���ݻ��׿� �ݴ�� �Ѵ�.
   m_bSendReady = !m_bSendReady ;
}


void CIPCAppDlg::SetRegstryMessage()
{
}

void CIPCAppDlg::SendData()
{   //������ data�� �����ϴ� �Լ�
   CString MsgHeader ; //�޽����� �պκ��� �����Ѵ�.
   MsgHeader.Format( "[��:��] " ) ;
   //header�� ���� �����Ϸ��� �޽����� ���� m_ListChat�� �߰��Ѵ�.
   m_ListChat.AddString( MsgHeader + m_stMessage ) ;
   // �Է��� �޽����� ���Ϸ� ����
   int nlength = m_stMessage.GetLength();
   unsigned char* ppayload = new unsigned char[nlength+1];
   memcpy(ppayload,(unsigned char*)(LPCTSTR)m_stMessage,nlength);
   ppayload[nlength] = '\0';
   // TCPLayer Port�� ����
   m_TCP->SetDestinPort(TCP_PORT_CHAT);
   // ���� data�� �޽��� ���̿� 1�� ���Ͽ� Send�Լ��� �Ѱ��ش�.
   m_ChatApp->Send(ppayload,m_stMessage.GetLength()+1);
   //ListChat�� ���� ��ũ���� �߰��Ѵ�.
   EditScroll(m_ListChat,( MsgHeader + m_stMessage ));
}

BOOL CIPCAppDlg::Receive(unsigned char *ppayload)
{   //�޼����� �Է¹޴� �κ�
   CString Msg;//�Է¹��� �޼����� �����ϴ� ����
   int len_ppayload = strlen((char *)ppayload); // ppayload�� ����
   // GetBuff �����Ҵ� �� �ʱ�ȭ
   unsigned char *GetBuff = (unsigned char *)malloc(len_ppayload);
   memset(GetBuff,0,len_ppayload);
   memcpy(GetBuff,ppayload,len_ppayload);
   GetBuff[len_ppayload] = '\0';
   // App Header�� �м��Ͽ�, ����Ʈ â�� �ѷ��� ������ �޽����� �����Ѵ�.
   // ������ �� �Ǵ� �޴� �ʰ� GetBuff�� ����� �޽��� ������ ��ģ��.
   // ��� ������ �����Ѵ�.
   Msg.Format("[��:��] %s",(char *)GetBuff);
   //m_ListChat�� ���� ���ڸ� �����Ѵ�.
   m_ListChat.AddString( (char*) Msg.GetBuffer(0) );
   //ListChat�� ���� ��ũ���� �߰��Ѵ�.
   EditScroll(m_ListChat,Msg);
   return TRUE ;
}

void EditScroll(CListBox& listbox, CString message){
   //�ԷµǴ� ���ڸ� ��� ���� �ֵ��� ���� ��ũ���� �߰��Ѵ�.
   CSize size;
   CDC *pDC = listbox.GetDC();
    // ���� ����Ʈ�ڽ��� ��Ʈ�� ���� DC�� �����Ų��.
    CFont* pOld = pDC->SelectObject(listbox.GetFont());
   size = pDC->GetTextExtent(message);
    size.cx += 3;
    pDC->SelectObject(pOld);
    listbox.ReleaseDC(pDC);
    // ���� ���ڿ��� Pixel ������ �ѱ��.
   if (listbox.GetHorizontalExtent() < size.cx)
    {   //���� listbox���� Ŭ��� �缳���Ѵ�.
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

void CIPCAppDlg::SetDlgState(int state) // ���̾�α� �ʱ�ȭ �κ�
{
   UpdateData( TRUE ) ;
   int i;
   CString device_description;
   //�� ���ҽ� id�� �ش��ϴ� ��ü�� �����Ѵ�.
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
   //�� ���¿� ���� ���̾�α��� ��뿩�θ� �����Ѵ�.
   switch( state )
   {
   case IPC_INITIALIZING : // ù ȭ�� ����
      pSendButton->EnableWindow( FALSE ) ;
      pMsgEdit->EnableWindow( FALSE ) ;
      m_ListChat.EnableWindow( FALSE ) ;
      pFilePathEdit->EnableWindow( FALSE );
      pFileSearchButton->EnableWindow( FALSE );
      pFileSendButton->EnableWindow( FALSE );
      break ;
   case IPC_READYTOSEND : // Send(S)��ư�� ������ �� ����
      pSendButton->EnableWindow( TRUE ) ;
      pMsgEdit->EnableWindow( TRUE ) ;
      m_ListChat.EnableWindow( TRUE ) ;
      pFilePathEdit->EnableWindow( TRUE );
      pFileSearchButton->EnableWindow( TRUE );
      pFileSendButton->EnableWindow( TRUE );
      break ;
   case IPC_WAITFORACK :   break ;
   case IPC_ERROR :      break ;
   case IPC_ADDR_SET :   // ����(&O)��ư�� ������ ��
      pSetAddrButton->SetWindowText( "�缳��(&R)" ) ; 
      pSrcEdit->EnableWindow( FALSE ) ;
      pDstEdit->EnableWindow( FALSE ) ;
      pSrcIPEdit->EnableWindow( FALSE );
      pDstIPEdit->EnableWindow( FALSE );
      pEnetNameCombo->EnableWindow( FALSE );
      m_NI->m_thrdSwitch = TRUE;
      break ;
   case IPC_ADDR_RESET : // �缳��(&R)��ư�� ������ ��
      pSetAddrButton->SetWindowText( "����(&O)" ) ; 
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
{   //File��ư�� ������ �߻��ϴ� �Լ�
   UpdateData(TRUE);
   // ������ ������ ã�Ƽ� ��θ� �߰�.
   CFileDialog dlg( true, "*.*", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, "All Files(*.*)|*.*|", NULL );
   if( dlg.DoModal() == IDOK )
   {
      m_filePath = dlg.GetPathName();
   }
   UpdateData(FALSE);
}


void CIPCAppDlg::OnSendFile() 
{   //FileSend��ư�� ���� ��� �߻��ϴ� �Լ�
   //UpdateData(TRUE);
   // TCP Port�� ����
   m_TCP->SetDestinPort(TCP_PORT_FILE);
   // ���ϰ�θ� FileAppLayer�� �Ѱ��ش�.
   ((CFileAppLayer*)GetUnderLayer())->Send( (unsigned char*)(LPCTSTR)m_filePath );
   //m_filePath="";//���ϰ��ĭ���ʱ�ȭ�Ѵ�.
   //(CEdit*) GetDlgItem( IDC_EDIT_FilePath )->SetFocus( );//�Է�Ŀ���� ���ϰ�� �Է�ĭ���� ����
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
{   //File��ư�� ������ �����ϰ� ��� ���θ� �����Ѵ�.
   CButton* pFileSendButton = (CButton*) GetDlgItem( IDC_BUTTON_FILESEND ) ;
   CButton* pFileSearchButton = (CButton*) GetDlgItem( IDC_BUTTON_FILE ) ;
   pFileSendButton->EnableWindow(bBool);
   pFileSearchButton->EnableWindow(bBool);
}

void CIPCAppDlg::OnLbnSelchangeListChat()
{
}