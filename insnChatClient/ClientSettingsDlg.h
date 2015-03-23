#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CClientDlg;

// CClientSettingsDlg dialog

class CClientSettingsDlg : public CDialog
{
	DECLARE_SERIAL(CClientSettingsDlg)

public:
	CClientSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientSettingsDlg();

	void SetClient(CClientDlg * pClient);

// Dialog Data
	enum { IDD = IDD_CLIENTSETTINGS };

	afx_msg void OnBnClickedMinimize();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void Serialize(CArchive &ar);
	virtual void OnOK();

	void SetNick() { m_txtClientNickName.GetWindowTextW(m_szNick); };
	void SetAddress();
	void SetPort();
	void SetMinimizeToTray();
	void SetShowBalloonTips();
	CString GetNick() { return m_szNick; };
	CString GetAddress() { return m_szAddress; };
	UINT GetPort() { return m_nPort; };
	BOOL GetMinimizeToTray() { return m_bMinimizeToTray; };
	BOOL GetShowBalloonTips() { return m_bShowBlnTips; };

	DECLARE_MESSAGE_MAP()

private:
	//members
	CString m_szNick, m_szAddress;
	UINT m_nPort;
	BOOL m_bMinimizeToTray, m_bShowBlnTips;
	CClientDlg * m_pClient;

	//controls
	CIPAddressCtrl m_ipAddressToServer;
	CEdit m_txtPortToServer;
	CEdit m_txtClientNickName;	
	CButton m_cbMinimize;
	CButton m_cbBalloonTips;
};
