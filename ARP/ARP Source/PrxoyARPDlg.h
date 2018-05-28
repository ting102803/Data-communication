
#pragma once

// PrxoyARPDlg ��ȭ �����Դϴ�.

class PrxoyARPDlg : public CDialog
{
	DECLARE_DYNAMIC(PrxoyARPDlg)

public:
	PrxoyARPDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PrxoyARPDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PARP_DIALOG };

private:
	inline void		ResetHeader( );

public:
	BOOL dd;
	//PARP �߰� ���̾�α�
	CComboBox mCombo_PARPDevice;	// PARP ComboBox
	CIPAddressCtrl mIP_PARPIP;		// PARP IP Address
	CString mEdit_PARPEther;		// PARP Ethernet Address

	void	SetProxyIPAddress(unsigned char* ip);
	void	SetProxyEtherAddress(unsigned char* ether);
	void	SetProxyDevName(unsigned char* devname);
	
	unsigned char*	GetProxyIPAddress();
	unsigned char*	GetProxyEtherAddress();
	unsigned char*	GetProxyDevName();

protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	PROXY_ENTRY m_proxy;

public:
	afx_msg void OnBnClickedPmOk();
	afx_msg void OnBnClickedPmCancel();
};
