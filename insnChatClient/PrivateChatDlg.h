#pragma once
#include "afxwin.h"
#include "PChatListBox.h"
#include "Message.h"


// CPrivateChatDlg dialog

class CPrivateChatDlg : public CDialog
{
	DECLARE_DYNAMIC(CPrivateChatDlg)

public:
	CPrivateChatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPrivateChatDlg();

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL Create(CClientDlg * pClient, CString szNick);
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	virtual void PostNcDestroy();

	CString GetNick() { return m_szNick; };
	CString GetPKey();
	void Print(CString);
	void PrintToPChat(CString);
	BOOL IsEncrypted();

// Dialog Data
	enum { IDD = IDD_PRIVATECHAT };

	HICON m_hIcon;
	CImageList m_imageList;

	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedCheckEncrypt();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CButton m_bEncrypt;
	CPChatListBox m_lstPChat;
	CEdit m_txtPMessage;
	CEdit m_txtPrivateKey;
	CStatic m_lblPrivateKey;

	CClientDlg * m_pClient;
	CString m_szNick;
	Message m_message;
};
