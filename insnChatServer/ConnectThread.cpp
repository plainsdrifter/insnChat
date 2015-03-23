#include "stdafx.h"
#include "ConnectThread.h"

/**********************************
*	CConnectThread implementation *
**********************************/

CConnectThread::CConnectThread()
{
}

CConnectThread::~CConnectThread()
{
}

BOOL CConnectThread::InitInstance()
{
	CWinThread::InitInstance();
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	m_serverSkt.Attach(m_hSocket);

	return TRUE;
}

BOOL CConnectThread::ExitInstance()
{
	CString message;
	message.Format(_T("Closing socket: %i. Terminating thread: %i"), m_serverSkt.m_hSocket, m_nThreadID);
	m_serverSkt.PrintToServerConsole(message);
	m_serverSkt.Close();
	
	return CWinThread::ExitInstance();
}

void CConnectThread::SetServer(CServerDlg * pServer)
{
	m_serverSkt.SetServer(pServer);
}