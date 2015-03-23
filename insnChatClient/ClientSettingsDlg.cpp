// ClientSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "insnChatClient.h"
#include "ClientSettingsDlg.h"
#include "ClientDlg.h"


// CClientSettingsDlg dialog

IMPLEMENT_SERIAL(CClientSettingsDlg, CDialog, VERSIONABLE_SCHEMA | 2);

CClientSettingsDlg::CClientSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientSettingsDlg::IDD, pParent)
{
	m_szNick = _T("");
	m_szAddress = _T("0.0.0.0");
	m_nPort = 0;
	m_bMinimizeToTray = FALSE;
	m_bShowBlnTips = FALSE;
}

CClientSettingsDlg::~CClientSettingsDlg()
{
}

void CClientSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CL_SETTINGS_IP, m_ipAddressToServer);
	DDX_Control(pDX, IDC_CL_SETTINGS_PORT, m_txtPortToServer);
	DDX_Control(pDX, IDC_CL_NICK, m_txtClientNickName);
	DDX_Control(pDX, IDC_MINIMIZE, m_cbMinimize);
	DDX_Control(pDX, IDC_BALLOONTIPS, m_cbBalloonTips);
}


BEGIN_MESSAGE_MAP(CClientSettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_MINIMIZE, &CClientSettingsDlg::OnBnClickedMinimize)
END_MESSAGE_MAP()

BOOL CClientSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString szByte,szPort;
	int i = 0;
	UINT f1 = 0,f2 = 0,f3 = 0,f4 = 0;
	CFile file;

	TCHAR szAppPath[MAX_PATH] = _T("");
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));

	file.Open(strAppDirectory+_T("\\client.dat"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeRead );

	if (file.GetLength() != 0)
		Serialize(CArchive(&file, CArchive::load));

	file.Close();

	m_txtClientNickName.SetWindowTextW(m_szNick);
	szPort.Format(_T("%i"), m_nPort);
	m_txtPortToServer.SetWindowTextW(szPort);
	m_cbMinimize.SetCheck(m_bMinimizeToTray);
	m_cbBalloonTips.SetCheck(m_bShowBlnTips);
	if (m_bMinimizeToTray)
		m_cbBalloonTips.EnableWindow(TRUE);
	else
		m_cbBalloonTips.EnableWindow(FALSE);
	
	if (i >= 0)
	{	
		szByte = m_szAddress.Tokenize(_T("."), i);
		f1 = _ttoi(szByte);
	}
	if (i >= 0)
	{	
		szByte = m_szAddress.Tokenize(_T("."), i);
		f2 = _ttoi(szByte);
	}
	if (i >= 0)
	{
		szByte = m_szAddress.Tokenize(_T("."), i);
		f3 = _ttoi(szByte);
	}
	if (i >= 0)
	{
		szByte = m_szAddress.Tokenize(_T("."), i);
		f4 = _ttoi(szByte);
	}

	m_ipAddressToServer.SetAddress(f1,f2,f3,f4);

	return TRUE;
}

void CClientSettingsDlg::SetAddress()
{
	BYTE f1, f2, f3, f4;

	m_ipAddressToServer.GetAddress(f1,f2,f3,f4);

	m_szAddress.Format(_T("%i.%i.%i.%i"),f1,f2,f3,f4);
}

void CClientSettingsDlg::SetPort()
{
	CString port;

	m_txtPortToServer.GetWindowTextW(port);

	m_nPort = _ttoi(port);
}

void CClientSettingsDlg::SetMinimizeToTray()
{
	if (IsDlgButtonChecked(m_cbMinimize.GetDlgCtrlID()) > 0)
		m_bMinimizeToTray = TRUE;
	else
		m_bMinimizeToTray = FALSE;
}

void CClientSettingsDlg::SetShowBalloonTips()
{
	if (IsDlgButtonChecked(m_cbBalloonTips.GetDlgCtrlID()) > 0)
		m_bShowBlnTips = TRUE;
	else
		m_bShowBlnTips = FALSE;
}

void CClientSettingsDlg::OnOK()
{
	SetNick();
	SetAddress();
	SetPort();
	SetMinimizeToTray();
	SetShowBalloonTips();

	TCHAR szAppPath[MAX_PATH] = _T("");
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));

	CFile file;
	file.Open(strAppDirectory+_T("\\client.dat"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite );
	Serialize(CArchive(&file, CArchive::store));

	file.Close();

	m_pClient->SetNick(GetNick());
	m_pClient->SetServerAddress(GetAddress());
	m_pClient->SetServerPort(GetPort());
	m_pClient->SetMinimizeToTray(GetMinimizeToTray());
	m_pClient->SetShowBalloonTips(GetShowBalloonTips());

	CDialog::OnOK();
}

void CClientSettingsDlg::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_szNick << m_szAddress << m_nPort << m_bMinimizeToTray << m_bShowBlnTips;
	}
	else
	{
		ar >> m_szNick >> m_szAddress >> m_nPort >> m_bMinimizeToTray >> m_bShowBlnTips;
	}

	ar.Close();
}

void CClientSettingsDlg::SetClient(CClientDlg * pClient)
{
	m_pClient = pClient;
}

// CClientSettingsDlg message handlers

void CClientSettingsDlg::OnBnClickedMinimize()
{
	if (IsDlgButtonChecked(m_cbMinimize.GetDlgCtrlID()) > 0)
		m_cbBalloonTips.EnableWindow(TRUE);
	else
	{
		m_cbBalloonTips.SetCheck(BST_UNCHECKED);
		m_cbBalloonTips.EnableWindow(FALSE);
	}
}
