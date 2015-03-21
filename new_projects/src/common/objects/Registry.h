char* ReadReg(HKEY hTopKey, char *szSubKey, char *szValueName, char *szValueData);
bool WriteReg(HKEY hTopKey, char *szSubKey, char *szValueName, char *szValueData);
bool DeleteRegValue(HKEY hTopKey, char *szSubKey, char *szValueName);
