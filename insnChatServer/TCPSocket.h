#pragma once

class CServerDlg;
class CConnectThread;

/**********************************
*			TCPSocket			  *
**********************************/
class TCPSocket : public CSocket
{
public:
	TCPSocket();
	virtual ~TCPSocket();

	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual BOOL Create(CServerDlg * pServer, UINT nSocketPort = 0, int nSocketType=SOCK_STREAM,
		LPCTSTR lpszSocketAddress = NULL);

	void SetServer(CServerDlg * pServer);
	void PrintToServerConsole(CString message);
	void HandleCommand(CString command, int n);
	void SetNick(CString newNick) { m_szClientNick = newNick; };
	CString GetNick() { return m_szClientNick; };

private:
	CServerDlg * m_pServer;
	CString m_szClientNick;
};
