// Stations.h

#ifndef __STATIONS_H_INCLUDED__
#define __STATIONS_H_INCLUDED__

#include "..\HardwareTest\StelaM.h"

/////////////////////////////////////////////////////////////////////////////
// CStationDevice

class CStationDevice
{
// Construction/destruction
public:
	CStationDevice(long DeviceType, long IpAddress);
	virtual ~CStationDevice();

// Public methods
public:
	CStationDevice *GetNext() { return m_pNext; }
	void Chain(CStationDevice *pNext);

// Public overridables
public:
	virtual MC_ErrorCode QueryDemodulatorStatus() = 0;
	virtual MC_ErrorCode QueryModulatorParams() = 0;
	virtual MC_ErrorCode QueryTranceiverStatus() = 0;

// Protected members
protected:
	long m_IpAddress;
	long m_DeviceType;
	CStationDevice *m_pNext;
	CStationDevice *m_pPrev;
};


/////////////////////////////////////////////////////////////////////////////
// CStation

class CStation
{
// Construction/destruction
public:
	CStation();
	virtual ~CStation();
// Public methods
	void SetStationID(long ID) { m_StationID = ID; }
	long GetStationID() { return m_StationID; }
	BOOL RebuildDevices();

// Protected methos
	BOOL AddDevice(long DeviceType, long IpAddress);
	BOOL RemoveDevice(long IpAddress);

// Protected members
	long m_StationID;
	CStationDevice *m_pDeviceChain;
	static CCuvsDoc *m_pDoc;
};


/////////////////////////////////////////////////////////////////////////////
// CStationAttachedBlock

class CStationAttachedBlock : public CAttachedBlock
{
// Construction/destruction
public:
	CStationAttachedBlock(int ViewIndex, CAbstractForm *pForm);
	virtual ~CStationAttachedBlock();

// Public methods
	CStation *GetStation() { return &m_Station; }

// Protected members
protected:
	CStation m_Station;
};


/////////////////////////////////////////////////////////////////////////////
// CStelaDevice

class CStelaDevice : public CStationDevice, public CSdmsAPI
{
// Construction/destruction
public:
	CStelaDevice(long DeviceType, long IpAddress);
// Public overridables
public:
	virtual MC_ErrorCode QueryDemodulatorStatus() { CDemIfStatus status; return GetRIfStatus(status, 1); }
	virtual MC_ErrorCode QueryModulatorParams() { CModIfParams params; return GetTIfParams(params, 1); }
	virtual MC_ErrorCode QueryTranceiverStatus() { return GetTranceiverStatus(); }
	virtual char *GetName() { return "StelaM"; } // TODO: replace this
};


#endif // __STATIONS_H_INCLUDED__