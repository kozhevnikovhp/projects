// GranSim2D_Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\GranSim2DDesign.H"
#include "..\GuiAPI.H"

void usage(int argc, char *argv[])
{
	char *pszExeFilePath = argv[0];
	char *pszExeFileName = strrchr(pszExeFilePath, C_SLASH);
	if (pszExeFileName)
		++pszExeFileName;
	else
		pszExeFileName = pszExeFilePath;
	printf("usage: %s <file>\n", pszExeFileName);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		usage(argc, argv);
		return 1;
	}
	CGuiStub GuiStub; g_pGui = &GuiStub;

	CGranSim2DDesign design;
	char *pszFileName = argv[1];
	if (!design.LoadFromFile(pszFileName))
	{
		printf("Cannot load design from file %s\n", pszFileName);
		return 1;
	}
	
	LOGICAL_TRUE;
	LOGICAL bNeedToContinue = design.StartSimulation();
	while (bNeedToContinue)
	{
	//	printf("T = %f\n", design.GetSimulationTime());
		bNeedToContinue = design.SimulateOneTimeStep();
	}
	design.StopSimulation();

	return 0;
}
