#pragma once

#include "resource.h"
#include "TCPSocket.h"

class CServerDlg;

/**********************************
*		CConnectThread			  *
**********************************/
class CConnectThread : public CWinThread
{
public:
	CConnectThread();
	virtual ~CConnectThread();

public:
	SOCKET m_hSocket;
	TCPSocket m_serverSkt;

	void SetServer(CServerDlg * pServer);

protected:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

};