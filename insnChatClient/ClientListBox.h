#pragma once

class CClientDlg;

// CClientListBox

class CClientListBox : public CListBox
{
	DECLARE_DYNAMIC(CClientListBox)

public:
	CClientListBox();
	virtual ~CClientListBox();

	virtual int InsertString(int nIndex, LPCTSTR lpszItem);

	CClientDlg * m_pClient;

	void SetClient(CClientDlg * pClient);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


