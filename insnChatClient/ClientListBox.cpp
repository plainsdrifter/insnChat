// ClientListBox.cpp : implementation file
//

#include "stdafx.h"
#include "insnChatClient.h"
#include "ClientListBox.h"


// CClientListBox

IMPLEMENT_DYNAMIC(CClientListBox, CListBox)

CClientListBox::CClientListBox()
{

}

CClientListBox::~CClientListBox()
{
}


BEGIN_MESSAGE_MAP(CClientListBox, CListBox)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CClientListBox message handlers

void CClientListBox::SetClient(CClientDlg *pClient)
{
	m_pClient = pClient;
}


void CClientListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (this->GetDlgCtrlID() == IDC_OTHERCLIENTS)
	{
		if (GetCurSel() != LB_ERR)
		{
			CString nick;
			GetText(GetCurSel(), nick);
			CButton * bSend = (CButton *)m_pClient->GetDlgItem(IDC_SENDFILE);
			if (nick == m_pClient->GetNick())
			{
				bSend->EnableWindow(FALSE);
			}
			else
			{
				bSend->EnableWindow(TRUE);
			}
		}
	}

	CListBox::OnLButtonUp(nFlags, point);
}

int CClientListBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
	int Pos;
	SCROLLINFO info;
	CTime time;
	CString inputStr;

	time = time.GetTickCount();
	inputStr.Format(_T("[%.2i:%.2i:%.2i] %s"), time.GetHour(), time.GetMinute(), time.GetSecond(), lpszItem);
	int nResult = CListBox::InsertString(nIndex, inputStr);

	int lines = GetCount();

	this->GetScrollInfo(SB_VERT, &info);

	Pos = info.nMax - info.nPage;

	if (Pos >= 0 && info.nPos == Pos) 
	{
		this->SendMessage(WM_VSCROLL, 1, 0);
	}

	return nResult;
}

void CClientListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (this->GetDlgCtrlID() == IDC_CHAT)
	{
		CMenu contextMenu, *tracker;

		contextMenu.LoadMenuW(IDR_CLIPBOARDMENU);
		tracker = contextMenu.GetSubMenu(0);

		ClientToScreen(&point);

		tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
			point.x , point.y , AfxGetMainWnd());

		CListBox::OnRButtonUp(nFlags, point);
	}
}

void CClientListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	WINDOWINFO wi;
	ZeroMemory(&wi, sizeof(wi));

	if (this->GetDlgCtrlID() == IDC_OTHERCLIENTS)
	{
		if (GetCurSel() != LB_ERR)
		{
			CPrivateChatDlg * pPChat;
			CString szNick;

			GetText(GetCurSel(), szNick);

			if (m_pClient->GetNick().Compare(szNick) != 0)
			{
				pPChat = m_pClient->GetPChat(szNick);
				
				if (pPChat == NULL)
				{
					pPChat = new CPrivateChatDlg;
					pPChat->Create(m_pClient, szNick);
					pPChat->ShowWindow(SW_SHOW);
					m_pClient->AddPChat(pPChat);
				}
				else
				{
					pPChat->ShowWindow(SW_SHOW);
					pPChat->GetWindowInfo(&wi);
					if (!(wi.dwWindowStatus && STATE_SYSTEM_FOCUSED))
						pPChat->FlashWindowEx(FLASHW_ALL, 2, 300);

				}
			}
		}
	}

	CListBox::OnLButtonDblClk(nFlags, point);
}
