#include "defs.h"
#include "SkyFrameNet.h"

/////////////////////////////////////////////////////////////////////////////////
// CSkyFrameNet class

CSkyFrameNet::CSkyFrameNet()
{
	m_pControlSystem = NULL;
}

CSkyFrameNet::CSkyFrameNet(IControlSystemAPI *pControlSystem)
{
	m_pControlSystem = pControlSystem;
}

CSkyFrameNet::~CSkyFrameNet()
{
}

//virtual
int CSkyFrameNet::GetTargetPictureID()
{
	return PICTURE_ID_NETWORK;
}

//virtual
int CSkyFrameNet::GetObjectPictureID(long ObjectType)
{
	int PictureID;
	switch (ObjectType)
	{
	case OBJECT_TYPE_AZS:	PictureID = PICTURE_ID_SATELLITE_DISH; break;
	case OBJECT_TYPE_UZS:	PictureID = PICTURE_ID_SATELLITE_DISH; break;
	case OBJECT_TYPE_CZS:	PictureID = PICTURE_ID_SATELLITE_DISH; break;
	default:				PictureID = PICTURE_ID_GEAR; break;
	}
	return PictureID;
}


//virtual
long CSkyFrameNet::GetNumberOfSupportedObjectTypes()
{
	return sizeof(m_pszObjectTypeDescs)/sizeof(m_pszObjectTypeDescs[0]) - 1;
}
	
//virtual
char *CSkyFrameNet::GetObjectTypeDescription(long ObjectType)
{
	if (ObjectType < 1 || ObjectType > GetNumberOfSupportedObjectTypes())
		return m_pszObjectTypeDescs[OBJECT_TYPE_UNKNOWN];
	return m_pszObjectTypeDescs[ObjectType];
}

//virtual
long CSkyFrameNet::HasEventLog(long ObjectType)
{
	switch (ObjectType)
	{
	case OBJECT_TYPE_CZS:
	case OBJECT_TYPE_UZS:
	case OBJECT_TYPE_AZS:
	case OBJECT_TYPE_MODEM_DATUM:
	case OBJECT_TYPE_MODEM_RADYNE:
	case OBJECT_TYPE_TRANCEIVER_WAVESAT:
	case OBJECT_TYPE_SDMS:
	case OBJECT_TYPE_SDMS_VSAT:
		return 1;
	case OBJECT_TYPE_FRAME_UZS:
	case OBJECT_TYPE_FRAME_AZS:
		return 0;
	default:
		return 0;
	}
}

//virtual
long CSkyFrameNet::HasMonitorAndControl(long ObjectType)
{
	switch (ObjectType)
	{
	case OBJECT_TYPE_MODEM_DATUM:
	case OBJECT_TYPE_MODEM_RADYNE:
	case OBJECT_TYPE_TRANCEIVER_WAVESAT:
	case OBJECT_TYPE_SDMS:
	case OBJECT_TYPE_SDMS_VSAT:
		return 1;
	case OBJECT_TYPE_CZS:
	case OBJECT_TYPE_UZS:
	case OBJECT_TYPE_AZS:
	case OBJECT_TYPE_FRAME_UZS:
	case OBJECT_TYPE_FRAME_AZS:
		return 0;
	default:
		return 0;
	}
}

//virtual
long CSkyFrameNet::HasTMI(long ObjectType)
{
	switch (ObjectType)
	{
	case OBJECT_TYPE_CZS:
	case OBJECT_TYPE_UZS:
	case OBJECT_TYPE_AZS:
	case OBJECT_TYPE_MODEM_DATUM:
	case OBJECT_TYPE_MODEM_RADYNE:
	case OBJECT_TYPE_TRANCEIVER_WAVESAT:
	case OBJECT_TYPE_SDMS:
	case OBJECT_TYPE_SDMS_VSAT:
		return 1;
	case OBJECT_TYPE_FRAME_UZS:
	case OBJECT_TYPE_FRAME_AZS:
		return 0;
	default:
		return 0;
	}
}

//virtual
long CSkyFrameNet::CanUseParentConnection(long ObjectType)
{
	switch (ObjectType)
	{
	case OBJECT_TYPE_MODEM_DATUM:
	case OBJECT_TYPE_MODEM_RADYNE:
	case OBJECT_TYPE_TRANCEIVER_WAVESAT:
		return 1;
	case OBJECT_TYPE_CZS:
	case OBJECT_TYPE_UZS:
	case OBJECT_TYPE_AZS:
	case OBJECT_TYPE_SDMS:
	case OBJECT_TYPE_SDMS_VSAT:
	case OBJECT_TYPE_FRAME_UZS:
	case OBJECT_TYPE_FRAME_AZS:
		return 0;
	default:
		return 0;
	}
}

//virtual
long CSkyFrameNet::CanObjectBeControlledBySerialPort(long ObjectType)
{
	switch (ObjectType)
	{
	case OBJECT_TYPE_MODEM_DATUM:
	case OBJECT_TYPE_MODEM_RADYNE:
	case OBJECT_TYPE_TRANCEIVER_WAVESAT:
	case OBJECT_TYPE_SDMS:
	case OBJECT_TYPE_SDMS_VSAT:
		return 1;
	case OBJECT_TYPE_CZS:
	case OBJECT_TYPE_UZS:
	case OBJECT_TYPE_AZS:
	case OBJECT_TYPE_FRAME_UZS:
	case OBJECT_TYPE_FRAME_AZS:
		return 0;
	default:
		return 0;
	}
}

//virtual
long CSkyFrameNet::CanObjectBeControlledByTelnet(long ObjectType)
{
	switch (ObjectType)
	{
	case OBJECT_TYPE_SDMS:
	case OBJECT_TYPE_SDMS_VSAT:
		return 1;
	case OBJECT_TYPE_MODEM_DATUM:
	case OBJECT_TYPE_MODEM_RADYNE:
	case OBJECT_TYPE_TRANCEIVER_WAVESAT:
	case OBJECT_TYPE_CZS:
	case OBJECT_TYPE_UZS:
	case OBJECT_TYPE_AZS:
	case OBJECT_TYPE_FRAME_UZS:
	case OBJECT_TYPE_FRAME_AZS:
		return 0;
	default:
		return 0;
	}
}

//virtual
long CSkyFrameNet::CanObjectBeControlledBySNMP(long ObjectType)
{
	return 0; // cannot :-)
}

//virtual
long CSkyFrameNet::CanObjectBeControlledByGPIB(long ObjectType)
{
	return 0; // cannot :-)
}

//virtual
long CSkyFrameNet::MC_HasSerialPortBusAddress(long ObjectType)
{
	switch (ObjectType)
	{
	case OBJECT_TYPE_MODEM_RADYNE:
	case OBJECT_TYPE_TRANCEIVER_WAVESAT:
		return 1;
	case OBJECT_TYPE_SDMS:
	case OBJECT_TYPE_SDMS_VSAT:
	case OBJECT_TYPE_MODEM_DATUM:
	case OBJECT_TYPE_CZS:
	case OBJECT_TYPE_UZS:
	case OBJECT_TYPE_AZS:
	case OBJECT_TYPE_FRAME_UZS:
	case OBJECT_TYPE_FRAME_AZS:
		return 0;
	default:
		return 0;
	}
}

//virtual
long CSkyFrameNet::MC_CanConfigureSerialPort(long ObjectType)
{
	switch (ObjectType)
	{
	case OBJECT_TYPE_MODEM_RADYNE:
	case OBJECT_TYPE_MODEM_DATUM:
		return 1;
	case OBJECT_TYPE_TRANCEIVER_WAVESAT:
	case OBJECT_TYPE_SDMS:
	case OBJECT_TYPE_SDMS_VSAT:
	case OBJECT_TYPE_CZS:
	case OBJECT_TYPE_UZS:
	case OBJECT_TYPE_AZS:
	case OBJECT_TYPE_FRAME_UZS:
	case OBJECT_TYPE_FRAME_AZS:
		return 0;
	default:
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// CSkyFrameNetRussian class

CSkyFrameNetRussian::CSkyFrameNetRussian(IControlSystemAPI *pControlSystem)
	: CSkyFrameNet(pControlSystem)
{
	m_pszObjectTypeDescs[OBJECT_TYPE_UNKNOWN] = "Неизвестный тип";
	m_pszObjectTypeDescs[OBJECT_TYPE_CZS] = "ЦЗС";
	m_pszObjectTypeDescs[OBJECT_TYPE_UZS] = "УЗС";
	m_pszObjectTypeDescs[OBJECT_TYPE_FRAME_UZS] = "Стойка УЗС";
	m_pszObjectTypeDescs[OBJECT_TYPE_AZS] = "АЗС";
	m_pszObjectTypeDescs[OBJECT_TYPE_FRAME_AZS] = "Министойка";
	m_pszObjectTypeDescs[OBJECT_TYPE_MODEM_DATUM] = "Модем Datum";
	m_pszObjectTypeDescs[OBJECT_TYPE_MODEM_RADYNE] = "Модем Radyne";
	m_pszObjectTypeDescs[OBJECT_TYPE_TRANCEIVER_WAVESAT] = "Трансивер Wavesat";
	m_pszObjectTypeDescs[OBJECT_TYPE_SDMS] = "SDMS";
	m_pszObjectTypeDescs[OBJECT_TYPE_SDMS_VSAT] = "SDMS-VSAT";
}

//virtual
char *CSkyFrameNetRussian::GetTargetDescription()
{
	return "Сеть SkyFrame";
}

/////////////////////////////////////////////////////////////////////////////////
// CSkyFrameNetEnglish class

CSkyFrameNetEnglish::CSkyFrameNetEnglish(IControlSystemAPI *pControlSystem)
	: CSkyFrameNet(pControlSystem)
{
	m_pszObjectTypeDescs[OBJECT_TYPE_UNKNOWN] = "Unknown type";
	m_pszObjectTypeDescs[OBJECT_TYPE_CZS] = "Central Station";
	m_pszObjectTypeDescs[OBJECT_TYPE_UZS] = "HUB Station";
	m_pszObjectTypeDescs[OBJECT_TYPE_FRAME_UZS] = "Frame";
	m_pszObjectTypeDescs[OBJECT_TYPE_AZS] = "VSAT";
	m_pszObjectTypeDescs[OBJECT_TYPE_FRAME_AZS] = "Miniframe";
	m_pszObjectTypeDescs[OBJECT_TYPE_MODEM_DATUM] = "Modem Datum";
	m_pszObjectTypeDescs[OBJECT_TYPE_MODEM_RADYNE] = "Modem Radyne";
	m_pszObjectTypeDescs[OBJECT_TYPE_TRANCEIVER_WAVESAT] = "Tranceiver Wavesat";
	m_pszObjectTypeDescs[OBJECT_TYPE_SDMS] = "SDMS";
	m_pszObjectTypeDescs[OBJECT_TYPE_SDMS_VSAT] = "SDMS-VSAT";
}

//virtual
char *CSkyFrameNetEnglish::GetTargetDescription()
{
	return "SkyFrame network";
}

