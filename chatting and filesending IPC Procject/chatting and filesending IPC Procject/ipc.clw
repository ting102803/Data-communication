; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CIPCAppDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ipc.h"

ClassCount=3
Class1=CIPCApp
Class2=CIPCAppDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_IPC_DIALOG

[CLS:CIPCApp]
Type=0
HeaderFile=ipc.h
ImplementationFile=ipc.cpp
Filter=N

[CLS:CIPCAppDlg]
Type=0
HeaderFile=IPCAppDlg.h
ImplementationFile=IPCAppDlg.cpp
Filter=D
LastObject=CIPCAppDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=IPCAppDlg.h
ImplementationFile=IPCAppDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_IPC_DIALOG]
Type=1
Class=CIPCAppDlg
ControlCount=10
Control1=IDC_EDIT_SRC,edit,1350631553
Control2=IDC_EDIT_DST,edit,1350631553
Control3=IDC_CHECK_TOALL,button,1342242819
Control4=IDC_BUTTON_ADDR,button,1342242816
Control5=IDC_EDIT_MSG,edit,1350631552
Control6=IDC_BUTTON_SEND,button,1342242816
Control7=IDC_STATIC,static,1342177297
Control8=IDC_LIST_CHAT,listbox,1352728833
Control9=IDC_STATIC,static,1342308864
Control10=IDC_STATIC,static,1342308864

