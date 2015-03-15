/* AddressTable.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	01 Jul 2006 - initial creation
*/

#include "stdafx.h"
#include "AddressTable.h"
#include "Network.h"

/////////////////////////////////////////////////////////////////////
// CAddressTable class implementation

//Constuctors/destructors
CAddressTable::CAddressTable()
{
	memset(m_ppppObjects, 0, sizeof(m_ppppObjects));
	m_ppCashedObject = NULL;
	m_CashedIpAddress = 0;
}

//virtual
CAddressTable::~CAddressTable()
{
	for (int octet1 = 0; octet1 < 256; octet1++)
	{
		if (m_ppppObjects[octet1])
		{
			for (int octet2 = 0; octet2 < 256; octet2++)
			{
				if (m_ppppObjects[octet1][octet2])
				{
					for (int octet3 = 0; octet3 < 256; octet3++)
					{
						if (m_ppppObjects[octet1][octet2][octet3])
						{
							for (int octet4 = 0; octet4 < 256; octet4++)
							{
								if (m_ppppObjects[octet1][octet2][octet3][octet4])
								{
									CNetObject *pObject = m_ppppObjects[octet1][octet2][octet3][octet4];
									delete pObject;
									m_ppppObjects[octet1][octet2][octet3][octet4] = NULL;
								}
							}
							free(m_ppppObjects[octet1][octet2][octet3]);
							m_ppppObjects[octet1][octet2][octet3] = NULL;
						}
						
					}
					free(m_ppppObjects[octet1][octet2]);
					m_ppppObjects[octet1][octet2] = NULL;
				}

			}
			free(m_ppppObjects[octet1]);
			m_ppppObjects[octet1] = NULL;
		}
	}
}

CNetObject *CAddressTable::FindObject(IPADDRESS_TYPE IpAddress)
{
	CNetObject **ppObject = FindPlaceForObject(IpAddress);
	ASSERT(ppObject);
	return *ppObject;
}

void CAddressTable::AddObject(CNetObject *pNewObject, IPADDRESS_TYPE IpAddress)
{
	CNetObject **ppObject = FindPlaceForObject(IpAddress);
	ASSERT(ppObject);
	*ppObject = pNewObject;
}

CNetObject **CAddressTable::FindPlaceForObject(IPADDRESS_TYPE IpAddress)
{
	SOctetConverter converter;

	if (m_CashedIpAddress == IpAddress)
		return m_ppCashedObject;

	converter.addr = IpAddress;

	// 1. First octet
	unsigned char octet1 = converter.bytes.b1;
	if (NULL == m_ppppObjects[octet1])
	{
		// allocate memory
		m_ppppObjects[octet1] = (CNetObject ****)calloc(256, sizeof(CNetObject ***));
	}

	// 2. Second octet
	unsigned char octet2 = converter.bytes.b2;
	if (NULL == m_ppppObjects[octet1][octet2])
	{
		// allocate memory
		m_ppppObjects[octet1][octet2] = (CNetObject ***)calloc(256, sizeof(CNetObject **));
	}

	// 3. Third octet
	unsigned char octet3 = converter.bytes.b3;
	if (NULL == m_ppppObjects[octet1][octet2][octet3])
	{
		// allocate memory
		m_ppppObjects[octet1][octet2][octet3] = (CNetObject **)calloc(256, sizeof(CNetObject *));
	}
	
	// 4. 4th octet
	unsigned char octet4 = converter.bytes.b4;
	
	m_CashedIpAddress = IpAddress;
	m_ppCashedObject = &m_ppppObjects[octet1][octet2][octet3][octet4];
	return m_ppCashedObject;
}

void CAddressTable::IncrementAddress(SOctetConverter &converter)
{

	if (++converter.bytes.b4 == 0)
	{
		if (++converter.bytes.b3 == 0)
		{
			if (++converter.bytes.b2 == 0)
			{
				++converter.bytes.b1;
			}
		}
	}
}

CNetObject *CAddressTable::GetNextObject(IPADDRESS_TYPE &IpAddress)
{
	SOctetConverter converter;
	converter.addr = IpAddress;

	CNetObject *pObject = NULL;
	while (1)
	{
		ScrollOctet1(converter);
		ScrollOctet2(converter);
		ScrollOctet3(converter);

		pObject = m_ppppObjects[converter.bytes.b1][converter.bytes.b2][converter.bytes.b3][converter.bytes.b4];

		if (pObject)
			break;
		IncrementAddress(converter);
	};

	IncrementAddress(converter);
	IpAddress = converter.addr;
	return pObject;		
}

void CAddressTable::ScrollOctet1(SOctetConverter &converter)
{
	while (m_ppppObjects[converter.bytes.b1] == NULL)
	{
		converter.bytes.b2 = 0;
		converter.bytes.b3 = 0;
		converter.bytes.b4 = 0;
		++converter.bytes.b1;
	}
	ScrollOctet2(converter);
}

void CAddressTable::ScrollOctet2(SOctetConverter &converter)
{
	while (m_ppppObjects[converter.bytes.b1][converter.bytes.b2] == NULL)
	{
		converter.bytes.b3 = 0;
		converter.bytes.b4 = 0;
		if (++converter.bytes.b2 == 0)
		{
			++converter.bytes.b1;
			ScrollOctet1(converter);
			break;
		}
	}
	ScrollOctet3(converter);
}

void CAddressTable::ScrollOctet3(SOctetConverter &converter)
{
	while (m_ppppObjects[converter.bytes.b1][converter.bytes.b2][converter.bytes.b3] == NULL)
	{
		converter.bytes.b4 = 0;
		if (++converter.bytes.b3 == 0)
		{
			++converter.bytes.b2;
			ScrollOctet2(converter);
			break;
		}
	}
}

void CAddressTable::ScrollOctet4(SOctetConverter &converter)
{
}
