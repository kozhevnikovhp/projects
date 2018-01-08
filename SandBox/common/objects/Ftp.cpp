/* File Ftp.cpp - implementation of all classes related to file transfer protocol

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	20 Feb 2003 - initial creation
	27 Feb 2003 - listen and connection threads added
	10 Mar 2003 - almost all commands implemented (except PASV and REST)
	25 Apr 2003 - PASV command implemented
	17 Aug 2004 - bugs in file receiving fixed
	09 Sep 2004 - CFtpClient class added
*/

#include "stdafx.h"
#include "Thread.h"
#include "Ftp.h"
#include "mmsystem.h"

const char *pszBackSlash = "\\";

///////////////////////////////////////////////////////////////////////////////////////////
// CFtpListenSocket class implementation

CFtpListenSocket::CFtpListenSocket()
{
	m_pServer = NULL;
}

//virtual
CFtpListenSocket::~CFtpListenSocket()
{
}

//virtual
CTcpSocket *CFtpListenSocket::CreateAcceptSocket()
{
	return m_pServer->CreateConnectSocket();
}

//virtual
void CFtpListenSocket::OnThreadStarted()
{
	TRACE("CFtpListenSocket : thread procedure started\n");
	ASSERT(m_pServer);
}

//virtual
LOGICAL CFtpListenSocket::ThreadBody()
{
	CFtpConnectSocket *pNewConnectionSocket = (CFtpConnectSocket *)Accept();
	if (pNewConnectionSocket)
	{ // start new connection thread and continue accepting incoming calls
		m_pServer->IncomingCallAccepted(pNewConnectionSocket);
	}
	return LOGICAL_TRUE;
} 

//virtual
void CFtpListenSocket::OnThreadEnded()
{
	TRACE("CFtpListenSocket : thread procedure ended\n");
}



///////////////////////////////////////////////////////////////////////////////////////////
// CFtpDataSocket class implementation

CFtpDataSocket::CFtpDataSocket()
{
	m_pConnectSocket = NULL;
	m_nTransferType = -1;
	m_pServer = NULL;
	m_bInitialized = FALSE;
	m_nStatus = XFERMODE_IDLE;
	m_pData = NULL;
	m_hMapping = INVALID_HANDLE_VALUE;
	m_hFile = INVALID_HANDLE_VALUE;
}

//virtual
CFtpDataSocket::~CFtpDataSocket()
{
}

//virtual
CTcpSocket *CFtpDataSocket::CreateAcceptSocket()
{
	// do not make another socket, but close existing and attach accepted
	Close();
	return this;
}

BOOL CFtpDataSocket::PrepareSendFile(const char *lpszFilename)
{
	FreeFile();
	m_nMaxSendChunkSize = 128;
	m_nStatus = XFERMODE_SEND;
	m_hFile = CreateFile(lpszFilename, GENERIC_READ,
							FILE_SHARE_READ, // another process or thread can open the same file for reading
							0, // default security descriptor
							OPEN_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL // no template
							);

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		BY_HANDLE_FILE_INFORMATION FileInfo;
		GetFileInformationByHandle(m_hFile, &FileInfo);

		m_hMapping = CreateFileMapping(m_hFile, 
							NULL, 
							PAGE_READONLY, 
							0, // less then 2Gb in size
							0,	// up to all size 
							NULL 
							);
		m_pData = (char *)MapViewOfFile(m_hMapping,
			FILE_MAP_READ,
			0,					// filepos high word
			0,
			0					// the entire file 
			);

		m_nTotalBytesSend = FileInfo.nFileSizeLow;
		m_nTotalBytesTransfered = 0;
		if (m_dwRestartOffset < m_nTotalBytesSend)
			m_nTotalBytesTransfered = m_dwRestartOffset;
		m_pConnectSocket->SendReply("150 Opening BINARY mode data connection for file transfer (%d bytes).", m_nTotalBytesSend);
	}
	else
	{
		m_pConnectSocket->ReportFileNotFound();
		return FALSE; // file not opened
	}


	return TRUE;
}

void CFtpDataSocket::FreeFile()
{
	if (m_pData)
		UnmapViewOfFile(m_pData);
	m_pData = NULL;

	if (m_hMapping != INVALID_HANDLE_VALUE)
		CloseHandle(m_hMapping);
	m_hMapping = INVALID_HANDLE_VALUE;

	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
}

/********************************************************************/
/*																	*/
/* Function name : CDataSocket::PrepareReceiveFile					*/
/* Description   : Prepare socket to receive a file.				*/
/*																	*/
/********************************************************************/
BOOL CFtpDataSocket::PrepareReceiveFile(const char *lpszFilename)
{
	m_nTotalBytesReceive = 0;
	m_nTotalBytesTransfered = 0;

	if (m_dwRestartOffset)
	{
//		m_File.SetLength(m_dwRestartOffset);
//		m_File.SeekToEnd();
	}
	m_hFile = CreateFile(lpszFilename, GENERIC_WRITE,
							0, // nobody can write to and read from this file
							0, // default security descriptor
							CREATE_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL // no template
							);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		// debug
		MessageBox(NULL, lpszFilename, "FtpServer:PrepareReceiveFile - cannot create file for writing!", MB_ICONEXCLAMATION);
		return FALSE;
	}
	m_pConnectSocket->SendReply("150 Opening BINARY mode data connection for file transfer.");
	m_nStatus = XFERMODE_RECEIVE;
	m_bTransferComplete = FALSE;

	return TRUE;
}

int CFtpDataSocket::SendBuffer(char *pBuffer, unsigned int nBytesToSend)
{
	unsigned int nSentBytes = 0;
	BOOL bSuccess = Write(pBuffer, nBytesToSend, nSentBytes);
	if (!bSuccess)
	{
		if (GetLastErrorCode() != WSAEWOULDBLOCK)
			return nSentBytes;
	}
	return nSentBytes;
}

int CFtpDataSocket::SendFile(const char *pszFileName)
{
	if (!PrepareSendFile(pszFileName))
		return FALSE;
	return Run();
}

int CFtpDataSocket::ReceiveFile()
{
	return Run();
}

//virtual
void CFtpDataSocket::OnThreadStarted()
{
	TRACE("CFtpDataSocket : thread procedure started\n");
	ASSERT(m_pServer);
}

//virtual
LOGICAL CFtpDataSocket::ThreadBody()
{
	if (IsTaskCompleted())
	{
		TaskCompleted();
		return FALSE; // task completed
	}
	
	if (m_nStatus == XFERMODE_SEND)
	{
		if (!SendNextChunk())
			return LOGICAL_FALSE;
	}
	else if (m_nStatus == XFERMODE_RECEIVE)
	{
		if (!ReadNextChunk())
		{
			TaskCompleted();
			return LOGICAL_FALSE;
		}
	}
	portableSwitchContext();
	return LOGICAL_TRUE;
} 

//virtual
void CFtpDataSocket::OnThreadEnded()
{
	m_pConnectSocket->DestroyDataConnection();
	TRACE("CFtpDataSocket : thread procedure ended\n");
}

BOOL CFtpDataSocket::SendNextChunk()
{
	unsigned long nThisChunkSize = m_nTotalBytesSend - m_nTotalBytesTransfered;
	if (m_nMaxSendChunkSize < 1)
		m_nMaxSendChunkSize = 1;
	if (nThisChunkSize > m_nMaxSendChunkSize)
		nThisChunkSize = m_nMaxSendChunkSize;
	unsigned long StartTime = timeGetTime();
	int nSentBytes = SendBuffer(m_pData+m_nTotalBytesTransfered, nThisChunkSize);
	unsigned long DeltaTime = timeGetTime()-StartTime;
	m_nTotalBytesTransfered += nSentBytes;
	TRACE("%d bytes sent (%d totally), deltatime = %d\n", nSentBytes, m_nTotalBytesTransfered, DeltaTime);
	printf("%d bytes sent (%d totally), deltatime = %d\n", nSentBytes, m_nTotalBytesTransfered, DeltaTime);
	if (nSentBytes == SOCKET_ERROR)
		return FALSE;
	Sleep(0);

	// update m_nMaxSendChunkSize
	if (DeltaTime < 50)
	{
		m_nMaxSendChunkSize = (int)(m_nMaxSendChunkSize * 1.05);
		if (m_nMaxSendChunkSize < 20)
			++m_nMaxSendChunkSize;
	}
	else if (DeltaTime < 200)
	{ // 50 < Delay < 200
		Sleep(DeltaTime*3/4);
	}
	else
	{ // Delay > 500 -> reduce speed
		m_nMaxSendChunkSize >>= 1;
		Sleep(DeltaTime*3/4);
	}
	return TRUE;
}

BOOL CFtpDataSocket::ReadNextChunk()
{
	m_bTransferComplete = FALSE;
	unsigned int nReceivedBytes = 1;
	unsigned long nWrittenBytes = 0;
	BOOL bSuccess = WaitForData(m_szReceivingBuffer, sizeof(m_szReceivingBuffer), nReceivedBytes);
	if (bSuccess)
	{
		m_nTotalBytesTransfered += nReceivedBytes;
		TRACE("%d bytes read (%d totally)\n", nReceivedBytes, m_nTotalBytesTransfered);
		WriteFile(m_hFile, m_szReceivingBuffer, nReceivedBytes, &nWrittenBytes, NULL);
		return TRUE;
	}
	else
	{
		m_bTransferComplete = TRUE;
		return FALSE;
	}
}

BOOL CFtpDataSocket::IsTaskCompleted()
{
	if (m_nStatus == XFERMODE_IDLE)
		return TRUE;
	else if (m_nStatus == XFERMODE_SEND)
		return (m_nTotalBytesSend == m_nTotalBytesTransfered);
	else return m_bTransferComplete;
}

void CFtpDataSocket::TaskCompleted()
{
	FreeFile();
	m_pConnectSocket->SendReply("226 Transfer complete");
	m_nStatus = XFERMODE_IDLE;
}

///////////////////////////////////////////////////////////////////////////////////////////
// CFtpConnectSocket class implementation

static SFtpCommand commandList[] = 
{
	{TOK_ABOR,	"ABOR", FALSE,	"Abort transfer: ABOR"}, 
	{TOK_BYE,	"BYE",  FALSE,	"Logout or break the connection: BYE"},
	{TOK_CDUP,	"CDUP", FALSE,	"Change to parent directory: CDUP"},
	{TOK_CWD,	"CWD",	TRUE,	"Change working directory: CWD [directory-name]"},
	{TOK_DELE,	"DELE", TRUE ,	"Delete file: DELE file-name"},
	{TOK_DIR,	"DIR",  FALSE,	"Get directory listing: DIR [path-name]"},
	{TOK_NLST,	"NLST",  FALSE,	"Get reduced directory listing (filenames only): NLST [path-name]"},
	{TOK_HELP,	"HELP",  FALSE, "Show help: HELP [command]"},
	{TOK_LIST,	"LIST", FALSE,	"Get directory listing: LIST [path-name]"}, 
	{TOK_MKD,	"XMKD",	TRUE,	"Make directory: MKD path-name"},
	{TOK_NOOP,	"NOOP", FALSE,	"Do nothing: NOOP"},
	{TOK_PASS,	"PASS", FALSE,	"Supply a user password: PASS password"},
	{TOK_PASV,	"PASV", FALSE,	"Set server in passive mode: PASV"},
	{TOK_PORT,	"PORT", TRUE,	"Specify the client port number: PORT a0,a1,a2,a3,a4,a5"},
	{TOK_PWD,	"PWD",	FALSE,	"Get current directory: PWD"},
	{TOK_XPWD,	"XPWD",	FALSE,	"Get current directory: XPWD"},
	{TOK_QUIT,	"QUIT",  FALSE,	"Logout or break the connection: QUIT"},
	{TOK_REST,	"REST", TRUE,	"Set restart transfer marker: REST marker"},
	{TOK_RETR,	"RETR", TRUE,	"Get file: RETR file-name"},
	{TOK_RMD,	"XRMD",	TRUE,	"Remove directory: RMD path-name"},
	{TOK_RNFR,	"RNFR", TRUE,	"Specify old path name of file to be renamed: RNFR file-name"},
	{TOK_RNTO,	"RNTO", TRUE,	"Specify new path name of file to be renamed: RNTO file-name"},
	{TOK_SIZE,	"SIZE", TRUE,	"Get filesize: SIZE file-name"},
	{TOK_STOR,	"STOR", TRUE,	"Store file: STOR file-name"},
	{TOK_SYST,	"SYST", FALSE,	"Get operating system type: SYST"},
	{TOK_TYPE,	"TYPE", TRUE,	"Set filetype: TYPE [A | I]"},
	{TOK_USER,	"USER", TRUE,	"Supply a username: USER username"},
	{TOK_ERROR,	"",		FALSE,  ""},
};

CFtpConnectSocket::CFtpConnectSocket()
{
	m_pServer = NULL;
	m_bPassiveMode = FALSE;
	m_bLoggedOn = FALSE;
	m_dwRestartOffset = 0;
	m_nDataRemotePort = -1;
	m_HomeDirNameLength = 0;
	m_pszCurrentDirRelative = m_szCurrentDir;
	m_szRenamePath[0] = 0;
}

//virtual
CFtpConnectSocket::~CFtpConnectSocket()
{
}

void CFtpConnectSocket::Stop()
{
	DestroyDataConnection();
	Close();
}

//virtual
void CFtpConnectSocket::OnThreadStarted()
{
	TRACE("CFtpConnectSocket : thread procedure started\n");
}

//virtual
LOGICAL CFtpConnectSocket::ThreadBody()
{
	if (!ReceiveCommand())
		return LOGICAL_FALSE; // break thread execution
	ParseCommand();
	return LOGICAL_TRUE;
} 

//virtual
void CFtpConnectSocket::OnThreadEnded()
{
	TRACE("CFtpConnectSocket : thread procedure ended\n");
}

BOOL CFtpConnectSocket::CreateDataConnection()
{
	if (!m_bPassiveMode)
	{
		if (m_DataSocket.Open())
		{
			m_DataSocket.SetRestartOffset(m_dwRestartOffset);

			// connect to remote site
			if (!m_DataSocket.Connect(m_DataRemoteHostAddress, m_nDataRemotePort))
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					SendReply("425 Can't open data connection.");
					DestroyDataConnection();
					return FALSE;
				}
			}
		}
		else
		{
			SendReply("421 Failed to create data connection socket.");
			return FALSE;
		}
	}
	else
	{
		m_DataSocket.SetRestartOffset(m_dwRestartOffset);
//		m_DataSocket.SetTransferType(nTransferType, TRUE);
	}
	return TRUE;
}

void CFtpConnectSocket::DestroyDataConnection()
{
	if (!m_DataSocket.IsOpened())
		return;

	m_DataSocket.Close();
	// reset transfer status
	m_DataRemoteHostAddress = 0;
	m_nDataRemotePort = -1;
	m_dwRestartOffset = 0;
	m_bPassiveMode = FALSE;
}

BOOL CFtpConnectSocket::ReceiveCommand()
{
	char *pszCurrentByte = m_szInputBuffer;
	unsigned int nReadBytes;
	while (1)
	{
		if (!Read(pszCurrentByte, 1, nReadBytes))
			break;
		if (*pszCurrentByte == 0x0A)
		{
			*pszCurrentByte = 0x00;
			return TRUE;
		}
		++pszCurrentByte;
	}
	return FALSE;
}

BOOL CFtpConnectSocket::ParseLine()
{
	m_pszCommand = m_szInputBuffer;
	m_pszArguments = "";

	char *pszBlank = strchr(m_pszCommand, ' ');
	if (!pszBlank)
	{
		pszBlank = m_pszCommand;
		while (isalpha(*pszBlank))
		{
			++pszBlank;
			if (*pszBlank == 0)
				break;
		}
		*pszBlank = 0;
		TRACE("%s\n", m_pszCommand);
		return TRUE; // no arguments
	}

	*pszBlank = 0; // terminate command string;
	m_pszArguments = pszBlank+1;

	// cut ending CR of/and LF and replace all '/' with '\' symbols	
	int i = 0;
	while (m_pszArguments[i] != 0)
	{
		if (i != 0)
		{
			if (m_pszArguments[i] == '/')
				m_pszArguments[i] = '\\';
			if (m_pszArguments[i] == 0x0D || m_pszArguments[i] == 0x0A)
			{
				m_pszArguments[i] = 0;
				break;
			}
		}
		++i;
	}
	TRACE("%s %s\n", m_pszCommand, m_pszArguments);
	return TRUE;
}

BOOL CFtpConnectSocket::ParseCommand()
{
	if (!ParseLine())
		return FALSE;

	if (strlen(m_pszCommand) == 0)
		return TRUE;

	// find command in command list
	for (m_nCommand = TOK_ABOR; m_nCommand < TOK_ERROR; m_nCommand++)
	{
		// found command ?
		if (!strcmp(m_pszCommand, commandList[m_nCommand].m_pszName))
		{
			// did we expect an argument ?
			if (commandList[m_nCommand].m_bHasArguments && (*m_pszArguments == 0))
			{
				SendReply("501 Syntax error: Invalid number of parameters.");
				return TRUE;
			}
			break;			
		}
	}

	if (m_nCommand == TOK_ERROR)
	{
		// command is not in our list
		SendReply("501 Syntax error: Command not understood.");
		return TRUE;
	}
	
	// no commands are accepted before successfull logged on
	if ((m_nCommand != TOK_USER && m_nCommand != TOK_PASS) && !m_bLoggedOn)
	{
		SendReply("530 Please login with USER and PASS.");
		return TRUE;
	}

	// process command
	switch (m_nCommand)
	{
		// specify username
		case TOK_USER:
			ProcessUserCmd(); break;

		// specify password
		case TOK_PASS:
			ProcessPassCmd(); break;
		
		// change transfer type
		case TOK_TYPE:
			ProcessTypeCmd(); break;

		// print current directory
		case TOK_PWD:
		case TOK_XPWD:
			ProcessPwdCmd(); break;

		// change to parent directory
		case TOK_CDUP:
			m_pszArguments = "..";
		// change working directory
		case TOK_CWD:
			ProcessCwdCmd(); break; 

		// specify IP and port (PORT a1,a2,a3,a4,p1,p2) -> IP address a1.a2.a3.a4, port p1*256+p2. 
		case TOK_PORT:
			ProcessPortCmd(); break;
		
		// switch to passive mode
		case TOK_PASV:
			ProcessPasvCmd(); break;

		// list current directory (or a specified file/directory)
		case TOK_LIST:
		case TOK_NLST:
		case TOK_DIR:
			ProcessDirCmd(); break;
		
		// retrieve file
		case TOK_RETR:
			ProcessRetrCmd(); break;

		// client wants to upload file
		case TOK_STOR:
			ProcessStorCmd(); break;
		
		// get file size
		case TOK_SIZE:
			ProcessSizeCmd(); break;
		
		// delete file
		case TOK_DELE:
			ProcessDeleCmd(); break;
		
		// remove directory
		case TOK_RMD:
			ProcessRmdCmd(); break;	
		
		// create directory
		case TOK_MKD:
			ProcessMkdCmd(); break;
		
		// rename file or directory (part 1)
		case TOK_RNFR:
			ProcessRnfrCmd(); break;

		// rename file or directory (part 2)
		case TOK_RNTO:
			ProcessRntoCmd(); break;

		// abort transfer
		case TOK_ABOR:
			ProcessAborCmd(); break;

		// get system info
		case TOK_SYST:
			SendReply("215 Windows_NT platform");
			break;
		
		// close connection
		case TOK_QUIT:
		case TOK_BYE:
			ProcessQuitCmd(); break;

		// restart transfer
		case TOK_REST:
			ProcessRestCmd(); break;

		// display help
		case TOK_HELP:
			ProcessHelpCmd(); break;

		// dummy instruction
		case TOK_NOOP:
			SendReply("200 OK"); break;

		default:
			SendReply("502 Command not implemented - Try HELP.");
			break;
	}
	return TRUE;
}

void CFtpConnectSocket::ProcessUserCmd()
{
	m_bLoggedOn = FALSE;
	strcpy(m_szUserName, m_pszArguments);

	int peer_addr_len = sizeof(m_Peer);
	if (getpeername(GetSocket(), &m_Peer, &peer_addr_len) == SOCKET_ERROR)
	{
		m_dwLastErrorCode = WSAGetLastError();
		return;
	} 
	struct sockaddr_in *pPeerAddrIn = (struct sockaddr_in *)&m_Peer;

	// tell FTP server a new user has connected
	m_pServer->OnNewUserIsLoggingOn(m_szUserName, pPeerAddrIn);

	SendReply("331 User name ok, need password.");
}

void CFtpConnectSocket::ProcessPassCmd()
{
	// already logged on ?
	if (m_bLoggedOn)
		SendReply("503 Login with USER first.");
	else
	{
		if (m_pServer->CanUserLogOn(m_szUserName, m_pszArguments, (struct sockaddr_in *)&m_Peer))
		{
			m_bLoggedOn = TRUE;
			SendReply("230 User successfully logged in.");
			// tell FTP server a new user has logged on
			struct sockaddr_in *pPeerAddrIn = (struct sockaddr_in *)&m_Peer;
			m_pServer->OnNewUserIsLoggedOn(m_szUserName, pPeerAddrIn);
			strcpy(m_szCurrentDir, m_pServer->GetUserHomeDir(m_szUserName));
			m_HomeDirNameLength = strlen(m_szCurrentDir);
			m_pszCurrentDirRelative = m_szCurrentDir+m_HomeDirNameLength;
		}
		else
			SendReply("530 Not logged in, user or password incorrect!");
	}
}

void CFtpConnectSocket::ProcessTypeCmd()
{
	// let's pretend we did something...
	SendReply("200 Type set to %s", m_pszArguments);
}

void CFtpConnectSocket::ProcessPwdCmd()
{
	SendReply("257 \"%s\" is current directory.", m_pszCurrentDirRelative);
}

BOOL FileExists(const char *lpszFileName, BOOL bIsDirCheck)
{
	// A quick'n'easy way to see if a file exists.
	DWORD dwAttributes = GetFileAttributes(lpszFileName);
    if (dwAttributes == 0xFFFFFFFF)
        return FALSE;

	if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		if (bIsDirCheck)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if (!bIsDirCheck)
			return TRUE;
		else
			return FALSE;
	}
}

int CFtpConnectSocket::ChangeDirectory(char *pszDesiredDir)
{
	if (!strcmp(pszDesiredDir, "."))
	{
		strcpy(m_szCurrentDir, m_pServer->GetUserHomeDir(m_szUserName));
		return 0;
	}
	else if (!strcmp(pszDesiredDir, ".."))
	{
		if (!strcmp(m_szCurrentDir, m_pServer->GetUserHomeDir(m_szUserName)))
			return 1; // no rights to go up of home dir
		char *pszLastSlash = strrchr(m_szCurrentDir, '\\');
		*pszLastSlash = 0; // terminate string here
		return 0;
	}
	strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
	strcat(m_szFreeUsedBuffer, pszBackSlash);
	strcat(m_szFreeUsedBuffer, pszDesiredDir);
	if (FileExists(m_szFreeUsedBuffer, TRUE))
	{
		strcpy(m_szCurrentDir, m_szFreeUsedBuffer);
		return 0; // OK
	}
	else
		return 2; // dir not found
}

void CFtpConnectSocket::ProcessCwdCmd()
{
	// try to change to specified directory
	char *pszDesiredDir = m_pszArguments;

	int nResult = ChangeDirectory(pszDesiredDir);
	switch (nResult)
	{
		case 0:
			m_pszCurrentDirRelative = m_szCurrentDir+m_HomeDirNameLength;
			if (*m_pszCurrentDirRelative == '\\' || *m_pszCurrentDirRelative == '/')
				m_pszCurrentDirRelative++;
			SendReply("250 CWD command successful. \"%s\" is current directory.", m_pszCurrentDirRelative);
			break;
		case 1:
			SendReply("550 CWD command failed. \"%s\": Permission denied.", pszDesiredDir);
			break;
		default:
			SendReply("550 CWD command failed. \"%s\": Directory not found.", pszDesiredDir);
			break;
	}
}

void CFtpConnectSocket::ProcessPortCmd()
{
	// replace first 3 commas with dots and 4th with terminating zero
	char *pszComma = m_pszArguments;
	for (int i = 1; i<=4; i++)
	{
		pszComma = strchr(pszComma, ',');
		if (!pszComma)
		{
			SendReply("550 Invalid port command.");
			return;
		}
		if (i == 4)
			*pszComma = 0; // terminating zero
		else
			*pszComma = '.';
		++pszComma;
	}
	m_DataRemoteHostAddress = inet_addr(m_pszArguments);

	// remote port
	m_nDataRemotePort = 256*atoi(pszComma);
	pszComma = strchr(pszComma, ',');
	if (!pszComma)
	{
		SendReply("550 Invalid port command.");
		return;
	}
	m_nDataRemotePort += atoi(pszComma+1);
	m_bPassiveMode = FALSE;
	SendReply("200 Port command successful.");
}

void CFtpConnectSocket::ProcessPasvCmd()
{
	// delete existing datasocket
	DestroyDataConnection();

	if (!m_DataSocket.Open())
	{
		DestroyDataConnection();	
		SendReply("421 Failed to create socket.");
		return;
	}
	// start listening
	m_DataSocket.Listen();
	char pszAddress[32];
	//Get port number
	m_DataSocket.GetSockName(pszAddress, m_nDataRemotePort);
	// get IP-address
	unsigned short nPort;
	GetSockName(pszAddress, nPort);
	// replace dots in IP-address
	char *pszDot = pszAddress;
	while (pszDot)
	{
		pszDot = strchr(pszAddress, '.');
		if (pszDot)
			*pszDot = ',';
	}

	SendReply("227 Entering Passive Mode (%s,%d,%d).", pszAddress, m_nDataRemotePort/256, m_nDataRemotePort%256);
	m_bPassiveMode = TRUE;
	m_DataSocket.Accept();
}
		
BOOL CFtpConnectSocket::SendDirectoryList(char *pszFileNameTemplate)
{
	WIN32_FIND_DATA fd;
	HANDLE hFile;
	BOOL bFound, bSuccess = TRUE;
	SYSTEMTIME time;
	char szFoo[32];
	char *MonthNames[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	// it's a file
	hFile = FindFirstFile(pszFileNameTemplate, &fd);
	bFound = (hFile!=INVALID_HANDLE_VALUE);

	while (bFound && bSuccess)
	{
		m_pszBuffer[0] = 0;
		
		// skip "." and ".." 
		if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
		{
			if (m_nCommand == TOK_DIR || m_nCommand == TOK_LIST)
			{ // full set of file properties (size, data, access rights etc) in contrast with NLST command (filename only)
				// file/directory
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					strcat(m_pszBuffer, "d");
				else
					strcat(m_pszBuffer, "-");

				// file attributes
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					strcat(m_pszBuffer, "r-");
				else
					strcat(m_pszBuffer, "rw");
				strcat(m_pszBuffer, "-rw-r--");

				// groups
				strcat(m_pszBuffer, " 1 root root ");

				// file size
				sprintf(szFoo, " %8d", fd.nFileSizeLow);
				strcat(m_pszBuffer, szFoo);
				
				// file date
				FileTimeToSystemTime(&fd.ftCreationTime, &time);
				//sprintf(szFoo, " %02d.%02d.%04d %02d:%02d:%02d ", time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond);
				sprintf(szFoo, " %s %02d %04d %02d:%02d ", MonthNames[time.wMonth-1], time.wDay, time.wYear, time.wHour, time.wMinute);
				strcat(m_pszBuffer, szFoo);
			}
			// file name
			strcat(m_pszBuffer, fd.cFileName);
			// end of line
			strcat(m_pszBuffer, "\r\n");
			int nBytesToSend = strlen(m_pszBuffer);
			bSuccess &= (m_DataSocket.SendBuffer(m_pszBuffer, nBytesToSend) == nBytesToSend);
		}
		bFound = FindNextFile(hFile, &fd);
	}
	FindClose(hFile);
	return bSuccess;
}

void CFtpConnectSocket::ProcessDirCmd()
{
	// if not PASV mode, we need a PORT comand first
	if (!m_bPassiveMode && (m_DataRemoteHostAddress == 0 || m_nDataRemotePort == -1))
	{
		ReportBadCommandSequence();
	}
	else
	{
		if (m_pServer->CanUserListDir(m_szUserName, m_szCurrentDir, m_pszArguments))
		{
			strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
			if (strlen(m_pszArguments) != 0)
			{
				strcat(m_szFreeUsedBuffer, pszBackSlash);
				strcat(m_szFreeUsedBuffer, m_pszArguments);
			}
			if (strchr(m_pszArguments, '*') == NULL && strchr(m_pszArguments, '*') == NULL)
			{ /* no wildcards */
				strcat(m_szFreeUsedBuffer, "\\*.*");
			}

			// create socket connection to transfer directory listing
			if (CreateDataConnection())
			{
				SendReply("150 Opening ASCII mode data connection for directory list."); 
				if (SendDirectoryList(m_szFreeUsedBuffer))
					SendReply("226 Transfer complete.");
				else
					SendReply("426 Connection closed; transfer aborted.");
			}
			DestroyDataConnection();
		}
		else
			ReportPermissionDenied();
	}
}

void CFtpConnectSocket::ProcessRetrCmd()
{
	// if not PASV mode, we need a PORT comand first
	if (!m_bPassiveMode && (m_DataRemoteHostAddress == 0 || m_nDataRemotePort == -1))
	{
		ReportBadCommandSequence();
		return;
	}
	
	if (m_pServer->CanUserDownloadFile(m_szUserName, m_szCurrentDir))
	{
		strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
		strcat(m_szFreeUsedBuffer, pszBackSlash);
		strcat(m_szFreeUsedBuffer, m_pszArguments);
		if (FileExists(m_szFreeUsedBuffer, FALSE))
		{
			// create socket connection for file transfer
			if (CreateDataConnection())
			{
				if (!m_DataSocket.SendFile(m_szFreeUsedBuffer))
					SendReply("550 Cannot open file.");
			}
		}
		else
			ReportFileNotFound();
	}
	else
		ReportPermissionDenied();
}

void CFtpConnectSocket::ProcessStorCmd()
{
	// if not PASV mode, we need a PORT comand first
	if (!m_bPassiveMode && (m_DataRemoteHostAddress == 0 || m_nDataRemotePort == -1))
	{
		ReportBadCommandSequence();
		return;
	}
	
	if (m_pServer->CanUserUploadFile(m_szUserName, m_szCurrentDir))
	{
		strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
		strcat(m_szFreeUsedBuffer, pszBackSlash);
		strcat(m_szFreeUsedBuffer, m_pszArguments);
		if (m_DataSocket.PrepareReceiveFile(m_szFreeUsedBuffer))
		{
			// create socket connection for file transfer
			if (CreateDataConnection())
			{
				BOOL bRun = m_DataSocket.ReceiveFile();
			}
		}
		else
			SendReply("550 Cannot create file.");
	}
	else
		ReportPermissionDenied();
}

void CFtpConnectSocket::ProcessSizeCmd()
{
	if (m_pServer->CanUserGetFileSize(m_szUserName, m_szCurrentDir))
	{
		strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
		strcat(m_szFreeUsedBuffer, pszBackSlash);
		strcat(m_szFreeUsedBuffer, m_pszArguments);
		if (FileExists(m_szFreeUsedBuffer, FALSE))
		{
			HANDLE hFile = CreateFile(m_szFreeUsedBuffer, 0,
							FILE_SHARE_READ, // another process or thread can open the same file for reading
							0, // default security descriptor
							OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL // no template
							);

			if (hFile != INVALID_HANDLE_VALUE)
			{
				BY_HANDLE_FILE_INFORMATION FileInfo;
				GetFileInformationByHandle(hFile, &FileInfo);
				SendReply("213 %d", FileInfo.nFileSizeLow);
			}
			else
				ReportFileNotFound();
			CloseHandle(hFile);
		}
		else
			ReportFileNotFound();
	}
	else
		ReportPermissionDenied();
}

void CFtpConnectSocket::ProcessDeleCmd()
{
	if (m_pServer->CanUserDeleteFile(m_szUserName, m_szCurrentDir))
	{
		strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
		strcat(m_szFreeUsedBuffer, pszBackSlash);
		strcat(m_szFreeUsedBuffer, m_pszArguments);
		if (FileExists(m_szFreeUsedBuffer, FALSE))
		{
			if (!DeleteFile(m_szFreeUsedBuffer))
				SendReply("450 Internal error deleting the file: \"%s\".",  m_pszArguments);
			else
				SendReply("250 File \"%s\" was deleted successfully.", m_pszArguments);
		}
		else
			ReportFileNotFound();
	}
	else
		ReportPermissionDenied();
}

void CFtpConnectSocket::ProcessRmdCmd()
{
	if (m_pServer->CanUserDeleteDir(m_szUserName, m_szCurrentDir))
	{
		strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
		strcat(m_szFreeUsedBuffer, pszBackSlash);
		strcat(m_szFreeUsedBuffer, m_pszArguments);
		if (FileExists(m_szFreeUsedBuffer, TRUE))
		{
			if (!RemoveDirectory(m_szFreeUsedBuffer))
			{
				if (GetLastError() == ERROR_DIR_NOT_EMPTY)
					SendReply("550 Directory not empty.");
				else
					SendReply("450 Internal error deleting the directory: \"%s\".",  m_pszArguments);
			}
			else
				SendReply("250 Directory \"%s\" was deleted successfully.", m_pszArguments);
		}
		else
			SendReply("550 Directory not found.");
	}
	else
		ReportPermissionDenied();
}

BOOL CFtpConnectSocket::CreateRelativePath(const char *pszDirPath)
{
	strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
	strcat(m_szFreeUsedBuffer, pszBackSlash);
	char *pszLastSlash = strrchr(m_szFreeUsedBuffer, '\\');
	strcat(m_szFreeUsedBuffer, pszDirPath);

	while (pszLastSlash)
	{
		pszLastSlash = strchr(pszLastSlash+1, '\\');
		if (pszLastSlash)
			*pszLastSlash = 0;
		CreateDirectory(m_szFreeUsedBuffer, NULL);
		if (pszLastSlash)
			*pszLastSlash = '\\';
	}
	return TRUE;
}

void CFtpConnectSocket::ProcessMkdCmd()
{
	if (m_pServer->CanUserMakeDir(m_szUserName, m_szCurrentDir))
	{
		if (!CreateRelativePath(m_pszArguments))
			SendReply("450 Internal error creating the directory %s.", m_pszArguments);
		else
			SendReply("250 Directory %s created successfully.", m_pszArguments);
	}
	else
		ReportPermissionDenied();
}

void CFtpConnectSocket::ProcessRnfrCmd()
{
	if (m_pServer->CanUserRenameFile(m_szUserName, m_szCurrentDir))
	{
		strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
		strcat(m_szFreeUsedBuffer, pszBackSlash);
		strcat(m_szFreeUsedBuffer, m_pszArguments);
		if (FileExists(m_szFreeUsedBuffer, TRUE) || FileExists(m_szFreeUsedBuffer, FALSE))
		{
			strcpy(m_szRenamePath, m_szFreeUsedBuffer);
			SendReply("350 File/directory exists, ready for destination name.");
		}
		else
			SendReply("550 File/directory not found.");
	}
	else
		ReportPermissionDenied();
}

void CFtpConnectSocket::ProcessRntoCmd()
{
	if (strlen(m_szRenamePath) == 0)
	{
		ReportBadCommandSequence();
		return;
	}
	strcpy(m_szFreeUsedBuffer, m_szCurrentDir);
	strcat(m_szFreeUsedBuffer, pszBackSlash);
	strcat(m_szFreeUsedBuffer, m_pszArguments);
	// check destination filename
	if (FileExists(m_szFreeUsedBuffer, TRUE) || FileExists(m_szFreeUsedBuffer, FALSE))
	{
		SendReply("550 File with this name already exists.");
		return;
	}
	// rename file
	if (!MoveFile(m_szRenamePath, m_szFreeUsedBuffer))
		SendReply("450 Internal error renaming the file/directory");
	else
		SendReply("250 File/directory renamed successfully.");
	m_szRenamePath[0] = 0;
}

void CFtpConnectSocket::ProcessAborCmd()
{
	// destroy data connection
	DestroyDataConnection();
	SendReply("426 Data connection closed.");
	SendReply("226 ABOR command successful.");
}

void CFtpConnectSocket::ProcessQuitCmd()
{
	m_pServer->OnNewUserIsLoggedOff(m_szUserName, (struct sockaddr_in *)&m_Peer);
	m_pServer->CloseSession(this);
}

void CFtpConnectSocket::ProcessRestCmd()
{
	SendReply("555 Attention!!! Command %s not implemented - ask me please.", m_pszCommand);
/*	if (!IsNumeric(strArguments.GetBuffer(strArguments.GetLength())))
	{
		strArguments.ReleaseBuffer();
		SendResponse("501 Invalid parameter.");
	}
	else
	{
		strArguments.ReleaseBuffer();
		m_dwRestartOffset = atol(strArguments);
		SendResponse("350 Restarting at %d.", m_dwRestartOffset);
	} */
}

void CFtpConnectSocket::ProcessHelpCmd()
{
	// if client did not specify a command, display all available commands
	if (!strcmp(m_pszArguments, ""))
	{
		SendReply("214 The following commands are recognized:\r\n");
		// find command in command list
		for (int i = TOK_ABOR; i < TOK_ERROR; i++)
			SendReply("%s\t%s", commandList[i].m_pszName, commandList[i].m_pszDescription);
		SendReply("214 HELP command successful.");
	}
	else
	{
		int nHelpCmd;
		// find command in command list
		for (nHelpCmd = TOK_ABOR; nHelpCmd < TOK_ERROR; nHelpCmd++)
		{
			// found command ?
			if (!strcmp(m_pszArguments, commandList[nHelpCmd].m_pszName))
				break;			
		}
		if (nHelpCmd != TOK_ERROR)
			SendReply("214 %s", commandList[nHelpCmd].m_pszDescription);
		else
			SendReply("501 Unknown command %s", m_pszArguments);
	}
}

BOOL CFtpConnectSocket::SendReply(const char *pstrFormat, ...)
{
	char szString[256];
	// format arguments and put them in szString
	va_list args;
	va_start(args, pstrFormat);
	vsprintf(szString, pstrFormat, args);
	if (!WriteString(szString))
		return FALSE;
	if (!WriteString("\r\n"))
		return FALSE;
	return TRUE;
}

BOOL CFtpConnectSocket::ReportPermissionDenied()
{
	return SendReply("550 Permission denied.");
}

BOOL CFtpConnectSocket::ReportFileNotFound()
{
	return SendReply("550 File not found.");
}

BOOL CFtpConnectSocket::ReportBadCommandSequence()
{
	return SendReply("550 Bad command sequence.");
}


///////////////////////////////////////////////////////////////////////////////////////////
// CFtpServer class implementation

CFtpServer::CFtpServer()
{
	m_bRunning = FALSE;
	m_nListenPort = 21; // default value for FTP
	m_ListenSocket.SetServer(this);
	for (int i = 0; i< sizeof(m_ConnectSocket)/sizeof(m_ConnectSocket[0]); i++)
		m_ConnectSocket[i].SetServer(this);
	GetCurrentDirectory(sizeof(m_szWorkingDir), m_szWorkingDir);
}

//virtual
CFtpServer::~CFtpServer()
{
	Stop();
}

BOOL CFtpServer::Start()
{
	// create the listen socket
	if (!m_ListenSocket.Open())
	{
		TRACE("ERROR! Listen socket not opened!\n");
		return FALSE;
	}
	// start listening
	if (!m_ListenSocket.Listen(m_nListenPort))
	{
		TRACE("ERROR! Listen socket listening FAILED!\n");
		return FALSE;
	}
	m_bRunning = TRUE;
	return m_ListenSocket.Run();
}

BOOL CFtpServer::Stop()
{
	if (!m_bRunning)
		return FALSE;

	m_bRunning = FALSE;	
	m_ListenSocket.Close();

	// close all active connections
	for (int i = 0; i < sizeof(m_ConnectSocket)/sizeof(m_ConnectSocket[0]); i++)
		m_ConnectSocket[i].Stop();
	return TRUE;
}

void CFtpServer::IncomingCallAccepted(CFtpConnectSocket *pConnectionSocket)
{
	if (pConnectionSocket == &m_TooBusySocket)
	{
		pConnectionSocket->SendReply("421 %s", GetTooBusyMessage());
		pConnectionSocket->Disconnect();
	}
	else
	{
		pConnectionSocket->SendReply("220 %s", GetWelcomeMessage());
		pConnectionSocket->Run();
	}
}

void CFtpServer::CloseSession(CFtpConnectSocket *pConnectionSocket)
{
	pConnectionSocket->SendReply("220 %s", GetGoodbyeMessage());
}

CFtpConnectSocket *CFtpServer::CreateConnectSocket()
{
	for (int i = 0; i < sizeof(m_ConnectSocket)/sizeof(m_ConnectSocket[0]); i++)
	{
		if (!m_ConnectSocket[i].IsRunning())
			return m_ConnectSocket+i;
	}
	return &m_TooBusySocket; // Too busy server
}


// CFtpClient implementation

CFtpClient::CFtpClient()
{
	m_pData = NULL;
	m_hMapping = INVALID_HANDLE_VALUE;
	m_hFile = INVALID_HANDLE_VALUE;
}


CFtpClient::~CFtpClient()
{
	CloseControlSocket();
}


//////////////////////////////////////////////////////////////////////
// Public Functions
//////////////////////////////////////////////////////////////////////


// function to connect & log on to FTP server
BOOL CFtpClient::LogOnToServer(CString hostname,int hostport,CString username, CString password, CString acct, CString fwhost,CString fwusername, CString fwpassword,int fwport,int logontype)
{
	int port,logonpoint=0;
	const int LO=-2, ER=-1;
	CString buf,temp;
	const int NUMLOGIN=9; // currently supports 9 different login sequences
	int logonseq[NUMLOGIN][100] =
	{
		// this array stores all of the logon sequences for the various firewalls 
		// in blocks of 3 nums. 1st num is command to send, 2nd num is next point in logon sequence array
		// if 200 series response is rec'd from server as the result of the command, 3rd num is next
		// point in logon sequence if 300 series rec'd
		{0,LO,3, 1,LO,6, 2,LO,ER}, // no firewall
		{3,6,3, 4,6,ER, 5,ER,9, 0,LO,12, 1,LO,15, 2,LO,ER}, // SITE hostname
		{3,6,3, 4,6,ER, 6,LO,9, 1,LO,12, 2,LO,ER}, // USER after logon
		{7,3,3, 0,LO,6, 1,LO,9, 2,LO,ER}, //proxy OPEN
		{3,6,3, 4,6,ER, 0,LO,9, 1,LO,12, 2,LO,ER}, // Transparent
		{6,LO,3, 1,LO,6, 2,LO,ER}, // USER with no logon
		{8,6,3, 4,6,ER, 0,LO,9, 1,LO,12, 2,LO,ER}, //USER fireID@remotehost
		{9,ER,3, 1,LO,6, 2,LO,ER}, //USER remoteID@remotehost fireID
		{10,LO,3, 11,LO,6, 2,LO,ER} // USER remoteID@fireID@remotehost
	};

	if(logontype<0||logontype>=NUMLOGIN) return FALSE; // illegal connect code
	// are we connecting directly to the host (logon type 0) or via a firewall? (logon type>0)
	if(!logontype)
	{
		temp=hostname;
		port=hostport;
	}
	else
	{
		temp=fwhost;
		port=fwport;
	}
	if(hostport != 21)
		hostname.Format(hostname+":%d",hostport); // add port to hostname (only if port is not 21)
	if(!OpenControlSocket(temp,port))
		return FALSE;
	if(!ExecCommand(""))
		return FALSE; // get initial connect msg off server
	// go through appropriate logon procedure
	while(1)
	{
		switch(logonseq[logontype][logonpoint])
		{
		case 0:
			temp="USER "+username;
			break;
		case 1:
			temp="PASS "+password;
			break;
		case 2:
			temp="ACCT "+acct;
			break;
		case 3:
			temp="USER "+fwusername;
			break;
		case 4:
			temp="PASS "+fwpassword;
			break;
		case 5:
			temp="SITE "+hostname;
			break;
		case 6:
			temp="USER "+username+"@"+hostname;
			break;
		case 7:
			temp="OPEN "+hostname;
			break;
		case 8:
			temp="USER "+fwusername+"@"+hostname;
			break;
		case 9:
			temp="USER "+username+"@"+hostname+" "+fwusername;
			break;
		case 10:
			temp="USER "+username+"@"+fwusername+"@"+hostname;
			break;
		case 11:
			temp="PASS "+password+"@"+fwpassword;
			break;
		}
		// send command, get response
		if (!WriteStr(temp))
			return FALSE;
		if (!ReadStr())
			return FALSE;
		// only these responses are valid
		if (m_fc != 2 && m_fc != 3)
			return FALSE;
		logonpoint = logonseq[logontype][logonpoint+m_fc-1]; //get next command from array
		switch(logonpoint)
		{
			case ER: // ER means summat has gone wrong
				return FALSE;
			case LO: // LO means we're fully logged on
				return TRUE;
		}
	}
}


// function to log off & close connection to FTP server
void CFtpClient::LogOffServer()
{
	WriteStr("QUIT");
	CloseControlSocket();
}


// function to execute commands on the FTP server
BOOL CFtpClient::ExecCommand(const char *pstrFormat, ...)
{
	char szString[256];
	// format arguments and put them in szString
	va_list args;
	va_start(args, pstrFormat);
	vsprintf(szString, pstrFormat, args);
	if (!WriteStr(szString))
		return FALSE;
	if (!WriteStr("\r\n"))
		return FALSE;

	if ((!ReadStr()) || (m_fc!=2))
		return FALSE;

	return TRUE;
}


// function to upload/download files
BOOL CFtpClient::UploadFile(const char *pszLocalFileName, const char *pszRemoteFileName, BOOL bPasv/* = FALSE*/)
{
	return TransferFile(pszLocalFileName, pszRemoteFileName, bPasv, FALSE);
}

BOOL CFtpClient::DownloadFile(const char *pszRemoteFileName, const char *pszLocalFileName, BOOL bPasv/* = FALSE*/)
{
	return TransferFile(pszLocalFileName, pszRemoteFileName, bPasv, TRUE);
}

BOOL CFtpClient::TransferFile(const char *pszLocalFileName, const char *pszRemoteFileName, BOOL pasv, BOOL get)
{
	CTcpSocket sockSrvr;
	CTcpSocket *pDataSocket = NULL;
	char buffer[2048];
	unsigned short PortNo;
	char pszServerIpAddress[32];
	unsigned int nReadBytes;
	unsigned long nWrittenBytes;
	
	// open local file
	BOOL bLocalFileOpen;
	if (get)
		bLocalFileOpen = PrepareReceiveFile(pszLocalFileName);
	else
		bLocalFileOpen = PrepareSendFile(pszLocalFileName);
	if (!bLocalFileOpen)
		return FALSE;

	// request BINARY mode
	if (!ExecCommand("TYPE I"))
		return FALSE;
	
	 // set up a PASSIVE type file transfer
	if (pasv)
	{
		if (!ExecCommand("PASV"))
			return FALSE;
		// extract connect port number and IP from string returned by server
		char *pszOpenBracket = strchr(m_pszReply, '(');
		if (!pszOpenBracket)
			return FALSE; // invalid reply
		char szIp1[4], szIp2[4], szIp3[4], szIp4[4];
		unsigned short PortNoMSB, PortNoLSB;
		if (sscanf(pszOpenBracket+1, "%s,%s,%s,%s,%d,%d", szIp1, szIp2, szIp3, szIp4, &PortNoMSB, &PortNoLSB) != 6)
			return FALSE;
		sprintf(pszServerIpAddress, "%s,%s,%s,%s", szIp1, szIp2, szIp3, szIp4);
		PortNo = 256*PortNoMSB + PortNoLSB;
	}
	else
	{ // set up a ACTIVE type file transfer
		// get the local IP address off the control channel socket
		char pszIpAddress[32];
		if (!m_CtrlSocket.GetSockName(pszIpAddress, PortNo))
			return FALSE;;
		while (1)
		{ // replace dots with commas
			char *pszDot = strchr(pszIpAddress, '.');
			if (!pszDot)
				break;
			*pszDot = ',';
		}
		// create listen socket  & start the socket listening
		if (!sockSrvr.Open() || !sockSrvr.Listen())
			return FALSE;
		char pszIpAddress2[32];
		// get the port that MFC chose
		if(!sockSrvr.GetSockName(pszIpAddress2, PortNo))
			return FALSE;
		// convert the port number to 2 bytes + add to the local IP
		char pszPortCommand[64];
		sprintf(pszPortCommand, "PORT %s,%d,%d", pszIpAddress, PortNo/256,PortNo % 256);
		// send PORT cmd to server
		if (!ExecCommand(pszPortCommand))
			return FALSE;
	}
	// send RETR/STOR command to server
	CString str = get ? "RETR ":"STOR ";
	str += pszRemoteFileName;
	if (!WriteStr(str))
		return FALSE;

	if (pasv)
	{// if PASV create the socket & initiate outbound data channel connection
		pDataSocket = new CTcpSocket();
		if (!pDataSocket || !pDataSocket->Open() || !pDataSocket->Connect(pszServerIpAddress, PortNo))
			return FALSE;
	}
	if (!ReadStr() || m_fc != 1)
	{
		if (pDataSocket)
		{
			pDataSocket->Close();
			delete pDataSocket;
		}
		return FALSE; // get response to RETR/STOR command
	}
	
	if (!pasv)
	{
		// if !PASV accept inbound data connection from server
		pDataSocket = sockSrvr.Accept();
		if (!pDataSocket)
			return FALSE;
	}

	while (1)
	{ // move data from/to server & read/write local file
		Sleep(0);
		if (get)
		{
			nReadBytes = -1;
			if (pDataSocket->Read(buffer, sizeof(buffer), nReadBytes))
				WriteFile(m_hFile, buffer, nReadBytes, &nWrittenBytes, NULL);
			else
				break;
		}
		else
		{
			if (!SendNextChunk(pDataSocket))
				break;
		}
	}
	if (pDataSocket)
	{
		pDataSocket->Close();
		delete pDataSocket;
	}
	FreeFile();

	if(!ExecCommand(""))
		return FALSE; // check transfer outcome msg from server
	return TRUE; // oh goody it worked.
}


// function to send a command string on the server control channel
BOOL CFtpClient::WriteStr(const char *pszString)
{
	return (m_CtrlSocket.WriteString((char *)pszString) && 
			m_CtrlSocket.WriteString("\r\n"));
}


// this function gets the server response line
BOOL CFtpClient::ReadStr()
{

	if (!ReadStr2())
		return FALSE;
	if (strlen(m_pszReply) < 4 || m_pszReply[3] != '-')
		return TRUE;
/*	int retcode = atol(m_pszReply);
	while(1)
	{ //handle multi-line server responses
		if (m_retmsg.GetLength() > 3 && (m_retmsg.GetAt(3) == ' ' && atol(m_retmsg) == retcode))
			return TRUE;
		if (!ReadStr2())
			return FALSE;
	} */
	return TRUE;
}

// read a single response line from the server control channel
BOOL CFtpClient::ReadStr2()
{
	int i = 0;
	unsigned int nReadBytes;
	while (1)
	{
		if (!m_CtrlSocket.Read(m_pszReply+i, 1, nReadBytes))
			break;
		if (m_pszReply[i] == '\n')
			break;
		++i;
	}
	m_pszReply[i] = 0;
	m_fc = m_pszReply[0]-(char)48; // get 1st digit of the return code (indicates primary result)
	return TRUE;
}


// open the control channel to the FTP server
BOOL CFtpClient::OpenControlSocket(CString serverhost, unsigned short serverport)
{
	if (!m_CtrlSocket.Open())
		return FALSE;
	if (!m_CtrlSocket.Connect(serverhost, serverport))
		return FALSE;
	return TRUE;
}


// close the control channel to the FTP server
void CFtpClient::CloseControlSocket()
{
	m_CtrlSocket.Close();
}

BOOL CFtpClient::PrepareSendFile(const char *lpszFilename)
{
	FreeFile();
	m_nMaxSendChunkSize = 128;
	m_hFile = CreateFile(lpszFilename, GENERIC_READ,
							FILE_SHARE_READ, // another process or thread can open the same file for reading
							0, // default security descriptor
							OPEN_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL // no template
							);

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		BY_HANDLE_FILE_INFORMATION FileInfo;
		GetFileInformationByHandle(m_hFile, &FileInfo);
		m_nTotalBytesToSend = FileInfo.nFileSizeLow;

		m_hMapping = CreateFileMapping(m_hFile, 
							NULL, 
							PAGE_READONLY, 
							0, // less then 2Gb in size
							0,	// up to all size 
							NULL 
							);
		m_pData = (char *)MapViewOfFile(m_hMapping,
			FILE_MAP_READ,
			0,					// filepos high word
			0,
			0					// the entire file 
			);

	}
	else
	{
//		m_pConnectSocket->ReportFileNotFound();
		return FALSE; // file not opened
	}
	m_nTotalBytesTransfered = 0;

	return TRUE;
}

void CFtpClient::FreeFile()
{
	if (m_pData)
		UnmapViewOfFile(m_pData);
	m_pData = NULL;

	if (m_hMapping != INVALID_HANDLE_VALUE)
		CloseHandle(m_hMapping);
	m_hMapping = INVALID_HANDLE_VALUE;

	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
}

BOOL CFtpClient::PrepareReceiveFile(const char *lpszFilename)
{
	m_hFile = CreateFile(lpszFilename, GENERIC_WRITE,
							0, // nobody can write to and read from this file
							0, // default security descriptor
							CREATE_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL // no template
							);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	m_nTotalBytesTransfered = 0;

	return TRUE;
}

BOOL CFtpClient::SendNextChunk(CTcpSocket *pSocket)
{
	unsigned int nThisChunkSize = m_nTotalBytesToSend - m_nTotalBytesTransfered;
	if (nThisChunkSize == 0)
		return FALSE; // nothing to transfer
	if (m_nMaxSendChunkSize < 1)
		m_nMaxSendChunkSize = 1;
	if (nThisChunkSize > m_nMaxSendChunkSize)
		nThisChunkSize = m_nMaxSendChunkSize;
	unsigned long StartTime = timeGetTime();
	unsigned int nSentBytes = 0;
	BOOL bSuccess = pSocket->Write(m_pData+m_nTotalBytesTransfered, nThisChunkSize, nSentBytes);
	if (!bSuccess)
		return FALSE;
	unsigned long DeltaTime = timeGetTime()-StartTime;
	m_nTotalBytesTransfered += nSentBytes;
	TRACE("%d bytes sent (%d totally), deltatime = %d\n", nSentBytes, m_nTotalBytesTransfered, DeltaTime);
	if (nSentBytes == SOCKET_ERROR)
		return FALSE;
	Sleep(0);

	// update m_nMaxSendChunkSize
	if (DeltaTime < 50)
	{
		m_nMaxSendChunkSize = (int)(m_nMaxSendChunkSize * 1.05);
		if (m_nMaxSendChunkSize < 20)
			++m_nMaxSendChunkSize;
	}
	else if (DeltaTime < 200)
	{ // 50 < Delay < 200
		Sleep(DeltaTime*3/4);
	}
	else
	{ // Delay > 500 -> reduce speed
		m_nMaxSendChunkSize >>= 1;
		Sleep(DeltaTime*3/4);
	}
	return TRUE;
}

BOOL CFtpClient::DeleteRemoteFile(const char *pszRemoteFileName)
{
	return ExecCommand("DELE %s", pszRemoteFileName);
}
