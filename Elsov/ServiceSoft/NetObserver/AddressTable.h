/* AddressTable.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	01 Jul 2006 - initial creation
*/

#ifndef __ADDRESS_TABLE_H_INCLUDED_
#define __ADDRESS_TABLE_H_INCLUDED_

#include "IpSockets.h"

//Forward declarations
class CNetObject;

class CAddressTable
{
//Constuctors/destructors
public:
	CAddressTable();
	virtual ~CAddressTable();

// Public methods
public:
	CNetObject *FindObject(IPADDRESS_TYPE IpAddress);
	void AddObject(CNetObject *pNewObject, IPADDRESS_TYPE IpAddress);
	CNetObject *GetNextObject(IPADDRESS_TYPE &IpAddress);

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CNetObject ****m_ppppObjects[256];
	
	// Cash for storing last packet and/or last host finding results
	CNetObject **m_ppCashedObject;
	IPADDRESS_TYPE m_CashedIpAddress;

// Private methods
private:
	CNetObject **FindPlaceForObject(IPADDRESS_TYPE IpAddress);
	void IncrementAddress(SOctetConverter &converter);
	void ScrollOctet1(SOctetConverter &converter);
	void ScrollOctet2(SOctetConverter &converter);
	void ScrollOctet3(SOctetConverter &converter);
	void ScrollOctet4(SOctetConverter &converter);

// Private overridables
private:

// Private members
private:
};

#endif //__ADDRESS_TABLE_H_INCLUDED_

