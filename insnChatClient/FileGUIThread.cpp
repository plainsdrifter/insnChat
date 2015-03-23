// FileGUIThread.cpp : implementation file
//

#include "stdafx.h"
#include "insnChatClient.h"
#include "FileGUIThread.h"


// CFileGUIThread

IMPLEMENT_DYNCREATE(CFileGUIThread, CWinThread)

CFileGUIThread::CFileGUIThread()
{
	m_hFileProgHwnd = 0;
}

CFileGUIThread::~CFileGUIThread()
{
}

BOOL CFileGUIThread::InitInstance()
{
	CWinThread::InitInstance();

	InitFileProgressDlg(m_szCaption, m_lFileSize, m_szFileName);

	return TRUE;
}

int CFileGUIThread::ExitInstance()
{
	m_pFileProg->DestroyWindow();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CFileGUIThread, CWinThread)
END_MESSAGE_MAP()

void CFileGUIThread::InitFileProgressDlg(CString szCaption, ULONGLONG lFileSize, CString szFile)
{
	m_pFileProg = new CFileProgressDlg;
	m_pFileProg->Create();
	m_pFileProg->ShowWindow(SW_SHOW);
	m_pFileProg->SetWindowTextW(szCaption+_T("..."));
	m_pFileProg->SetWndPos(0,0);
	m_pFileProg->SetFileSize(lFileSize);
	m_pFileProg->SetFileName(szCaption+_T(": ")+szFile);
	m_pFileProg->SetUpperRange();
	LONG lStartTime = (LONG)GetTickCount();
	m_pFileProg->SetStartTime(lStartTime);
	SetHwnd(m_pFileProg->GetSafeHwnd());
}

// CFileGUIThread message handlers
