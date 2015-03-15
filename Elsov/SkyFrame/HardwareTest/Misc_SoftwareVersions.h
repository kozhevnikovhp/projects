#ifndef __MISC_SOFTWARE_VERSIONS_H_INCLUDED__
#define __MISC_SOFTWARE_VERSIONS_H_INCLUDED__

// Forward declarations
class CSdmsAPI;

BOOL Misc_SetNewSdmsSoftware(CSdmsAPI *pSDMS, CString &SoftwareFileName, const char *pszFtpServerIpAddress);
BOOL Misc_ReloadSDMS(CSdmsAPI *pSDMS);

#endif //__MISC_SOFTWARE_VERSIONS_H_INCLUDED__