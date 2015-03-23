#include "stdafx.h"
#include "insnChatServer.h"
#include "TCPSocket.h"

// TCPSocket

/**********************************
*	TCPSocket implementation	  *
**********************************/

TCPSocket::TCPSocket()
{
}

TCPSocket::~TCPSocket()
{
}

BOOL TCPSocket::Create(CServerDlg * pServer, UINT nSockPort, int nSockType, LPCTSTR lpszSocketAddress)
{
	m_pServer = pServer;

	return CSocket::Create(nSockPort, nSockType, lpszSocketAddress);
}

void TCPSocket::OnAccept(int nErrorCode)
{
	CSocket socket;
	CString info, peer;
	USHORT port;

	Accept(socket);	

	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	if (SOCKET_ERROR != ::getpeername(socket.m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen))
	{
		port = sockAddr.sin_port;
		peer.Format(_T("%d.%d.%d.%d"), SS_BYTE1(&sockAddr),
				SS_BYTE2(&sockAddr),
				SS_BYTE3(&sockAddr),
				SS_BYTE4(&sockAddr));
	}

	if (m_pServer->IsBanned(_T(""), peer))
	{
		CString info(_T("/banned"));
		LPTSTR buf = info.GetBuffer();
		info.ReleaseBuffer();
		socket.Send(buf, 2*info.GetLength());
		m_pServer->HandleReturns(_T("Banned client from ")+peer+_T(" tried to connect!"));
		socket.Close();
		return;
	}
	else
	{
		CString info(_T("/connected"));
		LPTSTR buf = info.GetBuffer();
		info.ReleaseBuffer();
		socket.Send(buf, 2*info.GetLength());
	}

	CConnectThread * pThread = new CConnectThread; 
	pThread->CreateThread(CREATE_SUSPENDED,0);
	pThread->m_hSocket = socket.Detach();
	pThread->SetServer(m_pServer);
	pThread->ResumeThread();

	m_pServer->AddThread(pThread);
	info.Format(_T("Accepted connection from: %s:%i on: Thread: %i, Socket: %i")
		,peer,port,pThread->m_nThreadID, pThread->m_hSocket);
	m_pServer->HandleReturns(info);

	CSocket::OnAccept(nErrorCode);
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
			AfxMessageBox(_T("Error occured!"));
			Close();
		}
	default:
		info[nRead/2] = 0;
		int n = 0;
		CString szTemp(info);
		
		szTemp.Tokenize(_T(" #"),n);
		
		if (szTemp[n] == _T('/'))
		{
			HandleCommand(szTemp, n);			
		}
		else
		{
			if (m_pServer->IsChatEnabled())
			{
				m_pServer->HandleReturns(szTemp);
			}
			m_pServer->SendToClients(szTemp);
		}
	}

	CSocket::OnReceive(nErrorCode);
}

void TCPSocket::SetServer(CServerDlg * pServer)
{
	m_pServer = pServer;
}

void TCPSocket::PrintToServerConsole(CString message)
{
	m_pServer->HandleReturns(message);
}

void TCPSocket::HandleCommand(CString command, int n)
{
	CString comm = command.Tokenize(_T(" #"), n);

	if (comm.Compare(_T("/quit")) == 0)
	{
		m_pServer->SendToClients(_T("/dnick#")+GetNick());
		m_pServer->DisconnectClient(GetNick(), m_hSocket);
		m_pServer->HandleReturns(_T("Client: ")+GetNick()+_T(" disconnected..."));
	}
	
	if (comm == _T("/initnick"))
	{
		comm = command.Tokenize(_T("#"), n);
		if (!m_pServer->IsIdenticalNick(comm, m_hSocket))
		{
			SetNick(comm);
			m_pServer->AddClientToList(GetNick());
			m_pServer->SendToClients(_T("/anick#")+GetNick());
			m_pServer->SendClientList(m_hSocket);
			m_pServer->PrintToConsole(_T("Client: ")+GetNick()+_T(" connected!"));
		}
	}
	if (comm == _T("/nick"))
	{
		comm = command.Tokenize(_T("#"), n);
		if (!m_pServer->IsIdenticalNick(comm, m_hSocket))
		{
			CString szTemp, nick;
			nick = GetNick();
			m_pServer->RemoveNickFromList(nick);
			szTemp.Format(nick+_T(" changed nick to: ")+comm);
			m_pServer->PrintToConsole(szTemp);
			szTemp.Format(_T("/cnick#%s#%s"),nick,comm);
			m_pServer->SendToClients(szTemp);
			SetNick(comm);			 
			m_pServer->AddClientToList(GetNick());
		}
	}
	if (comm == _T("/send"))
	{
		CString nickto, nickfrom, ip, fname;
		nickto = command.Tokenize(_T("#"), n);
		nickfrom = command.Tokenize(_T("#"), n);
		ip = command.Tokenize(_T("#"), n);
		fname = command.Tokenize(_T("#"), n);
		m_pServer->SendFile(nickto, nickfrom, ip, fname);
	}
	if (comm == _T("/private"))
	{
		CString nickto, nickfrom, msg;
		nickto = command.Tokenize(_T("#"), n);
		msg = command.Tokenize(_T("#"), n);
		n = 0;
		nickfrom = command.Tokenize(_T(":"), n);
		m_pServer->SendPrivateMessage(nickto, nickfrom, msg);
	}
}

