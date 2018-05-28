
#include "stdafx.h"
#include "ARP.h"
#include "PrxoyARPDlg.h"
#include "afxdialogex.h"


// PrxoyARPDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(PrxoyARPDlg, CDialog)

PrxoyARPDlg::PrxoyARPDlg(CWnd* pParent)
	: CDialog(PrxoyARPDlg::IDD, pParent)
{
	ResetHeader();
}

PrxoyARPDlg::~PrxoyARPDlg()
{
}

void PrxoyARPDlg::ResetHeader()
{
	memset(m_proxy.proxy_ipaddr.addrs_i,0,4);
	memset(m_proxy.proxy_enetaddr.addrs_e,0,6);
	memset(m_proxy.proxy_devname,0,PM_DEVICE_NAME_LENGTH);
}

void PrxoyARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_PM_DEVICE,mCombo_PARPDevice);
	DDX_Control(pDX,IDC_PM_IPADDRESS,mIP_PARPIP);
	DDX_Text(pDX,IDC_PM_ETHERNET,mEdit_PARPEther);
}// proxy arp Entry 창에서 추가를 눌렀을때 device name, IPAddress, EtherAddress를 설정하게 해준다.


BEGIN_MESSAGE_MAP(PrxoyARPDlg, CDialog)
	ON_BN_CLICKED(ID_PM_OK, &PrxoyARPDlg::OnBnClickedPmOk)
	ON_BN_CLICKED(ID_PM_CANCEL, &PrxoyARPDlg::OnBnClickedPmCancel)
END_MESSAGE_MAP()


// PrxoyARPDlg 메시지 처리기입니다.
BOOL PrxoyARPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_PM_DEVICE))->AddString("Default Device");
	((CComboBox*)GetDlgItem(IDC_PM_DEVICE))->SetCurSel(0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void PrxoyARPDlg::SetProxyIPAddress(unsigned char* ip)
{
	memcpy(m_proxy.proxy_ipaddr.addrs_i,ip,4);
}

void PrxoyARPDlg::SetProxyEtherAddress(unsigned char* ether)
{
	memcpy(m_proxy.proxy_enetaddr.addrs_e,ether,6);
}

void PrxoyARPDlg::SetProxyDevName(unsigned char* devname)
{
	memcpy(m_proxy.proxy_devname,devname,strlen((char *)devname));
}
	
unsigned char* PrxoyARPDlg::GetProxyIPAddress()
{
	return m_proxy.proxy_ipaddr.addrs_i;
}

unsigned char* PrxoyARPDlg::GetProxyEtherAddress()
{
	return m_proxy.proxy_enetaddr.addrs_e;
}

unsigned char* PrxoyARPDlg::GetProxyDevName()
{
	return m_proxy.proxy_devname;
}

void PrxoyARPDlg::OnBnClickedPmOk()
{
	// 모달 대화상자에 입력된 값들을 메인 다이얼로그로 전송한다
	// Proxy Entry 값을 임의로 만든 Proxy Item 구조체에 값을 보낸다.
	// Device ->네트워크어댑터의 번호를 저장 ->저장되는 ip주소의 호스트가 어떤링크상에 연결되는지 나타내기위해
	CString devname;
	GetDlgItemText(IDC_PM_DEVICE,devname);
	SetProxyDevName((unsigned char *)devname.GetString());

	 // IP -> 대행자가 대신 arp 응답을 해주는 호스트의 ip 주소
	BYTE proxy_ip[4];
	mIP_PARPIP.GetAddress(proxy_ip[0],proxy_ip[1],proxy_ip[2],proxy_ip[3]);
	SetProxyIPAddress(proxy_ip);
	
	 // Ethernet ->그에 대응되는 물리적 주소로써 이더넷주소가 저장
	CString cstr, ether;
	GetDlgItemText(IDC_PM_ETHERNET,ether);
	unsigned char* arp_ether = (u_char *)malloc(sizeof(u_char)*6);
	for(int i=0;i<6;i++){
		AfxExtractSubString(cstr,ether,i,':');
		// strtoul -> 문자열을 원하는 진수로 변환 시켜준다.
		arp_ether[i] = (unsigned char)strtoul(cstr.GetString(),NULL,16);
	}
	arp_ether[6] = '\0';
	SetProxyEtherAddress(arp_ether);

	CDialog::OnOK();
}


void PrxoyARPDlg::OnBnClickedPmCancel()
{
	// 모달 대화상자 종료
	CDialog::OnCancel();
}
