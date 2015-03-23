#pragma once

#include "afxwin.h"
#include "afxcmn.h"

//#define SEND_BUFFER_SIZE 4096
#define WM_DOSTATS_FILE_PROGRESS_DIALOG (WM_USER + 1)
#define WM_STEP_FILE_PROGRESS_DIALOG (WM_USER + 2)

// CFileProgressDlg dialog
class CFileProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileProgressDlg)

public:
	CFileProgressDlg(CWnd * pParent = NULL);
	virtual ~CFileProgressDlg();

	virtual BOOL OnInitDialog();
	virtual BOOL Create();
	virtual void PostNcDestroy();

	ULONGLONG GetFileSize() { return m_fileSize; };
	void SetFileName(CString szName);
	void SetSpeed(CString szSpeed);
	void SetPercentage(CString szPercent);
	void SetFileSize(ULONGLONG size) { m_fileSize = size; };
	void SetUpperRange();
	void SetStartTime(LONG lStartTime) { m_lStartTime = lStartTime; };
	void StepIt();
	void SetWndPos(int x, int y);	
	void DoStats(LONG lByteCount);
	
// Dialog Data
	enum { IDD = IDD_SENDINGFILE };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	CStatic m_txtFilename;
	CStatic m_txtSpeed;
	CStatic m_progressLabel;
	CProgressCtrl m_barFileProgress;
	ULONGLONG m_fileSize;
	LONG m_lStartTime;
	int m_percent;
	ULONGLONG m_doneForNow;


protected:
	afx_msg LRESULT OnDoStats(WPARAM, LPARAM);
	afx_msg LRESULT OnStepIt(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()	
};
