#pragma once


// CInsnListBox

class CServerListBox : public CListBox
{
	DECLARE_DYNAMIC(CServerListBox)

public:
	CServerListBox();
	virtual ~CServerListBox();

	virtual int InsertString(int nIndex, LPCTSTR lpszItem);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


