// RouterDlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "Router.h"
#include "RouterDlg.h"
#include "RoutTableAdder.h"
#include "ProxyTableAdder.h"
#include "IPLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialog
{
public:
   CAboutDlg();
   

// 대화 상자 데이터입니다.
   enum { IDD = IDD_ABOUTBOX };

   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CRouterDlg 대화 상자
CRouterDlg::CRouterDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CRouterDlg::IDD, pParent), CBaseLayer("CRouterDlg")
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   //listIndex = -1;
   //ProxyListIndex = -1;
   // Layer 생성
   m_NILayer = new CNILayer("NI");
   m_EthernetLayer = new CEthernetLayer("Ethernet");
   m_ARPLayer = new CARPLayer("ARP");
   m_IPLayer = new CIPLayer("IP");
   //m_TCPLayer = new CTCPLayer("TCP");
   //// Layer 추가                              
   m_LayerMgr.AddLayer( this );            
   m_LayerMgr.AddLayer( m_NILayer );         
   m_LayerMgr.AddLayer( m_EthernetLayer );
   m_LayerMgr.AddLayer( m_ARPLayer );
   m_LayerMgr.AddLayer( m_IPLayer );         

   //Layer연결
   m_NILayer->SetUpperLayer(m_EthernetLayer);
   m_EthernetLayer->SetUnderLayer(m_NILayer);
   m_EthernetLayer->SetUpperLayer(m_IPLayer);
   m_EthernetLayer->SetUpperLayer(m_ARPLayer);
   m_ARPLayer->SetUnderLayer(m_EthernetLayer);
   m_ARPLayer->SetUpperLayer(m_IPLayer);
   m_IPLayer->SetUpperLayer(this);
   m_IPLayer->SetUnderLayer(m_ARPLayer);
   this->SetUnderLayer(m_IPLayer);
   //Layer 생성
}

void CRouterDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_ROUTING_TABLE, ListBox_RoutingTable);
   DDX_Control(pDX, IDC_CACHE_TABLE, ListBox_ARPCacheTable);
   DDX_Control(pDX, IDC_PROXY_TABLE, ListBox_ARPProxyTable);
   DDX_Control(pDX, IDC_NIC1_COMBO, m_nic1);
   DDX_Control(pDX, IDC_NIC2_COMBO, m_nic2);
   DDX_Control(pDX, IDC_IPADDRESS1, m_nic1_ip);
   DDX_Control(pDX, IDC_IPADDRESS2, m_nic2_ip);
}

BEGIN_MESSAGE_MAP(CRouterDlg, CDialog)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_CACHE_DELETE, &CRouterDlg::OnBnClickedCacheDelete)
   ON_BN_CLICKED(IDC_CACHE_DELETE_ALL, &CRouterDlg::OnBnClickedCacheDeleteAll)
   ON_BN_CLICKED(IDC_PROXY_DELETE, &CRouterDlg::OnBnClickedProxyDelete)
   ON_BN_CLICKED(IDC_PROXY_DELETE_ALL, &CRouterDlg::OnBnClickedProxyDeleteAll)
   ON_BN_CLICKED(IDC_PROXY_ADD, &CRouterDlg::OnBnClickedProxyAdd)
   ON_BN_CLICKED(IDCANCEL, &CRouterDlg::OnBnClickedCancel)
   ON_BN_CLICKED(IDC_NIC_SET_BUTTON, &CRouterDlg::OnBnClickedNicSetButton)
   ON_BN_CLICKED(IDC_ROUTING_ADD, &CRouterDlg::OnBnClickedRoutingAdd)
   ON_BN_CLICKED(IDC_ROUTING_DELETE, &CRouterDlg::OnBnClickedRoutingDelete)
   ON_CBN_SELCHANGE(IDC_NIC1_COMBO, &CRouterDlg::OnCbnSelchangeNic1Combo)
   ON_CBN_SELCHANGE(IDC_NIC2_COMBO, &CRouterDlg::OnCbnSelchangeNic2Combo)
END_MESSAGE_MAP()



BOOL CRouterDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

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

   SetIcon(m_hIcon, TRUE);         
   SetIcon(m_hIcon, FALSE);      

   
   // ListBox에 초기 Colum을 선언
   ListBox_RoutingTable.InsertColumn(0,_T("Destination"),LVCFMT_CENTER,100,-1);
   ListBox_RoutingTable.InsertColumn(1,_T("NetMask"),LVCFMT_CENTER,120,-1);
   ListBox_RoutingTable.InsertColumn(2,_T("Gateway"),LVCFMT_CENTER,80,-1);
   ListBox_RoutingTable.InsertColumn(3,_T("Flag"),LVCFMT_CENTER,49,-1);
   ListBox_RoutingTable.InsertColumn(4,_T("Interface"),LVCFMT_CENTER,80,-1);
   ListBox_RoutingTable.InsertColumn(5,_T("Metric"),LVCFMT_CENTER,49,-1);

   ListBox_ARPCacheTable.InsertColumn(0,_T("IP address"),LVCFMT_CENTER,100,-1);
   ListBox_ARPCacheTable.InsertColumn(1,_T("Mac address"),LVCFMT_CENTER,120,-1);
   ListBox_ARPCacheTable.InsertColumn(2,_T("Type"),LVCFMT_CENTER,80,-1);
   //ListBox_ARPCacheTable.InsertColumn(3,_T("Time"),LVCFMT_CENTER,49,-1);

   ListBox_ARPProxyTable.InsertColumn(0,_T("name"),LVCFMT_CENTER,60,-1);
   ListBox_ARPProxyTable.InsertColumn(1,_T("IP address"),LVCFMT_CENTER,120,-1);
   ListBox_ARPProxyTable.InsertColumn(2,_T("Mac address"),LVCFMT_CENTER,120,-1);

   setNicList(); //NicList Setting
   return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRouterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRouterDlg::OnPaint()
{
   if (IsIconic())
   {
      CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // 아이콘을 그립니다.
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

HCURSOR CRouterDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}


void CRouterDlg::OnBnClickedCacheDelete()
{
   //CacheDeleteAll버튼
   int index = -1;
   index = ListBox_ARPCacheTable.GetSelectionMark();
   if(index != -1){
      POSITION pos = m_ARPLayer->Cache_Table.FindIndex(index);
      m_ARPLayer->Cache_Table.RemoveAt(pos);
      m_ARPLayer->updateCacheTable();
   }
}

void CRouterDlg::OnBnClickedCacheDeleteAll()
{
   //CacheDeleteAll버튼
   m_ARPLayer->Cache_Table.RemoveAll();
   m_ARPLayer->updateCacheTable();
}
void CRouterDlg::OnBnClickedProxyDelete()
{
   //proxy delete버튼
   m_ARPLayer->Proxy_Table.RemoveAll();
   m_ARPLayer->updateProxyTable();
}

void CRouterDlg::OnBnClickedProxyDeleteAll()
{
   //proxy delete all 버튼
   int index = -1;
   index = ListBox_ARPProxyTable.GetSelectionMark();
   if(index != -1){
      POSITION pos = m_ARPLayer->Proxy_Table.FindIndex(index);
      m_ARPLayer->Proxy_Table.RemoveAt(pos);
      m_ARPLayer->updateProxyTable();
   }
}

void CRouterDlg::OnBnClickedProxyAdd()
{
   // proxy add 버튼
   CString str;
   unsigned char Ip[4];
   unsigned char Mac[8];
   ProxyTableAdder PDlg;
   if(   PDlg.DoModal() == IDOK)
   {
      str = PDlg.getName();
      memcpy(Ip , PDlg.getIp() , 4);
      memcpy(Mac , PDlg.getMac() , 6);
   }
}

void CRouterDlg::OnBnClickedCancel()
{
   // 종료 버튼
   exit(0);
}

void CRouterDlg::OnBnClickedNicSetButton()
{
   LPADAPTER adapter = NULL;      // 랜카드에 대한 정보를 저장하는 pointer 변수
   PPACKET_OID_DATA OidData;
   pcap_if_t *Devices;
   OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
   OidData->Oid = 0x01010101;
   OidData->Length = 6;
   ZeroMemory(OidData->Data,6);
   char DeviceName1[512];
   char DeviceName2[512];
   char strError[30];
   if(pcap_findalldevs_ex( PCAP_SRC_IF_STRING, NULL , &Devices , strError) != 0) {
      printf("pcap_findalldevs_ex() error : %s\n", strError);
   }
   m_nic1.GetLBText(m_nic1.GetCurSel() , DeviceName1);   // 콤보 박스에 선택된 Device의 이름을 받아온다
   m_nic2.GetLBText(m_nic2.GetCurSel() , DeviceName2);
   while(Devices != NULL) {
      if(!strcmp(Devices->description,DeviceName1))
         Device1 = Devices;
      if(!strcmp(Devices->description,DeviceName2))
         Device2 = Devices;
      Devices = Devices->next;
   }
   // device선택
   m_NILayer->SetDevice(Device1,1);
   m_NILayer->SetDevice(Device2,2);
   
   RtDlg.setDeviceList(Device1->description,Device2->description);
   //mac 주소 설정
   adapter = PacketOpenAdapter((Device1->name+8));
   PacketRequest( adapter, FALSE, OidData);
   m_EthernetLayer->SetSourceAddress(OidData->Data,1);
   adapter = PacketOpenAdapter((Device2->name+8));
   PacketRequest( adapter, FALSE, OidData);
   m_EthernetLayer->SetSourceAddress(OidData->Data,2);
   //ip주소 설정
   // 랜카드에 대한 정보들을 읽어와 Combo 박스에 추가하는 부분
   unsigned char nic1_ip[4];
   unsigned char nic2_ip[4];
   m_nic1_ip.GetAddress((BYTE &)nic1_ip[0],(BYTE &)nic1_ip[1],(BYTE &)nic1_ip[2],(BYTE &)nic1_ip[3]);
   m_nic2_ip.GetAddress((BYTE &)nic2_ip[0],(BYTE &)nic2_ip[1],(BYTE &)nic2_ip[2],(BYTE &)nic2_ip[3]);
   m_IPLayer->SetSrcIP(nic1_ip,1);
   m_IPLayer->SetSrcIP(nic2_ip,2);
   // receive Thread start


   m_NILayer->StartReadThread();
   GetDlgItem(IDC_NIC_SET_BUTTON)->EnableWindow(0);
}

void CRouterDlg::OnBnClickedRoutingAdd()
{
   // router Table Add버튼
   if( RtDlg.DoModal() == IDOK ){
      RoutingTable rt;
      memcpy(&rt.Destnation,RtDlg.GetDestIp(),6);
      rt.Flag = RtDlg.GetFlag();
      memcpy(&rt.Gateway,RtDlg.GetGateway(),6);
      memcpy(&rt.Netmask,RtDlg.GetNetmask(),6);
      rt.Interface = RtDlg.GetInterface();
      rt.Metric = RtDlg.GetMetric();
      route_table.AddTail(rt);
      UpdateRouteTable();
   }
}

void CRouterDlg::OnBnClickedRoutingDelete()
{
   
   int index = -1;
   index = ListBox_RoutingTable.GetSelectionMark();
   if(index != -1){
      POSITION pos = route_table.FindIndex(index);
      route_table.RemoveAt(pos);
      UpdateRouteTable();
   }// router Table delete버튼이다
}

// NicList Set
void CRouterDlg::setNicList(void)
{
   pcap_if_t *Devices;
   char strError[30];
   if(pcap_findalldevs_ex( PCAP_SRC_IF_STRING, NULL , &Devices , strError) != 0) {
      printf("pcap_findalldevs_ex() error : %s\n", strError);
   }

   //set device_1
   while(Devices != NULL) {
      m_nic1.AddString(Devices->description);
      m_nic2.AddString(Devices->description);
      Devices = Devices->next;
   }
   m_nic1.SetCurSel(0);
   m_nic2.SetCurSel(1);
}   //랜카드 장비를 선택한다

void CRouterDlg::add_route_table(unsigned char dest[4],unsigned char netmask[4],unsigned char gateway[4],unsigned char flag,char Interface[100],int metric)
{
   RoutingTable rt;
   memcpy(&rt.Destnation,dest,4);
   memcpy(&rt.Netmask,netmask,4);
   memcpy(&rt.Gateway,gateway,4);
   rt.Flag = flag;
   memcpy(&rt.Interface, Interface, 100);
   rt.Metric = metric;
}//라우트 테이블을 추가할때 쓰는 창이다

void CRouterDlg::UpdateRouteTable(void)
{
   ListBox_RoutingTable.DeleteAllItems(); //라우팅 테이블에있는거 전부 삭제
   CString dest,netmask,gateway,flag,Interface,metric; //
   POSITION index;
   RoutingTable entry; //head position
   for(int i=0;i<route_table.GetCount();i++){
      flag = "";
      index = route_table.FindIndex(i);
      entry = route_table.GetAt(index);
      dest.Format("%d.%d.%d.%d",entry.Destnation[0],entry.Destnation[1],entry.Destnation[2],entry.Destnation[3]);
	  // dest 에 "%d.%d.%d.%d"형식으로 entry.Destnation 저장
      netmask.Format("%d.%d.%d.%d",entry.Netmask[0],entry.Netmask[1],entry.Netmask[2],entry.Netmask[3]);
	  // netmask 에 "%d.%d.%d.%d" 형식으로 entry.netmask 저장
      gateway.Format("%d.%d.%d.%d",entry.Gateway[0],entry.Gateway[1],entry.Gateway[2],entry.Gateway[3]);
	  // gateway 에 "%d.%d.%d.%d" 형식으로 entry.gatewqy 저장
      if((entry.Flag & 0x01) == 0x01)//연산해서 0x01 이면 
         flag += "U";// 플래그변수에 U 추가
      if((entry.Flag & 0x02) == 0x02)// 연산해서 0x02 이면
         flag += "G"; //플래그 변수에 G 추가
      if((entry.Flag & 0x04) == 0x04)// 연산해서 0x04 면
         flag += "H";// 플래그 변수에 H 추가
      Interface.Format("%d",entry.Interface); //interface 에 entry.Interface(번호) 를 넣는다.
      metric.Format("%d",entry.Metric);// metric 에 Metric(번호)를 넣는다.
      ListBox_RoutingTable.InsertItem(i,dest);// RoutingTable 목적지 추가
      ListBox_RoutingTable.SetItem(i,1,LVIF_TEXT,netmask,0,0,0,NULL);// netmask 추가
      ListBox_RoutingTable.SetItem(i,2,LVIF_TEXT,gateway,0,0,0,NULL);// gateway 추가
      ListBox_RoutingTable.SetItem(i,3,LVIF_TEXT,flag,0,0,0,NULL);// flag 추가
      ListBox_RoutingTable.SetItem(i,4,LVIF_TEXT,Interface,0,0,0,NULL);// interface 추가
      ListBox_RoutingTable.SetItem(i,5,LVIF_TEXT,metric,0,0,0,NULL);// metric 추가
   }
   ListBox_RoutingTable.UpdateWindow();
}//입력받은 라우팅 테이블의 정보들을 라우팅테이블의 기능을 하도록 도착 네트워크주소와 마스킹 게이트웨이 Flag를 설정한다

int CRouterDlg::Routing(unsigned char destip[4]){
   POSITION index;
   RoutingTable entry;
   RoutingTable select_entry;
   entry.Interface = -2;
   select_entry.Interface = -2;
   unsigned char result[4];
   for(int i=0;i<route_table.GetCount();i++){
      index = route_table.FindIndex(i);
      entry = route_table.GetAt(index);
      if(select_entry.Interface == -2){//더이상 entry에 같은게 존재하지않을때
         for(int j=0;j<4;j++)
            result[j] = destip[j] & entry.Netmask[j];//masking을 통해 result값을 구한다
         if(!memcmp(result,entry.Destnation,4)){ //destation일치하는게 있을 경우
            if(((entry.Flag & 0x01) == 0x01) && ((entry.Flag & 0x02) == 0x02)){ //Flag에 UG라서 gateway로 보낸다
               select_entry = entry;
               m_IPLayer->SetDstIP(entry.Gateway);
            }
            else if(((entry.Flag & 0x01) == 0x01) && ((entry.Flag & 0x02) == 0x00)){ //gateway가 아닐경우에는 도착주소로 보낸다
               select_entry = entry;
               m_IPLayer->SetDstIP(destip);
            }
         }
      }
      else{ //entry에 존재할때
         for(int j=0;j<4;j++)
            result[j] = destip[j] & entry.Netmask[j];
         if(memcmp(result,entry.Netmask,4)){ //기존 select비트 보다 더 긴 마스킹을 만족할때
            for(int j=0;j<4;j++)
               result[j] = destip[j] & entry.Netmask[j];//masking을 하여 나온값을 result에 저장한다
            if(!memcmp(result,entry.Destnation,4)){ //나온 result값이 destation이 같을때
               if(((entry.Flag & 0x01) == 0x01) && ((entry.Flag & 0x02) == 0x02)){ //Flag가 UG라서 게이트웨이로 보내야한다
                  select_entry = entry;
                  m_IPLayer->SetDstIP(entry.Gateway);
               }
               else if(((entry.Flag & 0x01) == 0x01) && ((entry.Flag & 0x02) == 0x00)){ //Flag가 G가 아닐경우에는 해당 주소로 보낸다.
                  select_entry = entry;
                  m_IPLayer->SetDstIP(destip);
               }
            }
         }
         //더 적을땐 for문을 통과한다.
      }
   }
   return select_entry.Interface+1;
}

void CRouterDlg::OnCbnSelchangeNic1Combo()
{// ip주소 설정
}

void CRouterDlg::OnCbnSelchangeNic2Combo()
{ //ip 주소 설정
}