// FileProgressDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "insnChatClient.h"
#include "FileProgressDlg.h"

// CFileProgressDlg dialog

IMPLEMENT_DYNAMIC(CFileProgressDlg, CDialog)

CFileProgressDlg::CFileProgressDlg(CWnd * pParent /*=NULL*/)
	: CDialog(CFileProgressDlg::IDD, pParent)
{
}

CFileProgressDlg::~CFileProgressDlg()
{
}

void CFileProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILENAME, m_txtFilename);
	DDX_Control(pDX, IDC_FILEPROGRESS, m_barFileProgress);
	DDX_Control(pDX, IDC_SPEED, m_txtSpeed);
	DDX_Control(pDX, IDC_PROGRESS, m_progressLabel);
}

BEGIN_MESSAGE_MAP(CFileProgressDlg, CDialog)
	ON_MESSAGE( WM_DOSTATS_FILE_PROGRESS_DIALOG, &CFileProgressDlg::OnDoStats )
	ON_MESSAGE( WM_STEP_FILE_PROGRESS_DIALOG, &CFileProgressDlg::OnStepIt )
END_MESSAGE_MAP()

BOOL CFileProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_doneForNow = 0;
	m_percent = 0;

	return TRUE;
}

BOOL CFileProgressDlg::Create()
{
	return CDialog::Create(CFileProgressDlg::IDD, GetDesktopWindow());
}


void CFileProgressDlg::SetFileName(CString szName)
{
	m_txtFilename.SetWindowTextW(szName);
}

void CFileProgressDlg::SetSpeed(CString szSpeed)
{
	m_txtSpeed.SetWindowTextW(szSpeed);
}

void CFileProgressDlg::SetPercentage(CString szPercent)
{
	m_progressLabel.SetWindowTextW(szPercent);
}

void CFileProgressDlg::SetUpperRange()
{
	m_barFileProgress.SetRange(0, 100);
}

LRESULT CFileProgressDlg::OnStepIt(WPARAM wParam, LPARAM lParam)
{
	int done = (int)lParam;
	m_doneForNow += done;
	DOUBLE ratio = ((DOUBLE)m_doneForNow / (DOUBLE)m_fileSize);
	m_percent = (int)(ratio * 100);
	StepIt();
	return 0L;
}

void CFileProgressDlg::StepIt()
{ 
	m_barFileProgress.SetPos(m_percent);
}

LRESULT CFileProgressDlg::OnDoStats(WPARAM wParam, LPARAM lParam)
{
	LONG lByteCount = (LONG) lParam;
	DoStats(lByteCount);
	return 0L;
}

void CFileProgressDlg::DoStats(LONG lByteCount)
{
	DOUBLE dByteRate;
	LONG lMSecs;
	CString szSpeed;
	CString szPercent;

	lMSecs = (LONG)GetTickCount() - m_lStartTime;

	if (lByteCount > 0L && lMSecs > 0L)
	{
		dByteRate = lByteCount / lMSecs;
		szSpeed.Format(_T("Transfer rate: %5.2f (Kbytes/sec)"),
			(dByteRate*1000)/1024);//, (lByteRate*1000)%1024);
		szPercent.Format(_T("%d %%"), m_percent);
		SetSpeed(szSpeed);
		SetPercentage(szPercent);
	}
}

void CFileProgressDlg::SetWndPos(int x, int y)
{
	CRect rect;
	this->GetWindowRect(&rect);
	this->MoveWindow(x,y, rect.Width(), rect.Height(), 1);
}

void CFileProgressDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}