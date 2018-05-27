
#include "stdafx.h"
#include "ipc.h"
#include "IPCAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CIPCApp, CWinApp)

	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


CIPCApp::CIPCApp()
{

}

CIPCApp theApp;

BOOL CIPCApp::InitInstance()
{
	AfxEnableControlContainer();

	CIPCAppDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}
	return FALSE;
}
