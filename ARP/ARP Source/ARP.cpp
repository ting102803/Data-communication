

#include "stdafx.h"
#include "ARP.h"
#include "ARPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CARPApp
BEGIN_MESSAGE_MAP(CARPApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()
// CARPApp ����1

CARPApp::CARPApp()
{
}


// ������ CARPApp ��ü�Դϴ�.

CARPApp theApp;


// CARPApp �ʱ�ȭ

BOOL CARPApp::InitInstance()
{
	CWinApp::InitInstance();
	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;
	
	CARPDlg dlg;
	dlg.DoModal();
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE�� �����Ͽ� ������.
	return FALSE;
}

