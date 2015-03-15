// Power.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../SkyLane2048.h"

class CPowerModem : public CSkyLaneSL2048_Lband
{
public:
	CPowerModem() {}
	~CPowerModem() {}
};


int main(int argc, char* argv[])
{
	char *pszSNFormatString = "S/N(1) = \t%5.2f\t S/N(2) = \t%5.2f\t";
	BOOL bPowerManagementEnabled = TRUE;
	float MinSN, MaxSN;
	if (argc == 3)
	{
		sscanf(argv[1], "%f", &MinSN);
		sscanf(argv[2], "%f", &MaxSN);
	}
	if (argc == 1)
		bPowerManagementEnabled = FALSE;


	CPowerModem modem1, modem2;
	modem1.Connect(1);
	modem2.Connect(2);

	while (1)
	{
		float SN1 = 0.f, SN2 = 0.f;
		int nSN1 =0, nSN2 =0;
		DWORD TimeBegin = GetTickCount();
		while (GetTickCount() - TimeBegin < 2000)
		{
			double _SN1 = 0., _SN2 = 0.;
			modem1.GetEbNo(_SN1, 1); SN1 += _SN1; nSN1++;
			modem2.GetEbNo(_SN2, 1); SN2 += _SN2; nSN2++;
			Sleep(10);
		}
		SYSTEMTIME time;
		GetLocalTime(&time);
		FILE *fd=fopen("Power.txt", "a+");
		fprintf(fd, "%d.%d.%d\t%d:%d:%d\t",
				time.wDay, time.wMonth, time.wYear,
				time.wHour,	time.wMinute, time.wSecond); 
		SN1 /= nSN1;
		SN2 /= nSN2;
		printf(pszSNFormatString, SN1, SN2);
		fprintf(fd, pszSNFormatString, SN1, SN2);
		if (bPowerManagementEnabled)
		{
			BOOL bChangedOutput1 = FALSE;
			if (SN1 < MinSN || (SN1 > MaxSN))
			{
				float desired = (MinSN + MaxSN) * 0.5f;
				float DeltaLevel = (desired - SN1) * 0.5f; // to make transition more smooth
				modem2.ChangeOutputLevel(DeltaLevel, 1);
				printf("\t%5.1f", DeltaLevel);
				fprintf(fd, "\t%5.1f", DeltaLevel);
				bChangedOutput1 = TRUE;
			}
			else
			{
				printf("\t");
				fprintf(fd, "\t");
			// nothing to do
			}

			BOOL bChangedOutput2 = FALSE;
			if (SN2 < MinSN || (SN2 > MaxSN))
			{
				float desired = (MinSN+MaxSN)*0.5f;
				float DeltaLevel = (desired - SN2) *0.5f;
				modem1.ChangeOutputLevel(DeltaLevel, 1);
				printf("\t%5.1f", DeltaLevel);
				fprintf(fd, "\t%5.1f", DeltaLevel);
				bChangedOutput2 = TRUE;
			}
			else
			{
			// nothing to do
			}
			if (bChangedOutput1 || bChangedOutput2)
				Sleep(2000);
		}
		printf("\n");
		fprintf(fd, "\n");
		fclose(fd);
	}

	return 0;
}
