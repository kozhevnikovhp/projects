// Station.cpp

#include "stdafx.h"
#include "Station.h"
#include "DatabaseConstants.h"

/////////////////////////////////////////////////////////////////////////////
// CStationDevice

CStationDevice::CStationDevice(long DeviceType, long IpAddress)
{
	m_DeviceType = DeviceType;
	m_IpAddress = IpAddress;
	m_pPrev = NULL;
	m_pNext = NULL;
}

//virtual
CStationDevice::~CStationDevice()
{
}

void CStationDevice::Chain(CStationDevice *pNext)
{
	m_pNext = pNext;
	pNext->m_pPrev = this;
}

/////////////////////////////////////////////////////////////////////////////
// CStation

CStation::CStation()
{
	m_pDeviceChain = NULL;
}

//virtual
CStation::~CStation()
{
	for (CStationDevice *pDevice = m_pDeviceChain; pDevice;)
	{
		CStationDevice *pNextDevice = pDevice->GetNext();
		delete pDevice;
		pDevice = pNextDevice;
	}
}

BOOL CStation::AddDevice(long DeviceType, long IpAddress)
{
	CStationDevice *pNewDevice = NULL;
	if (DeviceType == DeviceType_ModemDatumSDMS || DeviceType == DeviceType_TranceiverWavesat)
		pNewDevice = new CStelaDevice(DeviceType, IpAddress);
	if (pNewDevice == NULL)
		return FALSE;
	if (m_pDeviceChain)
		pNewDevice->Chain(m_pDeviceChain);
	m_pDeviceChain = pNewDevice;
	return TRUE;
}

BOOL CStation::RemoveDevice(long IpAddress)
{
	return TRUE;
}

BOOL CStation::RebuildDevices()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CStationAttachedBlock

CStationAttachedBlock::CStationAttachedBlock(int ViewIndex, CAbstractForm *pForm)
	: CAttachedBlock(ViewIndex, pForm)
{
}

//virtual
CStationAttachedBlock::~CStationAttachedBlock()
{
}


/////////////////////////////////////////////////////////////////////////////
// CStelaDevice

CStelaDevice::CStelaDevice(long DeviceType, long IpAddress)
	: CStationDevice(DeviceType, IpAddress), CSdmsAPI()
{
}

