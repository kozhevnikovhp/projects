#include "../engine/gransim2d_design.h"
#include "../engine/gui_api.h"
#include "portable/portable.h"
#include <string.h>

using namespace common;
using namespace portable;

void usage(int argc, char *argv[])
{
	char *pszExeFilePath = argv[0];
	char *pszExeFileName = strrchr(pszExeFilePath, SUBDIRECTORIES_SEPARATOR);
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

	cGranSim2DDesign design;
	char *pszFileName = argv[1];
	if (!design.LoadFromFile(pszFileName))
	{
		printf("Cannot load design from file %s\n", pszFileName);
		return 1;
	}

	bool bNeedToContinue = design.StartSimulation();
	while (bNeedToContinue)
	{
	//	printf("T = %f\n", design.GetSimulationTime());
		bNeedToContinue = design.SimulateOneTimeStep();
	}
	design.StopSimulation();

	return 0;
}
