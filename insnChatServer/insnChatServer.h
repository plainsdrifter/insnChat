// insnChatServer.h : main header file for the insnChatServer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "ServerDlg.h"

// CinsnChatServerApp:
// See insnChatServer.cpp for the implementation of this class
//

class CinsnChatServerApp : public CWinApp
{
public:
	CinsnChatServerApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

// Implementation

public:
	DECLARE_MESSAGE_MAP()
};

extern CinsnChatServerApp theApp;