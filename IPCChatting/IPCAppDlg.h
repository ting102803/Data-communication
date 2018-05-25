// IPCAppDlg.h : header file
//

#if !defined(AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
#define AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_

#include "LayerManager.h"	// Added by ClassView
#include "ChatAppLayer.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "FileLayer.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg dialog

class CIPCAppDlg 
: public CDialog, 
  public CBaseLayer
{
// Construction
public:
	CIPCAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIPCAppDlg)
	enum { IDD = IDD_IPC_DIALOG };
	CListBox	m_ListChat;
	UINT	m_unDstAddr;
	UINT	m_unSrcAddr;
	CString	m_stMessage;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPCAppDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CIPCAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSendMessage();
	afx_msg void OnButtonAddrSet();
	afx_msg void OnCheckBroadcast();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL			Receive( unsigned char* ppayload );
	inline void		SendData( );

private:
	CLayerManager	m_LayerMgr;
	int				m_nAckReady;
	enum {			IPC_INITIALIZING, 
					IPC_READYTOSEND, 
					IPC_WAITFORACK,
					IPC_ERROR,
					IPC_BROADCASTMODE,
					IPC_UNICASTMODE,
					IPC_ADDR_SET,
					IPC_ADDR_RESET	} ;

	void			SetDlgState( int state );
	inline void		EndofProcess( );
	inline void		SetRegstryMessage( );
	LRESULT			OnRegSendMsg(WPARAM wParam, LPARAM lParam) ;
	LRESULT			OnRegAckMsg(WPARAM wParam, LPARAM lParam) ;

	BOOL			m_bSendReady;

// Object App
	CChatAppLayer*	m_ChatApp ;

// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
