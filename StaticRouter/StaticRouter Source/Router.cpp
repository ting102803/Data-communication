#include "stdafx.h"
#include "Router.h"
#include "RouterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRouterApp

BEGIN_MESSAGE_MAP(CRouterApp, CWinApp)
   ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CRouterApp::CRouterApp()
{
}

CRouterApp theApp;


// CRouterApp 초기화
BOOL CRouterApp::InitInstance()
{
   INITCOMMONCONTROLSEX InitCtrls;
   InitCtrls.dwSize = sizeof(InitCtrls);
   InitCtrls.dwICC = ICC_WIN95_CLASSES;
   InitCommonControlsEx(&InitCtrls);

   CWinApp::InitInstance();
   SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

   CRouterDlg dlg;
   m_pMainWnd = &dlg;
   INT_PTR nResponse = dlg.DoModal();
   if (nResponse == IDOK)
   {
   }
   else if (nResponse == IDCANCEL)
   {
   }
   return FALSE;
}