#pragma once

#include "FileThread.h"

class CClientDlg;

/**********************************
*			TCPSocket			  *
**********************************/
class TCPSocket :
	public CAsyncSocket
{
public:
	TCPSocket();
	virtual ~TCPSocket();

	virtual BOOL Create(CClientDlg * pClient, UINT nSocketPort = 0, int nSocketType = 1, LPCTSTR lpszSocketAddress = 0);
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);

	void HandleCommand(CString);

private:
	CClientDlg * m_pClient;
};
