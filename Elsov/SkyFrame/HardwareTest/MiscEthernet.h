#ifndef __MISC_ETHERNET_H_INCLUDED__
#define __MISC_ETHERNET_H_INCLUDED__

// Forward declarations
class CSdmsAPI;
class CIpHelper;

BOOL MiscGetSdmsEthernetIP(CSdmsAPI *pSdms, char *pszSdmsIpAddress, char *pszSdmsIpSubnetMask);
BOOL MiscGetSdmsEthernetIP(CSdmsAPI *pSdms, unsigned long &SdmsIpAddress, unsigned long &SdmsIpSubnetMask);
BOOL MiscSetSdmsEthernetIP(CSdmsAPI *pSdms, const char *pszSdmsIpAddress, const char *pszSdmsIpSubnetMask);
BOOL MiscGetSdmsEthernetMAC(CSdmsAPI *pSdms, char *pszMacAddress);
BOOL MiscSetSdmsEthernetMAC(CSdmsAPI *pSdms, const char *pszMacAddress);

BOOL MiscFindIpAddressForSDMS(unsigned long SdmsIpAddress, unsigned long SdmsIpSubnetMask, char *pszIpAddress);
BOOL MiscFindIpAddressForSDMS(const char *pszSdmsIpAddress, const char *pszSdmsIpSubnetMask, char *pszIpAddress);
BOOL MiscFindIpAddressForSDMS(CSdmsAPI *pSdms, char *pszIpAddress);

void MiscInitEthernetAdaptersCombo(CComboBox *pLocalHostCombo);
void MiscCopyAddressAndMask(CComboBox *pLocalHostCombo, CString &IpAddress, CString &SubnetMask);
void MiscTestSdmsEthernetPort(CSdmsAPI *pSdms, CString &Protocol, CWnd *pParentWindow);

#endif //__MISC_ETHERNET_H_INCLUDED__