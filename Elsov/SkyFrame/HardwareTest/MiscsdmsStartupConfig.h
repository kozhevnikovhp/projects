// forward declarations
class CSdmsAPI;

CString Misc_GetSdmsStartupConfig(CSdmsAPI *pSdms);
void Misc_LoadStartupConfigFromFile(CString &StartupConfig);
void Misc_SaveStartupConfigToFile(CString &StartupConfig);
void Misc_SetStartupConfig(CEdit &StartupConfigCtrl, CSdmsAPI *pSDMS);

