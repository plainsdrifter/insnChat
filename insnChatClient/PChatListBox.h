#pragma once


// CPChatListBox

class CPChatListBox : public CListBox
{
	DECLARE_DYNAMIC(CPChatListBox)

public:
	CPChatListBox();
	virtual ~CPChatListBox();

	inline void SetImageList(CImageList* pImgList = NULL) { m_pImageList = pImgList; }

	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual int InsertString(int nIndex, LPCTSTR lpszItem, int iIcon);
	virtual int InsertString(int nIndex, LPCTSTR lpszItem);

	afx_msg void OnDropFiles(HDROP hDropInfo);

protected:
	DECLARE_MESSAGE_MAP()

	CImageList * m_pImageList;
};


