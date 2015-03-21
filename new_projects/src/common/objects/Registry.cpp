#include "stdafx.h"

char* ReadReg(HKEY hTopKey, char *szSubKey, char *szValueName, char *szValueData)
{
	DWORD   dwDataSize=1;
	LONG    res=ERROR_SUCCESS+1;
	HKEY    hKey;

	LONG ec=RegOpenKeyEx(hTopKey,szSubKey,0,KEY_READ,&hKey);
	if (ec==ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey,szValueName,NULL,NULL,NULL,&dwDataSize)==ERROR_SUCCESS)
			res=RegQueryValueEx(hKey,szValueName,NULL,NULL,(unsigned char*)szValueData,&dwDataSize);
	}
	else
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			ec,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
			);
		// Process any inserts in lpMsgBuf.
		// ...
		// Display the string.
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}
	RegCloseKey(hKey);
	return res==ERROR_SUCCESS ? szValueData : 0;
}

bool WriteReg(HKEY hTopKey, char *szSubKey, char *szValueName, char *szValueData)
{
  HKEY   hKey;
  LONG   res=ERROR_SUCCESS+1;
  DWORD  disp;

  if(RegCreateKeyEx(hTopKey,szSubKey,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&disp)==ERROR_SUCCESS)
  {
    res=RegSetValueEx(hKey,szValueName,0,REG_SZ   ,(LPBYTE)szValueData,strlen((char*)szValueData)+1);
  }
  RegCloseKey(hKey);
  return res==ERROR_SUCCESS;
}

bool DeleteRegValue(HKEY hTopKey, char *szSubKey, char *szValueName)
{
  HKEY   hKey;
  LONG   res=ERROR_SUCCESS+1;
  DWORD  disp;

  if(RegCreateKeyEx(hTopKey,szSubKey,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&disp)==ERROR_SUCCESS)
  {
    res=RegDeleteValue(hKey,szValueName);
  }
  RegCloseKey(hKey);
  return res==ERROR_SUCCESS;
}
