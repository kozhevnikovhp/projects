#include "stdafx.h"
#include "Ftp.h"
#include "StelaM.h"
#include "MiscSdmsStartupConfig.h"
#include "MiscEthernet.h"

CString Misc_GetSdmsStartupConfig(CSdmsAPI *pSdms)
{
	CWaitCursor cursor;

	MC_ErrorCode EC = pSdms->Enable();
	if (EC != MC_OK)
		return "Cannot get startup config";
	EC = pSdms->Command("show startup-config");
	CString str = pSdms->GetMCReply();
	pSdms->GoTop();
	return str;
}

void Misc_LoadStartupConfigFromFile(CString &StartupConfig)
{
	CFileDialog fd(TRUE, _T("cfg"), NULL, 0,
		_T("Файлы конфигураций (*cfg)|*cfg|Все файлы (*.*)|*.*||"));
	if (fd.DoModal() == IDCANCEL) return;

	CWaitCursor cursor;

	StartupConfig.Empty();

	HANDLE hFile = CreateFile(fd.GetPathName(), GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxGetApp()->m_pMainWnd->MessageBox(fd.GetPathName(), "Cannot open file", MB_ICONEXCLAMATION);
		return;
	}

	DWORD ReadBytes;
	char ch;
	while (1)
	{
		if (!ReadFile(hFile, &ch, 1, &ReadBytes, NULL)
			|| ReadBytes == 0) break;
		if (ch == 0x0D)
			continue;
		else if (ch == 0x0A)
				StartupConfig += "\r\n";
		else
			StartupConfig += ch;
	}

	CloseHandle(hFile);
}

void Misc_SaveStartupConfigToFile(CString &StartupConfig)
{
	CFileDialog fd(FALSE, _T("cfg"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Файлы конфигураций (*cfg)|*cfg|Все файлы (*.*)|*.*||"));
	if (fd.DoModal()==IDCANCEL) return;

	CWaitCursor cursor;

	HANDLE hFile = CreateFile(fd.GetPathName(), GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxGetApp()->m_pMainWnd->MessageBox(fd.GetPathName(), "Cannot create file", MB_ICONEXCLAMATION);
		return;
	}

	DWORD WrittenBytes;
	for (int i = 0; i < StartupConfig.GetLength(); i++)
	{
		char ch = StartupConfig.GetAt(i);
		if (ch == '\r')
			continue;
		WriteFile(hFile, &ch, 1, &WrittenBytes, NULL);
	}

	CloseHandle(hFile);	
}

void Misc_SetStartupConfig(CEdit &StartupConfigCtrl, CSdmsAPI *pSDMS)
{
	const char *pszLocalStartupConfigFileName = "tmpfile.loc";
	const char *pszFtpStartupCfgFileName = "tmpfile.ftp";
	char pszFtpServerIpAddress[32];

	CWaitCursor cursor;
	BOOL bSuccess = MiscFindIpAddressForSDMS(pSDMS, pszFtpServerIpAddress);
	if (!bSuccess)
		return;

	BOOL bEnable = TRUE;
	MC_ErrorCode EC = pSDMS->EnableEthernetInterface(bEnable);
	if (bEnable == FALSE)
	{
		AfxGetApp()->m_pMainWnd->MessageBox("Cannot ENABLE ethernet interface\nSetting STARTUP-CONFIG will not be successful.", pszError, MB_ICONEXCLAMATION);
		return;
	}

	// Save file first
	HANDLE hFile = CreateFile(pszLocalStartupConfigFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxGetApp()->m_pMainWnd->MessageBox(pszLocalStartupConfigFileName, "Cannot create file", MB_ICONEXCLAMATION);
		return;
	}

	DWORD WrittenBytes;
	char ch;
	CString StartupConfig;
	
	// write config to tmp file skipping CR and not essential lines (like "---------------------")
	for (int nLine = 0; nLine < StartupConfigCtrl.GetLineCount(); nLine++)
	{
		char pszLine[1024];
		int nLength = StartupConfigCtrl.GetLine(nLine, pszLine, sizeof(pszLine)/sizeof(pszLine[0]));
		pszLine[nLength] = 0x00;
		// check if this line is essential
		unsigned int i;
		BOOL bEssential = FALSE;
		if (pszLine[0] != '%' && pszLine[0] != '!')
		{
			for (i = 0; i<strlen(pszLine); i++)
			{
				ch = *(pszLine+i);
				if (isdigit(ch) || isalpha(ch))
				{
					bEssential = TRUE;
					break;
				}
			}
		}
		if (bEssential)
		{
			for (i = 0; i<strlen(pszLine); i++)
			{
				ch = *(pszLine+i);
				if (ch == '\r')
					continue;
				WriteFile(hFile, &ch, 1, &WrittenBytes, NULL);
				StartupConfig += ch;
			}
		}
		WriteFile(hFile, "\n", 1, &WrittenBytes, NULL);
	}
	CloseHandle(hFile);

	CFtpServer FtpServer;
	FtpServer.Start();

	// Copy file to FTP-server
	CFtpClient FtpClient;
	bSuccess = FtpClient.LogOnToServer(pszFtpServerIpAddress, 21, pszFtpUsername, pszFtpPassword);
	if (!bSuccess)
	{
		AfxGetApp()->m_pMainWnd->MessageBox("Cannot connect to FTP-server", pszError, MB_ICONEXCLAMATION);
		return;
	}
	bSuccess = FtpClient.UploadFile(pszLocalStartupConfigFileName, pszFtpStartupCfgFileName);
	if (!bSuccess)
	{
		AfxGetApp()->m_pMainWnd->MessageBox("Cannot upload file to FTP-server", pszError, MB_ICONEXCLAMATION);
		return;
	}

	EC = pSDMS->LoadStartupConfig(pszFtpServerIpAddress, pszFtpStartupCfgFileName, pszFtpUsername, pszFtpPassword);
	char *pszReply = pSDMS->GetMCReply();
	if (strstr(pszReply, "rror") ||
		strstr(pszReply, "used"))
		bSuccess = FALSE;

	CString Message, Caption;
	long Flag;

	if (bSuccess)
	{
		Message = "It looks like startup config was loaded SUCCESSFULLY\n";
		Caption = "Success";
		Flag = MB_ICONINFORMATION;
	}
	else
	{
		Message = "Startup config loading FAILED !!!\n";
		Caption = pszError;
		Flag = MB_ICONEXCLAMATION;
	}
	Message += "Please look at SDMS reply:\n\n";
	Message += pSDMS->GetMCReply();
	Message += "\n";
	AfxGetApp()->m_pMainWnd->MessageBox(Message, Caption, MB_OK | Flag);


	FtpClient.DeleteRemoteFile(pszFtpStartupCfgFileName);
	DeleteFile(pszLocalStartupConfigFileName);
}
