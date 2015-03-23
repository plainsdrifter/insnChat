#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "TCPSocket.h"
#include "ServerListBox.h"
#include "ConnectThread.h"

#define SS_BYTE1(ssp) (((PSOCKADDR_IN)ssp)->sin_addr.S_un.S_un_b.s_b1)
#define SS_BYTE2(ssp) (((PSOCKADDR_IN)ssp)->sin_addr.S_un.S_un_b.s_b2)
#define SS_BYTE3(ssp) (((PSOCKADDR_IN)ssp)->sin_addr.S_un.S_un_b.s_b3)
#define SS_BYTE4(ssp) (((PSOCKADDR_IN)ssp)->sin_addr.S_un.S_un_b.s_b4)

#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 2)

#define GETRANDOM(min,max) ((rand()%(int)(((max)+1)-(min)))+(min))

#define BAN_MAX 100

// CServerDlg dialog
class CServerDlg : public CDialog
{
	DECLARE_SERIAL(CServerDlg);

private:
	struct BannedClient
	{
		int		id;
		CString nick;
		CString peer;
	} bannedCln;

	struct Cmd
	{
		CString szCmd;
		struct Cmd * next;
		struct Cmd * prev;
	} * cmdList;

	Cmd * entry;
	Cmd * cursor;

	BOOL bFirstToggle;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	HICON m_hIcon;
	NOTIFYICONDATA m_nidIconData;

	//overloaded methods
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	virtual void Serialize(CArchive &ar, int command);
	virtual void OnCancel();
	virtual void OnTrayLButtonDblClk();

	DECLARE_MESSAGE_MAP()

public:

	enum SerializeCommand { banlist = 1, initData = 2 };

	CServerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CServerDlg();

	// Dialog Data
	enum { IDD = IDD_SERVER };

	//methods
	void PrintToConsole(CString);
	void HandleReturns(CString);
	void SendToClients(CString message);
	void SendClientList(SOCKET hSocket);
	void HandleCommand(CString);
	void SendFile(CString szNickTo, CString szNickFrom, CString szIP, CString szFile);
	void AddBannedClient(BannedClient bc);
	void EnableChat(BOOL bSwitch) { m_bChatEnabled = bSwitch; };
	void InsertCmd(CString);
	void DealocateList();
	void SendPrivateMessage(CString szNickTo, CString szNickFrom, CString szMsg);
	BOOL AddClientToList(CString szClient);
	BOOL DisconnectClient(CString szClient, SOCKET hSocket);
	BOOL AddThread(CConnectThread * pThread);	
	BOOL IsIdenticalNick(CString nick, SOCKET hSocket);	
	BOOL RemoveNickFromList(CString nick);
	BOOL IsBanned(CString nick, CString peer);
	BOOL IsChatEnabled() { return m_bChatEnabled; };
	BOOL InitNotifyIconData();
	int GetClientsCount() { return m_lstClients.GetCount(); };
	int GetBanId();
	int GetListCount();
	CString GetClientText(int i) { CString temp; m_lstClients.GetText(i, temp); return temp; };
	CString GetNextNode();
	CString GetPrevNode();
	CConnectThread * GetThread(CString nick);
	CConnectThread * GetThread(SOCKET hSocket);

	//event handlers
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnBnStartServerClicked();
	afx_msg void OnBnStopServerClicked();
	afx_msg void OnKickClicked();
	afx_msg void OnClientInfoClicked();
	afx_msg void OnBanClicked();
	afx_msg void OnCopyClicked();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);

private:
	//controls
	CEdit m_txtCommandLine;
	CEdit m_txtPort;
	CIPAddressCtrl m_ipAddress;	
	CServerListBox m_lstConsole;
	CServerListBox m_lstClients;

	//members
	TCPSocket m_sktServer;
	CArray<CConnectThread*, CConnectThread*> m_ctArray;
	CArray<BannedClient, BannedClient> m_bcArray;
	BYTE m_b1, m_b2, m_b3, m_b4;
	CString m_szPort;
	BOOL m_bChatEnabled;
};
