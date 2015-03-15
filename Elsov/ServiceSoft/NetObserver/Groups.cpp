/* Groups.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	05 Dec 2007 - initial creation
	14 Mar 2008 - storing in database
	20 Mar 2008 - grouping (hosts by groups) loading/storing in/from DB
	21 Mar 2008 - default groups are dynamically allocated as ordinary ones
	06 Jun 2008 - appearance groups introduced (not completed)
	08 Jul 2008 - color schemes for appearance groups
*/

#include "stdafx.h"
#include "Groups.h"

CListOfGroups g_AllGroups;
CPingGroup *g_pPingDefaultGroup = NULL;
CAppearanceGroup *g_pAppearanceDefaultGroup = NULL;
CSnmpGroup *g_pSnmpDefaultGroup = NULL;

const char *pszCRLF = "\r\n";

/////////////////////////////////////////////////////////////////////////////////////
// CGroup implementation

//static
int CGroup::m_SerialNumber = 1;

CGroup::CGroup(LOGICAL bDefault)
{
	m_szName[0] = 0;
	m_bIsDefault = bDefault;
	m_ID = m_SerialNumber++;
}

//virtual
CGroup::~CGroup()
{
}

void CGroup::Init(SGroupDesc *pDesc)
{
	m_ID = pDesc->ID;
	SetName(pDesc->szName);
	ParseUserParamValuesStringDB(pDesc->szParams);
}

void CGroup::IncludeHost(CHost *pHost)
{
//	m_Hosts.AddIfNotExists(pHost);
}

void CGroup::ExcludeHost(CHost *pHost)
{
//	m_Hosts.Remove(pHost);
}

/////////////////////////////////////////////////////////////////////////////////////
// CPingGroup implementation

CPingGroup::CPingGroup(LOGICAL bDefault) : CGroup(bDefault)
{
	m_bKeepAliveEnabled = LOGICAL_TRUE;
	m_uKeepAlivePeriod = 60;
	m_szUserParamStringGUI[0] = m_szUserParamStringDB[0] = 0;

	sprintf(m_szName, "PingGroup%d", m_SerialNumber-1);
}

//virtual
CPingGroup::~CPingGroup()
{
}

//virtual
char *CPingGroup::GetUserParamValuesStringGUI()
{
	char szBuffer[32];

	// Enabled
	strcpy(m_szUserParamStringGUI, "Enabled = ");
	if (m_bKeepAliveEnabled)
		strcat(m_szUserParamStringGUI, "YES");
	else
		strcat(m_szUserParamStringGUI, "NO");
	strcat(m_szUserParamStringGUI, pszCRLF);

	// Period
	sprintf(szBuffer, "Period = %d sec", m_uKeepAlivePeriod);
	strcat(m_szUserParamStringGUI, szBuffer);
	strcat(m_szUserParamStringGUI, pszCRLF);

	return m_szUserParamStringGUI;
}

//virtual
char *CPingGroup::GetUserParamValuesStringDB()
{
	sprintf(m_szUserParamStringDB, GetDBFormatString(), m_bKeepAliveEnabled, m_uKeepAlivePeriod);
	return m_szUserParamStringDB;
}

//virtual
void CPingGroup::ParseUserParamValuesStringDB(char *pszParamString)
{
	sscanf(pszParamString, GetDBFormatString(), &m_bKeepAliveEnabled, &m_uKeepAlivePeriod);
}

//virtual
CGroup *CPingGroup::NewGroup(SGroupDesc *pDesc)
{
	ASSERT(this == g_pPingDefaultGroup);
	CGroup *pGroup = this;
	if (!pDesc->bDefault)
		pGroup = CreatePingGroup(LOGICAL_FALSE, LOGICAL_FALSE);
	pGroup->Init(pDesc);
	return pGroup;
}


/////////////////////////////////////////////////////////////////////////////////////
// CAppearanceGroup implementation

CAppearanceGroup::CAppearanceGroup(LOGICAL bDefault) : CGroup(bDefault)
{
	sprintf(m_szName, "AppearanceGroup%d", m_SerialNumber-1);

	m_OrdinaryColor.Init(0,128,0); // green
	m_WarningColor.Init(128,128,0); //yellow
	m_AlarmColor.Init(192,0,0);//red
	m_TextColor.Init(0,0,192);//blue
	InitSelectedColors();
}

//virtual
CAppearanceGroup::~CAppearanceGroup()
{
}

//virtual
char *CAppearanceGroup::GetUserParamValuesStringGUI()
{
	m_szUserParamStringGUI[0] = 0;
	char szBuffer[32];

	// Ordinary host color
	strcat(m_szUserParamStringGUI, "Ordinary color");
	sprintf(szBuffer, " = RGB(%d,%d,%d)\r\n", m_OrdinaryColor.R(), m_OrdinaryColor.G(), m_OrdinaryColor.B());
	strcat(m_szUserParamStringGUI, szBuffer);

	// Warning host color
	strcat(m_szUserParamStringGUI, "Warning color");
	sprintf(szBuffer, " = RGB(%d,%d,%d)\r\n", m_WarningColor.R(), m_WarningColor.G(), m_WarningColor.B());
	strcat(m_szUserParamStringGUI, szBuffer);

	// Alarm host color
	strcat(m_szUserParamStringGUI, "Alarm color");
	sprintf(szBuffer, " = RGB(%d,%d,%d)\r\n", m_AlarmColor.R(), m_AlarmColor.G(), m_AlarmColor.B());
	strcat(m_szUserParamStringGUI, szBuffer);

	// Text color
	strcat(m_szUserParamStringGUI, "Text color");
	sprintf(szBuffer, " = RGB(%d,%d,%d)\r\n", m_TextColor.R(), m_TextColor.G(), m_TextColor.B());
	strcat(m_szUserParamStringGUI, szBuffer);

	return m_szUserParamStringGUI;
	return m_szUserParamStringGUI;
}

//virtual
char *CAppearanceGroup::GetUserParamValuesStringDB()
{
	sprintf(m_szUserParamStringDB, GetDBFormatString(), m_OrdinaryColor.GetRGBA(), m_WarningColor.GetRGBA(), m_AlarmColor.GetRGBA(), m_TextColor.GetRGBA());
	return m_szUserParamStringDB;
}

//virtual
void CAppearanceGroup::ParseUserParamValuesStringDB(char *pszParamString)
{
	unsigned int OrdinaryColor, WarningColor, AlarmColor, TextColor;
	sscanf(pszParamString, GetDBFormatString(), &OrdinaryColor, &WarningColor, &AlarmColor, &TextColor);
	m_OrdinaryColor.Init(OrdinaryColor);
	m_WarningColor.Init(WarningColor);
	m_AlarmColor.Init(AlarmColor);
	m_TextColor.Init(TextColor);
	InitSelectedColors();
}

//virtual
CGroup *CAppearanceGroup::NewGroup(SGroupDesc *pDesc)
{
	ASSERT(this == g_pAppearanceDefaultGroup);
	CGroup *pGroup = this;
	if (!pDesc->bDefault)
		pGroup = CreateAppearanceGroup(LOGICAL_FALSE, LOGICAL_FALSE);
	pGroup->Init(pDesc);
	return pGroup;
}

void CAppearanceGroup::InitSelectedColors()
{
	m_OrdinarySelectedColor.InitAndHighlite(m_OrdinaryColor, 1.5);
	m_WarningSelectedColor.InitAndHighlite(m_WarningColor, 1.5);
	m_AlarmSelectedColor.InitAndHighlite(m_AlarmColor, 1.5);
	m_TextSelectedColor.InitAndHighlite(m_TextColor, 1.5);
}


/////////////////////////////////////////////////////////////////////////////////////
// CSnmpGroup implementation

CSnmpGroup::CSnmpGroup(LOGICAL bDefault) : CGroup(bDefault)
{
	sprintf(m_szName, "SnmpGroup%d", m_SerialNumber-1);
	m_bSnmpEnabled = LOGICAL_FALSE;
	strcpy(m_szCommunityName, "public");
	m_szPassword[0] = 0;
	m_nRetries = 3;
}

//virtual
CSnmpGroup::~CSnmpGroup()
{
}

//virtual
char *CSnmpGroup::GetUserParamValuesStringGUI()
{
	char szBuffer[128];

	// Enabled
	strcpy(m_szUserParamStringGUI, "Enabled = ");
	if (m_bSnmpEnabled)
		strcat(m_szUserParamStringGUI, "YES");
	else
		strcat(m_szUserParamStringGUI, "NO");
	strcat(m_szUserParamStringGUI, pszCRLF);

	// Community name
	strcat(m_szUserParamStringGUI, "Community name = ");
	strcat(m_szUserParamStringGUI, m_szCommunityName);
	strcat(m_szUserParamStringGUI, pszCRLF);

	// Password
	strcat(m_szUserParamStringGUI, "Password = ");
	strcat(m_szUserParamStringGUI, m_szPassword);
	strcat(m_szUserParamStringGUI, pszCRLF);

	// nRetries
	sprintf(szBuffer, "Retries = %d", m_nRetries);
	strcat(m_szUserParamStringGUI, szBuffer);
	strcat(m_szUserParamStringGUI, pszCRLF);
	
	return m_szUserParamStringGUI;
}

//virtual
char *CSnmpGroup::GetUserParamValuesStringDB()
{
	sprintf(m_szUserParamStringDB, GetDBFormatString(), m_bSnmpEnabled, m_szCommunityName, m_szPassword);
	return m_szUserParamStringDB;
}

//virtual
void CSnmpGroup::ParseUserParamValuesStringDB(char *pszParamString)
{
	sscanf(pszParamString, GetDBFormatString(), &m_bSnmpEnabled, m_szCommunityName, m_szPassword);
}

//virtual
CGroup *CSnmpGroup::NewGroup(SGroupDesc *pDesc)
{
	ASSERT(this == g_pSnmpDefaultGroup);
	CGroup *pGroup = this;
	if (!pDesc->bDefault)
		pGroup = CreateSnmpGroup(LOGICAL_FALSE, LOGICAL_FALSE);
	pGroup->Init(pDesc);
	return pGroup;
}


/////////////////////////////////////////////////////////////////////////////////////
// CListOfPingGroups implementation

CListOfGroups::CListOfGroups()
{
}

//virtual 
CListOfGroups::~CListOfGroups()
{
	for (iterator p = begin(); p != end(); p++)
	{
		CGroup *pGroup = *p;
		delete pGroup;
	}
}

CGroup *CListOfGroups::Find(long ID)
{
	for (iterator p = begin(); p != end(); p++)
	{
		CGroup *pGroup = *p;
		if (pGroup->GetID() == ID)
			return pGroup;
	}
	return NULL;
}


///////////////////////////////////////////////////////////////////////
// Misc

void GroupCreated(CGroup *pGroup, LOGICAL bNeedUpdate)
{
	g_AllGroups.push_back(pGroup);
	if (bNeedUpdate)
		g_pWholeNetwork->NewGroupCreated(pGroup);
}

CPingGroup *CreatePingGroup(LOGICAL bDefault, LOGICAL bNeedUpdate)
{
	CPingGroup *pNewGroup = new CPingGroup(bDefault);
	GroupCreated(pNewGroup, bNeedUpdate);
	return pNewGroup;
}

CAppearanceGroup *CreateAppearanceGroup(LOGICAL bDefault, LOGICAL bNeedUpdate)
{
	CAppearanceGroup *pNewGroup = new CAppearanceGroup(bDefault);
	GroupCreated(pNewGroup, bNeedUpdate);
	return pNewGroup;
}

CSnmpGroup *CreateSnmpGroup(LOGICAL bDefault, LOGICAL bNeedUpdate)
{
	CSnmpGroup *pNewGroup = new CSnmpGroup(bDefault);
	GroupCreated(pNewGroup, bNeedUpdate);
	return pNewGroup;
}


