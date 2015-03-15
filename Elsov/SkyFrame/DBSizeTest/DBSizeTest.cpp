// DBSizeTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DBSizeTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

#include "SizeSet.h"

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		CSizeSet set;
		if (set.Open())
		{
			for (int i = 1; i<=5000*3000; i++)
			{
				set.AddNew();
				int year = 2000 + (GetTickCount() % 4);
				int month = 1 + (GetTickCount() % 12);
				int day = 1+ (GetTickCount() %27);
				int hour = GetTickCount() % 24;
				int minutes = GetTickCount() % 60;
				int secs = GetTickCount() % 60;
				set.m_Time = CTime(year, month, day, hour, minutes, secs);
				set.m_ParamID = GetTickCount() % (1 << 14);
				set.m_Value = (double)(GetTickCount() % (1 << 10));
				set.Update();
				if (i % 1000 == 0)
					printf("%d records added\n", i);
			}
		}
	}

	return nRetCode;
}


