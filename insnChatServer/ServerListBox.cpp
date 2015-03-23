// InsnListBox.cpp : implementation file
//

#include "stdafx.h"
#include "insnChatServer.h"
#include "ServerListBox.h"


// CInsnListBox

IMPLEMENT_DYNAMIC(CServerListBox, CListBox)

CServerListBox::CServerListBox()
{

}

CServerListBox::~CServerListBox()
{
}


BEGIN_MESSAGE_MAP(CServerListBox, CListBox)
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()



// CInsnListBox message handlers



void CServerListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
	CMenu contextMenu, *tracker;

	switch (this->GetDlgCtrlID())
	{
	case IDC_CLIENTSLIST:

		contextMenu.LoadMenuW(IDR_PROPERTIES);
		tracker = contextMenu.GetSubMenu(0);

		ClientToScreen(&point);

		tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
			point.x , point.y , AfxGetMainWnd());

		CListBox::OnRButtonUp(nFlags, point);
		break;
	
	case IDC_SERVERCONSOLE:

		contextMenu.LoadMenuW(IDR_CLIPBOARDMENU);
		tracker = contextMenu.GetSubMenu(0);

		ClientToScreen(&point);

		tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
			point.x , point.y , AfxGetMainWnd());

		CListBox::OnRButtonUp(nFlags, point);
		break;
	}
}

int CServerListBox::InsertString(int nIndex, LPCTSTR lpszItem)
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