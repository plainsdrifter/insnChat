// PrivateChatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "insnChatClient.h"
#include "PrivateChatDlg.h"


// CPrivateChatDlg dialog

IMPLEMENT_DYNAMIC(CPrivateChatDlg, CDialog)

CPrivateChatDlg::CPrivateChatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrivateChatDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPrivateChatDlg::~CPrivateChatDlg()
{
}

void CPrivateChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENCRYPT, m_bEncrypt);
	DDX_Control(pDX, IDC_PRIVATE_CHAT_LIST, m_lstPChat);
	DDX_Control(pDX, IDC_PCHAT_MESSAGE, m_txtPMessage);
	DDX_Control(pDX, IDC_PRIVATE_KEY_STATIC, m_lblPrivateKey);
	DDX_Control(pDX, IDC_PRIVATE_KEY_EDIT, m_txtPrivateKey);
}


BEGIN_MESSAGE_MAP(CPrivateChatDlg, CDialog)
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_CHECK_ENCRYPT, &CPrivateChatDlg::OnBnClickedCheckEncrypt)
END_MESSAGE_MAP()


BOOL CPrivateChatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	//SetIcon(m_hIcon, FALSE);

	this->SetWindowTextW(m_szNick+_T(" -- insnChat"));

	m_imageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 5, 1);

	m_lstPChat.SetImageList(&m_imageList);

	return TRUE;
}

BOOL CPrivateChatDlg::Create(CClientDlg * pClient, CString szNick)
{
	m_pClient = pClient;
	m_szNick = szNick;

	return CDialog::Create(CPrivateChatDlg::IDD, GetDesktopWindow());
}

void CPrivateChatDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

void CPrivateChatDlg::OnCancel()
{
	//m_pClient->RemovePChat(GetNick());
	
	this->ShowWindow(SW_HIDE);
	//CDialog::OnCancel();
}

BOOL CPrivateChatDlg::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->lParam == 1835009 || pMsg->lParam == VK_RETURN)
		{
			if(IDC_PCHAT_MESSAGE == ::GetDlgCtrlID(pMsg->hwnd))
			{
				if (::GetAsyncKeyState(1900545) || ::GetAsyncKeyState(VK_CONTROL))
				{
				}
				else
				{
					CString msg;
					m_txtPMessage.GetWindowTextW(msg);
					m_txtPMessage.SetSel(0, -1);
					m_txtPMessage.Clear();
					if (m_pClient->IsConnected() && !msg.IsEmpty())
					{
						if (m_pClient->IsConnected(GetNick()))
						{
							if (IsEncrypted())
							{
								if (!GetPKey().IsEmpty())
								{
									m_message.setMessage(msg);
									m_message.setKey(GetPKey());
									m_message.encryptMessage();
									m_pClient->SendToServer(_T("/private#")+GetNick()+_T("#")+m_message.getEnMessage());
									PrintToPChat(m_pClient->GetNick()+_T(": ")+msg);
								}
								else
									Print(_T("Type in Private Key!"));
							}
							else
							{
								m_pClient->SendToServer(_T("/private#")+GetNick()+_T("#")+msg);
								PrintToPChat(m_pClient->GetNick()+_T(": ")+msg);
							}
						}
						else
							Print(_T("Client ")+GetNick()+_T(" is not connected!"));
					}
					else
					{
						Print(_T("Empty messages are not allowed!"));
						Print(_T("Check if you're connected to the server!"));
						return FALSE;
					}
				}
				return FALSE;
			}
			return FALSE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CPrivateChatDlg::Print(CString szMsg)
{
	int i = m_lstPChat.GetCount();
	m_lstPChat.InsertString(i, szMsg);
}

void CPrivateChatDlg::PrintToPChat(CString szMsg)
{
	CString line;
	int i = 0;
	line = szMsg.Tokenize(_T("\r\n"),i);
	while (line != _T(""))
	{
		Print(line);
		line = szMsg.Tokenize(_T("\r\n"),i);
	}
}

void CPrivateChatDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	if(::IsWindow(m_lstPChat.GetSafeHwnd()) && ::IsWindow(m_txtPMessage.GetSafeHwnd()))
	{
		RECT lst_rect, txt_rect;
		int cx, cy, nHeightEditControl;
		m_lstPChat.GetWindowRect(&lst_rect);
		m_txtPMessage.GetWindowRect(&txt_rect);
		ScreenToClient(&lst_rect);
		ScreenToClient(&txt_rect);
		cx = pRect->right - pRect->left;
		cy = pRect->bottom - pRect->top;
		nHeightEditControl = txt_rect.bottom - txt_rect.top;

		m_lstPChat.MoveWindow(lst_rect.left,
			lst_rect.top,
			cx - 2 * lst_rect.left - 9,
			cy - lst_rect.top - lst_rect.left - 125,
			TRUE);
		m_txtPMessage.MoveWindow(txt_rect.left,
			lst_rect.bottom + 5,
			cx - 2 * txt_rect.left - 9,
			nHeightEditControl,
			TRUE);
	}
}

void CPrivateChatDlg::OnBnClickedCheckEncrypt()
{
	if (IsDlgButtonChecked(m_bEncrypt.GetDlgCtrlID()) > 0)
	{
		m_lblPrivateKey.ShowWindow(SW_SHOW);
		m_txtPrivateKey.ShowWindow(SW_SHOW);
	}
	else
	{
		m_lblPrivateKey.ShowWindow(SW_HIDE);
		m_txtPrivateKey.Clear();
		m_txtPrivateKey.ShowWindow(SW_HIDE);
	}
}

BOOL CPrivateChatDlg::IsEncrypted()
{
	if (IsDlgButtonChecked(m_bEncrypt.GetDlgCtrlID()) > 0)
		return TRUE;
	else
		return FALSE;
}

CString CPrivateChatDlg::GetPKey()
{
	CString key;
	m_txtPrivateKey.GetWindowTextW(key);
	return key;
}
