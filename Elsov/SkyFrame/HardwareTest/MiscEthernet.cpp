#include "stdafx.h"
#include "IcmpSockets.h"
#include "MiscEthernet.h"
#include "IpHelper.h"
#include "StelaM.h"
#include "ProgressDlg.h"

void CannotGetSdmsIP()
{
	AfxGetApp()->m_pMainWnd->MessageBox("Cannot get IP-address and subnet mask of SDMS Ethernet port", pszError, MB_ICONSTOP);
}

void CannotGetSdmsEthernetMAC()
{
	AfxGetApp()->m_pMainWnd->MessageBox("Cannot get MAC-addressof SDMS Ethernet port", pszError, MB_ICONSTOP);
}

void CannotSetSdmsEthernetMAC()
{
	AfxGetApp()->m_pMainWnd->MessageBox("Cannot set MAC-addressof SDMS Ethernet port", pszError, MB_ICONSTOP);
}

BOOL MiscGetSdmsEthernetIP(CSdmsAPI *pSdms, char *pszSdmsIpAddress, char *pszSdmsIpSubnetMask)
{
	MC_ErrorCode EC = pSdms->GetEthernetIpAddress(pszSdmsIpAddress, pszSdmsIpSubnetMask);
	if (EC != MC_OK)
	{
		CannotGetSdmsIP();
		return FALSE;
	}
	return TRUE;
}

BOOL MiscGetSdmsEthernetIP(CSdmsAPI *pSdms, unsigned long &SdmsIpAddress, unsigned long &SdmsIpSubnetMask)
{
	MC_ErrorCode EC = pSdms->GetEthernetIpAddress(SdmsIpAddress, SdmsIpSubnetMask);
	if (EC != MC_OK)
	{
		CannotGetSdmsIP();
		return FALSE;
	}
	return TRUE;
}

BOOL MiscSetSdmsEthernetIP(CSdmsAPI *pSdms, const char *pszSdmsIpAddress, const char *pszSdmsIpSubnetMask)
{
	CWaitCursor cursor;
	// check if desired address matches one of existing adapter's address
	BOOL bMatch = FALSE;
	CIpHelper IpHelper;
	for (unsigned int i = 0; i < IpHelper.GetIpAddressesCount(); i++)
	{
		if (IpHelper.IsIpAddressLocal(i))
			continue;
		unsigned long IpAddress = IpHelper.GetIpAddress(i);
		unsigned long SubnetMask = IpHelper.GetIpSubnetMask(i);
		if (IpAddress == 0 || SubnetMask == 0)
			continue; // skip unconnected adapters
		if (IpAddress == inet_addr(pszSdmsIpAddress))
		{
			bMatch = TRUE;
			break;
		}
	}
	if (!bMatch)
	{
		MC_ErrorCode EC = pSdms->SetEthernetIpAddress(pszSdmsIpAddress, pszSdmsIpSubnetMask);
		if (EC != MC_OK)
		{
			AfxGetApp()->m_pMainWnd->MessageBox("IP-address setting was FAILED !", pszError, MB_ICONEXCLAMATION);
			return FALSE;
		}		
	}
	else
	{
		AfxGetApp()->m_pMainWnd->MessageBox("Incorrect IP-address", pszError, MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL MiscGetSdmsEthernetMAC(CSdmsAPI *pSdms, char *pszMacAddress)
{
	MC_ErrorCode EC = pSdms->GetEthernetMacAddress(pszMacAddress);
	if (EC != MC_OK)
	{
		CannotGetSdmsEthernetMAC();
		return FALSE;
	}
	return TRUE;
}

BOOL MiscSetSdmsEthernetMAC(CSdmsAPI *pSdms, const char *pszMacAddress)
{
	CWaitCursor cursor;
	MC_ErrorCode EC = pSdms->SetEthernetMacAddress(pszMacAddress);
	if (EC != MC_OK)
	{
		CannotSetSdmsEthernetMAC();
		return FALSE;
	}
	return TRUE;
}

BOOL MiscFindIpAddressForSDMS(unsigned long SdmsIpAddress, unsigned long SdmsIpSubnetMask, char *pszIpAddress)
{
	// find IP-address of ftp-server
	unsigned long FtpServerIpAddress = 0;
	CIpHelper IpHelper;
	for (unsigned int i = 0; i < IpHelper.GetIpAddressesCount(); i++)
	{
		if (IpHelper.IsIpAddressLocal(i))
			continue;
		DWORD LocalHostIpSubnetMask = IpHelper.GetIpSubnetMask(i);
		if (LocalHostIpSubnetMask != SdmsIpSubnetMask)
			continue;
		DWORD LocalHostIpAddress = IpHelper.GetIpAddress(i);
		if ((LocalHostIpAddress & LocalHostIpSubnetMask) == (SdmsIpAddress & SdmsIpSubnetMask))
			FtpServerIpAddress = LocalHostIpAddress;
	}
	if (FtpServerIpAddress == 0)
	{
		AfxGetApp()->m_pMainWnd->MessageBox("Cannot find appropriate ethernet adapter for SDMS", pszError, MB_ICONSTOP);
		return FALSE;
	}
	AddressToString(pszIpAddress, FtpServerIpAddress);

	return TRUE;
}

BOOL MiscFindIpAddressForSDMS(const char *pszSdmsIpAddress, const char *pszSdmsIpSubnetMask, char *pszIpAddress)
{
	return MiscFindIpAddressForSDMS(inet_addr(pszSdmsIpAddress), inet_addr(pszSdmsIpSubnetMask), pszIpAddress);
}

BOOL MiscFindIpAddressForSDMS(CSdmsAPI *pSdms, char *pszIpAddress)
{
	unsigned long SdmsIpAddress, SdmsIpSubnetMask;
	BOOL bSuccess = MiscGetSdmsEthernetIP(pSdms, SdmsIpAddress, SdmsIpSubnetMask);
	if (!bSuccess)
		return FALSE;
	return MiscFindIpAddressForSDMS(SdmsIpAddress, SdmsIpSubnetMask, pszIpAddress);
}

void MiscInitEthernetAdaptersCombo(CComboBox *pLocalHostCombo)
{
	char szIpAddress[32], szSubnetMask[32];
	pLocalHostCombo->ResetContent();
	CString str;
	CIpHelper IpHelper;
	for (unsigned int i = 0; i < IpHelper.GetIpAddressesCount(); i++)
	{
		if (IpHelper.IsIpAddressLocal(i))
			continue;
		DWORD IpAddress = IpHelper.GetIpAddress(i);
		DWORD SubnetMask = IpHelper.GetIpSubnetMask(i);
		if (IpAddress == 0 || SubnetMask == 0)
			continue; // skip unconnected adapters
		unsigned long Iface = IpHelper.GetIfaceIpAddressIndex(i);
		CString InterfaceDesc = IpHelper.GetIfaceDesc(Iface);
		str.Format("%s/%s ( %s )",
			AddressToString(szIpAddress, IpAddress),
			AddressToString(szSubnetMask, SubnetMask),
			InterfaceDesc);
		int index = pLocalHostCombo->AddString(str);
		pLocalHostCombo->SetItemData(index, i);		
	}
	pLocalHostCombo->SetCurSel(0);
}

void MiscCopyAddressAndMask(CComboBox *pLocalHostCombo, CString &IpAddress, CString &SubnetMask)
{
	int index = pLocalHostCombo->GetCurSel();
	if (index == CB_ERR)
		return;

	CIpHelper IpHelper;
	unsigned long AddressIndex = pLocalHostCombo->GetItemData(index);
	DWORD LocalHostIpAddress = IpHelper.GetIpAddress(AddressIndex);
	DWORD LocalHostSubnetMask = IpHelper.GetIpSubnetMask(AddressIndex);
	IpAddress.Format("%d.%d.%d.%d", LOBYTE(LOWORD(LocalHostIpAddress)), HIBYTE(LOWORD(LocalHostIpAddress)), LOBYTE(HIWORD(LocalHostIpAddress)), HIBYTE(HIWORD(LocalHostIpAddress)));
	SubnetMask.Format("%d.%d.%d.%d", LOBYTE(LOWORD(LocalHostSubnetMask)), HIBYTE(LOWORD(LocalHostSubnetMask)), LOBYTE(HIWORD(LocalHostSubnetMask)), HIBYTE(HIWORD(LocalHostSubnetMask)));
}

void MiscTestSdmsEthernetPort(CSdmsAPI *pSdms, CString &Protocol, CWnd *pParentWindow)
{
	Protocol = &afxChNil;
	CWaitCursor cursor;
	char *pszReply;
	char szSdmsIpAddress[32], szSdmsSubnetMask[32], szLocalHostIpAddress[32];
	if (!MiscGetSdmsEthernetIP(pSdms, szSdmsIpAddress, szSdmsSubnetMask))
		return;
	if (!MiscFindIpAddressForSDMS(szSdmsIpAddress, szSdmsSubnetMask, szLocalHostIpAddress))
		return;

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0, 6);
	char *pszTestStatus;

	// Enable interface
	pszTestStatus = "Enabling interface...";
	dlg.SetStatus(pszTestStatus);
	BOOL bEnable = TRUE;
	MC_ErrorCode EC = pSdms->EnableEthernetInterface(bEnable);
	if (bEnable == FALSE)
		AfxGetApp()->m_pMainWnd->MessageBox("Cannot ENABLE ethernet interface\nThis test will not be successful.", pszError, MB_ICONEXCLAMATION);

	// Ethernet port statistics
	if (dlg.CheckCancelButton())
		return;
	pszTestStatus = "Ethernet port statistics before test...";
	dlg.SetStatus(pszTestStatus);
	Protocol = pszTestStatus;
	Protocol += "\r\n";
	EC = pSdms->Enable();
	pParentWindow->UpdateData(FALSE);
	if (EC != MC_OK)
		return;
	EC = pSdms->Command("show interface ethernet 0");
	if (EC != MC_OK)
		return;
	pszReply = pSdms->GetMCReply();
	Protocol += pszReply;
	pParentWindow->UpdateData(FALSE);
	dlg.StepIt();

	// ICMP statistics
	if (dlg.CheckCancelButton())
		return;
	pszTestStatus = "ICMP statistics before test...";
	dlg.SetStatus(pszTestStatus);
	Protocol += "\r\n";
	Protocol += pszTestStatus;
	Protocol += "\r\n";
	dlg.SetStatus(pszTestStatus);
	EC = pSdms->Command("show icmp statistics");
	pszReply = pSdms->GetMCReply();
	Protocol += pszReply;
	dlg.StepIt();
	pParentWindow->UpdateData(FALSE);
	if (EC != MC_OK)
		return;

	// Ping local host (PC) -> SDMS
	if (dlg.CheckCancelButton())
		return;
	Protocol += "\r\nPing PC(";
	Protocol += szLocalHostIpAddress;
	Protocol += ") -> SDMS(";
	Protocol += szSdmsIpAddress;
	Protocol += ")...\r\n";
	dlg.SetStatus("Ping PC -> SDMS");
	CString str;
	CIcmpSocket sock;
	int rcvd_bytes = 0;
	int sent = 0, rcvd = 0, bad = 0, timed_out = 0;
	int total_delay = 0;
	unsigned long IPAddress = inet_addr(szSdmsIpAddress);
	for (int i = 0; i<5; i++)
	{
		sock.SendEchoRequest(IPAddress, 1000);
		sent++;
		sock.ReceiveEchoReply();
		if (sock.IsTimedOut())
		{
			timed_out++;
			Protocol += "Timeout expired\r\n";
		}
		else if (sock.IsBad())
		{
			bad++;
			Protocol += "Bad echo reply\r\n";
		}
		else
		{
			rcvd++;
			rcvd_bytes = sock.GetReceivedDataLength()+32;
			total_delay += sock.GetDelayTime();
			str.Format("%d bytes from %s delay=%d msec\r\n", rcvd_bytes, szSdmsIpAddress, total_delay);
		}
		Protocol += str;
		Sleep(500);
	}
	dlg.StepIt();

	// Ping  SDMS -> local host (PC)
	if (dlg.CheckCancelButton())
		return;
	dlg.SetStatus("Ping SDMS -> PC");
	Protocol += "\r\n\r\nPing SDMS (";
	Protocol += szSdmsIpAddress;
	Protocol += ") -> PC (";
	Protocol += szLocalHostIpAddress;
	Protocol += ")...\r\n";
	str.Format("ping %s\r\n", szLocalHostIpAddress);
	EC = pSdms->Command((char *)LPCTSTR(str));
	pszReply = pSdms->GetMCReply();
	dlg.StepIt();
	pParentWindow->UpdateData(FALSE);
/*	if (EC != MC_OK)
		return;*/
	Protocol += pszReply;

	// Ethernet port statistics
	if (dlg.CheckCancelButton())
		return;
	pszTestStatus = "Ethernet port statistics after test...";
	Protocol += "\r\n";
	Protocol += pszTestStatus;
	Protocol += "\r\n";
	dlg.SetStatus(pszTestStatus);
	EC = pSdms->Enable();
	pParentWindow->UpdateData(FALSE);
/*	if (EC != MC_OK)
		return;*/
	EC = pSdms->Command("show interface ethernet 0");
	if (EC != MC_OK)
		return;
	pszReply = pSdms->GetMCReply();
	Protocol += pszReply;
	dlg.StepIt();
	pParentWindow->UpdateData(FALSE);

	// ICMP statistics
	if (dlg.CheckCancelButton())
		return;
	Sleep(1000);
	pszTestStatus = "ICMP statistics after test...";
	Protocol += "\r\n";
	Protocol += pszTestStatus;
	Protocol += "\r\n";
	dlg.SetStatus(pszTestStatus);
	EC = pSdms->Command("show icmp statistics");
	pszReply = pSdms->GetMCReply();
	Protocol += pszReply;
	dlg.StepIt();

	pParentWindow->UpdateData(FALSE);
}
