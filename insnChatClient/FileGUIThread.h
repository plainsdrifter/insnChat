#pragma once

#include "resource.h"
#include "FileProgressDlg.h"

// CFileGUIThread

class CFileGUIThread : public CWinThread
{
	DECLARE_DYNCREATE(CFileGUIThread)

public:
	CFileGUIThread();
	virtual ~CFileGUIThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	CString m_szCaption;
	CString m_szFileName;
	ULONGLONG m_lFileSize;
	HWND m_hFileProgHwnd;

protected:
	DECLARE_MESSAGE_MAP()

public:
	void InitFileProgressDlg(CString szCaption, ULONGLONG lFileSize, CString szFile);
	void SetCaption(CString szCaption) { m_szCaption = szCaption; }
	void SetFileName(CString szFileName) { m_szFileName = szFileName; }
	void SetFileSize(ULONGLONG lFileSize) { m_lFileSize = lFileSize; }
	void SetHwnd(HWND hWnd) { m_hFileProgHwnd = hWnd; }
	HWND GetHwnd() { return m_hFileProgHwnd; }

private:
	CFileProgressDlg * m_pFileProg;
};