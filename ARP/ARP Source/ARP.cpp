

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
// CARPApp 생성1

CARPApp::CARPApp()
{
}


// 유일한 CARPApp 개체입니다.

CARPApp theApp;


// CARPApp 초기화

BOOL CARPApp::InitInstance()
{
	CWinApp::InitInstance();
	// 대화 상자에 셀 트리 뷰 또는
	// 셀 목록 뷰 컨트롤이 포함되어 있는 경우 셀 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;
	
	CARPDlg dlg;
	dlg.DoModal();
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를 리턴하여 끝낸다.
	return FALSE;
}

