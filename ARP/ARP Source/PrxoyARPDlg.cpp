
#include "stdafx.h"
#include "ARP.h"
#include "PrxoyARPDlg.h"
#include "afxdialogex.h"


// PrxoyARPDlg ��ȭ �����Դϴ�.

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
}// proxy arp Entry â���� �߰��� �������� device name, IPAddress, EtherAddress�� �����ϰ� ���ش�.


BEGIN_MESSAGE_MAP(PrxoyARPDlg, CDialog)
	ON_BN_CLICKED(ID_PM_OK, &PrxoyARPDlg::OnBnClickedPmOk)
	ON_BN_CLICKED(ID_PM_CANCEL, &PrxoyARPDlg::OnBnClickedPmCancel)
END_MESSAGE_MAP()


// PrxoyARPDlg �޽��� ó�����Դϴ�.
BOOL PrxoyARPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_PM_DEVICE))->AddString("Default Device");
	((CComboBox*)GetDlgItem(IDC_PM_DEVICE))->SetCurSel(0);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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
	// ��� ��ȭ���ڿ� �Էµ� ������ ���� ���̾�α׷� �����Ѵ�
	// Proxy Entry ���� ���Ƿ� ���� Proxy Item ����ü�� ���� ������.
	// Device ->��Ʈ��ũ������� ��ȣ�� ���� ->����Ǵ� ip�ּ��� ȣ��Ʈ�� ���ũ�� ����Ǵ��� ��Ÿ��������
	CString devname;
	GetDlgItemText(IDC_PM_DEVICE,devname);
	SetProxyDevName((unsigned char *)devname.GetString());

	 // IP -> �����ڰ� ��� arp ������ ���ִ� ȣ��Ʈ�� ip �ּ�
	BYTE proxy_ip[4];
	mIP_PARPIP.GetAddress(proxy_ip[0],proxy_ip[1],proxy_ip[2],proxy_ip[3]);
	SetProxyIPAddress(proxy_ip);
	
	 // Ethernet ->�׿� �����Ǵ� ������ �ּҷν� �̴����ּҰ� ����
	CString cstr, ether;
	GetDlgItemText(IDC_PM_ETHERNET,ether);
	unsigned char* arp_ether = (u_char *)malloc(sizeof(u_char)*6);
	for(int i=0;i<6;i++){
		AfxExtractSubString(cstr,ether,i,':');
		// strtoul -> ���ڿ��� ���ϴ� ������ ��ȯ �����ش�.
		arp_ether[i] = (unsigned char)strtoul(cstr.GetString(),NULL,16);
	}
	arp_ether[6] = '\0';
	SetProxyEtherAddress(arp_ether);

	CDialog::OnOK();
}


void PrxoyARPDlg::OnBnClickedPmCancel()
{
	// ��� ��ȭ���� ����
	CDialog::OnCancel();
}
