class CDevice;
class CSdmsAPI;

int OneObject(int argc, char* argv[], const char *pszCityName = "");

extern CRITICAL_SECTION g_CriticalSection;

typedef struct
{
	BOOL bAZS;
	DWORD address;
	double SN;
	double Power;
	BOOL bUrgentGetSN;
	double DesiredLevel;
	double CurrentLevel;
	char pszCityName[64];
	char pszConnectionName[64];
	BOOL bAutoUZSLevel;
} SRegionSync;

#define MAX_REGION_SIZE 100
const int MAX_CUVS_COUNT = 128; // max number of CUVS listeners

extern SRegionSync SyncArray[MAX_REGION_SIZE];

extern BOOL g_bUrgentGetSN;
extern CString g_UzsLogFilePath;

// CUVSes (reports a-la Arjanukhin's soft - OK, ALARM, WARNING, UNKNOWN)
extern char g_szCuvsAddress[MAX_CUVS_COUNT][32];
extern short g_CuvsPort[MAX_CUVS_COUNT];
extern int g_nCuvsCount;

// SNMP TRAPs
extern char g_szTrapAddress[MAX_CUVS_COUNT][32];
extern short g_TrapPort[MAX_CUVS_COUNT];
extern int g_nTrapCount;

extern char g_szRegionPrefix[MAX_CUVS_COUNT][64];

void ParseParamsForSdms(CSdmsAPI *pSDMS, char *pszArg);
void AddInfoToSyncData(CDevice *pDevice, const char *pszCityName, BOOL bAZS);
void EstablishConnection(CDevice *pDevice, CString &LogFilePath, char *pszReportLine);
SRegionSync *FindSyncData(char *pszIpAddress);
