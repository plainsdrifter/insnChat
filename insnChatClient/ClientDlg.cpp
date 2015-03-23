// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "insnChatClient.h"
#include "ClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CClientDlg dialog

IMPLEMENT_SERIAL(CClientDlg, CDialog, VERSIONABLE_SCHEMA | 2);

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	ZeroMemory(&m_nidIconData, sizeof(m_nidIconData));

	InitNotifyIconData();

	m_bIsConnected = FALSE;
	m_bInTray = FALSE;
}

CClientDlg::~CClientDlg()
{
	CString peer;
	UINT port;

	if (m_sktClient.m_hSocket != INVALID_SOCKET)
	{
		m_sktClient.GetPeerNameEx(peer, port);
		if (!peer.IsEmpty() && IsConnected())
		{
			SendToServer(_T("/quit"));
		}
	}

	m_sktClient.Close();
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHAT, m_lstChat);
	DDX_Control(pDX, IDC_OTHERCLIENTS, m_lstClients);
	DDX_Control(pDX, IDC_MESSAGE, m_txtMessage);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	//{{AFX_MSG_MAP(CClientDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnSendClicked)
	ON_BN_CLICKED(IDC_SETTINGS, &CClientDlg::OnBnClickedSettings)
	ON_BN_CLICKED(IDC_CONNECT, &CClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_SENDFILE, &CClientDlg::OnBnClickedSendFile)
	ON_COMMAND(ID__COPY, &CClientDlg::OnCopyClicked)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE, &CClientDlg::OnTrayNotify)
	ON_COMMAND(ID_A_RESTORE, &CClientDlg::OnTrayRestore)
	ON_COMMAND(ID_TRAY_CLOSE, &CClientDlg::OnTrayClose)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_lstClients.SetClient(this);
	m_lstChat.SetClient(this);

	m_nidIconData.hWnd = this->m_hWnd;
	m_nidIconData.hIcon = m_hIcon;
	wcscpy_s(m_nidIconData.szTip, _T("insnChat Client"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CClientDlg::InitNotifyIconData()
{
	m_nidIconData.cbSize			= NOTIFYICONDATA_V3_SIZE;//sizeof(NOTIFYICONDATA);	
	m_nidIconData.hWnd				= 0;
	m_nidIconData.uID				= 1;
	m_nidIconData.uCallbackMessage	= WM_TRAY_ICON_NOTIFY_MESSAGE;	
	m_nidIconData.hIcon				= 0;
	m_nidIconData.szTip[0]			= 0;
	m_nidIconData.uFlags			= NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
	//m_nidIconData.uTimeout = 2000;
	//m_nidIconData.uVersion = NOTIFYICON_VERSION;
	m_nidIconData.dwInfoFlags = NIIF_INFO;

	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CClientDlg::OnBnSendClicked()
{
	CString info;
	m_txtMessage.GetWindowTextW(info);
	m_txtMessage.SetSel(0,-1);
	m_txtMessage.Clear();
	info = info.TrimLeft();
	if (IsConnected() && !info.IsEmpty())
		SendToServer(info);
}

void CClientDlg::PrintToChat(CString output)
{
	int n = m_lstChat.GetCount();
	m_lstChat.InsertString(n, output);
}

BOOL CClientDlg::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->lParam == 1835009 || pMsg->lParam == VK_RETURN)
		{
			if(IDC_MESSAGE == ::GetDlgCtrlID(pMsg->hwnd))
			{
				if (::GetAsyncKeyState(1900545) || ::GetAsyncKeyState(VK_CONTROL))
				{
				}
				else
				{
					CString info;
					m_txtMessage.GetWindowTextW(info);
					m_txtMessage.SetSel(0,-1);
					m_txtMessage.Clear();
					info = info.TrimLeft();
					if (IsConnected() && !info.IsEmpty())
						SendToServer(info);
				}
			}
		}
		if (pMsg->lParam == 3014657)
		{
			if (IDC_CHAT == ::GetDlgCtrlID(pMsg->hwnd))
			{
				if (::GetAsyncKeyState(1900545) || ::GetAsyncKeyState(VK_CONTROL))
				{
					OnCopyClicked();
				}
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CClientDlg::HandleReturns(CString info)
{
	CString line;
	int i = 0;
	line = info.Tokenize(_T("\r\n"),i);
	while (line != _T(""))
	{
		PrintToChat(line);
		line = info.Tokenize(_T("\r\n"),i);
	}
	ShowBalloonTip(info);
}

void CClientDlg::SendToServer(CString message)
{
	int i = 0;
	CString command;
	command = message.Tokenize(_T("#"), i);
	if ( command == _T("/nick"))
	{
		SetNick(message.Tokenize(_T("#"), i));
	}

	message = GetNick() + _T(": ") + message;
	int nSent;
	LPTSTR buf = message.GetBuffer();
	message.ReleaseBuffer();
	nSent = m_sktClient.Send(buf, 2*message.GetLength());
	switch(nSent)
	{
	case SOCKET_ERROR:
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			error.Format(_T("Error occured on sending! Error: %i"), GetLastError());
			AfxMessageBox(error);
			m_sktClient.Close();
		}
	}
}

void CClientDlg::OnBnClickedSettings()
{
	CClientSettingsDlg * SettingsDlg = new CClientSettingsDlg;
	SettingsDlg->SetClient(this);
	SettingsDlg->DoModal();
}

void CClientDlg::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_szNick << m_szAddress << m_nPort << m_bMinimize << m_bShowBlnTips;
	}
	else
	{
		ar >> m_szNick >> m_szAddress >> m_nPort >> m_bMinimize >> m_bShowBlnTips;
	}

	ar.Close();
}

void CClientDlg::OnBnClickedConnect()
{
	CString status;
	CButton * setButton = (CButton *)GetDlgItem(IDC_SETTINGS);
	CButton * conButton = (CButton *)GetDlgItem(IDC_CONNECT);
	CButton * sendButton = (CButton *)GetDlgItem(IDC_SENDFILE);
	conButton->GetWindowTextW(status);

	TCHAR szAppPath[MAX_PATH] = _T("");
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));

	if (status.Compare(_T("Connect")) == 0)
	{
		CFileException fileEx;
		CFile file;
		if(!file.Open(strAppDirectory+_T("\\client.dat"),
			CFile::modeCreate | 
			CFile::modeNoTruncate | 
			CFile::modeRead |
			CFile::shareExclusive, 
			&fileEx))
		{
			TCHAR szError[1024];
			fileEx.GetErrorMessage(szError, 1024);
			CString msg;
			msg.Format(_T("Error reading client.dat file: %s"), szError);
			PrintToChat(msg);
		}
		else
		{
			if (file.GetLength() != 0)
				Serialize(CArchive(&file, CArchive::load));

			file.Close();

			m_sktClient.Create(this, 0, SOCK_STREAM, 0);
			m_sktClient.Connect(m_szAddress, m_nPort);
		}
	}
	else if (status.Compare(_T("Disconnect")) == 0)
	{
		if (IsConnected())
			SendToServer(_T("/quit"));
		
		setButton->EnableWindow(TRUE);
		sendButton->EnableWindow(FALSE);
		conButton->SetWindowTextW(_T("Connect"));

		m_lstClients.ResetContent();

		CFileException fileEx;
		CFile file;
		if(!file.Open(strAppDirectory+_T("\\client.dat"), 
			CFile::modeCreate | 
			CFile::modeWrite |
			CFile::shareExclusive,
			&fileEx))
		{
			TCHAR szError[1024];
			fileEx.GetErrorMessage(szError, 1024);
			CString msg;
			msg.Format(_T("Error writing client.dat file: %s"), szError);
			PrintToChat(msg);
		}
		else
		{
			Serialize(CArchive(&file, CArchive::store));

			file.Close();
		}
			
		m_sktClient.Close();

		SetConnected(FALSE);

		PrintToChat(_T("Disconnected from server..."));
	}
}

BOOL CClientDlg::AddClientToList(CString client)
{
	if (m_lstClients.AddString(client) != LB_ERR)
		return TRUE;
	else
		return FALSE;
}

BOOL CClientDlg::IsConnected(CString szNick)
{
	CString text;
	for (int i = 0; i < m_lstClients.GetCount(); i++)
	{
		m_lstClients.GetText(i, text);
		if ( text.Compare(szNick) == 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CClientDlg::RemoveClient(CString client)
{
	CString text;
	for (int i = 0; i < m_lstClients.GetCount(); i++)
	{
		m_lstClients.GetText(i, text);
		if ( text.Compare(client) == 0 )
		{
			m_lstClients.DeleteString(i);
			return TRUE;
		}
	}
	return FALSE;
}

void CClientDlg::AddPChat(CPrivateChatDlg * pPChat)
{
	m_pcArray.Add(pPChat);
}

BOOL CClientDlg::RemovePChat(CString szNick)
{
	for (int i = 0; i < m_pcArray.GetCount(); i++)
	{
		if (m_pcArray[i]->GetNick().Compare(szNick) == 0)
		{
			m_pcArray.RemoveAt(i);
			return TRUE;
		}
	}
	return FALSE;
}

void CClientDlg::HandlePChatMsg(CString szNick, CString szMsg)
{
	CPrivateChatDlg * pPChat;
	WINDOWINFO wi;
	ZeroMemory(&wi, sizeof(wi));

	pPChat = GetPChat(szNick);
	if (pPChat == NULL)
	{
		pPChat = new CPrivateChatDlg;
		pPChat->Create(this, szNick);
		pPChat->ShowWindow(SW_SHOW);
		AddPChat(pPChat);
		pPChat->PrintToPChat(szNick+_T(": ")+szMsg);
		pPChat->FlashWindowEx(FLASHW_ALL, 2, 300);

	}
	else
	{
		if (pPChat->IsEncrypted() && !pPChat->GetPKey().IsEmpty())
		{
			m_message.setMessage(szMsg);
			m_message.setKey(pPChat->GetPKey());
			m_message.decryptMessage();
			pPChat->ShowWindow(SW_SHOW);
			pPChat->PrintToPChat(szNick+_T(": ")+m_message.getUnMessage());
			pPChat->GetWindowInfo(&wi);
			if (!(wi.dwWindowStatus && STATE_SYSTEM_FOCUSED))
				pPChat->FlashWindowEx(FLASHW_ALL, 2, 300);


		}
		else
		{
			pPChat->ShowWindow(SW_SHOW);
			pPChat->PrintToPChat(szNick+_T(": ")+szMsg);
			pPChat->GetWindowInfo(&wi);
			if (!(wi.dwWindowStatus && STATE_SYSTEM_FOCUSED))
				pPChat->FlashWindowEx(FLASHW_ALL, 2, 300);
		}
	}
}

CPrivateChatDlg * CClientDlg::GetPChat(CString szNick)
{
	for (int i = 0; i < m_pcArray.GetCount(); i++)
	{
		if (m_pcArray[i]->GetNick().Compare(szNick) == 0)
			return m_pcArray[i];
	}
	return NULL;
}

void CClientDlg::OnCancel()
{
	CFile file;
	TCHAR szAppPath[MAX_PATH] = _T("");
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	file.Open(strAppDirectory+_T("\\client.dat"), CFile::modeCreate | CFile::modeWrite);

	Serialize(CArchive(&file, CArchive::store));

	file.Close();

	CDialog::OnCancel();
}

CString CClientDlg::FileOpen() 
{
   CString szFilters = _T("All Files (*.*)|*.*||");

   CFileDialog fileDlg(TRUE, NULL, NULL,
	   OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST , szFilters, this);

   fileDlg.GetOFN().lpstrTitle = _T("Browse For File To Send");
   
   if( fileDlg.DoModal() == IDOK )
   {
	  CString pathName = fileDlg.GetPathName();
	  return pathName;
   }
   else return NULL;
}

void CClientDlg::OnBnClickedSendFile()
{
	CString nickTo, fPath, fName;
	int i = m_lstClients.GetCurSel();
	if (i != LB_ERR)
	{
		m_lstClients.GetText(i, nickTo);
	}
	else
	{
		PrintToChat(_T("Failed to send! Make sure you selected a client!"));
		return;
	}

	fPath = FileOpen();
	if (!fPath.IsEmpty())
	{
		int n = 0;
		CString line = fPath.Tokenize(_T("\\"), n);
		while (line != _T(""))
		{
			fName = line;
			line =  fPath.Tokenize(_T("\\"), n);
		}
	}

	if (!fName.IsEmpty())
	{
		SOCKADDR_IN sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = m_sktClient.GetSockName((SOCKADDR*)&sockAddr, &nSockAddrLen);
		if (bResult)
		{
			CFileThread * pThread = new CFileThread;
			pThread->CreateThread(CREATE_SUSPENDED, 0);
			pThread->IsSending = TRUE;
			pThread->szFilePath = fPath;
			pThread->ResumeThread();

			CString szLocalIP;
			szLocalIP.Format(_T("%d.%d.%d.%d"), SS_BYTE1(&sockAddr),
				SS_BYTE2(&sockAddr),
				SS_BYTE3(&sockAddr),
				SS_BYTE4(&sockAddr));

			SendToServer(_T("/send#")+nickTo+_T("#")+GetNick()+_T("#")+szLocalIP+_T("#")+fName);
		}
		else
		{
			DWORD dwErr = m_sktClient.GetLastError();
			TCHAR szErr[256];
			switch(dwErr)
			{
				case WSANOTINITIALISED:
					wsprintf(szErr,_T("A successful AfxSocketInit must occur before using this API."));
					break;
				case WSAENETDOWN:
					wsprintf(szErr,_T("The Windows Sockets implementation detected that the network subsystem failed."));
					break;
				case WSAEFAULT:
					wsprintf(szErr,_T("The lpSockAddrLen argument is not large enough."));
					break;
				case WSAEINPROGRESS:
					wsprintf(szErr,_T("A blocking Windows Sockets operation is in progress."));
					break;
				case WSAENOTSOCK:
					wsprintf(szErr,_T("The descriptor is not a socket."));
					break;
				case WSAEINVAL:
					wsprintf(szErr,_T("The socket has not been bound to an address with Bind."));
					break;
				default:
					wsprintf(szErr,_T("Function failed with error: %d"), dwErr);
					break;
            }
			AfxMessageBox(szErr, MB_ICONERROR);
		}
	}
}

void CClientDlg::OnCopyClicked()
{
	if ( OpenClipboard() )
	{
		if( !EmptyClipboard() )
		{
			AfxMessageBox( _T("Cannot empty the Clipboard") );
			return;
		}
		CString szText;
		m_lstChat.GetText(m_lstChat.GetCurSel(), szText);
		if (!szText.IsEmpty())
		{
			int n = 0;
			szText.Tokenize(_T(" "), n);
			szText.Tokenize(_T(" "), n);
			szText = szText.Mid(n);

			HGLOBAL hData;
			hData = GlobalAlloc(GMEM_MOVEABLE, szText.GetLength()*2+1);

			LPTSTR pchData;
			pchData = (LPTSTR)GlobalLock(hData);

			memcpy(pchData, szText.GetBuffer(), szText.GetLength()*2+1);

			GlobalUnlock(hData);
			if ( ::SetClipboardData( CF_UNICODETEXT, hData ) == NULL )
				{
					AfxMessageBox( _T("Unable to set Clipboard data") );
					CloseClipboard();
					return;
				}
			CloseClipboard();
		}
	}
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if((nID & 0xFFF0) == SC_MINIMIZE)
	{
		if (GetMinimize())
		{
			if (Shell_NotifyIcon(NIM_ADD, &m_nidIconData))
			{
				this->ShowWindow(SW_HIDE);
				m_bInTray = TRUE;
			}
		}
		else
			CDialog::OnSysCommand(nID, lParam);
	}
	else
		CDialog::OnSysCommand(nID, lParam);
}

LRESULT CClientDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID; 
    UINT uMsg; 
 
    uID = (UINT) wParam; 
    uMsg = (UINT) lParam; 
 
	if (uID != 1)
		return 0;
	
	CPoint pt;	

    switch (uMsg ) 
	{
	case WM_RBUTTONUP:
		GetCursorPos(&pt);
		OnTrayRButtonUp(pt);
		break;
	case WM_LBUTTONDBLCLK:
		OnTrayLButtonDblClk();
		break;
	}
	return 0;
}

void CClientDlg::OnTrayLButtonDblClk()
{
	if (Shell_NotifyIcon(NIM_DELETE, &m_nidIconData))
	{
		this->ShowWindow(SW_SHOW);
		m_bInTray = FALSE;
	}
}

void CClientDlg::OnTrayRButtonUp(CPoint pt)
{
	CMenu contextMenu, *tracker;

	contextMenu.LoadMenuW(IDR_TRAYMENU);
	tracker = contextMenu.GetSubMenu(0);

	tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
		pt.x , pt.y , AfxGetMainWnd());
}

void CClientDlg::OnTrayRestore()
{
	OnTrayLButtonDblClk();
}

void CClientDlg::OnTrayClose()
{
	if (Shell_NotifyIcon(NIM_DELETE, &m_nidIconData))
		OnCancel();
}

void CClientDlg::ShowBalloonTip(CString info)
{
	if (m_bInTray && m_bShowBlnTips)
	{
		//m_nidIconData.uFlags |= NIF_INFO;
		wcscpy_s(m_nidIconData.szInfo, info);
		wcscpy_s(m_nidIconData.szInfoTitle, _T("Gavote naujà þinutæ!"));
		Shell_NotifyIcon(NIM_MODIFY, &m_nidIconData);
		wcscpy_s(m_nidIconData.szInfo, _T(""));
		wcscpy_s(m_nidIconData.szInfoTitle, _T(""));
	}
}
