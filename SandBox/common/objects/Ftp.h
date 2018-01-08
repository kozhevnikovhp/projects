/* File Ftp.h - declaration of all classes related to file transfer protocol

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	20 Feb 2003 - initial creation
	27 Feb 2003 - listen and connection threads added
	10 Mar 2003 - almost all commands implemented (except PASV and REST)
	25 Apr 2003 - PASV command implemented
	17 Aug 2004 - bugs in file receiving fixed
	09 Sep 2004 - CFtpClient class added
*/

#ifndef __FTP_H_INCLUDED__
#define __FTP_H_INCLUDED__

#include "stdafx.h"
#include "TcpSockets.h"
#include "ThreadObject.h"
#include "TcpThreadSocket.h"

// Forward declarations
class CFtpServer;

///////////////////////////////////////////////////////////////////////////////////////////////
// CFtpDataSocket class declaration

const int XFERMODE_IDLE	= 0;
const int XFERMODE_LIST	= 1;
const int XFERMODE_SEND	= 2;
const int XFERMODE_RECEIVE = 3;
const int XFERMODE_ERROR =	4;

class CFtpDataSocket : public CTcpThreadSocket
{
	friend class CFtpConnectSocket;
//Attributes
public:
	CFtpDataSocket();
	virtual ~CFtpDataSocket();

// Public overridables
public:

// Common socket methods
	virtual CTcpSocket *CreateAcceptSocket();

// Public members
public:

// Protected methods
protected:
	void Init(CFtpServer *pServer, CFtpConnectSocket *pConnectSocket) { m_pServer = pServer; m_pConnectSocket = pConnectSocket; }
	void SetRestartOffset(DWORD dwOffset) { m_dwRestartOffset = dwOffset; }
	int SendFile(const char *pszFileName);
	int ReceiveFile();
	int SendBuffer(char *pBuffer, unsigned int nBytesToSend);
	BOOL PrepareReceiveFile(const char *lpszFilename);
	BOOL PrepareSendFile(const char *lpszFilename);
	void FreeFile();
	BOOL SendNextChunk();
	BOOL ReadNextChunk();
	void TaskCompleted();
	BOOL IsTaskCompleted();

// Protected overridables
protected:
	virtual void OnThreadStarted();
	virtual LOGICAL ThreadBody();
	virtual void OnThreadEnded();
	
// Protected members
protected:
	char m_szReceivingBuffer[1024];
	BOOL m_bTransferComplete;
	int m_nTransferType;
	CFtpConnectSocket *m_pConnectSocket;
	CFtpServer *m_pServer;
	DWORD m_dwRestartOffset;
	BOOL m_bInitialized;
	unsigned long m_nTotalBytesTransfered;
	unsigned long m_nTotalBytesReceive;
	unsigned long m_nTotalBytesSend;
	HANDLE m_hFile, m_hMapping;
	unsigned long m_nMaxSendChunkSize;
	char *m_pData;
	int m_nStatus;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// CFtpConnectSocket class declaration

enum // Token ID's
{
	TOK_ABOR, TOK_BYE, TOK_CDUP, TOK_CWD,
	TOK_DELE, TOK_DIR, TOK_NLST, TOK_HELP, TOK_LIST,
	TOK_MKD, TOK_NOOP, TOK_PASS, TOK_PASV, 
	TOK_PORT, TOK_PWD, TOK_XPWD, TOK_QUIT, TOK_REST,
	TOK_RETR, TOK_RMD, TOK_RNFR, TOK_RNTO, 
	TOK_SIZE, TOK_STOR, TOK_SYST, TOK_TYPE, 
	TOK_USER, TOK_ERROR,
};

struct SFtpCommand
{
	int m_nTokenID;
	char *m_pszName;
	BOOL m_bHasArguments;
	char *m_pszDescription;
};

class CFtpConnectSocket : public CTcpThreadSocket
{
//Attributes
public:
	CFtpConnectSocket();
	virtual ~CFtpConnectSocket();

// Public methods
public:
	void SetServer(CFtpServer *pServer) { m_pServer = pServer; m_DataSocket.Init(pServer, this); }
	BOOL IsRunning() { return NeedContinueThread(); }
	void Stop();
	BOOL CreateDataConnection();
	void DestroyDataConnection();
	BOOL SendReply(const char *pstrFormat, ...);
	BOOL ReportFileNotFound();
	BOOL ReportPermissionDenied();
	BOOL ReportBadCommandSequence();

// Public overridables
public:
// Common socket methods

// Public members
public:

// Protected methods
protected:
	BOOL ReceiveCommand();
	BOOL ParseCommand();
	BOOL ParseLine();
	void ProcessUserCmd();
	void ProcessPassCmd();
	void ProcessTypeCmd();
	void ProcessPwdCmd();
	void ProcessCwdCmd();
	void ProcessPortCmd();
	void ProcessPasvCmd();
	void ProcessDirCmd();
	void ProcessRetrCmd();
	void ProcessStorCmd();
	void ProcessSizeCmd();
	void ProcessDeleCmd();
	void ProcessRmdCmd();
	void ProcessMkdCmd();
	void ProcessRnfrCmd();
	void ProcessRntoCmd();
	void ProcessAborCmd();
	void ProcessQuitCmd();
	void ProcessRestCmd();
	void ProcessHelpCmd();
	BOOL SendDirectoryList(char *pszFileNameTemplate);
	int ChangeDirectory(char *pszDesiredDir);
	BOOL CreateRelativePath(const char *pszDirPath);

// Protected overridables
protected:
	virtual void OnThreadStarted();
	virtual LOGICAL ThreadBody();
	virtual void OnThreadEnded();

// Protected members
protected:
	CFtpServer *m_pServer;
	CFtpDataSocket m_DataSocket;
	char m_szInputBuffer[1024];
	char *m_pszCommand;
	int m_nCommand;
	char *m_pszArguments;
	BOOL m_bLoggedOn;
	BOOL m_bPassiveMode;
	DWORD m_dwRestartOffset;
	char m_pszBuffer[1024];
	char m_szFreeUsedBuffer[1024];
	char m_szRenamePath[1024];
	char m_szUserName[128];
	char m_szPeerName[16];
	char m_szCurrentDir[256];
	char *m_pszCurrentDirRelative;
	struct sockaddr m_Peer;
	int m_HomeDirNameLength;
	// data socket parameters - IP-address and IP-port
	DWORD m_DataRemoteHostAddress;
	unsigned short m_nDataRemotePort;
};


///////////////////////////////////////////////////////////////////////////////////////////////
// CFtpListenSocket class declaration

class CFtpListenSocket : public CTcpThreadSocket
{
//Attributes
public:
	CFtpListenSocket();
	virtual ~CFtpListenSocket();

// Public methods
public:
	void SetServer(CFtpServer *pServer) { m_pServer = pServer; }

// Public overridables
public:
// Common socket methods
	virtual CTcpSocket *CreateAcceptSocket();

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:
	virtual void OnThreadStarted();
	virtual LOGICAL ThreadBody();
	virtual void OnThreadEnded();

// Protected members
protected:
	CFtpServer *m_pServer;
};


/////////////////////////////////////////////////////////////////////////////////////////
// CFtpServer class declaration

class CFtpServer
{
// Construction/destruction
public:
	CFtpServer();
	virtual ~CFtpServer();
// Public methods
public:
	BOOL Start();
	BOOL Stop();
	void SetListenPort(unsigned short nPort) { m_nListenPort = nPort; }
	unsigned short GetListenPort() { return m_nListenPort; }
	void IncomingCallAccepted(CFtpConnectSocket *pConnectionSocket);
	void CloseSession(CFtpConnectSocket *pConnectionSocket);

// Public overridables
public:
	virtual void OnNewUserIsLoggingOn(const char *pszUserName, struct sockaddr_in *pRemotePeer) {}
	virtual BOOL CanUserLogOn(const char *pszUserName, const char *pszPassword, struct sockaddr_in *pRemotePeer) { return TRUE; }
	virtual void OnNewUserIsLoggedOn(const char *pszUserName, struct sockaddr_in *pRemotePeer)
	{
		printf("User <%s> is logged on from %s\n", pszUserName, inet_ntoa(pRemotePeer->sin_addr));
	}
	virtual void OnNewUserIsLoggedOff(const char *pszUserName, struct sockaddr_in *pRemotePeer)
	{
		printf("User <%s> is logged off from %s\n", pszUserName, inet_ntoa(pRemotePeer->sin_addr));
	}
	virtual BOOL CanUserDownloadFile(const char *pszUserName, const char *pszWorkingDir) { return TRUE; }
	virtual BOOL CanUserUploadFile(const char *pszUserName, const char *pszWorkingDir) { return TRUE; }
	virtual BOOL CanUserDeleteFile(const char *pszUserName, const char *pszWorkingDir) { return TRUE; }
	virtual BOOL CanUserRenameFile(const char *pszUserName, const char *pszWorkingDir) { return TRUE; }
	virtual BOOL CanUserDeleteDir(const char *pszUserName, const char *pszWorkingDir) { return TRUE; }
	virtual BOOL CanUserMakeDir(const char *pszUserName, const char *pszWorkingDir) { return TRUE; }
	virtual BOOL CanUserGetFileSize(const char *pszUserName, const char *pszWorkingDir) { return TRUE; }
	virtual BOOL CanUserListDir(const char *pszUserName, const char *pszWorkingDir, const char *) { return TRUE; }
	virtual char *GetUserHomeDir(const char *pszUserName) { return m_szWorkingDir; }

// Protected methods
	CFtpConnectSocket *CreateConnectSocket();

// Protected overridables
	virtual char *GetWelcomeMessage() { return "Welcome to Evgeny Kozhevnikov's FTP Server"; }
	virtual char *GetGoodbyeMessage() { return "Evgeny Kozhevnikov thanks you for using his FTP-server."; }
	virtual char *GetTooBusyMessage() { return "Too many users are connected to this FTP Server now. Please try again later."; }

protected:
// Protected members
protected:
	BOOL m_bRunning;
	CFtpListenSocket m_ListenSocket;
	unsigned short m_nListenPort;
	CFtpConnectSocket m_ConnectSocket[100];
	CFtpConnectSocket m_TooBusySocket;
	char m_szWorkingDir[1024];
};


class CFtpClient
{
public:
	char m_pszReply[1024];
	void LogOffServer();
	BOOL LogOnToServer(CString hostname,int hostport,CString username, CString password, CString acct = "", CString fwhost = "", CString fwusername = "", CString fwpassword = "", int fwport = 0, int logontype = 0);
	CFtpClient();
	~CFtpClient();
	BOOL UploadFile(const char *pszLocalFileName, const char *pszRemoteFileName, BOOL bPasv = FALSE);
	BOOL DownloadFile(const char *pszRemoteFileName, const char *pszLocalFileName, BOOL bPasv = FALSE);
	BOOL DeleteRemoteFile(const char *pszRemoteFileName);
	BOOL ExecCommand(const char *pstrFormat, ...);
	BOOL ReadStr();
	BOOL WriteStr(const char *pszString);

private:
	CTcpSocket m_CtrlSocket;
	int m_fc;
	HANDLE m_hFile, m_hMapping;
	char *m_pData;
	unsigned long m_nTotalBytesTransfered;
	unsigned long m_nTotalBytesToSend;
	unsigned long m_nMaxSendChunkSize;

	BOOL TransferFile(const char *pszLocalFileName, const char *pszRemoteFileName, BOOL pasv, BOOL get);
	BOOL ReadStr2();
	BOOL OpenControlSocket(CString serverhost,unsigned short serverport);
	void CloseControlSocket();

protected:
	BOOL PrepareReceiveFile(const char *lpszFilename);
	BOOL PrepareSendFile(const char *lpszFilename);
	void FreeFile();
	BOOL SendNextChunk(CTcpSocket *pSocket);
	BOOL ReadNextChunk();
};


#endif // __FTP_H_INCLUDED__