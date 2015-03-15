/* OdbcDatabase.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	25 Oct 2006 - initial creation
	22 Nov 2006 - storing network objects and their properties in database
	28 Nov 2006 - loading network objects and their properties from database
	29 Nov 2006 - storing/loading flags of network objects in/from database
	20 May 2007 - traffic table

*/

#include "stdafx.h"
#include "OdbcDatabase.h"
#include "Network.h"

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
			pThisSubnetDesc->InitNetObject();
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
		} //end of switch

		MoveNext();
	}
	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::StoreStringValue(char *pszValue, BOOL &bStored)
{
	if (m_VALUE_STR.Compare(pszValue))
	{
		Edit();
		m_VALUE_STR = pszValue;
		bStored = Update();
	}
	else
		bStored = TRUE;

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::StoreLongValue(long Value, BOOL &bStored)
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
		bStored = TRUE;

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::AddStringValue(long ObjectID, long PropertyID, char *pszValue, BOOL bAdded)
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

LOGICAL CNetObjectPropertiesRecordset::AddLongValue(long ObjectID, long PropertyID, long uValue, BOOL bAdded)
{
	AddNew();
	m_OBJECT_ID = ObjectID;
	m_PROPERTY_ID = PropertyID;
	m_VALUE_STR.Format("%ld", uValue);
	bAdded = Update();
	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::AddGivenName(long ID, char *pszName, BOOL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_GIVEN_NAME, pszName, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddOwnName(long ID, char *pszName, BOOL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_OWN_NAME, pszName, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddIpAddress(long ID, long uIP, BOOL bAdded)
{
	return AddLongValue(ID, NET_OBJECT_PROPERTY_IP_ADDRESS, uIP, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddSubnetMask(long ID, long uMask, BOOL bAdded)
{
	return AddLongValue(ID, NET_OBJECT_PROPERTY_SUBNET_MASK, uMask, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddIfaceMacAddress(long ID, char *pszAddress, BOOL bAdded)
{
	return AddStringValue(ID, NET_OBJECT_PROPERTY_IFACE_MAC, pszAddress, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddFlags(long ID, long flags, BOOL bAdded)
{
	return AddLongValue(ID, NET_OBJECT_PROPERTY_FLAGS, flags, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::AddDistance(long ID, long distance, BOOL bAdded)
{
	return AddLongValue(ID, NET_OBJECT_PROPERTY_DISTANCE, distance, bAdded);
}

LOGICAL CNetObjectPropertiesRecordset::StoreProperties(CSubnet *pSubnet)
{
	m_strFilter = "OBJECT_ID = ?";
	m_Param1 = pSubnet->GetDatabaseID();
	if (!Requery())
		return LOGICAL_FALSE;

	BOOL bMacAddressUpdated = FALSE;
	BOOL bIpAddressUpdated = FALSE;
	BOOL bSubnetMaskUpdated = FALSE;
	BOOL bGivenNameUpdated = FALSE;
	BOOL bFlagsUpdated = FALSE;
	BOOL bDistanceUpdated = FALSE;

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
			StoreStringValue(pSubnet->GetIfaceMacAddress(), bMacAddressUpdated);
			break;
		case NET_OBJECT_PROPERTY_FLAGS:
			StoreLongValue(pSubnet->GetFlags(), bFlagsUpdated);
			break;
		case NET_OBJECT_PROPERTY_DISTANCE:
			StoreLongValue(pSubnet->GetDistance(), bDistanceUpdated);
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

	if (!bMacAddressUpdated)
		AddIfaceMacAddress(pSubnet->GetDatabaseID(), pSubnet->GetIfaceMacAddress(), !bMacAddressUpdated);
	
	if (!bFlagsUpdated)
		AddFlags(pSubnet->GetDatabaseID(), pSubnet->GetFlags(), bFlagsUpdated);

	if (!bDistanceUpdated)
		AddDistance(pSubnet->GetDatabaseID(), pSubnet->GetDistance(), bDistanceUpdated);

	return LOGICAL_TRUE;
}

LOGICAL CNetObjectPropertiesRecordset::StoreProperties(CHost *pHost)
{
	m_strFilter = "OBJECT_ID = ?";
	m_Param1 = pHost->GetDatabaseID();
	if (!Requery())
		return LOGICAL_FALSE;

	BOOL bMacAddressUpdated = FALSE;
	BOOL bIpAddressUpdated = FALSE;
	BOOL bSubnetMaskUpdated = FALSE;
	BOOL bGivenNameUpdated = FALSE;
	BOOL bOwnNameUpdated = FALSE;
	BOOL bFlagsUpdated = FALSE;
	BOOL bDistanceUpdated = FALSE;
	while (!IsEOF())
	{
		char *pszValue = (char *)LPCTSTR(m_VALUE_STR);
		switch (m_PROPERTY_ID)
		{
		case NET_OBJECT_PROPERTY_GIVEN_NAME:
			StoreStringValue(pHost->GetGivenName(), bGivenNameUpdated);
			break;
		case NET_OBJECT_PROPERTY_OWN_NAME:
			StoreStringValue(pHost->GetName(), bOwnNameUpdated);
			break;
		case NET_OBJECT_PROPERTY_IP_ADDRESS:
			StoreLongValue(pHost->GetIpAddress(), bIpAddressUpdated);
			break;
		case NET_OBJECT_PROPERTY_SUBNET_MASK:
			StoreLongValue(pHost->GetSubnetMask(), bSubnetMaskUpdated);
			break;
		case NET_OBJECT_PROPERTY_IFACE_MAC:
			StoreStringValue(pHost->GetIfaceMacAddress(), bMacAddressUpdated);
			break;
		case NET_OBJECT_PROPERTY_FLAGS:
			StoreLongValue(pHost->GetFlags(), bFlagsUpdated);
			break;
		case NET_OBJECT_PROPERTY_DISTANCE:
			StoreLongValue(pHost->GetDistance(), bDistanceUpdated);
			break;
		} //end of switch

		MoveNext();
	}

	if (!bGivenNameUpdated)
		AddGivenName(pHost->GetDatabaseID(), pHost->GetGivenName(), bGivenNameUpdated);

	if (!bOwnNameUpdated)
		AddOwnName(pHost->GetDatabaseID(), pHost->GetName(), bOwnNameUpdated);

	if (!bIpAddressUpdated)
		AddIpAddress(pHost->GetDatabaseID(), pHost->GetIpAddress(), bIpAddressUpdated);

	if (!bSubnetMaskUpdated)
		AddSubnetMask(pHost->GetDatabaseID(), pHost->GetSubnetMask(), bSubnetMaskUpdated);

	if (!bMacAddressUpdated)
		AddIfaceMacAddress(pHost->GetDatabaseID(), pHost->GetIfaceMacAddress(), bMacAddressUpdated);

	if (!bFlagsUpdated)
		AddFlags(pHost->GetDatabaseID(), pHost->GetFlags(), bFlagsUpdated);

	if (!bDistanceUpdated)
		AddDistance(pHost->GetDatabaseID(), pHost->GetDistance(), bDistanceUpdated);

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
// COdbcDatabase

const char *pszRegistrySectionName = "OdbcDatabase";
const char *pszRegistryConnectionString = "ConnectionString";

COdbcDatabase::COdbcDatabase()
{
	m_pSubnetDescs = NULL;
	m_nSubnets = 0;
	m_bSubnetsDirty = TRUE; // requery is required

	m_pHostDescs = NULL;
	m_nHosts = 0;
	m_bHostsDirty = TRUE; // requery is required
}

//virtual
COdbcDatabase::~COdbcDatabase()
{
	if (m_pSubnetDescs)
		free(m_pSubnetDescs);
	if (m_pHostDescs)
		free(m_pHostDescs);
}
	
//virtual
LOGICAL COdbcDatabase::Create()
{
	CStackCriticalSection section(&m_CriticalSection);
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

//	QuerySubnets(m_pSubnetDescs, m_nSubnets);
//	QueryHosts(m_pHostDescs, m_nHosts);
	return LOGICAL_TRUE;
}

//virtual
void COdbcDatabase::StoreTrafficRecord(STrafficRecord *pRecord)
{
	CStackCriticalSection section(&m_CriticalSection);
	TRY
	{
		CTrafficRecordset set(this);
		set.NewRecord(pRecord);
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при записи трафика", MB_ICONEXCLAMATION);
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
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблицы Traffic", MB_ICONEXCLAMATION);
		bSuccess = LOGICAL_FALSE;
	END_CATCH

	return bSuccess;
}

//virtual
LOGICAL COdbcDatabase::QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets)
{
	CStackCriticalSection section(&m_CriticalSection);
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
		m_bSubnetsDirty = FALSE;
	}
	pSubnetDescs = m_pSubnetDescs;
	nSubnets = m_nSubnets;

	return LOGICAL_TRUE;
}

//virtual
LOGICAL COdbcDatabase::QueryHosts(SHostDesc *&pHostDescs, int &nHosts)
{
	CStackCriticalSection section(&m_CriticalSection);
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
		m_bHostsDirty = FALSE;
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
void COdbcDatabase::NewHostCreated(CHost *pHost)
{
	CStackCriticalSection section(&m_CriticalSection);
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
	CStackCriticalSection section(&m_CriticalSection);
	// Store host properties
	CNetObjectPropertiesRecordset PropertiesSet(this);
	PropertiesSet.StoreProperties(pHost);
}

//virtual
void COdbcDatabase::NewSubnetCreated(CSubnet *pSubnet)
{
	CStackCriticalSection section(&m_CriticalSection);
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
	CStackCriticalSection section(&m_CriticalSection);
	// Store subnet properties
	CNetObjectPropertiesRecordset PropertiesSet(this);
	PropertiesSet.StoreProperties(pSubnet);
}

//virtual
void COdbcDatabase::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
}

//virtual
void COdbcDatabase::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
}

