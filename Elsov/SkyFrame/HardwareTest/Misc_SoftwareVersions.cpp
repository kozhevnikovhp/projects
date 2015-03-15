#include "stdafx.h"
#include "Misc_SoftwareVersions.h"
#include "ProgressDlg.h"
#include "Ftp.h"

#include "StelaM.h"

BOOL Misc_SetNewSdmsSoftware(CSdmsAPI *pSDMS, CString &SoftwareFileName, const char *pszFtpServerIpAddress)
{
	const char *pszFtpSoftwareFileName = "NewSoftware.ftp";
	
	if (AfxGetApp()->m_pMainWnd->MessageBox("Are you sure?", "Please be careful", MB_ICONQUESTION | MB_YESNO) != IDYES)
		return FALSE;

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetStatus("Sending newer software version...");

	CFtpServer FtpServer;
	FtpServer.Start();

	CFtpClient FtpClient;
	BOOL bSuccess = FtpClient.LogOnToServer(pszFtpServerIpAddress, 21, pszFtpUsername, pszFtpPassword);
	if (!bSuccess)
	{
		AfxGetApp()->m_pMainWnd->MessageBox("Cannot connect to FTP-server", pszError, MB_ICONEXCLAMATION);
		return FALSE;
	}
	bSuccess = FtpClient.UploadFile(SoftwareFileName, pszFtpSoftwareFileName);
	if (!bSuccess)
	{
		AfxGetApp()->m_pMainWnd->MessageBox("Cannot upload file to FTP-server", pszError, MB_ICONEXCLAMATION);
		return FALSE;
	}
	MC_ErrorCode EC = pSDMS->SetupSoftware(pszFtpServerIpAddress, pszFtpSoftwareFileName, pszFtpUsername, pszFtpPassword);

	FtpClient.DeleteRemoteFile(pszFtpSoftwareFileName);
	FtpServer.Stop();

	return (EC == MC_OK);
}


BOOL Misc_ReloadSDMS(CSdmsAPI *pSDMS)
{
	const int MaxTimeToWait = 150;

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, MaxTimeToWait);
	
	pSDMS->Reload();
	// wait for reload...
	CString status;
	time_t start, finish;
	double delta_time;
	time(&start);
	do
	{
		time(&finish);
		delta_time = difftime(finish, start);
		status.Format("New software transferred. Time of reloading %3.0f sec.", delta_time);
		dlg.SetStatus(status);
		dlg.SetPos((int)delta_time);
		if (dlg.CheckCancelButton())
			break;
		if (pSDMS->IsMCConnectionAlive() == MC_OK)
			break;
	} while (delta_time < MaxTimeToWait);
	BOOL bSuccess = pSDMS->IsMCConnectionAlive() == MC_OK;
	if (!bSuccess)
		AfxGetApp()->m_pMainWnd->MessageBox("Device is not successfully reloaded", pszError, MB_ICONEXCLAMATION);

	return bSuccess;
}