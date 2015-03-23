// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "insnChatServer.h"
#include "ServerDlg.h"


// CServerDlg dialog

IMPLEMENT_SERIAL(CServerDlg, CDialog, VERSIONABLE_SCHEMA | 2);

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	InitNotifyIconData();

	m_bChatEnabled = FALSE;

	srand(unsigned(time(NULL)));

	cmdList = NULL;
	cursor = cmdList;

	bFirstToggle = FALSE;
}

CServerDlg::~CServerDlg()
{
	DealocateList(); //cmdList deallocated
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMMANDLINE, m_txtCommandLine);
	DDX_Control(pDX, IDC_SERVERCONSOLE, m_lstConsole);
	DDX_Control(pDX, IDC_IPADDRESS, m_ipAddress);
	DDX_Control(pDX, IDC_PORT, m_txtPort);
	DDX_Control(pDX, IDC_CLIENTSLIST, m_lstClients);
}


BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTSERVER, &CServerDlg::OnBnStartServerClicked)
	ON_BN_CLICKED(IDC_STOPSERVER, &CServerDlg::OnBnStopServerClicked)
	ON_COMMAND(ID__KICKCLIENT, &CServerDlg::OnKickClicked)
	ON_COMMAND(ID_KICK_CLIENTINFO, &CServerDlg::OnClientInfoClicked)
	ON_COMMAND(ID__BANCLIENT, &CServerDlg::OnBanClicked)
	ON_COMMAND(ID__COPY, &CServerDlg::OnCopyClicked)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE, &CServerDlg::OnTrayNotify)
END_MESSAGE_MAP()

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//set to defaults

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_nidIconData.hWnd = this->m_hWnd;
	m_nidIconData.hIcon = m_hIcon;
	wcscpy_s(m_nidIconData.szTip, _T("insnChat Server"));

	TCHAR szAppPath[MAX_PATH] = _T("");
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));

	CFile file;
	file.Open(strAppDirectory+_T("\\server.dat"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeRead);

	if (file.GetLength() != 0)
		Serialize(CArchive(&file, CArchive::load), CServerDlg::initData);

	file.Close();

	m_ipAddress.SetAddress(m_b1,m_b2,m_b3,m_b4);
	m_txtPort.SetWindowTextW(m_szPort);

	file.Open(strAppDirectory+_T("\\banned.dat"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeRead);

	if (file.GetLength() != 0)
		Serialize(CArchive(&file, CArchive::load), CServerDlg::banlist);

	file.Close();

	return TRUE;
}

BOOL CServerDlg::InitNotifyIconData()
{
	m_nidIconData.cbSize			= sizeof(NOTIFYICONDATA);	
	m_nidIconData.hWnd				= 0;
	m_nidIconData.uID				= 2;
	m_nidIconData.uCallbackMessage	= WM_TRAY_ICON_NOTIFY_MESSAGE;	
	m_nidIconData.hIcon				= 0;
	m_nidIconData.szTip[0]			= 0;
	m_nidIconData.uFlags			= NIF_MESSAGE | NIF_ICON | NIF_TIP;

	return TRUE;
}

void CServerDlg::OnPaint()
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

void CServerDlg::Serialize(CArchive &ar, int command)
{
	//command : 1 == banlist, 2 == init/close
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		switch (command)
		{
		case 1:
			ar << m_bcArray.GetCount();
			for (int i = 0; i < m_bcArray.GetCount(); i++)
			{
				ar << m_bcArray[i].id << m_bcArray[i].nick << m_bcArray[i].peer;
			}
			break;
		case 2:
			ar << m_b1 << m_b2 << m_b3 << m_b4 << m_szPort;
			break;
		}
	}
	else
	{
		switch (command)
		{
		case 1:
			int count;
			ar >> count;
			for (int i = 0; i < count; i++)
			{
				ar >> bannedCln .id >> bannedCln.nick >> bannedCln.peer;
				AddBannedClient(bannedCln);
			}
			break;
		case 2:
			ar >> m_b1 >> m_b2 >> m_b3 >> m_b4 >> m_szPort;
			break;
		}
			
	}

}

HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// CServerDlg message handlers

void CServerDlg::OnBnStartServerClicked()
{
	CString szIP, szPort;
	UINT nPort;

	BYTE f1, f2, f3, f4;

	HandleReturns(_T("Starting insnChat server..."));

	m_ipAddress.GetAddress(f1,f2,f3,f4);

	szIP.Format(_T("%i.%i.%i.%i"),f1,f2,f3,f4);

	m_txtPort.GetWindowTextW(szPort);
	nPort = _ttoi(szPort);

	if (m_sktServer.Create(this, nPort, SOCK_STREAM, szIP))
	{

		m_sktServer.GetSockName(szIP, nPort);
		szPort.Format(_T("%i"),nPort);
		HandleReturns(_T("Server socket created : ") + szIP +_T(":")+ szPort);
		m_sktServer.Listen();
		HandleReturns(_T("Started to Listen().."));
	}
	else
	{
		AfxMessageBox(_T("Unable to create socket!"), MB_ICONERROR);
	}

	m_ipAddress.EnableWindow(FALSE);
	m_txtPort.SetReadOnly();
	CButton * pButStop = (CButton*)GetDlgItem(IDC_STOPSERVER);
	pButStop->EnableWindow(TRUE);

	CButton * pButStart = (CButton*)GetDlgItem(IDC_STARTSERVER);
	pButStart->EnableWindow(FALSE);
}

void CServerDlg::OnBnStopServerClicked()
{
	SendToClients(_T("insnChat Server disconnected..."));

	for (int i = 0; i < m_ctArray.GetCount(); i++)
	{
		m_ctArray[i]->PostThreadMessageW(WM_QUIT, 0, 0);
	}

	m_sktServer.Close();
	HandleReturns(_T("Server socket closed!"));

	m_ctArray.RemoveAll();

	m_lstClients.ResetContent();

	m_ipAddress.EnableWindow(TRUE);
	m_txtPort.SetReadOnly(FALSE);
	CButton * pButStop = (CButton*)GetDlgItem(IDC_STOPSERVER);
	pButStop->EnableWindow(FALSE);

	CButton * pButStart = (CButton*)GetDlgItem(IDC_STARTSERVER);
	pButStart->EnableWindow(TRUE);
}

void CServerDlg::PrintToConsole(CString output)
{
	int n = m_lstConsole.GetCount();
	m_lstConsole.InsertString(n, output);
}

void CServerDlg::HandleReturns(CString output)
{
	CString line;
	int i = 0;
	line = output.Tokenize(_T("\r\n"),i);
	while (line != _T(""))
	{
		PrintToConsole(line);
		line = output.Tokenize(_T("\r\n"),i);
	}
}

void CServerDlg::AddBannedClient(BannedClient bc)
{
	m_bcArray.Add(bc);
}

BOOL CServerDlg::AddThread(CConnectThread * pThread)
{
	m_ctArray.Add(pThread);
	return TRUE;
}

BOOL CServerDlg::AddClientToList(CString szClient)
{
	if (m_lstClients.AddString(szClient) != LB_ERR)
		return TRUE;
	else
		return FALSE;
}

BOOL CServerDlg::DisconnectClient(CString szClient, SOCKET hSocket)
{
	for (int n = 0; n < m_ctArray.GetCount(); n++)
	{
		if (m_ctArray[n]->m_serverSkt.m_hSocket == hSocket)
		{
			m_ctArray[n]->PostThreadMessageW(WM_QUIT, 0, 0);
			m_ctArray.RemoveAt(n);
			break;
		}
	}

	if (m_lstClients.FindString(-1, szClient) != LB_ERR)
	{
		for (int i = 0; i < m_lstClients.GetCount(); i++)
		{
			CString client;
			m_lstClients.GetText(i, client);
			if (client.Compare(szClient) == 0)
			{
				if (m_lstClients.DeleteString(i) != LB_ERR)
				{
					return TRUE;
				}
				else
				{
					HandleReturns(_T("Error removing client from list!"));
					return FALSE;
				}
			}
		}		
	}
	else
	{
		HandleReturns(_T("Error finding client int the list!"));
		return FALSE;
	}
	return FALSE;
}

BOOL CServerDlg::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->lParam == 1835009 || pMsg->lParam == VK_RETURN)
		{
			if (IDC_COMMANDLINE == ::GetDlgCtrlID(pMsg->hwnd))
			{
				CString text;
				m_txtCommandLine.GetWindowTextW(text);
				m_txtCommandLine.SetSel(0,-1);
				m_txtCommandLine.Clear();
				HandleReturns(text);
				HandleCommand(text);
				InsertCmd(text);
			}
		}
		if (pMsg->lParam == VK_DOWN || pMsg->lParam == 22020097)
		{
			if (IDC_COMMANDLINE == ::GetDlgCtrlID(pMsg->hwnd))
			{
				m_txtCommandLine.SetWindowTextW(GetNextNode());
			}
		}
		if (pMsg->lParam == VK_UP || pMsg->lParam == 21495809)
		{
			if (IDC_COMMANDLINE == ::GetDlgCtrlID(pMsg->hwnd))
			{
				m_txtCommandLine.SetWindowTextW(GetPrevNode());
			}
		}
	}
	if (pMsg->lParam == 3014657)//v
	{
		if (IDC_SERVERCONSOLE == ::GetDlgCtrlID(pMsg->hwnd))
		{
			if (::GetAsyncKeyState(1900545) || ::GetAsyncKeyState(VK_CONTROL))
			{
				OnCopyClicked();
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CServerDlg::SendToClients(CString message)
{
	LPTSTR buf;

	buf = message.GetBuffer();
	message.ReleaseBuffer();
	for (int i = 0; i < m_ctArray.GetCount(); i++)
	{
		m_ctArray[i]->m_serverSkt.Send(buf, 2*message.GetLength());
	}
}

CConnectThread * CServerDlg::GetThread(CString nick)
{
	for (int i = 0; i < m_ctArray.GetCount(); i++)
	{
		if (nick == m_ctArray[i]->m_serverSkt.GetNick())
		{
			return m_ctArray[i];
		}
	}
	return NULL;
}

CConnectThread * CServerDlg::GetThread(SOCKET hSocket)
{
	for (int i = 0; i < m_ctArray.GetCount(); i++)
	{
		if (hSocket == m_ctArray[i]->m_serverSkt.m_hSocket)
		{
			return m_ctArray[i];
		}
	}
	return NULL;
}

BOOL CServerDlg::IsIdenticalNick(CString nick, SOCKET hSocket)
{
	CString text;
	for (int i= 0; i < m_lstClients.GetCount(); i++)
	{
		m_lstClients.GetText(i, text);
		if (text.Compare(nick) == 0)
		{
			CString msg(_T("There is already a client connected with your nick! Change your nick and reconnect."));
			LPTSTR buf = msg.GetBuffer();
			msg.ReleaseBuffer();
			CConnectThread * pThread = GetThread(hSocket);
			if (pThread != NULL)
			{
				pThread->m_serverSkt.Send(buf, 2*msg.GetLength());
				DisconnectClient(_T(""), pThread->m_serverSkt.m_hSocket);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CServerDlg::IsBanned(CString nick, CString peer)
{
	for (int i = 0; i < m_bcArray.GetCount(); i++)
	{
		if ((nick == m_bcArray[i].nick) || (peer == m_bcArray[i].peer))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CServerDlg::RemoveNickFromList(CString nick)
{
	if (m_lstClients.FindString(-1, nick) != LB_ERR)
	{
		for (int i = 0; i < m_lstClients.GetCount(); i++)
		{
			CString text;
			m_lstClients.GetText(i, text);
			if (text.Compare(nick) == 0)
			{
				if (m_lstClients.DeleteString(i) != LB_ERR)
				{
					return TRUE;
				}
				else
				{
					HandleReturns(_T("Error removing client from list!"));
					return FALSE;
				}
			}
		}		
	}
	else
	{
		HandleReturns(_T("Error finding client int the list!"));
		return FALSE;
	}
	return FALSE;
}

void CServerDlg::SendClientList(SOCKET hSocket)
{
	CConnectThread * pThread = GetThread(hSocket);
	if (pThread != NULL)
	{
		CString msg;
		LPTSTR buf;
		for ( int i = 0; i < GetClientsCount(); i++)
		{
			if (GetClientText(i) != pThread->m_serverSkt.GetNick())
			{
				msg = _T("/anick#")+GetClientText(i);
				buf = msg.GetBuffer();
				msg.ReleaseBuffer();
				pThread->m_serverSkt.Send(buf, 2*msg.GetLength());
			}
		}
	}
}

void CServerDlg::HandleCommand(CString command)
{
	int i = 0;
	CConnectThread * pThread;
	CString comm = command.Tokenize(_T(" "), i);
	if (comm == _T("/kick"))
	{
		comm = command.Tokenize(_T(" "), i);
		pThread = GetThread(comm);
		if (pThread != NULL)
		{
			CString msg(_T("You've been kicked from server!"));
			LPTSTR buf = msg.GetBuffer();
			msg.ReleaseBuffer();
			pThread->m_serverSkt.Send(buf, 2*msg.GetLength());
			DisconnectClient(comm, pThread->m_serverSkt.m_hSocket);
			SendToClients(_T("/dnick#")+comm);
		}
		else
			PrintToConsole(_T("Failed to get thread by specified nick!"));
		return;
	}
	if (comm == _T("/ban"))
	{
		comm = command.Tokenize(_T(" "), i);
		pThread = GetThread(comm);
		if (pThread != NULL)
		{
			if ((bannedCln.id = GetBanId()) == -1)
			{
				PrintToConsole(_T("Error occured: Ban list if full!"));
				return;
			}
			CString peer;
			SOCKADDR_IN sockAddr;
			memset(&sockAddr, 0, sizeof(sockAddr));
			int nSockAddrLen = sizeof(sockAddr);
			if (SOCKET_ERROR != ::getpeername(pThread->m_serverSkt.m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen))
			{
				peer.Format(_T("%d.%d.%d.%d"), SS_BYTE1(&sockAddr),
						SS_BYTE2(&sockAddr),
						SS_BYTE3(&sockAddr),
						SS_BYTE4(&sockAddr));//GETRANDOM(0,256));
			}
			
			bannedCln.nick = pThread->m_serverSkt.GetNick();
			bannedCln.peer = peer;
			AddBannedClient(bannedCln);

			CString msg(_T("You've been added to the banned list!"));
			LPTSTR buf = msg.GetBuffer();
			msg.ReleaseBuffer();
			pThread->m_serverSkt.Send(buf, 2*msg.GetLength());

			DisconnectClient(comm, pThread->m_serverSkt.m_hSocket);
			SendToClients(_T("/dnick#")+comm);
		}
		else
			PrintToConsole(_T("Failed to get thread by specified nick!"));
		return;
	}
	if (comm == _T("/unban"))
	{
		CString id;
		comm = command.Tokenize(_T(" "), i);
		for (int i = 0; i < m_bcArray.GetCount(); i++)
		{
			id.Format(_T("%i"), m_bcArray[i].id);
			if ((comm == id) || (comm == m_bcArray[i].nick) || (comm == m_bcArray[i].peer))
			{
				m_bcArray.RemoveAt(i);
				PrintToConsole(_T("Unban successful!"));
				break;
			}
		}
		return;
	}
	if (comm == _T("/banlist"))
	{
		CString id;
		if (0 < m_bcArray.GetCount())
		{
			for (int i = 0; i < m_bcArray.GetCount(); i++)
			{
				id.Format(_T("%i"),m_bcArray[i].id);
				PrintToConsole(_T("List of banned clients:"));
				PrintToConsole(_T("Ban ID: ")+id+
					_T(" -- Nick: ")+m_bcArray[i].nick+
					_T(" -- Peer address: ")+m_bcArray[i].peer);
			}
		}
		else
		{
			PrintToConsole(_T("Ban list is empty!"));
		}
		return;
	}
	if (comm == _T("/chat"))
	{
		if (IsChatEnabled())
		{
			EnableChat(FALSE);
			PrintToConsole(_T("Chat is now turned <<OFF>>"));
		}
		else
		{
			EnableChat(TRUE);
			PrintToConsole(_T("Chat is now turned <<ON>>"));
		}
		return;
	}
	if (comm == _T("/help") || comm == _T("/h"))
	{
		HandleReturns(_T("Available server commands:\n")
			_T("/kick\n")
			_T("/ban\n")
			_T("/unban\n")
			_T("/banlist\n")
			_T("/chat\n")
			_T("/help\n"));
		return;
	}
	if (comm == _T("/say"))
	{
		comm = command.Mid(i);
		SendToClients(_T("Server: ")+comm);
		return;
	}
	if (comm[0] == _T('/'))
	{
		HandleReturns(_T("Unknown command \'")+comm+_T("\'\nUse \'/help\' to see available commands."));
		return;
	}
//TO DO:
//bnick
//banip
}

void CServerDlg::OnKickClicked()
{
	CString nick;
	int i;
	if (m_lstClients.GetCurSel() != LB_ERR)
	{
		i = m_lstClients.GetCurSel();
		m_lstClients.GetText(i, nick);
		PrintToConsole(_T("/kick ")+nick);
		HandleCommand(_T("/kick ")+nick);
	}
	else
		PrintToConsole(_T("Failed to kick client! Make sure you selected a client!"));

}

void CServerDlg::OnClientInfoClicked()
{
	CConnectThread * pThread;
	int i;
	CString nick;

	if (m_lstClients.GetCurSel() != LB_ERR)
	{
		i = m_lstClients.GetCurSel();
		m_lstClients.GetText(i, nick);
		pThread = GetThread(nick);
		if (pThread != NULL)
		{
			CString info, peer;
			UINT port, socket, threadid;
			SOCKADDR_IN sockAddr;
			memset(&sockAddr, 0, sizeof(sockAddr));
			int nSockAddrLen = sizeof(sockAddr);
			if (SOCKET_ERROR != pThread->m_serverSkt.GetPeerName((SOCKADDR*)&sockAddr, &nSockAddrLen))
			{
				port = sockAddr.sin_port;
				peer.Format(_T("%d.%d.%d.%d"), SS_BYTE1(&sockAddr),
						SS_BYTE2(&sockAddr),
						SS_BYTE3(&sockAddr),
						SS_BYTE4(&sockAddr));
			}
			nick = pThread->m_serverSkt.GetNick();
			socket = pThread->m_serverSkt.m_hSocket;
			threadid = pThread->m_nThreadID;
			info.Format(_T("Client info:\nNick: %s -- Peer address: %s -- Port: %i\nThread ID: %i -- Socket: %i"), 
				nick, peer, port, threadid, socket);
			HandleReturns(info);
		}
		else
			PrintToConsole(_T("Failed to get thread by nick!"));
	}
	else
		PrintToConsole(_T("Failed to show info on client! Make sure you selected a client!"));
}

void CServerDlg::OnBanClicked()
{
	int i;
	CString nick;

	if (m_lstClients.GetCurSel() != LB_ERR)
	{
		i = m_lstClients.GetCurSel();
		m_lstClients.GetText(i, nick);
		PrintToConsole(_T("/ban ")+nick);
		HandleCommand(_T("/ban ")+nick);
	}
	else
		PrintToConsole(_T("Failed to ban client! Make sure you selected a client!"));
}

void CServerDlg::OnCancel()
{
	CFile file;
	TCHAR szAppPath[MAX_PATH] = _T("");
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));

	file.Open(strAppDirectory+_T("\\banned.dat"), CFile::modeCreate | CFile::modeWrite);

	Serialize(CArchive(&file, CArchive::store), CServerDlg::banlist);

	file.Close();

	m_ipAddress.GetAddress(m_b1,m_b2,m_b3,m_b4);
	m_txtPort.GetWindowTextW(m_szPort);

	file.Open(strAppDirectory+_T("\\server.dat"), CFile::modeCreate | CFile::modeWrite);

	Serialize(CArchive(&file, CArchive::store), CServerDlg::initData);

	file.Close();

	CDialog::OnCancel();
}

void CServerDlg::SendFile(CString szNickTo, CString szNickFrom, CString szIP, CString szFile)
{
	CConnectThread * pThread = GetThread(szNickTo);
	if (pThread != NULL)
	{
		CString msg;
		msg = _T("/send#"+szNickFrom+_T("#")+szIP+_T("#")+szFile);
		LPTSTR buf;
		buf = msg.GetBuffer();
		msg.ReleaseBuffer();
		pThread->m_serverSkt.Send(buf, 2*msg.GetLength());
	}
}

void CServerDlg::SendPrivateMessage(CString szNickTo, CString szNickFrom, CString szMsg)
{
	CConnectThread * pThread = GetThread(szNickTo);
	if (pThread != NULL)
	{
		CString msg;
		msg = _T("/private#"+szNickFrom+_T("#")+szMsg);
		LPTSTR buf;
		buf = msg.GetBuffer();
		msg.ReleaseBuffer();
		pThread->m_serverSkt.Send(buf, 2*msg.GetLength());
	}
}

void CServerDlg::OnCopyClicked()
{
	if ( OpenClipboard() )
	{
		if( !EmptyClipboard() )
		{
			AfxMessageBox( _T("Cannot empty the Clipboard!") );
			return;
		}
		CString szText;
		m_lstConsole.GetText(m_lstConsole.GetCurSel(), szText);
		if (!szText.IsEmpty())
		{
			HGLOBAL hData;
			hData = GlobalAlloc(GMEM_MOVEABLE, szText.GetLength()*2+1);

			LPTSTR pchData;
			pchData = (LPTSTR)GlobalLock(hData);

			memcpy(pchData, szText.GetBuffer(), szText.GetLength()*2+1);

			GlobalUnlock(hData);
			if ( ::SetClipboardData( CF_UNICODETEXT, hData ) == NULL )
				{
					AfxMessageBox( _T("Unable to set Clipboard data!") );
					CloseClipboard();
					return;
				}
			CloseClipboard();
		}
	}
	else
	{
		AfxMessageBox(_T("Unable to open Clipboard!"));
	}
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_MINIMIZE)
		{
			if (Shell_NotifyIcon(NIM_ADD, &m_nidIconData))
				this->ShowWindow(SW_HIDE);
		}
		else
			CDialog::OnSysCommand(nID, lParam);
}

LRESULT CServerDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID; 
    UINT uMsg; 
 
    uID = (UINT) wParam; 
    uMsg = (UINT) lParam; 
 
	if (uID != 2)
		return 0;
	
	CPoint pt;	

    switch (uMsg ) 
	{
	//case WM_RBUTTONUP:
	//	GetCursorPos(&pt);
	//	//ClientToScreen(&pt);
	//	OnTrayRButtonUp(pt);
	//	break;
	case WM_LBUTTONDBLCLK:
		OnTrayLButtonDblClk();
		break;
	}
	return 0;
}

void CServerDlg::OnTrayLButtonDblClk()
{
	if (Shell_NotifyIcon(NIM_DELETE, &m_nidIconData))
		this->ShowWindow(SW_SHOW);
}

int CServerDlg::GetBanId()
{
	int count = m_bcArray.GetCount();

	if (count >= BAN_MAX)
		return -1;

	int id = GETRANDOM(100,999);

	if (count > 0)
	{
		for (int i = 0; i < count; i++)
		{
			if (id == m_bcArray[i].id)
			{
				return GetBanId();
			}
		}
	}
	else
		return id;

	return id;
}

void CServerDlg::InsertCmd(CString szCmd)
{
	entry = new Cmd;
	entry->next = NULL;
	entry->prev = NULL;

	if (cmdList == NULL)
	{
		entry->szCmd = szCmd;
		cmdList = entry;
	}
	else
	{
		entry->szCmd = szCmd;
		cmdList->prev = entry;
		entry->next = cmdList;
		cmdList = entry;
		cmdList->prev = NULL;
	}
	if (GetListCount() >= 10)
	{
		//delete last node
		Cmd * n = entry; // <-- GetListCount() directs 'entry' to last node
		entry->prev->next = NULL;
		delete n;
	}
	cursor = cmdList;
	bFirstToggle = TRUE;
}

CString CServerDlg::GetNextNode()
{
	if (cmdList != NULL)
	{
		if (bFirstToggle)
		{
			bFirstToggle = FALSE;
			return cursor->szCmd;
		}
		if (cursor->next == NULL)
			return cursor->szCmd;
		else if (cursor->next != NULL)
		{
			cursor = cursor->next;
			return cursor->szCmd;
		}
	}
	return NULL;	
}

CString CServerDlg::GetPrevNode()
{
	if (cmdList != NULL)
	{
		if (bFirstToggle)
		{
			bFirstToggle = FALSE;
			return cursor->szCmd;
		}
		if (cursor->prev == NULL)
			return cursor->szCmd;
		else if (cursor->prev != NULL)
		{
			cursor = cursor->prev;
			return cursor->szCmd;

		}
	}
	return NULL;
}

//CAUTION! -- pointer 'entry' is directed to last node after calling GetListCount()
int CServerDlg::GetListCount()
{
	int count = 0;
	entry = cmdList;

	while (entry->next != NULL)
	{
		count++;
		entry = entry->next;
	}
	return count;
}

void CServerDlg::DealocateList()
{
	if (cmdList != NULL)
	{
		while(cmdList->next != NULL)
		{
			entry = cmdList;
			cmdList = cmdList->next;
			delete entry;
		}
		delete cmdList;
	}
}