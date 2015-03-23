#include "StdAfx.h"
#include "insnChatClient.h"
#include "TCPSocket.h"



/**********************************
*	TCPSocket implementation	  *
**********************************/

TCPSocket::TCPSocket()
{
}

TCPSocket::~TCPSocket()
{
}

BOOL TCPSocket::Create(CClientDlg * pClient, UINT nSocketPort, int nSocketType, LPCTSTR lpszSocketAddress)
{
	m_pClient = pClient;	

	if (CAsyncSocket::Create(nSocketPort, nSocketType, FD_READ | FD_WRITE | FD_CONNECT , lpszSocketAddress))
	{

		m_pClient->PrintToChat(_T("Socket created..."));
		return TRUE;
	}
	else
	{
		m_pClient->PrintToChat(_T("Failed to create socket!"));
		return FALSE;
	}
}

void TCPSocket::OnAccept(int nErrorCode)
{
	CAsyncSocket::OnAccept(nErrorCode);
}

void TCPSocket::OnConnect(int nErrorCode)
{
	if (0 != nErrorCode)
   {
      switch( nErrorCode )
      {
         case WSAEADDRINUSE: 
            m_pClient->PrintToChat(_T("The specified address is already in use."));
            break;
         case WSAEADDRNOTAVAIL: 
            m_pClient->PrintToChat(_T("The specified address is not available from the local machine."));
            break;
         case WSAEAFNOSUPPORT: 
            m_pClient->PrintToChat(_T("Addresses in the specified family cannot be used with this socket."));
            break;
         case WSAECONNREFUSED: 
            m_pClient->PrintToChat(_T("The attempt to connect was forcefully rejected."));
            break;
         case WSAEDESTADDRREQ: 
            m_pClient->PrintToChat(_T("A destination address is required."));
            break;
         case WSAEFAULT: 
            m_pClient->PrintToChat(_T("The lpSockAddrLen argument is incorrect."));
            break;
         case WSAEINVAL: 
            m_pClient->PrintToChat(_T("The socket is already bound to an address."));
            break;
         case WSAEISCONN: 
            m_pClient->PrintToChat(_T("The socket is already connected."));
            break;
         case WSAEMFILE: 
            m_pClient->PrintToChat(_T("No more file descriptors are available."));
            break;
         case WSAENETUNREACH: 
            m_pClient->PrintToChat(_T("The network cannot be reached from this host at this time."));
            break;
         case WSAENOBUFS: 
            m_pClient->PrintToChat(_T("No buffer space is available. The socket cannot be connected."));
            break;
         case WSAENOTCONN: 
            m_pClient->PrintToChat(_T("The socket is not connected."));
            break;
         case WSAENOTSOCK: 
            m_pClient->PrintToChat(_T("The descriptor is a file, not a socket."));
            break;
         case WSAETIMEDOUT: 
            m_pClient->PrintToChat(_T("The attempt to connect timed out without establishing a connection."));
            break;
         default:
            TCHAR szError[256];
            wsprintf(szError, _T("OnConnect error: %d"), nErrorCode);
			m_pClient->PrintToChat(szError);
            //AfxMessageBox(szError);
            break;
      }
	  m_pClient->CloseSocket();
	  m_pClient->PrintToChat(_T("Socket closed."));
	}
	else
	{
		CString address;

		SOCKADDR_IN sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = GetPeerName((SOCKADDR*)&sockAddr, &nSockAddrLen);
		if (bResult)
		{
			address.Format(_T("%d.%d.%d.%d"), SS_BYTE1(&sockAddr),
					SS_BYTE2(&sockAddr),
					SS_BYTE3(&sockAddr),
					SS_BYTE4(&sockAddr));
			m_pClient->PrintToChat(_T("Connected to: ")+address);
		}
		CButton * setButton = (CButton *)m_pClient->GetDlgItem(IDC_SETTINGS);
		CButton * conButton = (CButton *)m_pClient->GetDlgItem(IDC_CONNECT);
		setButton->EnableWindow(FALSE);
		conButton->SetWindowTextW(_T("Disconnect"));
	}

	CAsyncSocket::OnConnect(nErrorCode);
}

void TCPSocket::OnReceive(int nErrorCode)
{
	TCHAR info[4096];
	int nRead;
	nRead = Receive(info, 4096);

	switch(nRead)
	{
	case SOCKET_ERROR:
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			AfxMessageBox(_T("Error occured on receiving!"));
			Close();
		}
	default:
		info[nRead/2] = 0;
		CString szTemp(info);

		if (szTemp[0] == _T('/'))
		{
			HandleCommand(szTemp);
		}
		else
		{
			m_pClient->HandleReturns(info);
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

void TCPSocket::HandleCommand(CString command)
{
	CString comm;
	int i = 0;

	comm = command.Tokenize(_T("#"), i);

	if (comm == _T("/anick"))
	{
		comm = command.Tokenize(_T("#"), i);
		m_pClient->AddClientToList(comm);
		m_pClient->PrintToChat(_T("Client: ")+comm+_T(" connected!"));
	}

	if (comm == _T("/dnick"))
	{
		comm = command.Tokenize(_T("#"), i);
		m_pClient->RemoveClient(comm);
		m_pClient->PrintToChat(_T("Client: ")+comm+_T(" disconnected!"));
	}
	if (comm == _T("/rnick"))
	{
		m_pClient->ResetClientList();
	}
	if (comm == _T("/cnick"))
	{
		comm = command.Tokenize(_T("#"), i);
		CString oldNick(comm);
		comm = command.Tokenize(_T("#"), i);
		m_pClient->RemoveClient(oldNick);
		m_pClient->AddClientToList(comm);
		m_pClient->PrintToChat(oldNick+_T(" changed nick to: ")+comm);
	}
	if (comm == _T("/send"))
	{
		comm = command.Tokenize(_T("#"), i);
		CString nick = comm;
		comm = command.Tokenize(_T("#"), i);
		CString ip = comm;
		comm = command.Tokenize(_T("#"), i);
		CString file = comm;

		if (m_pClient->IsInTray())
			m_pClient->RestoreFromTray();

		CFileThread * pThread = new CFileThread;
		pThread->CreateThread(CREATE_SUSPENDED, 0);
		pThread->IsSending = FALSE;
		pThread->szIP = ip;			
		pThread->szFileName = file;
		pThread->szNick = nick;
		pThread->ResumeThread();
	}
	if (comm == _T("/banned"))
	{
		m_pClient->PrintToChat(_T("You are banned from the server!"));
		m_pClient->CloseSocket();
	}
	if (comm == _T("/connected"))
	{
		m_pClient->SendToServer(_T("/initnick#")+m_pClient->GetNick());
		m_pClient->SetConnected(TRUE);
	}
	if (comm == _T("/private"))
	{
		CString nick, msg;
		nick = command.Tokenize(_T("#"), i);
		msg = command.Tokenize(_T("#"), i);

		m_pClient->HandlePChatMsg(nick, msg);
	}
}