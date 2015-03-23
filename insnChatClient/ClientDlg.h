// ClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "TCPSocket.h"
#include "ClientSettingsDlg.h"
#include "ClientListBox.h"
#include "FileProgressDlg.h"
#include "PrivateChatDlg.h"

#define SS_BYTE1(ssp) (((PSOCKADDR_IN)ssp)->sin_addr.S_un.S_un_b.s_b1)
#define SS_BYTE2(ssp) (((PSOCKADDR_IN)ssp)->sin_addr.S_un.S_un_b.s_b2)
#define SS_BYTE3(ssp) (((PSOCKADDR_IN)ssp)->sin_addr.S_un.S_un_b.s_b3)
#define SS_BYTE4(ssp) (((PSOCKADDR_IN)ssp)->sin_addr.S_un.S_un_b.s_b4)

#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)

// CClientDlg dialog
class CClientDlg : public CDialog
{
	DECLARE_SERIAL(CClientDlg);

// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CClientDlg();

	// Dialog Data
	enum { IDD = IDD_CHATCLIENT };

	//methods
	void PrintToChat(CString);
	void HandleReturns(CString);
	void SendToServer(CString);
	void SetConnected(BOOL bIsConnected) { m_bIsConnected = bIsConnected; };
	void SetNick(CString newNick) { m_szNick = newNick; };
	void SetServerAddress(CString newAddress) { m_szAddress = newAddress; };
	void SetServerPort(UINT newPort) { m_nPort = newPort; };
	void SetMinimizeToTray(BOOL bMinimize) { m_bMinimize = bMinimize; };
	void SetShowBalloonTips(BOOL bShow) { m_bShowBlnTips = bShow; };
	void ResetClientList() { m_lstClients.ResetContent(); };
	void ShowBalloonTip(CString);
	void RestoreFromTray() { OnTrayLButtonDblClk(); };
	void AddPChat(CPrivateChatDlg*);
	void HandlePChatMsg(CString szNick, CString szMsg);
	void CloseSocket() { m_sktClient.Close(); };
	CString GetNick() { return m_szNick; };	
	CString GetServerAddress() { return m_szAddress; };
	CString FileOpen();
	BOOL InitNotifyIconData();
	BOOL IsConnected() { return m_bIsConnected; };
	BOOL IsConnected(CString szNick);
	BOOL AddClientToList(CString);
	BOOL RemoveClient(CString);
	BOOL GetMinimize() { return m_bMinimize; };
	BOOL IsInTray() { return m_bInTray; };
	BOOL GetShowBlnTips() { return m_bShowBlnTips; };
	BOOL RemovePChat(CString);
	UINT GetServerPort() { return m_nPort; };
	CPrivateChatDlg * GetPChat(CString);
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	virtual void Serialize(CArchive &ar);
	virtual void OnTrayLButtonDblClk();
	virtual void OnTrayRButtonUp(CPoint pt);

// Implementation
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_nidIconData;

	//overloaded methods
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnBnSendClicked();
	afx_msg void OnBnClickedSettings();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedSendFile();
	afx_msg void OnCopyClicked();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTrayRestore();
	afx_msg void OnTrayClose();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

private:
	//members
	CString m_szNick, m_szAddress;
	UINT m_nPort;
	TCPSocket m_sktClient;
	HANDLE hData;
	BOOL m_bIsConnected, m_bMinimize, m_bShowBlnTips, m_bInTray;
	Message m_message;
	CArray<CPrivateChatDlg*, CPrivateChatDlg*> m_pcArray;

	//controls
	CClientListBox m_lstChat;
	CClientListBox m_lstClients;
	CEdit m_txtMessage;
};
