#pragma once

#define SEND_BUFFER_SIZE    4096
#define RECV_BUFFER_SIZE	4096
#define AGREED_PORT			16000
#define SPEED_UPDATE_RATE	500
/**********************************
*			CFileThread			  *
**********************************/
class CFileThread : public CWinThread
{
public:
	CFileThread();
	virtual ~CFileThread();

public:
	BOOL IsSending;
	BOOL IsAccepted;
	CString szFileName;
	CString szFilePath;
	CString szIP;
	CString szNewPath;
	CString szNick;

private:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	
	BOOL SendFile(CString filePath);
	BOOL ReceiveFile(CString szIP, CString fName);
	BOOL GetFolder(CString &folderPath);
};