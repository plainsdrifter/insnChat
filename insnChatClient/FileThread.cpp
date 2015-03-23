#include "StdAfx.h"
#include "FileThread.h"
#include "FileGUIThread.h"

/**********************************
*	CFileThread implementation	  *
**********************************/
CFileThread::CFileThread()
{
	IsAccepted = FALSE;
}

CFileThread::~CFileThread()
{
}

BOOL CFileThread::InitInstance()
{
	CWinThread::InitInstance();

	if(!AfxSocketInit())
	{
		AfxMessageBox(_T("CFileThread: Failed to Initialize Sockets!"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if (IsSending)
	{
		SendFile(szFilePath);
	}
	else
	{
		ReceiveFile(szIP, szFileName);
	}

	return TRUE;
}

BOOL CFileThread::ExitInstance()
{
	if (IsSending && IsAccepted)
	{
		AfxMessageBox(_T("Sending complete!"), MB_ICONINFORMATION);
	}
	else if (IsSending && !IsAccepted)
	{
		AfxMessageBox(_T("Receiver didn't accept your file!"), MB_ICONINFORMATION);
	}
	else if (!IsSending && IsAccepted)
	{
		AfxMessageBox(_T("Download complete!"), MB_ICONINFORMATION);
	}
	//else if (!IsSending && !IsAccepted)
	//{}
	return CWinThread::ExitInstance();
}

BOOL CFileThread::SendFile(CString filePath)
{
	CString errMsg;
	CSocket sockSrvr;
	DWORD nErrorCode;
	TCHAR szErr[256];
	BOOL bRet = TRUE;
	ULONG fileLength, cbLeftToSend;    
    BYTE* sendData = NULL;
    CFile sourceFile;
    CFileException fe;
    BOOL bFileIsOpen = FALSE;

    if (!sockSrvr.Create(AGREED_PORT))
	{
		nErrorCode = GetLastError();
		wsprintf(szErr, _T("Error occured in SendFile() CSocket::Create, error code: %d"), nErrorCode);
		AfxMessageBox(szErr, MB_ICONERROR);
		this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
	}

    sockSrvr.Listen(1);
    CSocket sockConnection;

    if (!sockSrvr.Accept(sockConnection))
	{
		nErrorCode = GetLastError();
		wsprintf(szErr, _T("Error occured in SendFile() CSocket::Accept, error code: %d"), nErrorCode);
		AfxMessageBox(szErr, MB_ICONERROR);
		this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
	}

	TCHAR msg[32];
	int iAcceptRead = sockConnection.Receive(msg, 32);
	msg[iAcceptRead/2] = 0;
	CString info(msg);
	if (info.Compare(_T("ACCEPT")) != 0)
	{
		sockConnection.Close();
		sockSrvr.Close();
		this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
	}

	IsAccepted = TRUE;
    
    if( !( bFileIsOpen = sourceFile.Open( filePath, 
           CFile::modeRead | CFile::typeBinary, &fe ) ) )
    {
        TCHAR strCause[256];
        fe.GetErrorMessage( strCause, 255 );
		errMsg.Format( _T("SendFile encountered an error while opening the local file\n")
			_T("File name = %s\nCause = %s\nm_cause = %d\n\tm_IOsError = %d\n"), 
			fe.m_strFileName, strCause, fe.m_cause, fe.m_lOsError );
        
        this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
    }

	if (ULONG_MAX < sourceFile.GetLength())
	{
		AfxMessageBox(_T("File is too big for transfer!"), MB_ICONINFORMATION);
		sockConnection.Close();
		sockSrvr.Close();
		this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
	}

	fileLength = (ULONG)sourceFile.GetLength();
    fileLength = htonl(fileLength);

    cbLeftToSend = sizeof(fileLength);

    do
    {
        int cbBytesSent;
        BYTE* bp = (BYTE*)(&fileLength) + sizeof(fileLength) - cbLeftToSend;
        cbBytesSent = sockConnection.Send(bp, cbLeftToSend);
        
        if (cbBytesSent == SOCKET_ERROR)
        {
            int iErr = ::GetLastError();
			errMsg.Format(_T("SendFile returned a socket error while sending file length\n")
				_T("Number of Bytes sent = %d\nGetLastError = %d\n"), cbBytesSent, iErr);
            
            this->PostThreadMessageW(WM_QUIT, 0, 0);
			return FALSE;
        }
        
        cbLeftToSend -= cbBytesSent;
    }
    while ( cbLeftToSend>0 );
    
    
    // now send the file's data

	sendData = new BYTE[SEND_BUFFER_SIZE]; 
    
    cbLeftToSend = (ULONG)sourceFile.GetLength();

	int updateDlg = 0;
	LONG lByteCount = 0;
	CFileGUIThread * pFileGUI = new CFileGUIThread;
	pFileGUI->CreateThread(CREATE_SUSPENDED, 0);
	pFileGUI->SetCaption(_T("Sending file"));
	pFileGUI->SetFileSize(cbLeftToSend);
	pFileGUI->SetFileName(filePath);
	pFileGUI->ResumeThread();
	
    do
    {
        int sendThisTime, doneSoFar, buffOffset;
        
        sendThisTime = sourceFile.Read( sendData, SEND_BUFFER_SIZE );
        buffOffset = 0;
        
        do
        {
            doneSoFar = sockConnection.Send( sendData + buffOffset, 
                                                    sendThisTime ); 
            
            if ( doneSoFar == SOCKET_ERROR )
            {
                int iErr = ::GetLastError();
				errMsg.Format( _T("SendFile returned a socket error ")
                    _T("while sending chunked file data\n")
                    _T("Number of Bytes sent = %d\n")
                    _T("GetLastError = %d\n"), doneSoFar, iErr );
				AfxMessageBox(errMsg, MB_ICONERROR);                
                bRet = FALSE;
                goto PreReturnCleanup;
            }
            
            buffOffset += doneSoFar;
			lByteCount += doneSoFar;
            sendThisTime -= doneSoFar;
            cbLeftToSend -= doneSoFar;

			::PostMessage(pFileGUI->GetHwnd(), WM_STEP_FILE_PROGRESS_DIALOG, 0, (LPARAM)doneSoFar);
        }
        while ( sendThisTime > 0 );

		if (updateDlg == SPEED_UPDATE_RATE)
		{
			::PostMessage(pFileGUI->GetHwnd(), WM_DOSTATS_FILE_PROGRESS_DIALOG, 0, (LPARAM)lByteCount);
			updateDlg = 0;
		}
		updateDlg++;
    }
    while ( cbLeftToSend > 0 );

PreReturnCleanup: // labelled goto destination

    delete[] sendData;
    
    if ( bFileIsOpen )
        sourceFile.Close();
   
    sockConnection.Close();
	sockSrvr.Close();
    
	pFileGUI->PostThreadMessageW(WM_QUIT,0,0);
	this->PostThreadMessageW(WM_QUIT, 0, 0);
	
    return bRet;
}

BOOL CFileThread::ReceiveFile(CString szIP, CString fName)
{
	CString errMsg;
	DWORD nErrorCode;
	TCHAR szErr[256];
	CRect fpRect;
	CSocket sockClient;
	BOOL bRet = TRUE; // return value
    ULONG dataLength, cbBytesRet, cbLeftToReceive;
    BYTE* recdData = NULL;    
    CFile destFile;
    CFileException fe;
    BOOL bFileIsOpen = FALSE;

    if (!sockClient.Create())
	{
		nErrorCode = GetLastError();
		wsprintf(szErr, _T("Error occured in ReceiveFile() CSocket::Create, error code: %d"), nErrorCode);
		AfxMessageBox(szErr, MB_ICONERROR);
		this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
	}
    
    if (!sockClient.Connect(szIP, AGREED_PORT))
	{
		AfxMessageBox(_T("Connect to sender failed! ")+szIP, MB_ICONERROR);
		this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
	}
	
	if (AfxMessageBox(szNick+_T(" is trying to send you a file: ")+szFileName+_T("!\n\nDo you accept?"), MB_ICONQUESTION|MB_YESNO) == IDYES)
	{
		GetFolder(szNewPath);
		CString msg(_T("ACCEPT"));
		LPTSTR buf = msg.GetBuffer();
		msg.ReleaseBuffer();
		sockClient.Send(buf, 2*msg.GetLength());
		IsAccepted = TRUE;
	}
	else
	{
		sockClient.Close();
		bRet = FALSE;
		this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
	}
	
	
    
	if( !( bFileIsOpen = destFile.Open( szNewPath+_T("\\")+fName, CFile::modeCreate | 
           CFile::modeWrite | CFile::typeBinary, &fe ) ) )
    {
        TCHAR strCause[256];
        fe.GetErrorMessage( strCause, 255 );
		errMsg.Format( _T("ReceiveFile encountered ") 
          _T("an error while opening the local file\n")
          _T("File name = %s\n\tCause = %s\n\tm_cause = %d\nm_IOsError = %d\n"),
          fe.m_strFileName, strCause, fe.m_cause, fe.m_lOsError );
		AfxMessageBox(errMsg, MB_ICONERROR);

        this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
    }    
    
    // get the file's size first

    cbLeftToReceive = sizeof( dataLength );
    
    do
    {
        BYTE* bp = (BYTE*)(&dataLength) + sizeof(dataLength) - cbLeftToReceive;
        cbBytesRet = sockClient.Receive( bp, cbLeftToReceive );
        
        if ( cbBytesRet == SOCKET_ERROR || cbBytesRet == 0 )
        {
            int iErr = ::GetLastError();
			errMsg.Format( _T("ReceiveFile returned ")
              _T("a socket error while getting file length\n")
              _T("Number of Bytes received (zero means connection was closed) = %d\n")
              _T("GetLastError = %d\n"), cbBytesRet, iErr );
			AfxMessageBox(errMsg, MB_ICONERROR);
            
            this->PostThreadMessageW(WM_QUIT, 0, 0);
			return FALSE;
        }
        
        cbLeftToReceive -= cbBytesRet;
        
    }
    while ( cbLeftToReceive > 0 );

    dataLength = ntohl( dataLength );

	if (dataLength < 0 || dataLength > ULONG_MAX)
	{
		sockClient.Close();
		this->PostThreadMessageW(WM_QUIT, 0, 0);
		return FALSE;
	}

    recdData = new byte[RECV_BUFFER_SIZE];
    cbLeftToReceive = dataLength;

	int updateDlg = 0;
	LONG lByteCount = 0;
	CFileGUIThread * pFileGUI = new CFileGUIThread;
	pFileGUI->CreateThread(CREATE_SUSPENDED, 0);
	pFileGUI->SetCaption(_T("Receiving file"));
	pFileGUI->SetFileSize(cbLeftToReceive);
	pFileGUI->SetFileName(fName);
	pFileGUI->ResumeThread();
    
    do
    {    
        int iiGet, iiRecd;

        iiGet = (cbLeftToReceive<RECV_BUFFER_SIZE) ? 
                      cbLeftToReceive : RECV_BUFFER_SIZE;
        iiRecd = sockClient.Receive( recdData, iiGet );

        if ( iiRecd == SOCKET_ERROR || iiRecd == 0 )
        {
            int iErr = ::GetLastError();
			errMsg.Format( _T("ReceiveFile returned a socket error " )
                _T("while getting chunked file data\n")
                _T("Number of Bytes received (zero means connection was closed) = %d\n")
                _T("GetLastError = %d\n"), iiRecd, iErr );
            AfxMessageBox(errMsg, MB_ICONERROR);
            bRet = FALSE;
            goto PreReturnCleanup;
        }

        destFile.Write( recdData, iiRecd); // Write it

        cbLeftToReceive -= iiRecd;
		lByteCount += iiRecd;

		::PostMessage(pFileGUI->GetHwnd(), WM_STEP_FILE_PROGRESS_DIALOG, 0, (LPARAM)iiRecd);

		if (updateDlg == SPEED_UPDATE_RATE)
		{
			::PostMessage(pFileGUI->GetHwnd(), WM_DOSTATS_FILE_PROGRESS_DIALOG, 0, (LPARAM)lByteCount);
			updateDlg = 0;
		}
		updateDlg++;
    } 
    while ( cbLeftToReceive > 0 );

PreReturnCleanup: // labelled "goto" destination

    delete[] recdData;        
    
    if ( bFileIsOpen )
        destFile.Close();
    
    sockClient.Close();

	pFileGUI->PostThreadMessageW(WM_QUIT, 0, 0);
	this->PostThreadMessageW(WM_QUIT, 0, 0);
    
    return bRet;
}

BOOL CFileThread::GetFolder(CString &folderPath) 
{
   BOOL retVal = false;
   LPCTSTR szCaption = NULL;
   HWND hOwner = AfxGetApp()->m_pMainWnd->GetSafeHwnd();//GetDesktopWindow();

   BROWSEINFO bi;
   memset(&bi, 0, sizeof(bi));

   bi.ulFlags   = BIF_USENEWUI;
   bi.hwndOwner = hOwner;
   bi.lpszTitle = szCaption;

   ::OleInitialize(NULL);

   LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

   if(pIDL != NULL)
   {
      TCHAR buffer[_MAX_PATH] = {'\0'};
      if(::SHGetPathFromIDList(pIDL, buffer) != 0)
      {
         folderPath = buffer;
         retVal = true;
      }
      CoTaskMemFree(pIDL);
   }
   ::OleUninitialize();

   return retVal;
}