/* OdbcDatabase.h

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

#ifndef __ODBC_DATABASE_H_INCLUDED__
#define __ODBC_DATABASE_H_INCLUDED__

#include "DatabaseAPI.h"

// Forward declarations
class COdbcDatabase;

class COdbcRecordset : public CRecordset
{
public:
	COdbcRecordset(CDatabase *pDB = NULL);
	DECLARE_DYNAMIC(COdbcRecordset)
	virtual BOOL Requery();
protected:
	COdbcDatabase *GetDB() { return m_pDB; }
	int CountRecords();

//protected members
protected:
	COdbcDatabase *m_pDB;
};

/////////////////////////////////////////////////////////////////////////////
// CNetObjectRecordset recordset

class CNetObjectRecordset : public COdbcRecordset
{
	friend class COdbcDatabase;
public:
	CNetObjectRecordset(CDatabase *pDatabase = NULL);
	DECLARE_DYNAMIC(CNetObjectRecordset)

// Field/Param Data
	//{{AFX_FIELD(CNetObjectRecordset, COdbcRecordset)
	long	m_ID;
	long	m_OBJECT_TYPE;
	//}}AFX_FIELD
	long m_Param1;

// Overrides
	//{{AFX_VIRTUAL(CNetObjectRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange *pFX);  // RFX support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	LOGICAL QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets);
	LOGICAL NewSubnet(CSubnet *pSubnet);
	LOGICAL QueryHosts(SHostDesc *&pHostDescs, int &nHosts);
	LOGICAL NewHost(CHost *pHost);
	LOGICAL GetLastObjectID(long &ID, long ObjectType);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CNetObjectPropertiesRecordset recordset

class CNetObjectPropertiesRecordset : public COdbcRecordset
{
	friend class COdbcDatabase;
public:
	CNetObjectPropertiesRecordset(CDatabase *pDatabase = NULL);
	DECLARE_DYNAMIC(CNetObjectPropertiesRecordset)

// Field/Param Data
	//{{AFX_FIELD(CNetObjectPropertiesRecordset, COdbcRecordset)
	long	m_OBJECT_ID;
	long	m_PROPERTY_ID;
	CString	m_VALUE_STR;
	//}}AFX_FIELD
	long m_Param1;


// Overrides
	//{{AFX_VIRTUAL(CNetObjectPropertiesRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange *pFX);  // RFX support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	LOGICAL LoadProperties(SSubnetDesc *pDesc);
	LOGICAL LoadProperties(SHostDesc *pDesc);
	LOGICAL StoreProperties(CSubnet *pSubnet);
	LOGICAL StoreProperties(CHost *pHost);
	LOGICAL StoreStringValue(char *pszValue, LOGICAL &bStored);
	LOGICAL StoreLongValue(long Value, LOGICAL &bStored);
	LOGICAL AddStringValue(long ObjectID, long PropertyID, char *pszValue, LOGICAL bAdded);
	LOGICAL AddLongValue(long ObjectID, long PropertyID, long uValue, LOGICAL bAdded);
	LOGICAL AddGivenName(long ID, char *pszName, LOGICAL bAdded);
	LOGICAL AddOwnName(long ID, char *pszName, LOGICAL bAdded);
	LOGICAL AddNetBiosDomainName(long ID, char *pszName, LOGICAL bAdded);
	LOGICAL AddIpAddress(long ID, long uIP, LOGICAL bAdded);
	LOGICAL AddSubnetMask(long ID, long uMask, LOGICAL bAdded);
	LOGICAL AddIfaceMacAddress(long ID, char *pszAddress, LOGICAL bAdded);
	LOGICAL AddOwnMacAddress(long ID, char *pszAddress, LOGICAL bAdded);
	LOGICAL AddFlags(long ID, long flags, LOGICAL bAdded);
	LOGICAL AddServicesFlags(long ID, long flags, LOGICAL bAdded);
	LOGICAL AddDistance(long ID, long distance, LOGICAL bAdded);
	LOGICAL AddSubnetColorScheme(long ID, char *pszValue, LOGICAL bAdded);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CTrafficRecordset recordset

class CTrafficRecordset : public COdbcRecordset
{
	friend class COdbcDatabase;
public:
	CTrafficRecordset(CDatabase *pDatabase = NULL);
	DECLARE_DYNAMIC(CTrafficRecordset)

// Field/Param Data
	//{{AFX_FIELD(CTrafficRecordset, COdbcRecordset)
	CTime	m_FROM_TIME;
	CTime	m_TO_TIME;
	long	m_SRC_ID;
	long	m_DST_ID;
	long	m_PROTO;
	long	m_PORT_TYPE;
	long	m_PACKETS;
	long	m_OCTETS;
	//}}AFX_FIELD
	long m_Param1;
	long m_Param2;
	CTime m_Param3;
	CTime m_Param4;
	CTime m_Param5;
	CTime m_Param6;
	CTime m_Param7;
	CTime m_Param8;

// Overrides
	//{{AFX_VIRTUAL(CTrafficRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange *pFX);  // RFX support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	LOGICAL NewRecord(STrafficRecord *pRecord);
	LOGICAL QueryTrafficFromHost(CHost *pHost, unsigned short proto, CTime StartTime, CTime EndTime, STrafficRecord *&pRecords, int &nRecords);
	LOGICAL QueryTrafficToHost(CHost *pHost, unsigned short proto, CTime StartTime, CTime EndTime, STrafficRecord *&pRecords, int &nRecords);
	LOGICAL Query(CHost *pHost, unsigned short proto, CTime StartTime, CTime EndTime, STrafficRecord *&pRecords, int &nRecords);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CGroupsRecordset recordset

class CGroupsRecordset : public COdbcRecordset
{
	friend class COdbcDatabase;
public:
	CGroupsRecordset(CDatabase *pDatabase = NULL);
	DECLARE_DYNAMIC(CGroupsRecordset)

// Field/Param Data
	//{{AFX_FIELD(CGroupsRecordset, COdbcRecordset)
	long	m_ID;
	CString	m_NAME;
	long	m_TYPE;
	long	m_IS_DEFAULT;
	CString	m_PARAMS;
	//}}AFX_FIELD
	long m_Param1;

// Overrides
	//{{AFX_VIRTUAL(CGroupsRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange *pFX);  // RFX support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	LOGICAL QueryGroups(SGroupDesc *&pGroups, int &nGroups);
	LOGICAL GroupChanged(CGroup *pGroup);
	LOGICAL GetLastGroupID(long &ID);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
};

class CGroupingRecordset : public COdbcRecordset
{
	friend class COdbcDatabase;
public:
	CGroupingRecordset(CDatabase *pDatabase = NULL);
	DECLARE_DYNAMIC(CGroupingRecordset)

// Field/Param Data
	//{{AFX_FIELD(CGroupingRecordset, COdbcRecordset)
	long	m_HOST_ID;
	long	m_GROUP_ID;
	//}}AFX_FIELD
	long m_Param1;
	long m_Param2;

// Overrides
	//{{AFX_VIRTUAL(CGroupingRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange *pFX);  // RFX support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	LOGICAL Load(CHost *pHost);
	LOGICAL Store(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup);

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
};

class COdbcDatabase : public CDatabase, public IDatabaseAPI
{
// Construction/destruction
public:
	COdbcDatabase();
	virtual ~COdbcDatabase();
	
// Public interface
public:
	virtual LOGICAL Create();
	virtual void StoreTrafficRecord(STrafficRecord *pRecord);
	virtual void MergeHosts(SHostDesc *pDstDesc, SHostDesc *pSrcDesc);
	virtual void DeleteHost(unsigned long ID);
	// Queries
	virtual LOGICAL QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets);
	virtual LOGICAL QueryHosts(SHostDesc *&pHostDescs, int &nHosts);
	virtual SSubnetDesc *FindSubnet(CSubnet *pSubnet);
	virtual SHostDesc *FindHost(CHost *pHost);
	virtual LOGICAL QueryTrafficFromHost(CHost *pHost, unsigned short proto, time_t StartTime, time_t EndTime, STrafficRecord *&pRecords, int &nRecords);
	virtual LOGICAL QueryTrafficToHost(CHost *pHost, unsigned short proto, time_t StartTime, time_t EndTime, STrafficRecord *&pRecords, int &nRecords);
	virtual LOGICAL QueryGroups(SGroupDesc *&pGroups, int &nGroups);
	virtual LOGICAL LoadHostGrouping(CHost *pHost);

	// Events
	virtual void NewHostCreated(CHost *pHost);
	virtual void HostChanged(CHost *pHost);
	virtual void NewSubnetCreated(CSubnet *pSubnet);
	virtual void SubnetChanged(CSubnet *pSubnet);
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void NewGroupCreated(CGroup *pGroup);
	virtual void GroupChanged(CGroup *pGroup);
	virtual void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup);

// Protected overridables
protected:
	virtual LOGICAL CreateTables();
	virtual LOGICAL CreateObjectsTable();
	virtual LOGICAL CreateObjectPropertiesTable();
	virtual LOGICAL CreateEventLogTable();
	virtual LOGICAL CreateStringTable();
	virtual LOGICAL CreateTrafficTable();
	virtual LOGICAL CreateGroupsTable();
	virtual LOGICAL CreateGroupingTable();

// Protected methods

// Protected members
	CString m_ConnectionString;

	SSubnetDesc *m_pSubnetDescs;
	int m_nSubnets;
	LOGICAL m_bSubnetsDirty;

	SHostDesc *m_pHostDescs;
	int m_nHosts;
	LOGICAL m_bHostsDirty;

	STrafficRecord *m_pTrafficRecords;
	int m_nTrafficRecords;

	CPortableCriticalSection m_CriticalSection;
};

#endif //__ODBC_DATABASE_H_INCLUDED__
/////////////////////////////////////////////////////////////////////////////
// CGroupingRecordset recordset

