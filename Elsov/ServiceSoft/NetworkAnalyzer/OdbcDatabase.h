/* OdbcDatabase.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	25 Oct 2006 - initial creation
	22 Nov 2006 - storing network objects and their properties in database
	28 Nov 2006 - loading network objects and their properties from database
	29 Nov 2006 - storing/loading flags of network objects in/from database
	20 May 2007 - traffic table

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
	LOGICAL StoreStringValue(char *pszValue, BOOL &bStored);
	LOGICAL StoreLongValue(long Value, BOOL &bStored);
	LOGICAL AddStringValue(long ObjectID, long PropertyID, char *pszValue, BOOL bAdded);
	LOGICAL AddLongValue(long ObjectID, long PropertyID, long uValue, BOOL bAdded);
	LOGICAL AddGivenName(long ID, char *pszName, BOOL bAdded);
	LOGICAL AddOwnName(long ID, char *pszName, BOOL bAdded);
	LOGICAL AddIpAddress(long ID, long uIP, BOOL bAdded);
	LOGICAL AddSubnetMask(long ID, long uMask, BOOL bAdded);
	LOGICAL AddIfaceMacAddress(long ID, char *pszAddress, BOOL bAdded);
	LOGICAL AddFlags(long ID, long flags, BOOL bAdded);
	LOGICAL AddDistance(long ID, long distance, BOOL bAdded);

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


// Overrides
	//{{AFX_VIRTUAL(CTrafficRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange *pFX);  // RFX support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	LOGICAL NewRecord(STrafficRecord *pRecord);

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
	// Queries
	virtual LOGICAL QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets);
	virtual LOGICAL QueryHosts(SHostDesc *&pHostDescs, int &nHosts);
	virtual SSubnetDesc *FindSubnet(CSubnet *pSubnet);
	virtual SHostDesc *FindHost(CHost *pHost);

	// Events
	virtual void NewHostCreated(CHost *pHost);
	virtual void HostChanged(CHost *pHost);
	virtual void NewSubnetCreated(CSubnet *pSubnet);
	virtual void SubnetChanged(CSubnet *pSubnet);
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);

// Protected overridables
protected:
	virtual LOGICAL CreateTables();
	virtual LOGICAL CreateObjectsTable();
	virtual LOGICAL CreateObjectPropertiesTable();
	virtual LOGICAL CreateEventLogTable();
	virtual LOGICAL CreateStringTable();
	virtual LOGICAL CreateTrafficTable();


// Protected members
	CString m_ConnectionString;

	SSubnetDesc *m_pSubnetDescs;
	int m_nSubnets;
	BOOL m_bSubnetsDirty;

	SHostDesc *m_pHostDescs;
	int m_nHosts;
	BOOL m_bHostsDirty;

	CPortableCriticalSection m_CriticalSection;
};

#endif //__ODBC_DATABASE_H_INCLUDED__
