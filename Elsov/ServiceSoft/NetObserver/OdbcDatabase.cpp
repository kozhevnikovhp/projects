/* OdbcDatabase.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	25 Oct 2006 - initial creation
	22 Nov 2006 - storing network objects and their properties in database
	28 Nov 2006 - loading network objects and their properties from database
	29 Nov 2006 - storing/loading flags of network objects in/from database
	20 May 2007 - traffic table
	03 Jul 2007 - traffic records queries
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables
	28 Nov 2007 - own host's MAC-address
	14 Mar 2008 - groups table, storing group params
	20 Mar 2008 - grouping (hosts by groups) loading/storing
	04 Apr 2008 - memory leak in traffic quieries
	15 Apr 2008 - (Windows) services flags; host merging (due to DHCP, for example)
	22 Jul 2008 - subnet color scheme
*/

#include "stdafx.h"
#include "OdbcDatabase.h"
#include "Network.h"
#include "Groups.h"

IDatabaseAPI *g_pDB = NULL;

///////////////////////////////////////////////////////////////////////////
// COdbcRecordset class implementation

IMPLEMENT_DYNAMIC(COdbcRecordset, CRecordset)

COdbcRecordset::COdbcRecordset(CDatabase *pDB)
 : CRecordset(pDB)
{
	m_pDB = (COdbcDatabase *)pDB;
}

//virtual
BOOL COdbcRecordset::Requery()
{
	if (!IsOpen() && !Open())
		return FALSE;
	return CRecordset::Requery();
}

int COdbcRecordset::CountRecords()
{
	int nRecords = 0;
	while (!IsEOF())
	{
		++nRecords;
		MoveNext();
	}
	MoveFirst();
	
	return nRecords;
}

/////////////////////////////////////////////////////////////////////////////
// CNetObjectRecordset

IMPLEMENT_DYNAMIC(CNetObjectRecordset, CRecordset)

CNetObjectRecordset::CNetObjectRecordset(CDatabase *pdb)
	: COdbcRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CNetObjectRecordset)
	m_ID = 0;
	m_OBJECT_TYPE = 0;
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams = 1;
}


CString CNetObjectRecordset::GetDefaultSQL()
{
	return _T("[NetObjects]");
}

void CNetObjectRecordset::DoFieldExchange(CFieldExchange *pFX)
{
	//{{AFX_FIELD_MAP(CNetObjectRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Long(pFX, _T("[OBJECT_TYPE]"), m_OBJECT_TYPE);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
}

/////////////////////////////////////////////////////////////////////////////
// CNetObjectRecordset diagnostics

#ifdef _DEBUG
void CNetObjectRecordset::AssertValid() const
{
	COdbcRecordset::AssertValid();
}

void CNetObjectRecordset::Dump(CDumpContext &dc) const
{
	COdbcRecordset::Dump(dc);
}
#endif //_DEBUG

LOGICAL CNetObjectRecordset::QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets)
{
	m_strFilter = "OBJECT_TYPE = ?";
	m_Param1 = NET_OBJECT_TYPE_SUBNET;
	Requery();
	// Count subnets
	nSubnets = 0;
	while (!IsEOF())
	{
		++nSubnets;
		MoveNext();
	}

	if (nSubnets > 0)
	{
		MoveFirst();
		pSubnetDescs = (SSubnetDesc *)realloc(pSubnetDescs, nSubnets*sizeof(SSubnetDesc));
		SSubnetDesc *pThisSubnetDesc = pSubnetDescs;
		while (!IsEOF())
		{
			pThisSubnetDesc->Init();
			pThisSubnetDesc->ID = m_ID;

			++pThisSubnetDesc;
			MoveNext();
		}
	}

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectRecordset::QueryHosts(SHostDesc *&pDescs, int &nHosts)
{
	m_strFilter = "OBJECT_TYPE = ?";
	m_Param1 = NET_OBJECT_TYPE_HOST;
	Requery();
	// Count hosts
	nHosts = 0;
	while (!IsEOF())
	{
		++nHosts;
		MoveNext();
	}

	if (nHosts > 0)
	{
		MoveFirst();
		pDescs = (SHostDesc *)realloc(pDescs, nHosts*sizeof(SHostDesc));
		SHostDesc *pThisDesc = pDescs;
		while (!IsEOF())
		{
			pThisDesc->Init();
			pThisDesc->ID = m_ID;

			++pThisDesc;
			MoveNext();
		}
	}

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectRecordset::NewSubnet(CSubnet *pSubnet)
{
	if (!IsOpen() && !Open())
		return LOGICAL_FALSE;

	AddNew();
	m_OBJECT_TYPE = NET_OBJECT_TYPE_SUBNET;
	Update();

	// Get newly assigned ID and set it to subnet
	if (GetLastObjectID(m_ID, NET_OBJECT_TYPE_SUBNET) != LOGICAL_TRUE)
		return LOGICAL_FALSE;
	pSubnet->SetDatabaseID(m_ID);

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectRecordset::NewHost(CHost *pHost)
{
	if (!IsOpen() && !Open())
		return LOGICAL_FALSE;

	AddNew();
	m_OBJECT_TYPE = NET_OBJECT_TYPE_HOST;
	Update();

	// Get newly assigned ID and set it to subnet
	if (GetLastObjectID(m_ID, NET_OBJECT_TYPE_HOST) != LOGICAL_TRUE)
		return LOGICAL_FALSE;
	pHost->SetDatabaseID(m_ID);

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectRecordset::GetLastObjectID(long &ID, long ObjectType)
{
	if (!IsOpen() && !Open())
		return LOGICAL_FALSE;

	m_strFilter = "OBJECT_TYPE = ?";
	m_Param1 = ObjectType;
	m_strSort = "ID";
	Requery();
	if (IsEOF())
		return LOGICAL_FALSE;
	MoveLast();
	ID = m_ID;

	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetObjectPropertiesRecordset

IMPLEMENT_DYNAMIC(CNetObjectPropertiesRecordset, COdbcRecordset)

CNetObjectPropertiesRecordset::CNetObjectPropertiesRecordset(CDatabase *pdb)
	: COdbcRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CNetObjectPropertiesRecordset)
	m_OBJECT_ID = 0;
	m_PROPERTY_ID = 0;
	m_VALUE_STR = _T("");
	m_nFields = 3;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams = 1;
}


CString CNetObjectPropertiesRecordset::GetDefaultSQL()
{
	return _T("[ObjectProperties]");
}

void CNetObjectPropertiesRecordset::DoFieldExchange(CFieldExchange *pFX)
{
	//{{AFX_FIELD_MAP(CNetObjectPropertiesRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[OBJECT_ID]"), m_OBJECT_ID);
	RFX_Long(pFX, _T("[PROPERTY_ID]"), m_PROPERTY_ID);
	RFX_Text(pFX, _T("[VALUE_STR]"), m_VALUE_STR);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
}

LOGICAL CNetObjectPropertiesRecordset::LoadProperties(SSubnetDesc *pDesc)
{
	m_strFilter = "OBJECT_ID = ?";
	m_Param1 = pDesc->ID;
	if (!Requery())
		return LOGICAL_FALSE;

	while (!IsEOF())
	{
		char *pszValue = (char *)LPCTSTR(m_VALUE_STR);
		switch (m_PROPERTY_ID)
		{
		case NET_OBJECT_PROPERTY_GIVEN_NAME:
			strcpy(pDesc->szGivenName, pszValue);
			break;
		case NET_OBJECT_PROPERTY_IP_ADDRESS:
			sscanf(pszValue, "%ld", &pDesc->uIpAddress);
			break;
		case NET_OBJECT_PROPERTY_SUBNET_MASK:
			sscanf(pszValue, "%ld", &pDesc->uSubnetMask);
			break;
		case NET_OBJECT_PROPERTY_IFACE_MAC:
			strcpy(pDesc->szIfaceMacAddress, pszValue);
			break;
		case NET_OBJECT_PROPERTY_FLAGS:
			sscanf(pszValue, "%ld", &pDesc->flags);
			pDesc->flags &= ~SELECTED_FLAG; // switch selection off if set
			break;
		case NET_OBJECT_PROPERTY_DISTANCE:
			sscanf(pszValue, "%ld", &pDesc->nDistanceInHops);
			break;
		case NET_OBJECT_PROPERTY_SUBNET_COLOR_SCHEME:
			unsigned int uiColor, uiTextColor;
			sscanf(pszValue, "%lu;%lu", &uiColor, &uiTextColor);
			pDesc->Color.Init(uiColor);
			pDesc->TextColor.Init(uiTextColor);
			break;
		} //end of switch

		MoveNext();
	}
	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::LoadProperties(SHostDesc *pDesc)
{
	m_strFilter = "OBJECT_ID = ?";
	m_Param1 = pDesc->ID;
	if (!Requery())
		return LOGICAL_FALSE;

	while (!IsEOF())
	{
		char *pszValue = (char *)LPCTSTR(m_VALUE_STR);
		switch (m_PROPERTY_ID)
		{
		case NET_OBJECT_PROPERTY_GIVEN_NAME:
			strcpy(pDesc->szGivenName, pszValue);
			break;
		case NET_OBJECT_PROPERTY_OWN_NAME:
			strcpy(pDesc->szName, pszValue);
			break;
		case NET_OBJECT_PROPERTY_IP_ADDRESS:
			sscanf(pszValue, "%ld", &pDesc->uIpAddress);
			break;
		case NET_OBJECT_PROPERTY_SUBNET_MASK:
			sscanf(pszValue, "%ld", &pDesc->uSubnetMask);
			break;
		case NET_OBJECT_PROPERTY_IFACE_MAC:
			strcpy(pDesc->szIfaceMacAddress, pszValue);
			break;
		case NET_OBJECT_PROPERTY_FLAGS:
			sscanf(pszValue, "%ld", &pDesc->flags);
			pDesc->flags &= ~SELECTED_FLAG; // switch selection off if set
			break;
		case NET_OBJECT_PROPERTY_DISTANCE:
			sscanf(pszValue, "%ld", &pDesc->nDistanceInHops);
			break;
		case NET_OBJECT_PROPERTY_OWN_MAC:
			strcpy(pDesc->szOwnMacAddress, pszValue);
			break;
		case NET_OBJECT_PROPERTY_NETBIOS_DOMAIN_NAME:
			strcpy(pDesc->szNetBiosDomainName, pszValue);
			break;
		case NET_OBJECT_PROPERTY_SERVICES_FLAGS:
			sscanf(pszValue, "%ld", &pDesc->ServicesFlags);
			break;
		} //end of switch

		MoveNext();
	}
	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::StoreStringValue(char *pszValue, LOGICAL &bStored)
{
	if (m_VALUE_STR.Compare(pszValue))
	{
		Edit();
		m_VALUE_STR = pszValue;
		bStored = Update();
	}
	else
		bStored = LOGICAL_TRUE;

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::StoreLongValue(long Value, LOGICAL &bStored)
{
	char *pszValue = (char *)LPCTSTR(m_VALUE_STR);
	long CurrentValue = 0;
	sscanf(pszValue, "%ld", &CurrentValue);
	if (CurrentValue != Value)
	{
		Edit();
		m_VALUE_STR.Format("%ld", Value);
		bStored = Update();
	}
	else
		bStored = LOGICAL_TRUE;

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::AddStringValue(long ObjectID, long PropertyID, char *pszValue, LOGICAL bAdded)
{
	if (strlen(pszValue) > 0)
	{
		AddNew();
		m_OBJECT_ID = ObjectID;
		m_PROPERTY_ID = PropertyID;
		m_VALUE_STR = pszValue;
		bAdded = Update();
	}
	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::AddLongValue(long ObjectID, long PropertyID, long uValue, LOGICAL bAdded)
{
	AddNew();
	m_OBJECT_ID = ObjectID;
	m_PROPERTY_ID = PropertyID;
	m_VALUE_STR.Format("%ld", uValue);
	bAdded = Update();
	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::AddGivenName(long ID, char *pszName, LOGICAL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_GIVEN_NAME, pszName, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddOwnName(long ID, char *pszName, LOGICAL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_OWN_NAME, pszName, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddNetBiosDomainName(long ID, char *pszName, LOGICAL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_NETBIOS_DOMAIN_NAME, pszName, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddIpAddress(long ID, long uIP, LOGICAL bAdded)
{
	return AddLongValue(ID, NET_OBJECT_PROPERTY_IP_ADDRESS, uIP, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddSubnetMask(long ID, long uMask, LOGICAL bAdded)
{
	return AddLongValue(ID, NET_OBJECT_PROPERTY_SUBNET_MASK, uMask, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddIfaceMacAddress(long ID, char *pszAddress, LOGICAL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_IFACE_MAC, pszAddress, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddOwnMacAddress(long ID, char *pszAddress, LOGICAL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_OWN_MAC, pszAddress, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddFlags(long ID, long flags, LOGICAL bAdded)
{
	if (flags == 0)
		return LOGICAL_TRUE; // skip creation if flags are empty
	return AddLongValue(ID, NET_OBJECT_PROPERTY_FLAGS, flags, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddServicesFlags(long ID, long flags, LOGICAL bAdded)
{
	if (flags == 0)
		return LOGICAL_TRUE; // skip creation if flags are empty
	return AddLongValue(ID, NET_OBJECT_PROPERTY_SERVICES_FLAGS, flags, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddDistance(long ID, long distance, LOGICAL bAdded)
{
	return AddLongValue(ID, NET_OBJECT_PROPERTY_DISTANCE, distance, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddSubnetColorScheme(long ID, char *pszValue, LOGICAL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_SUBNET_COLOR_SCHEME, pszValue, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::StoreProperties(CSubnet *pSubnet)
{
	m_strFilter = "OBJECT_ID = ?";
	m_Param1 = pSubnet->GetDatabaseID();
	if (!Requery())
		return LOGICAL_FALSE;

	LOGICAL bIFaceMacAddressUpdated = LOGICAL_FALSE;
	LOGICAL bIpAddressUpdated = LOGICAL_FALSE;
	LOGICAL bSubnetMaskUpdated = LOGICAL_FALSE;
	LOGICAL bGivenNameUpdated = LOGICAL_FALSE;
	LOGICAL bFlagsUpdated = LOGICAL_FALSE;
	LOGICAL bDistanceUpdated = LOGICAL_FALSE;
	LOGICAL bColorSchemeUpdated = LOGICAL_FALSE;

	while (!IsEOF())
	{
		char *pszValue = (char *)LPCTSTR(m_VALUE_STR);
		switch (m_PROPERTY_ID)
		{
		case NET_OBJECT_PROPERTY_GIVEN_NAME:
			StoreStringValue(pSubnet->GetGivenName(), bGivenNameUpdated);
			break;
		case NET_OBJECT_PROPERTY_IP_ADDRESS:
			StoreLongValue(pSubnet->GetIpAddress(), bIpAddressUpdated);
			break;
		case NET_OBJECT_PROPERTY_SUBNET_MASK:
			StoreLongValue(pSubnet->GetSubnetMask(), bSubnetMaskUpdated);
			break;
		case NET_OBJECT_PROPERTY_IFACE_MAC:
			StoreStringValue(pSubnet->GetIfaceMacAddress(), bIFaceMacAddressUpdated);
			break;
		case NET_OBJECT_PROPERTY_FLAGS:
			StoreLongValue(pSubnet->GetFlags(), bFlagsUpdated);
			break;
		case NET_OBJECT_PROPERTY_DISTANCE:
			StoreLongValue(pSubnet->GetDistance(), bDistanceUpdated);
			break;
		case NET_OBJECT_PROPERTY_SUBNET_COLOR_SCHEME:
			StoreStringValue(pSubnet->GetColorSchemeDbString(), bColorSchemeUpdated);
			break;
		} //end of switch

		MoveNext();
	}

	if (!bGivenNameUpdated)
		AddGivenName(pSubnet->GetDatabaseID(), pSubnet->GetGivenName(), bGivenNameUpdated);

	if (!bIpAddressUpdated)
		AddIpAddress(pSubnet->GetDatabaseID(), pSubnet->GetIpAddress(), bIpAddressUpdated);

	if (!bSubnetMaskUpdated)
		AddSubnetMask(pSubnet->GetDatabaseID(), pSubnet->GetSubnetMask(), bSubnetMaskUpdated);

	if (!bIFaceMacAddressUpdated)
		AddIfaceMacAddress(pSubnet->GetDatabaseID(), pSubnet->GetIfaceMacAddress(), !bIFaceMacAddressUpdated);
	
	if (!bFlagsUpdated)
		AddFlags(pSubnet->GetDatabaseID(), pSubnet->GetFlags(), bFlagsUpdated);

	if (!bDistanceUpdated)
		AddDistance(pSubnet->GetDatabaseID(), pSubnet->GetDistance(), bDistanceUpdated);

	if (!bColorSchemeUpdated)
		AddSubnetColorScheme(pSubnet->GetDatabaseID(), pSubnet->GetColorSchemeDbString(), bColorSchemeUpdated);

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::StoreProperties(CHost *pHost)
{
	m_strFilter = "OBJECT_ID = ?";
	m_Param1 = pHost->GetDatabaseID();
	if (!Requery())
		return LOGICAL_FALSE;

	LOGICAL bIFaceMacAddressUpdated = LOGICAL_FALSE;
	LOGICAL bOwnMacAddressUpdated = LOGICAL_FALSE;
	LOGICAL bIpAddressUpdated = LOGICAL_FALSE;
	LOGICAL bSubnetMaskUpdated = LOGICAL_FALSE;
	LOGICAL bGivenNameUpdated = LOGICAL_FALSE;
	LOGICAL bNetBiosDomainNameUpdated = LOGICAL_FALSE;
	LOGICAL bOwnNameUpdated = LOGICAL_FALSE;
	LOGICAL bFlagsUpdated = LOGICAL_FALSE;
	LOGICAL bServicesFlagsUpdated = LOGICAL_FALSE;
	LOGICAL bDistanceUpdated = LOGICAL_FALSE;
	while (!IsEOF())
	{
		char *pszValue = (char *)LPCTSTR(m_VALUE_STR);
		switch (m_PROPERTY_ID)
		{
		case NET_OBJECT_PROPERTY_GIVEN_NAME:
			StoreStringValue(pHost->GetGivenName(), bGivenNameUpdated);
			break;
		case NET_OBJECT_PROPERTY_OWN_NAME:
			StoreStringValue(pHost->GetOwnName(), bOwnNameUpdated);
			break;
		case NET_OBJECT_PROPERTY_IP_ADDRESS:
			StoreLongValue(pHost->GetIpAddress(), bIpAddressUpdated);
			break;
		case NET_OBJECT_PROPERTY_SUBNET_MASK:
			StoreLongValue(pHost->GetSubnetMask(), bSubnetMaskUpdated);
			break;
		case NET_OBJECT_PROPERTY_IFACE_MAC:
			StoreStringValue(pHost->GetIfaceMacAddress(), bIFaceMacAddressUpdated);
			break;
		case NET_OBJECT_PROPERTY_FLAGS:
			StoreLongValue(pHost->GetFlags(), bFlagsUpdated);
			break;
		case NET_OBJECT_PROPERTY_DISTANCE:
			StoreLongValue(pHost->GetDistance(), bDistanceUpdated);
			break;
		case NET_OBJECT_PROPERTY_OWN_MAC:
			StoreStringValue(pHost->GetMacAddress(), bOwnMacAddressUpdated);
			break;
		case NET_OBJECT_PROPERTY_NETBIOS_DOMAIN_NAME:
			StoreStringValue(pHost->GetNetBiosDomainName(), bNetBiosDomainNameUpdated);
			break;
		case NET_OBJECT_PROPERTY_SERVICES_FLAGS:
			StoreLongValue(pHost->GetServicesFlags(), bServicesFlagsUpdated);
			break;
		} //end of switch

		MoveNext();
	}

	if (!bGivenNameUpdated)
		AddGivenName(pHost->GetDatabaseID(), pHost->GetGivenName(), bGivenNameUpdated);

	if (!bOwnNameUpdated)
		AddOwnName(pHost->GetDatabaseID(), pHost->GetOwnName(), bOwnNameUpdated);

	if (!bNetBiosDomainNameUpdated)
		AddNetBiosDomainName(pHost->GetDatabaseID(), pHost->GetNetBiosDomainName(), bNetBiosDomainNameUpdated);

	if (!bIpAddressUpdated)
		AddIpAddress(pHost->GetDatabaseID(), pHost->GetIpAddress(), bIpAddressUpdated);

	if (!bSubnetMaskUpdated)
		AddSubnetMask(pHost->GetDatabaseID(), pHost->GetSubnetMask(), bSubnetMaskUpdated);

	if (!bIFaceMacAddressUpdated)
		AddIfaceMacAddress(pHost->GetDatabaseID(), pHost->GetIfaceMacAddress(), bIFaceMacAddressUpdated);

	if (!bFlagsUpdated)
		AddFlags(pHost->GetDatabaseID(), pHost->GetFlags(), bFlagsUpdated);

	if (!bDistanceUpdated)
		AddDistance(pHost->GetDatabaseID(), pHost->GetDistance(), bDistanceUpdated);

	if (!bOwnMacAddressUpdated)
		AddOwnMacAddress(pHost->GetDatabaseID(), pHost->GetMacAddress(), bOwnMacAddressUpdated);

	if (!bServicesFlagsUpdated)
		AddServicesFlags(pHost->GetDatabaseID(), pHost->GetServicesFlags(), bServicesFlagsUpdated);

	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetObjectPropertiesRecordset diagnostics

#ifdef _DEBUG
void CNetObjectPropertiesRecordset::AssertValid() const
{
	COdbcRecordset::AssertValid();
}

void CNetObjectPropertiesRecordset::Dump(CDumpContext &dc) const
{
	COdbcRecordset::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CTrafficRecordset

IMPLEMENT_DYNAMIC(CTrafficRecordset, COdbcRecordset)

CTrafficRecordset::CTrafficRecordset(CDatabase *pdb)
	: COdbcRecordset(pdb)
{
	m_FROM_TIME = CTime::GetCurrentTime();
	m_TO_TIME = m_FROM_TIME;
	//{{AFX_FIELD_INIT(CTrafficRecordset)
	m_SRC_ID = 0;
	m_DST_ID = 0;
	m_PROTO = 0;
	m_PORT_TYPE = 0;
	m_PACKETS = 0;
	m_OCTETS = 0;
	m_nFields = 8;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams = 8;
	m_Param3 = m_Param4 = m_FROM_TIME;
	m_Param5 = m_Param6 = m_FROM_TIME;
	m_Param7 = m_Param8 = m_FROM_TIME;
}


CString CTrafficRecordset::GetDefaultSQL()
{
	return _T("[Traffic]");
}

void CTrafficRecordset::DoFieldExchange(CFieldExchange *pFX)
{
	//{{AFX_FIELD_MAP(CTrafficRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Date(pFX, _T("[FROM_TIME]"), m_FROM_TIME);
	RFX_Date(pFX, _T("[TO_TIME]"), m_TO_TIME);
	RFX_Long(pFX, _T("[SRC_ID]"), m_SRC_ID);
	RFX_Long(pFX, _T("[DST_ID]"), m_DST_ID);
	RFX_Long(pFX, _T("[PROTO]"), m_PROTO);
	RFX_Long(pFX, _T("[PORT_TYPE]"), m_PORT_TYPE);
	RFX_Long(pFX, _T("[PACKETS]"), m_PACKETS);
	RFX_Long(pFX, _T("[OCTETS]"), m_OCTETS);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
	RFX_Long(pFX,"", m_Param2);
	RFX_Date(pFX,"", m_Param3);
	RFX_Date(pFX,"", m_Param4);
	RFX_Date(pFX,"", m_Param5);
	RFX_Date(pFX,"", m_Param6);
	RFX_Date(pFX,"", m_Param7);
	RFX_Date(pFX,"", m_Param8);
}

LOGICAL CTrafficRecordset::NewRecord(STrafficRecord *pRecord)
{
	if (!IsOpen() && !Open())
		return LOGICAL_FALSE;

	AddNew();
	m_FROM_TIME = CTime(pRecord->FromTime);
	m_TO_TIME = CTime(pRecord->ToTime);
	m_SRC_ID = pRecord->pSrcHost->GetDatabaseID();
	m_DST_ID = -1;
	if (pRecord->pDstHost)
		m_DST_ID = pRecord->pDstHost->GetDatabaseID();
	m_PROTO = pRecord->proto;
	m_PORT_TYPE = pRecord->port;
	m_OCTETS = pRecord->octets;
	m_PACKETS = pRecord->packets;
	return Update();
}

LOGICAL CTrafficRecordset::Query(CHost *pHost, unsigned short proto, CTime StartTime, CTime EndTime, STrafficRecord *&pRecords, int &nRecords)
{
	nRecords = 0;
	m_strFilter += " AND PROTO = ? AND ( (FROM_TIME >= ? AND FROM_TIME <= ?) OR (TO_TIME >= ? AND TO_TIME <= ?) OR (FROM_TIME <= ? AND TO_TIME >= ?) )";
	m_Param1 = pHost->GetDatabaseID();
	m_Param2 = proto;
	m_Param3 = m_Param5 = m_Param7 = StartTime;
	m_Param4 = m_Param6 = m_Param8 = EndTime;
	Requery();
	if (IsEOF())
		return LOGICAL_TRUE;
	nRecords = CountRecords();

	if (nRecords > 0)
	{
		pRecords = (STrafficRecord *)realloc(pRecords, nRecords*sizeof(STrafficRecord));
		STrafficRecord *pThisRecord = pRecords;
		while (!IsEOF())
		{
			pThisRecord->FromTime = m_FROM_TIME.GetTime();
			pThisRecord->ToTime = m_TO_TIME.GetTime();
			if (pHost->GetDatabaseID() == m_SRC_ID)
				pThisRecord->pSrcHost = pHost;
			else
				pThisRecord->pSrcHost = NULL; // TODO - find appropriate pointer to be able to show traffic on host-by-host basis
			if (pHost->GetDatabaseID() == m_DST_ID)
				pThisRecord->pDstHost = pHost;
			else
				pThisRecord->pDstHost = NULL; // TODO - find appropriate pointer to be able to show traffic on host-by-host basis
			pThisRecord->proto = m_PROTO;
			pThisRecord->port = m_PORT_TYPE;
			double duration = pThisRecord->ToTime - pThisRecord->FromTime;
			pThisRecord->packets = m_PACKETS;
			pThisRecord->packets_per_second = pThisRecord->packets/duration;
			pThisRecord->octets = m_OCTETS;
			pThisRecord->octets_per_second = pThisRecord->octets/duration;
			
			++pThisRecord;			
			MoveNext();
		}
	}

	return LOGICAL_TRUE;
}

LOGICAL CTrafficRecordset::QueryTrafficFromHost(CHost *pHost, unsigned short proto, CTime StartTime, CTime EndTime, STrafficRecord *&pRecords, int &nRecords)
{
	m_strFilter = "SRC_ID = ?";
	return Query(pHost, proto, StartTime, EndTime, pRecords, nRecords);
}

LOGICAL CTrafficRecordset::QueryTrafficToHost(CHost *pHost, unsigned short proto, CTime StartTime, CTime EndTime, STrafficRecord *&pRecords, int &nRecords)
{
	m_strFilter = "DST_ID = ?";
	return Query(pHost, proto, StartTime, EndTime, pRecords, nRecords);
}

/////////////////////////////////////////////////////////////////////////////
// CTrafficRecordset diagnostics

#ifdef _DEBUG
void CTrafficRecordset::AssertValid() const
{
	COdbcRecordset::AssertValid();
}

void CTrafficRecordset::Dump(CDumpContext &dc) const
{
	COdbcRecordset::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CGroupsRecordset

IMPLEMENT_DYNAMIC(CGroupsRecordset, COdbcRecordset)

CGroupsRecordset::CGroupsRecordset(CDatabase *pdb)
	: COdbcRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CGroupsRecordset)
	m_ID = 0;
	m_NAME = _T("");
	m_TYPE = 0;
	m_IS_DEFAULT = 0;
	m_PARAMS = _T("");
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams = 1;
}

CString CGroupsRecordset::GetDefaultSQL()
{
	return _T("[Groups]");
}

void CGroupsRecordset::DoFieldExchange(CFieldExchange *pFX)
{
	//{{AFX_FIELD_MAP(CGroupsRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[NAME]"), m_NAME);
	RFX_Long(pFX, _T("[TYPE]"), m_TYPE);
	RFX_Long(pFX, _T("[IS_DEFAULT]"), m_IS_DEFAULT);
	RFX_Text(pFX, _T("[PARAMS]"), m_PARAMS);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
}

LOGICAL CGroupsRecordset::QueryGroups(SGroupDesc *&pGroups, int &nGroups)
{
	m_strFilter.Empty(); // all groups
	Requery();
	// Count groups
	nGroups = 0;
	while (!IsEOF())
	{
		++nGroups;
		MoveNext();
	}

	if (nGroups > 0)
	{
		MoveFirst();
		pGroups = (SGroupDesc *)realloc(pGroups, nGroups*sizeof(SGroupDesc));
		SGroupDesc *pThisDesc = pGroups;
		while (!IsEOF())
		{
			pThisDesc->ID = m_ID;
			pThisDesc->Type = m_TYPE;
			pThisDesc->bDefault = m_IS_DEFAULT;
			strcpy(pThisDesc->szName, m_NAME);
			strcpy(pThisDesc->szParams, m_PARAMS);

			++pThisDesc;
			MoveNext();
		}
	}

	return LOGICAL_TRUE;
}

LOGICAL CGroupsRecordset::GroupChanged(CGroup *pGroup)
{
	m_strFilter = "ID = ?";
	m_Param1 = pGroup->GetID();

	LOGICAL bSuccess = Requery();
	if (IsEOF())
	{
		AddNew();
		m_TYPE = pGroup->GetType();
		m_NAME = pGroup->GetName();
		m_IS_DEFAULT = pGroup->IsDefault();
		m_PARAMS = pGroup->GetUserParamValuesStringDB();
		bSuccess = Update();

		if (bSuccess)
		{
			long ID;
			bSuccess = GetLastGroupID(ID);
			if (bSuccess)
				pGroup->SetID(ID);
		}
	}
	else
	{
		Edit();
		m_NAME = pGroup->GetName();
		m_PARAMS = pGroup->GetUserParamValuesStringDB();
		bSuccess = Update();
	}
	return bSuccess;
}

LOGICAL CGroupsRecordset::GetLastGroupID(long &ID)
{
	if (!IsOpen() && !Open())
		return LOGICAL_FALSE;

	m_strFilter.Empty();
	m_strSort = "ID";
	Requery();
	if (IsEOF())
		return LOGICAL_FALSE;
	MoveLast();
	ID = m_ID;

	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGroupsRecordset diagnostics

#ifdef _DEBUG
void CGroupsRecordset::AssertValid() const
{
	COdbcRecordset::AssertValid();
}

void CGroupsRecordset::Dump(CDumpContext &dc) const
{
	COdbcRecordset::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CGroupingRecordset

IMPLEMENT_DYNAMIC(CGroupingRecordset, COdbcRecordset)

CGroupingRecordset::CGroupingRecordset(CDatabase *pdb)
	: COdbcRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CGroupingRecordset)
	m_HOST_ID = 0;
	m_GROUP_ID = 0;
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams = 2;
}

CString CGroupingRecordset::GetDefaultSQL()
{
	return _T("[Grouping]");
}

void CGroupingRecordset::DoFieldExchange(CFieldExchange *pFX)
{
	//{{AFX_FIELD_MAP(CGroupingRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[HOST_ID]"), m_HOST_ID);
	RFX_Long(pFX, _T("[GROUP_ID]"), m_GROUP_ID);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
	RFX_Long(pFX,"", m_Param2);
}

LOGICAL CGroupingRecordset::Load(CHost *pHost)
{
	m_strFilter = "HOST_ID = ?";
	m_Param1 = pHost->GetDatabaseID();
	if (!Requery())
		return LOGICAL_FALSE;

	while (!IsEOF())
	{
		pHost->SetGroup(m_GROUP_ID, LOGICAL_FALSE);
		MoveNext();
	}

	return LOGICAL_TRUE;
}

LOGICAL CGroupingRecordset::Store(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup)
{
	m_strFilter = "HOST_ID = ? AND GROUP_ID = ?";
	m_Param1 = pHost->GetDatabaseID();
	m_Param2 = pOldGroup->GetID();
	if (!Requery())
		return LOGICAL_FALSE;
	if (IsEOF())
	{
		AddNew();
		m_HOST_ID = pHost->GetDatabaseID();
	}
	else
		Edit();
	m_GROUP_ID = pNewGroup->GetID();

	return Update();
}

/////////////////////////////////////////////////////////////////////////////
// CGroupingRecordset diagnostics

#ifdef _DEBUG
void CGroupingRecordset::AssertValid() const
{
	COdbcRecordset::AssertValid();
}

void CGroupingRecordset::Dump(CDumpContext& dc) const
{
	COdbcRecordset::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// COdbcDatabase

const char *pszRegistrySectionName = "OdbcDatabase";
const char *pszRegistryConnectionString = "ConnectionString";

COdbcDatabase::COdbcDatabase()
{
	m_pSubnetDescs = NULL;
	m_nSubnets = 0;
	m_bSubnetsDirty = LOGICAL_TRUE; // requery is required

	m_pHostDescs = NULL;
	m_nHosts = 0;
	m_bHostsDirty = LOGICAL_TRUE; // requery is required

	m_pTrafficRecords = NULL;
	m_nTrafficRecords = 0;
}

//virtual
COdbcDatabase::~COdbcDatabase()
{
	if (m_pSubnetDescs)
		free(m_pSubnetDescs);
	if (m_pHostDescs)
		free(m_pHostDescs);
	if (m_pTrafficRecords)
		free(m_pTrafficRecords);
}
	
//virtual
LOGICAL COdbcDatabase::Create()
{
	CStackCriticalSection section(&m_CriticalSection, "ODBC database");
	LOGICAL bSuccess = LOGICAL_FALSE;
	m_ConnectionString = "";//AfxGetApp()->GetProfileString(pszRegistrySectionName, pszRegistryConnectionString, "");
	if (m_ConnectionString.IsEmpty())
	{ // Open ODBC dialog
		bSuccess = Open(NULL);
	}
	else
	{
		bSuccess = Open(m_ConnectionString);
	}
	
	if (bSuccess)
	{
		CreateTables();
	}
	else
	{
		return LOGICAL_FALSE;
	}

	m_ConnectionString = GetConnect();
	AfxGetApp()->WriteProfileString(pszRegistrySectionName, pszRegistryConnectionString, m_ConnectionString);

	return LOGICAL_TRUE;
}

//virtual
void COdbcDatabase::StoreTrafficRecord(STrafficRecord *pRecord)
{
	CStackCriticalSection section(&m_CriticalSection, "Storing traffic...");
	CTrafficRecordset set(this);
	TRY
	{
		set.NewRecord(pRecord);
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при записи трафика", MB_ICONEXCLAMATION);
	END_CATCH
}

//virtual
void COdbcDatabase::MergeHosts(SHostDesc *pDstDesc, SHostDesc *pSrcDesc)
{
	CStackCriticalSection section(&m_CriticalSection, "Host merging...");
	TRY
	{
		CString sql;
		// Traffic
		sql.Format("UPDATE TRAFFIC SET SRC_ID = %ld WHERE SRC_ID = %ld", pDstDesc->ID, pSrcDesc->ID);
		ExecuteSQL(sql);
		sql.Format("UPDATE TRAFFIC SET DST_ID = %ld WHERE DST_ID = %ld", pDstDesc->ID, pSrcDesc->ID);
		ExecuteSQL(sql);
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при Host merging", MB_ICONEXCLAMATION);
	END_CATCH
}

//virtual
void COdbcDatabase::DeleteHost(unsigned long ID)
{
	CStackCriticalSection section(&m_CriticalSection, "Host deleting...");
	TRY
	{
		CString sql;
		sql.Format("DELETE FROM NetObjects WHERE ID = %ld", ID);
		ExecuteSQL(sql);
/*		CNetObjectRecordset set(this);
		set.m_strFilter = "ID = ?";
		set.m_Param1 = ID;
		set.Requery();
		while (!set.IsEOF())
		{
			set.Delete();
		}*/
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при Host deleting", MB_ICONEXCLAMATION);
	END_CATCH
}

//virtual
LOGICAL COdbcDatabase::CreateTables()
{
	LOGICAL bSuccess = LOGICAL_TRUE;

	bSuccess |=	CreateObjectsTable();
	bSuccess |=	CreateObjectPropertiesTable();
	bSuccess |=	CreateEventLogTable();
	bSuccess |=	CreateStringTable();
	bSuccess |= CreateTrafficTable();
	bSuccess |= CreateGroupsTable();
	bSuccess |= CreateGroupingTable();

	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::CreateObjectsTable()
{
	if (!IsOpen())
		return LOGICAL_FALSE;
	LOGICAL bSuccess = LOGICAL_TRUE;

	TRY
	{
		ExecuteSQL("CREATE TABLE NetObjects	(				\
					ID				COUNTER,				\
					OBJECT_TYPE		INTEGER		NOT NULL,	\
					PRIMARY KEY(ID))");
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблицы NetObjects", MB_ICONEXCLAMATION);
		bSuccess = LOGICAL_FALSE;
	END_CATCH
	
	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::CreateObjectPropertiesTable()
{
	if (!IsOpen())
		return LOGICAL_FALSE;
	LOGICAL bSuccess = LOGICAL_TRUE;

	TRY
	{
		ExecuteSQL("CREATE TABLE ObjectProperties					\
					(												\
					OBJECT_ID		INTEGER		NOT NULL,			\
					PROPERTY_ID		INTEGER		NOT NULL,			\
					VALUE_STR		VARCHAR(128),					\
					FOREIGN KEY (OBJECT_ID) REFERENCES NetObjects (ID) ON DELETE CASCADE	\
					)");

	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблицы ObjectProperties", MB_ICONEXCLAMATION);
		bSuccess = LOGICAL_FALSE;
	END_CATCH
	
	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::CreateEventLogTable()
{
	if (!IsOpen())
		return LOGICAL_FALSE;
	LOGICAL bSuccess = LOGICAL_TRUE;

	TRY
	{
		ExecuteSQL("CREATE TABLE EventLog					\
					(										\
					EVENT_ID		COUNTER,				\
					EVENT_TYPE		INTEGER,				\
					OBJECT_ID		INTEGER		NOT NULL,	\
					EVENT_TIME		DATETIME,				\
					COMMENT_ID		INTEGER,				\
					PRIMARY KEY(EVENT_TYPE, EVENT_TIME),	\
					FOREIGN KEY (OBJECT_ID) REFERENCES NetObjects (ID) ON DELETE CASCADE	\
					)										\
				");
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблицы EventLog", MB_ICONEXCLAMATION);
		bSuccess = LOGICAL_FALSE;
	END_CATCH
	
	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::CreateStringTable()
{
	if (!IsOpen())
		return LOGICAL_FALSE;
	LOGICAL bSuccess = LOGICAL_TRUE;

	TRY
	{
		ExecuteSQL("CREATE TABLE Strings	(			\
				ID				COUNTER	,				\
				Content			VARCHAR(128),			\
				NextChunkID		INTEGER					\
				)										\
				");												  
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблицы Strings", MB_ICONEXCLAMATION);
		bSuccess = LOGICAL_FALSE;
	END_CATCH

	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::CreateTrafficTable()
{
	if (!IsOpen())
		return LOGICAL_FALSE;
	LOGICAL bSuccess = LOGICAL_TRUE;

	TRY
	{
		ExecuteSQL("CREATE TABLE Traffic					\
					(										\
					FROM_TIME		DATETIME,				\
					TO_TIME			DATETIME,				\
					SRC_ID			INTEGER		NOT NULL,	\
					DST_ID			INTEGER		NOT NULL,	\
					PROTO			INTEGER		NOT NULL,				\
					PORT_TYPE		INTEGER		NOT NULL,				\
					PACKETS			INTEGER		NOT NULL,				\
					OCTETS			INTEGER		NOT NULL,				\
					FOREIGN KEY (SRC_ID) REFERENCES NetObjects (ID) ON DELETE CASCADE	\
					)										\
				");
		ExecuteSQL("CREATE INDEX  FROM_TIME_INDEX_ID ON Traffic (FROM_TIME)");
		ExecuteSQL("CREATE INDEX  TO_TIME_INDEX_ID ON Traffic (TO_TIME)");
		ExecuteSQL("CREATE INDEX  SRC_INDEX_ID ON Traffic (SRC_ID)");
		ExecuteSQL("CREATE INDEX  DST_INDEX_ID ON Traffic (DST_ID)");
		ExecuteSQL("CREATE INDEX  PROTO_INDEX_ID ON Traffic (PROTO)");
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблицы Traffic", MB_ICONEXCLAMATION);
		bSuccess = LOGICAL_FALSE;
	END_CATCH

	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::CreateGroupsTable()
{
	if (!IsOpen())
		return LOGICAL_FALSE;
	LOGICAL bSuccess = LOGICAL_TRUE;

	TRY
	{
		ExecuteSQL("CREATE TABLE Groups					\
					(												\
					ID				COUNTER,			\
					NAME			VARCHAR(128),					\
					TYPE			INTEGER		NOT NULL,			\
					IS_DEFAULT		INTEGER		NOT NULL,			\
					PARAMS			VARCHAR(255)					\
					)");
		ExecuteSQL("CREATE INDEX  GROUP_ID_INDEX_ID ON Groups (ID)");
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблицы Groups", MB_ICONEXCLAMATION);
		bSuccess = LOGICAL_FALSE;
	END_CATCH

	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::CreateGroupingTable()
{
	if (!IsOpen())
		return LOGICAL_FALSE;
	LOGICAL bSuccess = LOGICAL_TRUE;

	TRY
	{
		ExecuteSQL("CREATE TABLE Grouping					\
					(										\
					HOST_ID		INTEGER		NOT NULL,		\
					GROUP_ID	INTEGER		NOT NULL		\
					)");
		ExecuteSQL("CREATE INDEX  HOST_ID_INDEX_ID ON Grouping (HOST_ID)");
		ExecuteSQL("CREATE INDEX  GROUP_ID_INDEX_ID ON Grouping (GROUP_ID)");
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблицы Grouping", MB_ICONEXCLAMATION);
		bSuccess = LOGICAL_FALSE;
	END_CATCH

	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets)
{
	CStackCriticalSection section(&m_CriticalSection, "Query Subnets");
	if (m_bSubnetsDirty)
	{
		CNetObjectRecordset SubnetSet(this);
		if (SubnetSet.QuerySubnets(m_pSubnetDescs, m_nSubnets) != LOGICAL_TRUE)
			return LOGICAL_FALSE;

		// We have got subnet list, fill property fields now
		CNetObjectPropertiesRecordset PropertySet(this);
		for (int i = 0; i < m_nSubnets; i++)
		{
			SSubnetDesc *pThisDesc = m_pSubnetDescs+i;
			PropertySet.LoadProperties(pThisDesc);
		}
		m_bSubnetsDirty = LOGICAL_FALSE;
	}
	pSubnetDescs = m_pSubnetDescs;
	nSubnets = m_nSubnets;

	return LOGICAL_TRUE;
}

//virtual
LOGICAL COdbcDatabase::QueryHosts(SHostDesc *&pHostDescs, int &nHosts)
{
	CStackCriticalSection section(&m_CriticalSection, "Query hosts");
	if (m_bHostsDirty)
	{
		CNetObjectRecordset HostSet(this);
		if (HostSet.QueryHosts(m_pHostDescs, m_nHosts) != LOGICAL_TRUE)
			return LOGICAL_FALSE;

		// We have got subnet list, fill property fields now
		CNetObjectPropertiesRecordset PropertySet(this);
		for (int i = 0; i < m_nHosts; i++)
		{
			SHostDesc *pThisDesc = m_pHostDescs+i;
			PropertySet.LoadProperties(pThisDesc);
		}
		m_bHostsDirty = LOGICAL_FALSE;
	}
	pHostDescs = m_pHostDescs;
	nHosts = m_nHosts;

	return LOGICAL_TRUE;
}

//virtual
SSubnetDesc *COdbcDatabase::FindSubnet(CSubnet *pSubnet)
{
	QuerySubnets(m_pSubnetDescs, m_nSubnets);
	for (int i = 0; i < m_nSubnets; i++)
	{
		SSubnetDesc *pDesc = m_pSubnetDescs+i;
		if (pSubnet->Compare(pDesc))
			return pDesc;
	}
	return NULL;
}

//virtual
SHostDesc *COdbcDatabase::FindHost(CHost *pHost)
{
	QueryHosts(m_pHostDescs, m_nHosts);
	for (int i = 0; i < m_nHosts; i++)
	{
		SHostDesc *pDesc = m_pHostDescs+i;
		if (pHost->Compare(pDesc))
			return pDesc;
	}
	return NULL;
}

//virtual
LOGICAL COdbcDatabase::QueryTrafficFromHost(CHost *pHost, unsigned short proto, time_t StartTime, time_t EndTime, STrafficRecord *&pRecords, int &nRecords)
{
	CStackCriticalSection section(&m_CriticalSection, "QueryTrafficFromHost");
	CTrafficRecordset set(this);
	LOGICAL bSuccess = set.QueryTrafficFromHost(pHost, proto, CTime(StartTime), CTime(EndTime), m_pTrafficRecords, nRecords);
	pRecords = m_pTrafficRecords;
	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::QueryTrafficToHost(CHost *pHost, unsigned short proto, time_t StartTime, time_t EndTime, STrafficRecord *&pRecords, int &nRecords)
{
	CStackCriticalSection section(&m_CriticalSection, "QueryTrafficToHost");
	CTrafficRecordset set(this);
	LOGICAL bSuccess = set.QueryTrafficToHost(pHost, proto, CTime(StartTime), CTime(EndTime), m_pTrafficRecords, nRecords);
	pRecords = m_pTrafficRecords;
	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::QueryGroups(SGroupDesc *&pGroups, int &nGroups)
{
	CStackCriticalSection section(&m_CriticalSection, "QueryGroups");
	CGroupsRecordset set(this);
	return set.QueryGroups(pGroups, nGroups);
}

//virtual
void COdbcDatabase::NewHostCreated(CHost *pHost)
{
	CStackCriticalSection section(&m_CriticalSection, "ODBC:NewHostCreated");
	SHostDesc *pDesc = FindHost(pHost);
	if (pDesc)
	{
		pHost->SetDatabaseID(pDesc->ID);
		return; // already in DB
	}
	// Add host record
	CNetObjectRecordset HostSet(this);
	HostSet.NewHost(pHost);
	// Store host properties
	CNetObjectPropertiesRecordset PropertiesSet(this);
	PropertiesSet.StoreProperties(pHost);
}

//virtual
void COdbcDatabase::HostChanged(CHost *pHost)
{
	CStackCriticalSection section(&m_CriticalSection, "ODBC:HostChanged");
	// Store host properties
	CNetObjectPropertiesRecordset PropertiesSet(this);
	PropertiesSet.StoreProperties(pHost);
}

//virtual
void COdbcDatabase::NewSubnetCreated(CSubnet *pSubnet)
{
	CStackCriticalSection section(&m_CriticalSection, "ODBC:SubnetCreated");
	SSubnetDesc *pDesc = FindSubnet(pSubnet);
	if (pDesc)
	{
		pSubnet->SetDatabaseID(pDesc->ID);
		return; // already in DB
	}
	// Add subnet record
	CNetObjectRecordset SubnetSet(this);
	SubnetSet.NewSubnet(pSubnet);
	// Store subnet properties
	CNetObjectPropertiesRecordset PropertiesSet(this);
	PropertiesSet.StoreProperties(pSubnet);
}

//virtual
void COdbcDatabase::SubnetChanged(CSubnet *pSubnet)
{
	CStackCriticalSection section(&m_CriticalSection, "ODBC:SubnetChanged");
	// Store subnet properties
	CNetObjectPropertiesRecordset PropertiesSet(this);
	PropertiesSet.StoreProperties(pSubnet);
}

//virtual
void COdbcDatabase::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	HostChanged(pHost);
	SubnetChanged(pSubnet);
}

//virtual
void COdbcDatabase::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
}

//virtual
void COdbcDatabase::NewGroupCreated(CGroup *pGroup)
{
	GroupChanged(pGroup);
}

//virtual
void COdbcDatabase::GroupChanged(CGroup *pGroup)
{
	CStackCriticalSection section(&m_CriticalSection, "Group");
	CGroupsRecordset set(this);
	set.GroupChanged(pGroup);
}

//virtual
void COdbcDatabase::HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup)
{
	CStackCriticalSection section(&m_CriticalSection, "Grouping");
	CGroupingRecordset set(this);
	set.Store(pHost, pOldGroup, pNewGroup);
}

//virtual
LOGICAL COdbcDatabase::LoadHostGrouping(CHost *pHost)
{
	CStackCriticalSection section(&m_CriticalSection, "Grouping");
	CGroupingRecordset set(this);
	return set.Load(pHost);
}

