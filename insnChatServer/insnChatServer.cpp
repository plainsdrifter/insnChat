// insnChatServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "insnChatServer.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CinsnChatServerApp

BEGIN_MESSAGE_MAP(CinsnChatServerApp, CWinApp)
END_MESSAGE_MAP()


// CinsnChatServerApp construction

CinsnChatServerApp::CinsnChatServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CinsnChatServerApp object

CinsnChatServerApp theApp;


// CinsnChatServerApp initialization

BOOL CinsnChatServerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("insnChatServer"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

BOOL CinsnChatServerApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

// CinsnChatServerApp message handlers
