#include "stdafx.h"
#include "Router.h"
#include "RoutTableAdder.h"

IMPLEMENT_DYNAMIC(CRoutTableAdder, CDialog)

CRoutTableAdder::CRoutTableAdder()
   : CDialog(CRoutTableAdder::IDD, NULL)
   , m_metric(0)
{
}

CRoutTableAdder::~CRoutTableAdder()
{
}

//각 버튼에 해당하는 이벤트를 연결한다.
void CRoutTableAdder::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_DESTINATION_IPADDRESS, m_add_dest);
   DDX_Control(pDX, IDC_NETMASK_IPADDRESS, m_add_netmask);
   DDX_Control(pDX, IDC_GATEWAY_IPADDRESS, m_gateway);
   DDX_Control(pDX, IDC_INTERFACE_COMBO, m_add_interface);
   DDX_Control(pDX, IDC_UP_CHECK, m_flag_u);
   DDX_Control(pDX, IDC_GATEWAY_CHECK, m_flag_g);
   DDX_Control(pDX, IDC_HOST_CHECK, m_flag_h);
   DDX_Text(pDX, IDC_METRIC_EDIT, m_metric);
   m_add_interface.AddString(d1);
   m_add_interface.AddString(d2);
   m_add_interface.SetCurSel(0);
}


BEGIN_MESSAGE_MAP(CRoutTableAdder, CDialog)
   ON_BN_CLICKED(IDOK, &CRoutTableAdder::OnBnClickedOk)
   ON_BN_CLICKED(IDCANCEL, &CRoutTableAdder::OnBnClickedCancel)
END_MESSAGE_MAP()

void CRoutTableAdder::OnBnClickedOk()
{
   char buffer[16];
   m_add_dest.GetWindowText(buffer, sizeof(buffer));
   sscanf(buffer, "%d.%d.%d.%d", &dest_ip[0], &dest_ip[1], &dest_ip[2], &dest_ip[3], sizeof(buffer));

   m_add_netmask.GetWindowText(buffer, sizeof(buffer));
   sscanf(buffer, "%d.%d.%d.%d", &net_ip[0], &net_ip[1], &net_ip[2], &net_ip[3], sizeof(buffer));

   m_gateway.GetWindowText(buffer, sizeof(buffer));
   sscanf(buffer, "%d.%d.%d.%d", &gate_ip[0], &gate_ip[1], &gate_ip[2], &gate_ip[3], sizeof(buffer));
   
   router_interface = m_add_interface.GetCurSel();

   flag = 0x00;
   if(m_flag_u.GetCheck())
      flag = flag | 0x01;
   if(m_flag_g.GetCheck())
      flag = flag | 0x02;
   if(m_flag_h.GetCheck())
      flag = flag | 0x04;

   CDialog::OnOK();
}


void CRoutTableAdder::OnBnClickedCancel()
{
   OnCancel();
}

unsigned char * CRoutTableAdder::GetDestIp(void)
{
   return dest_ip;
}

unsigned char * CRoutTableAdder::GetNetmask(void)
{
   return net_ip;
}

unsigned char * CRoutTableAdder::GetGateway(void)
{
   return gate_ip;
}

int CRoutTableAdder::GetInterface(void)
{
   return router_interface;
}

int CRoutTableAdder::GetMetric(void)
{
   return m_metric;
}

unsigned char CRoutTableAdder::GetFlag(void)
{
   return flag;
}

void CRoutTableAdder::setDeviceList(CString dev1,CString dev2){
   d1 = dev1;
   d2 = dev2;
}