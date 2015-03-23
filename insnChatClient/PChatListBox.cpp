// PChatListBox.cpp : implementation file
//

#include "stdafx.h"
#include "insnChatClient.h"
#include "PChatListBox.h"


// CPChatListBox

IMPLEMENT_DYNAMIC(CPChatListBox, CListBox)

CPChatListBox::CPChatListBox()
{

}

CPChatListBox::~CPChatListBox()
{
}


BEGIN_MESSAGE_MAP(CPChatListBox, CListBox)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

void CPChatListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if ((int)lpDrawItemStruct->itemID < 0)
	{
		// If there are no elements in the List Box 
		// based on whether the list box has Focus or not 
		// draw the Focus Rect or Erase it,
		if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && 
			(lpDrawItemStruct->itemState & ODS_FOCUS))
		{
			lpDrawItemStruct->rcItem.bottom = 16;
			pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
		}
		else if ((lpDrawItemStruct->itemAction & ODA_FOCUS) &&	
			!(lpDrawItemStruct->itemState & ODS_FOCUS)) 
		{
			lpDrawItemStruct->rcItem.bottom = 16;
			pDC->DrawFocusRect(&lpDrawItemStruct->rcItem); 
		}
		return;
	}

    CRect  rcItem(lpDrawItemStruct->rcItem); // To draw the focus rect.
    CRect  rClient(rcItem); // Rect to highlight the Item
    CRect  rText(rcItem); // Rect To display the Text
    CPoint Pt(rcItem.left, rcItem.top); // Point To draw the Image

	// Image information in the item data.
	int iImg = (int)lpDrawItemStruct->itemData;

	// if the Image list exists for the list box
	// adjust the Rect sizes to accomodate the Image for each item.
	if(m_pImageList && (iImg != -1))
	{
		rClient.left += 32;
		rText.left += 32;
		rText.top += 2;
	}
	else
	{
		rText.top += 2;
	}


	COLORREF crText;
	CString strText;

	// If item selected, draw the highlight rectangle.
	// Or if item deselected, draw the rectangle using the window color.
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
		 (lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		CBrush br(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(&rClient, &br);
	}
	else if (!(lpDrawItemStruct->itemState & ODS_SELECTED) && 
		(lpDrawItemStruct->itemAction & ODA_SELECT)) 
	{
		CBrush br(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(&rClient, &br);
	}

	// If the item has focus, draw the focus rect.
	// If the item does not have focus, erase the focus rect.
	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && 
		(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		pDC->DrawFocusRect(&rcItem); 
	}
	else if ((lpDrawItemStruct->itemAction & ODA_FOCUS) &&	
		!(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		pDC->DrawFocusRect(&rcItem); 
	}

	// To draw the Text set the background mode to Transparent.
	int iBkMode = pDC->SetBkMode(TRANSPARENT);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		crText = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	else if (lpDrawItemStruct->itemState & ODS_DISABLED)
		crText = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
	else
		crText = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

	// Get the item text.
	GetText(lpDrawItemStruct->itemID, strText);

	// Setup the text format.
	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	if (GetStyle() & LBS_USETABSTOPS)
		nFormat |= DT_EXPANDTABS;
	

	// if the ImageList is Existing and there is an associated Image
	// for the Item, draw the Image.
	if(m_pImageList && (iImg != -1 ) )
		m_pImageList->Draw(pDC,iImg,Pt,ILD_NORMAL);
	
	//Draw the Text
	pDC->DrawText(strText, -1, &rText, nFormat | DT_CALCRECT);
	pDC->DrawText(strText, -1, &rText, nFormat);

	pDC->SetTextColor(crText); 
	pDC->SetBkMode(iBkMode);
}

void CPChatListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 16;
}

int CPChatListBox::InsertString(int nIndex, LPCTSTR lpszItem, int iIcon)
{
	int iRet = CListBox::InsertString(nIndex, lpszItem);
	if (iRet >= 0)
	{
		SetItemHeight(iRet, 32);
		SetItemData(iRet, iIcon);
	}
	return iRet;
}

int CPChatListBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
	int Pos;
	SCROLLINFO info;
	CTime time;
	CString inputStr;

	time = time.GetTickCount();
	inputStr.Format(_T("[%.2i:%.2i:%.2i] %s"), time.GetHour(), time.GetMinute(), time.GetSecond(), lpszItem);
	int nResult = CListBox::InsertString(nIndex, inputStr);
	if (nResult >= 0)
		SetItemData(nResult, -1);

	this->GetScrollInfo(SB_VERT, &info);

	Pos = info.nMax - info.nPage;

	if (Pos >= 0 && info.nPos == Pos) 
	{
		this->PostMessage(WM_VSCROLL, 1, 0);
	}

	return nResult;
}

void CPChatListBox::OnDropFiles(HDROP hDropInfo)
{
	UINT i = 0;
	UINT nFiles = DragQueryFile(hDropInfo, (UINT) -1, NULL, 0);
	if (nFiles > 1)
	{
		AfxMessageBox(_T("You can drop only one file simultaneously!"), MB_ICONINFORMATION);
		return;
	}
	LPTSTR szFileName = new TCHAR[_MAX_PATH];
	DragQueryFile(hDropInfo, i, szFileName, _MAX_PATH);

	SHFILEINFO shfi;
	ZeroMemory(&shfi, sizeof(shfi));

	SHGetFileInfo(szFileName, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi), 
		SHGFI_ATTRIBUTES | SHGFI_ICON | SHGFI_LARGEICON);
	if (shfi.dwAttributes & SFGAO_FOLDER)
	{
		AfxMessageBox(_T("You can not drop folders!"), MB_ICONINFORMATION);
		return;
	}
	else
	{
		int iIcon = m_pImageList->Add(shfi.hIcon);
		int iRet = InsertString(i, szFileName, iIcon);
		DestroyIcon(shfi.hIcon);
	}

	DragFinish(hDropInfo);
	delete [] szFileName;
	CListBox::OnDropFiles(hDropInfo);
}
