// ModemComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "hardwaretest.h"
#include "Modem.h"
#include "ModemComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cModulationTypeCombo

cModulationTypeCombo::cModulationTypeCombo()
{
}

cModulationTypeCombo::~cModulationTypeCombo()
{
}

void cModulationTypeCombo::initR(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetRModulationTypeCount(); i++)
	{
		int index = AddString(pModem->GetRModulationTypeName(i));
		SetItemData(index, i);
	}
	int ModulationType;
	pModem->GetRModulationType(ModulationType, DeviceNumber);
	SelectByDataValue(ModulationType);
}

void cModulationTypeCombo::initT(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetTModulationTypeCount(); i++)
	{
		int index = AddString(pModem->GetTModulationTypeName(i));
		SetItemData(index, i);
	}
	int ModulationType;
	pModem->GetTModulationType(ModulationType, DeviceNumber);
	SelectByDataValue(ModulationType);
}


BEGIN_MESSAGE_MAP(cModulationTypeCombo, cHardwareComboBox)
	//{{AFX_MSG_MAP(cModulationTypeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cModulationTypeCombo message handlers



/////////////////////////////////////////////////////////////////////////////
// cFecModeCombo

cFecModeCombo::cFecModeCombo()
{
}

cFecModeCombo::~cFecModeCombo()
{
}

void cFecModeCombo::initR(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetRFecModesCount(); i++)
	{
		int index = AddString(pModem->GetRFecModeName(i));
		SetItemData(index, i);
	}
	int FecMode;
	pModem->GetRFecMode(FecMode, DeviceNumber);
	SelectByDataValue(FecMode);
}

void cFecModeCombo::initT(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetTFecModesCount(); i++)
	{
		int index = AddString(pModem->GetTFecModeName(i));
		SetItemData(index, i);
	}
	int FecMode;
	pModem->GetTFecMode(FecMode, DeviceNumber);
	SelectByDataValue(FecMode);
}

BEGIN_MESSAGE_MAP(cFecModeCombo, cHardwareComboBox)
	//{{AFX_MSG_MAP(cFecModeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cFecModeCombo message handlers



/////////////////////////////////////////////////////////////////////////////
// cDifferentialModeCombo

cDifferentialModeCombo::cDifferentialModeCombo()
{
}

cDifferentialModeCombo::~cDifferentialModeCombo()
{
}

void cDifferentialModeCombo::initR(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetDiffDecoderModeCount(); i++)
	{
		int index = AddString(pModem->GetDiffDecoderModeName(i));
		SetItemData(index, i);
	}
	int Mode;
	pModem->GetDiffDecoderMode(Mode, DeviceNumber);
	SelectByDataValue(Mode);
}

void cDifferentialModeCombo::initT(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetDiffEncoderModeCount(); i++)
	{
		int index = AddString(pModem->GetDiffEncoderModeName(i));
		SetItemData(index, i);
	}
	int Mode;
	pModem->GetDiffEncoderMode(Mode, DeviceNumber);
	SelectByDataValue(Mode);
}


BEGIN_MESSAGE_MAP(cDifferentialModeCombo, cHardwareComboBox)
	//{{AFX_MSG_MAP(cDifferentialModeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cDifferentialModeCombo message handlers


/////////////////////////////////////////////////////////////////////////////
// cClockSourceComboBox

cClockSourceComboBox::cClockSourceComboBox()
{
}

cClockSourceComboBox::~cClockSourceComboBox()
{
}

void cClockSourceComboBox::initR(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetRDataClockSourcesCount(); i++)
	{
		int index = AddString(pModem->GetRDataClockSourceName(i));
		SetItemData(index, i);
	}
	int ClockSource;
	pModem->GetRDataClockSource(ClockSource, DeviceNumber);
	SelectByDataValue(ClockSource);
}

void cClockSourceComboBox::initT(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetTDataClockSourcesCount(); i++)
	{
		int index = AddString(pModem->GetTDataClockSourceName(i));
		SetItemData(index, i);
	}
	int ClockSource;
	pModem->GetTDataClockSource(ClockSource, DeviceNumber);
	SelectByDataValue(ClockSource);
}

BEGIN_MESSAGE_MAP(cClockSourceComboBox, cHardwareComboBox)
	//{{AFX_MSG_MAP(cClockSourceComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// cClockSourceComboBox message handlers

/////////////////////////////////////////////////////////////////////////////
// cPowerSupplyCombo

cPowerSupplyCombo::cPowerSupplyCombo()
{
}

cPowerSupplyCombo::~cPowerSupplyCombo()
{
}

void cPowerSupplyCombo::initR(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetRPowerSupplyModesCount(); i++)
	{
		int index = AddString(pModem->GetRPowerSupplyModeName(i));
		SetItemData(index, i);
	}
	int PowerSupplyMode;
	pModem->GetRPowerSupplyMode(PowerSupplyMode, DeviceNumber);
	SelectByDataValue(PowerSupplyMode);
}

void cPowerSupplyCombo::initT(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetTPowerSupplyModesCount(); i++)
	{
		int index = AddString(pModem->GetTPowerSupplyModeName(i));
		SetItemData(index, i);
	}
	int PowerSupplyMode;
	pModem->GetTPowerSupplyMode(PowerSupplyMode, DeviceNumber);
	SelectByDataValue(PowerSupplyMode);
}

BEGIN_MESSAGE_MAP(cPowerSupplyCombo, cHardwareComboBox)
	//{{AFX_MSG_MAP(cPowerSupplyCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cPowerSupplyCombo message handlers

/////////////////////////////////////////////////////////////////////////////
// cScramblerCombo

cScramblerCombo::cScramblerCombo()
{
}

cScramblerCombo::~cScramblerCombo()
{
}

void cScramblerCombo::initR(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetDescramblerModesCount(); i++)
	{
		int index = AddString(pModem->GetDescramblerModeName(i));
		SetItemData(index, i);
	}
	int mode;
	pModem->GetDescramblerMode(mode, DeviceNumber);
	SelectByDataValue(mode);
}

void cScramblerCombo::initT(CModem *pModem, int DeviceNumber)
{
	ResetContent();
	for (int i = 0; i < pModem->GetScramblerModesCount(); i++)
	{
		int index = AddString(pModem->GetScramblerModeName(i));
		SetItemData(index, i);
	}
	int mode;
	pModem->GetScramblerMode(mode, DeviceNumber);
	SelectByDataValue(mode);
}

BEGIN_MESSAGE_MAP(cScramblerCombo, cHardwareComboBox)
	//{{AFX_MSG_MAP(cScramblerCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cScramblerCombo message handlers


