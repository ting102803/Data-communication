
#include "stdafx.h"
#include "ARP.h"
#include "ARPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//layer manager를 사용하여 layer들을 연결시킴
CARPDlg::CARPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CARPDlg::IDD, pParent), 
	CBaseLayer( "ARPDlg" ),
	m_bSendReady(FALSE)
{
	mEdit_GARPEther = _T("");
	mEdit_MyEther = _T("");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// 연결시킬 레이어들을 LayerManger에 추가
	m_LayerMgr.AddLayer( this ) ;	
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ;
	m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ;
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) ) ;
	m_LayerMgr.AddLayer( new CIPLayer( "IP" ) ) ;

	// 레이어 연결. 
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *ARP ( *ARPDlg ) ) ) )" );

	// 레이어 객체 생성
	m_IP = (CIPLayer *)m_LayerMgr.GetLayer("IP");
	m_ARP = (CARPLayer *)m_LayerMgr.GetLayer("ARP");
	m_Ether = (CEthernetLayer *)m_LayerMgr.GetLayer("Ethernet");
	m_NI = (CNILayer *)m_LayerMgr.GetLayer("NI");

}

BOOL CARPDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN :
		switch( pMsg->wParam )
		{
		case VK_RETURN : 
			if ( ::GetDlgCtrlID( ::GetFocus( ) ) == IDC_EDIT_MSG ) 
				OnSendARP( ) ;					return FALSE ;
		case VK_ESCAPE : return FALSE ;
		}
		break ;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CARPDlg::DoDataExchange(CDataExchange* pDX)
{
	// 컨트롤과 변수를 잇는 작업
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_ARP_TABLE,mList_ARPCache);
	DDX_Control(pDX,IDC_ARP_IPADDRESS,mIP_ARPIP);
	DDX_Control(pDX,IDC_PARP_TABLE,mList_PARPEntry);
	DDX_Text(pDX,IDC_GARP_ETHER,mEdit_GARPEther);
	DDX_Control(pDX,IDC_MYDEV_LIST,mCombo_MyDevice);
	DDX_Text(pDX,IDC_MYDEV_ETHERNET,mEdit_MyEther);
	DDX_Control(pDX,IDC_MYDEV_IPADDRESS,mIP_MyIP);
}

//message처리를 위한 연결
BEGIN_MESSAGE_MAP(CARPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ARP_IPSEND, &CARPDlg::OnSendARP)
	ON_BN_CLICKED(IDC_GARP_SEND, &CARPDlg::OnSendGARP)
	ON_BN_CLICKED(IDC_MYDEV_SELECT, &CARPDlg::OnSettingMyDev)
	ON_BN_CLICKED(IDC_CANCEL, &CARPDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_PARP_ADD, &CARPDlg::OnAddPARP)
	ON_BN_CLICKED(IDC_EXIT, &CARPDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_ARP_ITEMDELETE, &CARPDlg::OnARPItemDelete)
	ON_BN_CLICKED(IDC_ARP_ALLDELETE, &CARPDlg::OnARPAllDelete)
	ON_BN_CLICKED(IDC_PARP_DELETE, &CARPDlg::OnPARPEntryDelete)
	ON_CBN_SELCHANGE(IDC_MYDEV_LIST, &CARPDlg::OnGetMacAddress)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PARP_TABLE, &CARPDlg::OnLvnItemchangedParpTable)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_ARP_IPADDRESS, &CARPDlg::OnIpnFieldchangedArpIpaddress)
	ON_EN_CHANGE(IDC_MYDEV_ETHERNET, &CARPDlg::OnEnChangeMydevEthernet)
END_MESSAGE_MAP()


//dialog초기화 부분/
BOOL CARPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	//아이콘 및 프로그램 화면을 설정한다.
	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	SetDlgState(ARP_INITIALIZING);
	SetDlgState(ARP_DEVICE);


	((CListCtrl*)GetDlgItem(IDC_ARP_TABLE))->InsertColumn(0,"IP Address",LVCFMT_LEFT,180);
	((CListCtrl*)GetDlgItem(IDC_ARP_TABLE))->InsertColumn(1,"Ethernet Address",LVCFMT_LEFT,180);
	((CListCtrl*)GetDlgItem(IDC_ARP_TABLE))->InsertColumn(2,"Status",LVCFMT_LEFT,130);

	((CListCtrl*)GetDlgItem(IDC_PARP_TABLE))->InsertColumn(0,"Device",LVCFMT_LEFT,180);
	((CListCtrl*)GetDlgItem(IDC_PARP_TABLE))->InsertColumn(1,"IP Address",LVCFMT_LEFT,180);
	((CListCtrl*)GetDlgItem(IDC_PARP_TABLE))->InsertColumn(2,"Ethernet Address",LVCFMT_LEFT,180);

	return TRUE;
}

void CARPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( nID == SC_CLOSE )
	{
		if ( MessageBox( "종료하시겠습니까?", "Question", 
			MB_YESNO | MB_ICONQUESTION ) == IDNO )
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

void CARPDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer( ) ;
}

//실행시 다이얼로그를 그리기 위한 함수
void CARPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CARPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//상태에 따른 버튼이나 기능들의 활성화를 위한 함수
void CARPDlg::SetDlgState( int state )
{
	UpdateData( TRUE ) ;
	int i;
	CString device_description;

	CButton*	pPARP_AddButton = (CButton*) GetDlgItem( IDC_PARP_ADD ) ;
	CButton*	pPARP_DeleteButton = (CButton*) GetDlgItem( IDC_PARP_DELETE ) ;
	CButton*	pGARP_SendButton = (CButton*) GetDlgItem( IDC_GARP_SEND ) ;
	CButton*	pMY_SelectButton = (CButton*) GetDlgItem( IDC_MYDEV_SELECT ) ;
	CButton*	pARP_IPSendButton = (CButton*) GetDlgItem( IDC_ARP_IPSEND ) ;
	CButton*	pARP_ItemDeleteButton = (CButton*) GetDlgItem( IDC_ARP_ITEMDELETE ) ;
	CButton*	pARP_AllDeleteButton = (CButton*) GetDlgItem( IDC_ARP_ALLDELETE ) ;
	CButton*	p_ExitButton = (CButton*) GetDlgItem( IDC_EXIT ) ;
	CButton*	p_CancelButton = (CButton*) GetDlgItem( IDC_CANCEL ) ;

	CEdit*		pGARP_EtherEdit = (CEdit*) GetDlgItem( IDC_GARP_ETHER ) ;
	CEdit*		pMY_EtherEdit = (CEdit*) GetDlgItem( IDC_MYDEV_ETHERNET ) ;

	CComboBox*	pMY_EtherComboBox = (CComboBox*)GetDlgItem( IDC_MYDEV_LIST );

	CIPAddressCtrl* pARP_IPAddrCtrl = (CIPAddressCtrl*) GetDlgItem( IDC_ARP_IPADDRESS ) ;
	CIPAddressCtrl* pMY_IPAddrCtrl = (CIPAddressCtrl*) GetDlgItem( IDC_MYDEV_IPADDRESS ) ;

	switch(state)
	{
	case ARP_INITIALIZING:
		pPARP_AddButton->EnableWindow(FALSE);
		pPARP_DeleteButton->EnableWindow(FALSE);
		pGARP_SendButton->EnableWindow(FALSE);
		pARP_IPSendButton->EnableWindow(FALSE);
		pARP_ItemDeleteButton->EnableWindow(FALSE);
		pARP_AllDeleteButton->EnableWindow(FALSE);
		p_ExitButton->EnableWindow(FALSE);

		pGARP_EtherEdit->EnableWindow(FALSE);
		pARP_IPAddrCtrl->EnableWindow(FALSE);
		break;
	case ARP_OPERATING:
		pPARP_AddButton->EnableWindow(TRUE);
		pPARP_DeleteButton->EnableWindow(TRUE);
		pGARP_SendButton->EnableWindow(TRUE);
		pARP_IPSendButton->EnableWindow(TRUE);
		pARP_ItemDeleteButton->EnableWindow(TRUE);
		pARP_AllDeleteButton->EnableWindow(TRUE);
		p_ExitButton->EnableWindow(TRUE);

		pGARP_EtherEdit->EnableWindow(TRUE);
		pARP_IPAddrCtrl->EnableWindow(TRUE);

		pMY_EtherComboBox->EnableWindow(FALSE);
		pMY_IPAddrCtrl->EnableWindow(FALSE);
		pMY_SelectButton->EnableWindow(FALSE);
		p_CancelButton->EnableWindow(FALSE);
		break;
	case ARP_ENDPROCESSING:
		pPARP_AddButton->EnableWindow(FALSE);
		pPARP_DeleteButton->EnableWindow(FALSE);
		pGARP_SendButton->EnableWindow(FALSE);
		pARP_IPSendButton->EnableWindow(FALSE);
		pARP_ItemDeleteButton->EnableWindow(FALSE);
		pARP_AllDeleteButton->EnableWindow(FALSE);
		p_ExitButton->EnableWindow(FALSE);
		
		pGARP_EtherEdit->EnableWindow(FALSE);
		pARP_IPAddrCtrl->EnableWindow(FALSE);

		pMY_EtherComboBox->EnableWindow(TRUE);
		pMY_IPAddrCtrl->EnableWindow(TRUE);
		pMY_SelectButton->EnableWindow(TRUE);
		p_CancelButton->EnableWindow(TRUE);
		break;
	case ARP_DEVICE:
		for(i=0;i<NI_COUNT_NIC;i++){
			if(!m_NI->GetAdapterObject(i))
				break;
			device_description = m_NI->GetAdapterObject(i)->description;
			device_description.Trim();
			pMY_EtherComboBox->AddString(device_description);
		}
		
		pMY_EtherComboBox->SetCurSel(0);
		CString nicName = m_NI->GetAdapterObject(0)->name;
		
		mEdit_MyEther = m_NI->GetNICardAddress((char *)nicName.GetString());
		
		break;
	}

	UpdateData( FALSE ) ;
}


// IP 주소로 매칭된 MAC 주소를 받게됨.
BOOL CARPDlg::Receive( unsigned char* ppayload )
{
	CacheTableUpdate();
	

	int index = (int)ppayload;

	if(index >= 0) {
		KillTimer(index);
		SetTimer(index,TIMER_VALIDTIME,NULL);
	}
	return TRUE;
}

/* ARP Part */
 // Item Delete 버튼
void CARPDlg::OnARPItemDelete()
{
	
	UpdateData( TRUE ) ;
	POSITION pos = mList_ARPCache.GetFirstSelectedItemPosition();

	int nItem;
	if(pos!=NULL)
	{
		while(pos) {
			nItem = mList_ARPCache.GetNextSelectedItem(pos);
		}
		m_ARP->CacheDeleteItem(nItem);

		CacheTableUpdate();
	}
	UpdateData( FALSE ) ;
}

 // All Delete 버튼
void CARPDlg::OnARPAllDelete()
{
	UpdateData( TRUE ) ;

	m_ARP->CacheAllDelete();

	CacheTableUpdate();

	UpdateData( FALSE ) ;
}

//ARP message를 생서하여 send를 호출하며 중복되는 IP들을 검사한다.
void CARPDlg::OnSendARP()
{
	UpdateData( TRUE ) ;

	BYTE ip[4];
	IP_ADDR chk_ip;
	mIP_ARPIP.GetAddress(ip[0],ip[1],ip[2],ip[3]);
	chk_ip.addr_i0 = ip[0]; chk_ip.addr_i1 = ip[1];
	chk_ip.addr_i2 = ip[2]; chk_ip.addr_i3 = ip[3];

	if(m_ARP->CacheSearchExist(chk_ip))
	{
		CACHE_ENTRY cache_entry = m_ARP->GetSelectCacheEntry(chk_ip);		
		if(cache_entry.cache_enetaddr.addrs_e!=NULL)
		{
			CString etherAlreadyExist;
			etherAlreadyExist.Format("IP가 이미 존재합니다.\nEthernet : %.02x:%.02x:%.02x:%.02x:%.02x:%.02x",cache_entry.cache_enetaddr.addr_e0
				,cache_entry.cache_enetaddr.addr_e1
				,cache_entry.cache_enetaddr.addr_e2
				,cache_entry.cache_enetaddr.addr_e3
				,cache_entry.cache_enetaddr.addr_e4
				,cache_entry.cache_enetaddr.addr_e5);
			AfxMessageBox(etherAlreadyExist);
		}
		return;
	}

	IP_ADDR arp_ip;
	mIP_MyIP.GetAddress(ip[0],ip[1],ip[2],ip[3]);
	arp_ip.addr_i0 = ip[0]; arp_ip.addr_i1 = ip[1];
	arp_ip.addr_i2 = ip[2]; arp_ip.addr_i3 = ip[3];

	if(m_ARP->ProxySearchExist(chk_ip)) {
		AfxMessageBox("Proxy ARP Entry에 이미 존재하는 IP 주소입니다.\n");

		return;
	}

	if(memcmp(chk_ip.addrs_i,arp_ip.addrs_i,4) == 0) {
		AfxMessageBox("입력하신 IP는 현재 Device의 IP 주소입니다.\n");

		return;
	}

	SettingARPMsg(); 

	m_ARP->Send(NULL,ARP_HEADER_SIZE);

	int index;
	if((index = m_ARP->CacheAddItem(NULL))>-1)
		SetTimer(index,TIMER_REPLYTIME,NULL);
	CacheTableUpdate();
	UpdateData( FALSE ) ;
}

/* Gratuitous ARP Part */

// Gratuitous ARP Send 버튼
void CARPDlg::OnSendGARP()
{
	UpdateData( TRUE ) ;
	
	SettingGARPMsg(); 
	m_ARP->Send(NULL,ARP_HEADER_SIZE);
	
	UpdateData( FALSE ) ;
}

/* My Device Part */
//자신의 dvice와 ip를 설정함
void CARPDlg::OnSettingMyDev()
{
	UpdateData( TRUE ) ;

	unsigned char my_ip[4];
	mIP_MyIP.GetAddress(my_ip[0],my_ip[1],my_ip[2],my_ip[3]);
	
	m_ARP->SetSenderIPAddress(my_ip);
	m_ARP->SetSenderEtherAddress(MacAddrToHexInt(mEdit_MyEther));

	int nIndex = mCombo_MyDevice.GetCurSel();
	m_NI->SetAdapterNumber(nIndex);

	m_NI->PacketStartDriver();
	
	SetDlgState( ARP_OPERATING ) ;

	UpdateData( FALSE ) ;
}

/* Proxy ARP Entry Part */
// Proxy ARP Entry 에 원하는 값을 입력하여 추가함
void CARPDlg::OnAddPARP() 
{
	PrxoyARPDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		PROXY_ENTRY proxy;
		memcpy(proxy.proxy_ipaddr.addrs_i,dlg.GetProxyIPAddress(),4);
		if(m_ARP->ProxySearchExist(proxy.proxy_ipaddr)){
			AfxMessageBox("IP가 테이블에 존재합니다.");
			return;
		}
		memcpy(proxy.proxy_enetaddr.addrs_e,dlg.GetProxyEtherAddress(),6);
		memcpy(proxy.proxy_devname,dlg.GetProxyDevName(),PM_DEVICE_NAME_LENGTH);

		m_ARP->ProxyAddItem(proxy);
		
		ProxyTableUpdate();
	}
}

// Proxy ARP Entry 에서 선택한 값을 삭제
void CARPDlg::OnPARPEntryDelete()
{
	POSITION pos = mList_PARPEntry.GetFirstSelectedItemPosition();
	
	int nItem;
	if(pos!=NULL)
	{
		while(pos) {
			nItem = mList_PARPEntry.GetNextSelectedItem(pos);
		}

		m_ARP->ProxyDeleteItem(nItem);

		ProxyTableUpdate();
	}
}

/* Main Dialog Part */
// ARP 동작 종료
void CARPDlg::OnBnClickedExit()
{	
	UpdateData( TRUE ) ;
	
	SetDlgState(ARP_ENDPROCESSING);

	m_ARP->CacheAllDelete();

	CacheTableUpdate();

	UpdateData( FALSE ) ;
}

 // 취소 Button
void CARPDlg::OnBnClickedCancel()
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
}

void CARPDlg::OnGetMacAddress()
{
	UpdateData ( TRUE );

	CComboBox*	pMY_EtherComboBox = (CComboBox*)GetDlgItem( IDC_MYDEV_LIST );
	
	int sIndex = pMY_EtherComboBox->GetCurSel();
	CString nicName = m_NI->GetAdapterObject(sIndex)->name;

	mEdit_MyEther = m_NI->GetNICardAddress((char *)nicName.GetString());

	UpdateData ( FALSE );
}

// Timer 설정 함수
void CARPDlg::OnTimer(UINT nIDEvent) 
{
	KillTimer( nIDEvent );
	m_ARP->CacheDeleteByTimer(nIDEvent);
	CacheTableUpdate();

	CDialog::OnTimer(nIDEvent);
}

// MAC 주소를 16진수로 변환 시켜 줌
unsigned char* CARPDlg::MacAddrToHexInt(CString ether)
{
	CString noColStr;
	unsigned char *arp_ether = (u_char *)malloc(sizeof(u_char)*6);

	for(int i=0; i<6; i++) {
		AfxExtractSubString(noColStr,ether,i,':');
		arp_ether[i] = (unsigned char)strtoul(noColStr.GetString(),NULL,16);
	}
	arp_ether[6] = '\0';

	return arp_ether;
}

void CARPDlg::CacheTableUpdate()
{
	mList_ARPCache.DeleteAllItems();

	LPARP_NODE head;
	LPARP_NODE currNode;

	head = m_ARP->cache_head;
	currNode = head;
	while(currNode!=NULL)
	{
		LV_ITEM lvItem;
		lvItem.iItem = mList_ARPCache.GetItemCount();
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 0;

		// IP
		CString ip;
		ip.Format("%d.%d.%d.%d",currNode->key.cache_ipaddr.addr_i0,
			currNode->key.cache_ipaddr.addr_i1,
			currNode->key.cache_ipaddr.addr_i2,
			currNode->key.cache_ipaddr.addr_i3);
		mList_ARPCache.SetItemText(lvItem.iItem, 0, (LPSTR)(LPCTSTR)ip);
		
		lvItem.pszText = (LPSTR)(LPCTSTR)ip;
		mList_ARPCache.InsertItem(&lvItem);

		// MAC
		CString ether;
		ether.Format("%.02x:%.02x:%.02x:%.02x:%.02x:%.02x",currNode->key.cache_enetaddr.addr_e0,
			currNode->key.cache_enetaddr.addr_e1,
			currNode->key.cache_enetaddr.addr_e2,
			currNode->key.cache_enetaddr.addr_e3,
			currNode->key.cache_enetaddr.addr_e4,
			currNode->key.cache_enetaddr.addr_e5);
		mList_ARPCache.SetItemText(lvItem.iItem, 1, (LPSTR)(LPCTSTR)ether);

		// Type
		CString type;
		switch(currNode->key.cache_type)
		{
			case CACHE_INCOMPLETE: type = "incomplete";	break;
			case CACHE_COMPLETE: type = "complete";		break;
		}
		mList_ARPCache.SetItemText(lvItem.iItem, 2, (LPSTR)(LPCTSTR)type);
		
		currNode = currNode->next;
	}
}

void CARPDlg::ProxyTableUpdate()
{
	mList_PARPEntry.DeleteAllItems();

	LPPARP_NODE head;
	LPPARP_NODE currNode;

	head = m_ARP->proxy_head;
	currNode = head;
	while(currNode!=NULL)
	{
		LV_ITEM lvItem;
		lvItem.iItem = mList_PARPEntry.GetItemCount();
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 0;

		// Device
		CString type;
		type = currNode->key.proxy_devname;
		mList_PARPEntry.SetItemText(lvItem.iItem, 0, (LPSTR)(LPCTSTR)type);

		lvItem.pszText = (LPSTR)(LPCTSTR)type;
		mList_PARPEntry.InsertItem(&lvItem);

		// IP
		CString ip;
		ip.Format("%d.%d.%d.%d",currNode->key.proxy_ipaddr.addr_i0,
			currNode->key.proxy_ipaddr.addr_i1,
			currNode->key.proxy_ipaddr.addr_i2,
			currNode->key.proxy_ipaddr.addr_i3);
		mList_PARPEntry.SetItemText(lvItem.iItem, 1, (LPSTR)(LPCTSTR)ip);	

		// MAC
		CString ether;
		ether.Format("%.02x:%.02x:%.02x:%.02x:%.02x:%.02x",currNode->key.proxy_enetaddr.addr_e0,
			currNode->key.proxy_enetaddr.addr_e1,
			currNode->key.proxy_enetaddr.addr_e2,
			currNode->key.proxy_enetaddr.addr_e3,
			currNode->key.proxy_enetaddr.addr_e4,
			currNode->key.proxy_enetaddr.addr_e5);
		mList_PARPEntry.SetItemText(lvItem.iItem, 2, (LPSTR)(LPCTSTR)ether);
	
		currNode = currNode->next;
	}
}

// ARP Message 설정
inline void CARPDlg::SettingARPMsg()
{
	unsigned char ether_DestEther[6];
	unsigned char arp_SenderIP[4];
	unsigned char arp_TargetIP[4];
	unsigned char *arp_SenderEther;
	unsigned char arp_TargetEther[6];
	
	arp_SenderEther = (u_char *)malloc(sizeof(u_char)*6);

	mIP_MyIP.GetAddress(arp_SenderIP[0],arp_SenderIP[1],arp_SenderIP[2],arp_SenderIP[3]);
	mIP_ARPIP.GetAddress(arp_TargetIP[0],arp_TargetIP[1],arp_TargetIP[2],arp_TargetIP[3]);

	arp_SenderEther = MacAddrToHexInt(mEdit_MyEther);

	memset(arp_TargetEther,0,6); 


	// ARP Header 설정
	m_ARP->SetHardType(ntohs(ARP_HARDTYPE));		// 1
	m_ARP->SetProtType(ntohs(ARP_PROTOTYPE_IP));	// 0x0800
	m_ARP->SetOpcode(ntohs(ARP_OPCODE_REQUEST));	// 1

	// Byte Ordering 하지 않는 것.
	m_ARP->SetSenderEtherAddress(arp_SenderEther);	// Sender의 MAC 주소
	m_ARP->SetSenderIPAddress(arp_SenderIP);		// Sender의 IP 주소
	m_ARP->SetTargetEtherAddress(arp_TargetEther);	// Target의 MAC 주소
	m_ARP->SetTargetIPAddress(arp_TargetIP);		// Target의 IP 주소

	// Ethernet Header 설정
	memset(ether_DestEther,0xff,6);					// Broadcast
	m_Ether->SetEnetDstAddress(ether_DestEther);	// Broadcast
	m_Ether->SetEnetSrcAddress(arp_SenderEther);	// Sender MAC 주소
}

// Gratuitous ARP Message 설정
inline void CARPDlg::SettingGARPMsg()
{
	unsigned char broadcast[6];
	unsigned char arp_SenderIP[4];
	unsigned char arp_TargetIP[4];
	unsigned char *arp_SenderEther;
	unsigned char arp_TargetEther[6];

	arp_SenderEther = (u_char *)malloc(sizeof(u_char)*6);

	mIP_MyIP.GetAddress(arp_SenderIP[0],arp_SenderIP[1],arp_SenderIP[2],arp_SenderIP[3]);
	mIP_ARPIP.GetAddress(arp_TargetIP[0],arp_TargetIP[1],arp_TargetIP[2],arp_TargetIP[3]);

	arp_SenderEther = MacAddrToHexInt(mEdit_GARPEther);

	memset(arp_TargetEther,0,6);

	// ARP Header 설정

	m_ARP->SetHardType(ntohs(ARP_HARDTYPE));	
	m_ARP->SetProtType(ntohs(ARP_PROTOTYPE_IP));	
	m_ARP->SetOpcode(ntohs(ARP_OPCODE_REQUEST));	
	//sender 와 target의 IP가 동일하게 설정된다.
	m_ARP->SetSenderEtherAddress(arp_SenderEther);	
	m_ARP->SetSenderIPAddress(arp_SenderIP);
	m_ARP->SetTargetEtherAddress(arp_TargetEther);
	m_ARP->SetTargetIPAddress(arp_SenderIP);

	memset(broadcast,0xff,6);
	m_Ether->SetEnetDstAddress(broadcast);
	m_Ether->SetEnetSrcAddress(arp_SenderEther);
}

void CARPDlg::OnLvnItemchangedParpTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
}


void CARPDlg::OnIpnFieldchangedArpIpaddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	*pResult = 0;
}


void CARPDlg::OnEnChangeMydevEthernet()
{

}
