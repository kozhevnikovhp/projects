/* Groups.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	05 Dec 2007 - initial creation
	14 Mar 2008 - storing in database
	20 Mar 2008 - grouping (hosts by groups) loading/storing in/from DB
	21 Mar 2008 - default groups are dynamically allocated as ordinary ones
	06 Jun 2008 - appearance groups introduced (not completed)
	08 Jul 2008 - color schemes for appearance groups
	26 Aug 2008 - SNMP groups
*/

#ifndef __GROUPS_H_INCLUDED__
#define __GROUPS_H_INCLUDED__

#include "Network.h"


/////////////////////////////////////////////////////////////////////////////
// CGroup class

class CGroup
{
//Constuctors/destructors
public:
	CGroup(LOGICAL bDefault);
	virtual ~CGroup();

// Public methods
public:
	long GetID() { return m_ID; }
	void SetID(long ID) { m_ID = ID; }
	void Init(SGroupDesc *pDesc);
	LOGICAL IsEqualType(CGroup *pAnotherGroup) { return (GetType() == pAnotherGroup->GetType()); }
	LOGICAL IsOfType(long type) { return (GetType() == type); }
	LOGICAL IsDefault() { return m_bIsDefault; }
	void BeDefault() { m_bIsDefault = LOGICAL_TRUE; }
	char *GetName() { return m_szName; }
	int GetHostCount() { return m_Hosts.size(); }
	CHost *GetHost(int iHost) { return (CHost *)m_Hosts[iHost]; }
	void SetName(const char *pszName) { strcpy(m_szName, pszName); }
	void IncludeHost(CHost *pHost);
	void ExcludeHost(CHost *pHost);

// Public overridables
public:
	virtual long GetType() = 0;
	virtual char *GetUserParamValuesStringGUI() = 0;
	virtual char *GetUserParamValuesStringDB() = 0;
	virtual void ParseUserParamValuesStringDB(char *pszParamString) = 0;
	virtual char *GetDBFormatString() = 0;
	virtual CGroup *NewGroup(SGroupDesc *pDesc) = 0;

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	long m_ID;
	char m_szName[128];
	LOGICAL m_bIsDefault;
	CListOfNetObjects m_Hosts;
	static int m_SerialNumber;
};


/////////////////////////////////////////////////////////////////////////////
// CPingGroup class

class CPingGroup : public CGroup
{
//Constuctors/destructors
public:
	CPingGroup(LOGICAL bDefault);
	virtual ~CPingGroup();

// Public methods
public:
	LOGICAL IsKeepAliveEnabled() { return m_bKeepAliveEnabled; }
	void EnableKeepAlive(LOGICAL bEnabled) { m_bKeepAliveEnabled = bEnabled; }
	unsigned int GetKeepAlivePeriod() { return m_uKeepAlivePeriod; }
	void SetKeepAlivePeriod(unsigned int uPeriod) { m_uKeepAlivePeriod = uPeriod; }

// Public overridables
public:
	virtual long GetType() { return PING_GROUP_INDEX; }
	virtual char *GetUserParamValuesStringGUI();
	virtual char *GetUserParamValuesStringDB();
	virtual void ParseUserParamValuesStringDB(char *pszParamString);
	virtual char *GetDBFormatString() { return "%ld;%ld"; }
	virtual CGroup *NewGroup(SGroupDesc *pDesc);

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	LOGICAL m_bKeepAliveEnabled;
	unsigned int m_uKeepAlivePeriod;

	char m_szUserParamStringGUI[64], m_szUserParamStringDB[32];
};


/////////////////////////////////////////////////////////////////////////////
// CAppearanceGroup class

class CAppearanceGroup : public CGroup
{
//Constuctors/destructors
public:
	CAppearanceGroup(LOGICAL bDefault);
	virtual ~CAppearanceGroup();

// Public methods
public:
	SRGBA *GetOrdinaryColor() { return &m_OrdinaryColor; }
	SRGBA *GetOrdinarySelectedColor() { return &m_OrdinarySelectedColor; }

	SRGBA *GetWarningColor() { return &m_WarningColor; }
	SRGBA *GetWarningSelectedColor() { return &m_WarningSelectedColor; }

	SRGBA *GetAlarmColor() { return &m_AlarmColor; }
	SRGBA *GetAlarmSelectedColor() { return &m_AlarmSelectedColor; }
	
	SRGBA *GetTextColor() { return &m_TextColor; }
	SRGBA *GetTextSelectedColor() { return &m_TextSelectedColor; }

	void InitSelectedColors();

	// Public overridables
public:
	virtual long GetType() { return APPEARANCE_GROUP_INDEX; }
	virtual char *GetUserParamValuesStringGUI();
	virtual char *GetUserParamValuesStringDB();
	virtual void ParseUserParamValuesStringDB(char *pszParamString);
	virtual char *GetDBFormatString() { return "%lu;%lu;%lu;%lu"; }
	virtual CGroup *NewGroup(SGroupDesc *pDesc);

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

	char m_szUserParamStringGUI[512], m_szUserParamStringDB[64];
	SRGBA m_OrdinaryColor, m_OrdinarySelectedColor;
	SRGBA m_WarningColor, m_WarningSelectedColor;
	SRGBA m_AlarmColor, m_AlarmSelectedColor;
	SRGBA m_TextColor, m_TextSelectedColor;
};


/////////////////////////////////////////////////////////////////////////////
// CSnmpGroup class

const int MAX_SNMP_COMMUNITY_NAME_LEN = 128;
const int MAX_SNMP_PASSWORD_NAME_LEN = 128;

class CSnmpGroup : public CGroup
{
//Constuctors/destructors
public:
	CSnmpGroup(LOGICAL bDefault);
	virtual ~CSnmpGroup();

// Public methods
public:
	LOGICAL IsSnmpEnabled() { return m_bSnmpEnabled; }
	void EnableSnmp(LOGICAL bEnable) { m_bSnmpEnabled = bEnable; }
	char *GetCommunityName() { return m_szCommunityName; }
	void SetCommunityName(const char *pszCommunity) { strcpy(m_szCommunityName, pszCommunity); }
	char *GetPassword() { return m_szPassword; }
	void SetPassword(const char *pszPassword) { strcpy(m_szPassword, pszPassword); }
	int GetRetries() { return m_nRetries; }
	void SetRetries(int nRetries) { m_nRetries = nRetries; }

	// Public overridables
public:
	virtual long GetType() { return SNMP_GROUP_INDEX; }
	virtual char *GetUserParamValuesStringGUI();
	virtual char *GetUserParamValuesStringDB();
	virtual void ParseUserParamValuesStringDB(char *pszParamString);
	virtual char *GetDBFormatString() { return "%lu;%lu;%lu;%lu"; }
	virtual CGroup *NewGroup(SGroupDesc *pDesc);

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

	LOGICAL m_bSnmpEnabled;
	char m_szCommunityName[MAX_SNMP_COMMUNITY_NAME_LEN];
	char m_szPassword[MAX_SNMP_PASSWORD_NAME_LEN];
	int m_nRetries;
	char m_szUserParamStringGUI[512], m_szUserParamStringDB[512];
};


class CListOfGroups : public std::vector<CGroup *>
{
//Constuctors/destructors
public:
	CListOfGroups();
	virtual ~CListOfGroups();

// Public methods
public:
	CGroup *Find(long ID);

// Public overridables
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
};

extern CListOfGroups g_AllGroups;
extern CPingGroup *g_pPingDefaultGroup;
extern CAppearanceGroup *g_pAppearanceDefaultGroup;
extern CSnmpGroup *g_pSnmpDefaultGroup;

///////////////////////////////////////////////////////////////////////
// Misc

void GroupCreated(CGroup *pGroup, LOGICAL bNeedUpdate);
CPingGroup *CreatePingGroup(LOGICAL bDefault, LOGICAL bNeedUpdate);
CAppearanceGroup *CreateAppearanceGroup(LOGICAL bDefault, LOGICAL bNeedUpdate);
CSnmpGroup *CreateSnmpGroup(LOGICAL bDefault, LOGICAL bNeedUpdate);


#endif //__GROUPS_H_INCLUDED__