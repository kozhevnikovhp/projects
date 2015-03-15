/* MibParser.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	27 Sep 2008 - initial creation
	10 Nov 2008 - tree structure
	20 Nov 2008 - OBJECT_TYPE, SYNTAX clause started

*/

#include "stdafx.h"
#include "MibParser.h"

const char *PSZ_MIB_EXTENSION = ".mib";

static struct SMibKeyWord KeyWords[] =
{
    { "OBSOLETE", MIB_KEYWORD_OBSOLETE },
    { "OPAQUE", MIB_KEYWORD_OPAQUE },
    { "OPTIONAL", MIB_KEYWORD_OPTIONAL },
    { "LAST-UPDATED", MIB_KEYWORD_LAST_UPDATED },
    { "ORGANIZATION", MIB_KEYWORD_ORGANIZATION },
    { "CONTACT-INFO", MIB_KEYWORD_CONTACT_INFO },
    { "MODULE-IDENTITY", MIB_KEYWORD_MODULE_IDENTITY },
    { "MODULE-COMPLIANCE", MIB_KEYWORD_MODULE_COMPLIANCE},
    { "DEFINITIONS", MIB_KEYWORD_DEFINITIONS },
    { "END", MIB_KEYWORD_END },
    { "AUGMENTS", MIB_KEYWORD_AUGMENTS },
    { "NOT-ACCESSIBLE", MIB_KEYWORD_NOT_ACCESSIBLE },
    { "WRITE-ONLY", MIB_KEYWORD_WRITE_ONLY },
    { "NSAPADDRESS", MIB_KEYWORD_NSAP_ADDRESS },
    { "UNITS", MIB_KEYWORD_UNITS },
    { "REFERENCE", MIB_KEYWORD_REFERENCE },
    { "NUM-ENTRIES", MIB_KEYWORD_NUM_ENTRIES },
    { "BITSTRING", MIB_KEYWORD_BITSTRING },
    { "BIT", MIB_KEYWORD_BIT },
    { "BITS", MIB_KEYWORD_BITS },
    { "COUNTER64", MIB_KEYWORD_COUNTER64 },
    { "TIMETICKS", MIB_KEYWORD_TIMETICKS },
    { "NOTIFICATION-TYPE", MIB_KEYWORD_NOTIFICATION_TYPE },
    { "OBJECT-GROUP", MIB_KEYWORD_OBJECT_GROUP },
    { "OBJECT-IDENTITY", MIB_KEYWORD_OBJECT_IDENTITY },
    { "IDENTIFIER", MIB_KEYWORD_IDENTIFIER },
    { "OBJECT", MIB_KEYWORD_OBJECT },
    { "NETWORKADDRESS", MIB_KEYWORD_NETWORK_ADDRESS },
    { "GAUGE", MIB_KEYWORD_GAUGE },
    { "GAUGE32", MIB_KEYWORD_GAUGE32 },
    { "UNSIGNED32", MIB_KEYWORD_UNSIGNED32 },
    { "READ-WRITE", MIB_KEYWORD_READ_WRITE },
    { "READ-CREATE", MIB_KEYWORD_READ_CREATE },
    { "OCTETSTRING", MIB_KEYWORD_OCTETSTRING },
    { "OCTET", MIB_KEYWORD_OCTET },
    { "STRING", MIB_KEYWORD_STRING },
//    { "OF", MIB_KEYWORD_OF },
    { "SEQUENCE", MIB_KEYWORD_SEQUENCE },
    { "NULL", MIB_KEYWORD_NULL },
    { "IPADDRESS", MIB_KEYWORD_IPADDRESS },
    { "UINTEGER32", MIB_KEYWORD_UINTEGER32 },
    { "INTEGER", MIB_KEYWORD_INTEGER },
    { "INTEGER32", MIB_KEYWORD_INTEGER32 },
    { "COUNTER", MIB_KEYWORD_COUNTER },
    { "COUNTER32", MIB_KEYWORD_COUNTER32 },
    { "READ-ONLY", MIB_KEYWORD_READ_ONLY },
    { "DESCRIPTION", MIB_KEYWORD_DESCRIPTION },
    { "INDEX", MIB_KEYWORD_INDEX },
    { "DEFVAL", MIB_KEYWORD_DEFVAL },
    { "DEPRECATED", MIB_KEYWORD_DEPRECATED },
    { "SIZE", MIB_KEYWORD_SIZE },
    { "MAX-ACCESS", MIB_KEYWORD_MAX_ACCESS },
    { "ACCESS", MIB_KEYWORD_ACCESS },
    { "MANDATORY", MIB_KEYWORD_MANDATORY },
    { "CURRENT", MIB_KEYWORD_CURRENT },
    { "STATUS", MIB_KEYWORD_STATUS },
    { "SYNTAX", MIB_KEYWORD_SYNTAX },
    { "OBJECT-TYPE", MIB_KEYWORD_OBJECT_TYPE },
    { "TRAP-TYPE", MIB_KEYWORD_TRAP_TYPE },
    { "ENTERPRISE", MIB_KEYWORD_ENTERPRISE },
    { "BEGIN", MIB_KEYWORD_BEGIN },
    { "IMPORTS", MIB_KEYWORD_IMPORTS },
    { "EXPORTS", MIB_KEYWORD_EXPORTS },
    { "ACCESSIBLE-FOR-NOTIFY", MIB_KEYWORD_ACCESSIBLE_FOR_NOTIFY },
    { "TEXTUAL-CONVENTION", MIB_KEYWORD_TEXTUAL_CONVENTION },
    { "NOTIFICATION-GROUP", MIB_KEYWORD_NOTIFICATION_GROUP },
    { "DISPLAY-HINT", MIB_KEYWORD_DISPLAY_HINT },
    { "DisplayString", MIB_KEYWORD_DISPLAY_STRING },
    { "PhysAddress", MIB_KEYWORD_PHYS_ADDRESS },
	{ "TruthValue", MIB_KEYWORD_TRUTH_VALUE },
	{ "RowStatus", MIB_KEYWORD_ROW_STATUS },
    { "TimeStamp", MIB_KEYWORD_TIMESTAMP },
	{ "AutonomousType", MIB_KEYWORD_AUTONOMOUS_TYPE },
	{ "TestAndIncr", MIB_KEYWORD_TEST_AND_INCR },
    { "FROM", MIB_KEYWORD_FROM },
    { "AGENT-CAPABILITIES", MIB_KEYWORD_AGENT_CAPABILITIES },
    { "MACRO", MIB_KEYWORD_MACRO },
    { "IMPLIED", MIB_KEYWORD_IMPLIED },
    { "SUPPORTS", MIB_KEYWORD_SUPPORTS },
    { "INCLUDES", MIB_KEYWORD_INCLUDES },
    { "VARIATION", MIB_KEYWORD_VARIATION },
    { "REVISION", MIB_KEYWORD_REVISION },
    { "NOT-IMPLEMENTED", MIB_KEYWORD_NOT_IMPLEMENTED },
    { "OBJECTS", MIB_KEYWORD_OBJECTS },
    { "NOTIFICATIONS", MIB_KEYWORD_NOTIFICATIONS },
    { "MODULE", MIB_KEYWORD_MODULE },
    { "MIN-ACCESS", MIB_KEYWORD_MIN_ACCESS },
    { "PRODUCT-RELEASE", MIB_KEYWORD_PRODUCT_RELEASE },
    { "WRITE-SYNTAX", MIB_KEYWORD_WRITE_SYNTAX },
    { "CREATION-REQUIRES", MIB_KEYWORD_CREATION_REQUIRES },
    { "MANDATORY-GROUPS", MIB_KEYWORD_MANDATORY_GROUPS },
    { "GROUP", MIB_KEYWORD_GROUP },
    { "CHOICE", MIB_KEYWORD_CHOICE },
    { "IMPLICIT", MIB_KEYWORD_IMPLICIT },
    { "OBJECTSYNTAX", MIB_KEYWORD_OBJECT_SYNTAX },
    { "SIMPLESYNTAX", MIB_KEYWORD_SIMPLE_SYNTAX },
    { "APPLICATIONSYNTAX", MIB_KEYWORD_APPLICATION_SYNTAX },
    { "OBJECTNAME", MIB_KEYWORD_OBJECT_NAME },
    { "NOTIFICATIONNAME", MIB_KEYWORD_NOTIFICATION_NAME },
    { "VARIABLES", MIB_KEYWORD_VARIABLES }
};

static CMibModules g_MibModules;


/////////////////////////////////////////////////////////////////////
// class CMibNodes

CMibNodes::CMibNodes()
{
}

//virtual
CMibNodes::~CMibNodes()
{
}

CMibNode *CMibNodes::FindNode(char *pszNodeName) const
{
	for (const_iterator it = begin(); it != end(); it++)
	{
		CMibNode *pNode = *it;
		if (pNode->IsNameEqual(pszNodeName))
			return pNode;
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////
// class CMibObjectSyntax

CMibObjectSyntax::CMibObjectSyntax()
{
}

//virtual
CMibObjectSyntax::~CMibObjectSyntax()
{
}


/////////////////////////////////////////////////////////////////////
// class CMibObjectSyntaxInteger

CMibObjectSyntaxInteger::CMibObjectSyntaxInteger()
{
}

//virtual
CMibObjectSyntaxInteger::~CMibObjectSyntaxInteger()
{
}

//virtual
void CMibObjectSyntaxInteger::ProcessSpecificationToken(CTextString &token)
{
}

/////////////////////////////////////////////////////////////////////
// class CMibObjectSyntaxSequence

CMibObjectSyntaxSequence::CMibObjectSyntaxSequence()
{
	m_bOf = LOGICAL_FALSE;
	m_pSequenceOfWhat = NULL;
}

//virtual
CMibObjectSyntaxSequence::~CMibObjectSyntaxSequence()
{
}

//virtual
void CMibObjectSyntaxSequence::ProcessSpecificationToken(CTextString &token)
{
	// TODO: replace stricmp with IsEqualNoCase!
	if (stricmp(token.GetText(), "OF") == 0)
	{
		m_bOf = LOGICAL_TRUE;
	}
	else
	{
		m_strSequenceOfWhat = token;
	}
}

/////////////////////////////////////////////////////////////////////
// class CMibObjectSyntaxOctetString

CMibObjectSyntaxOctetString::CMibObjectSyntaxOctetString()
{
}

//virtual
CMibObjectSyntaxOctetString::~CMibObjectSyntaxOctetString()
{
}

//virtual
void CMibObjectSyntaxOctetString::ProcessSpecificationToken(CTextString &token)
{
}

/////////////////////////////////////////////////////////////////////
// class CMibObjectType

CMibObjectType::CMibObjectType()
{
	m_pSyntax = NULL;
}

//virtual
CMibObjectType::~CMibObjectType()
{
	if (m_pSyntax)
		delete m_pSyntax;
}


/////////////////////////////////////////////////////////////////////
// class CMibNode

CMibNode::CMibNode()
{
	m_pTree = NULL;
	m_iLastOidDigit = 0;
	m_pszSymbolicName = NULL;
	m_pszDecimalName = NULL;
	m_pParent = NULL;
	m_pWasClonedFrom = NULL;
	m_pObjectType = NULL;
}

CMibNode::CMibNode(CMibTree *pTree)
{
	m_pTree = pTree;
	m_pTree->NodeCreated(this);
	m_iLastOidDigit = 0;
	m_pszSymbolicName = NULL;
	m_pszDecimalName = NULL;
	m_pParent = NULL;
	m_pWasClonedFrom = NULL;
	m_pObjectType = NULL;
}

CMibNode::CMibNode(CMibTree *pTree, unsigned int iLastDigit, char *pszSymbolicName, CMibNode *pParent)
{
	char szTmpBuffer[32];
	m_pTree = pTree;
	m_pTree->NodeCreated(this);
	m_iLastOidDigit = iLastDigit;
	m_pszSymbolicName = NULL;
	m_pszDecimalName = NULL;
	if (pszSymbolicName)
		m_pszSymbolicName = strdup(pszSymbolicName);
	m_pParent = pParent;
	m_pWasClonedFrom = NULL;
	m_pObjectType = NULL;
	m_FullTextSpec = GetSymbolicName();
	sprintf(szTmpBuffer, "%d", iLastDigit);
	m_pszDecimalName = strdup(szTmpBuffer);
	m_FullDecimalSpec = szTmpBuffer;
	if (m_pParent)
		m_pParent->NewChild(this);
}

//virtual
CMibNode::~CMibNode()
{
	if (m_pszSymbolicName)
		free(m_pszSymbolicName);
	if (m_pszDecimalName)
		free(m_pszDecimalName);
	if (m_pObjectType)
		delete m_pObjectType;
}

CMibNode *CMibNode::Clone(CMibTree *pTree)
{
	CMibNode *pNewNode = new CMibNode(pTree);
	pNewNode->ClonedFrom(this);
	// All the children (and grandchildren recursively)
	for (CMibNodes::iterator it = m_Children.begin(); it != m_Children.end(); it++)
	{
		CMibNode *pChild = *it;
		CMibNode *pNewChild = pChild->Clone(pTree);
		pNewNode->NewChild(pNewChild);
	}
	return pNewNode;
}

void CMibNode::Clean()
{
	m_Children.resize(0);
}

char *CMibNode::GetFullTextSpec()
{
	if (m_pWasClonedFrom)
		return m_pWasClonedFrom->GetFullTextSpec();
	return m_FullTextSpec.GetText(); 
}

char *CMibNode::GetFullDecimalSpec()
{
	if (m_pWasClonedFrom)
		return m_pWasClonedFrom->GetFullDecimalSpec();
	return m_FullDecimalSpec.GetText(); 
}

char *CMibNode::GetSymbolicName()
{
	if (m_pWasClonedFrom)
		return m_pWasClonedFrom->GetSymbolicName();
	if (!m_pszSymbolicName)
		return "";
	return m_pszSymbolicName;
}

char *CMibNode::GetDecimalName()
{
	if (m_pWasClonedFrom)
		return m_pWasClonedFrom->GetDecimalName();
	if (!m_pszSymbolicName)
		return "";
	return m_pszDecimalName;
}

LOGICAL CMibNode::IsNameEqual(char *pszName)
{
	return (stricmp(GetSymbolicName(), pszName) == 0);		
}

unsigned int CMibNode::GetLastOidDigit()
{ 
	if (m_pWasClonedFrom)
		return m_pWasClonedFrom->GetLastOidDigit();
	return m_iLastOidDigit;
}

void CMibNode::NewChild(CMibNode *pNewChild)
{
	LOGICAL bInserted = LOGICAL_FALSE;
	for (CMibNodes::iterator it = m_Children.begin(); it != m_Children.end(); it++)
	{
		CMibNode *pChild = *it;
		if (pChild->GetLastOidDigit() == pNewChild->GetLastOidDigit())
			return; // in children list already
		if (pChild->GetLastOidDigit() > pNewChild->GetLastOidDigit())
		{
			m_Children.insert(it, pNewChild);
			bInserted = LOGICAL_TRUE;
			break;
		}
	}
	if (!bInserted)
		m_Children.push_back(pNewChild);
	
	pNewChild->SetParent(this);
}

void CMibNode::SetParent(CMibNode *pParent)
{
	m_pParent = pParent;

	// Full text spec (dot-separated symbolic names)
	m_FullTextSpec = pParent->GetFullTextSpec();
	m_FullTextSpec += ".";
	m_FullTextSpec += GetSymbolicName();

	// Integer.integer.integer etc text spec
	m_FullDecimalSpec = pParent->GetFullDecimalSpec();
	m_FullDecimalSpec += ".";
	m_FullDecimalSpec += GetDecimalName();

}

LOGICAL CMibNode::DoesNodeExist(CMibNode *pNode)
{
	for (CMibNodes::iterator it = m_Children.begin(); it != m_Children.end(); it++)
	{
		CMibNode *pChild = *it;
		if (pChild->DoesNodeExist(pNode))
			return LOGICAL_TRUE;
	}
	return LOGICAL_FALSE;
}

CMibNode *CMibNode::FindNode(char *pszNodeName)
{
	if (IsNameEqual(pszNodeName))
		return this;
	for (CMibNodes::iterator it = m_Children.begin(); it != m_Children.end(); it++)
	{
		CMibNode *pChild = *it;
		CMibNode *pNode = pChild->FindNode(pszNodeName);
		if (pNode)
			return pNode;
	}
	return NULL;
}

LOGICAL CMibNode::VisitChildren(CMibTreeNodeVisitor *pVisitor)
{
	for (CMibNodes::iterator it = m_Children.begin(); it != m_Children.end(); it++)
	{
		CMibNode *pNode = *it;
		pNode->Visit(pVisitor);
	}
	return LOGICAL_TRUE;
}

LOGICAL CMibNode::Visit(CMibTreeNodeVisitor *pVisitor)
{
	TRACE("%s\n", m_pszSymbolicName);
	pVisitor->Visit(this);
	pVisitor->LowerLevel();
	VisitChildren(pVisitor);
	pVisitor->UpperLevel();

	return LOGICAL_TRUE;
}


/////////////////////////////////////////////////////////////////////
// class CDelayedDesc

CDelayedDesc::CDelayedDesc(CTextString &ObjectName, CTextString &ParentName, unsigned int nDecimalSpec)
{
	m_ObjectName = ObjectName;
	m_ParentName = ParentName;
	m_iDecimalSpec = nDecimalSpec;
}


//virtual
CDelayedDesc::~CDelayedDesc()
{
}

CMibNode *CDelayedDesc::TryToCreate(CMibNode *pPotentiallyParentNode, CMibTree *pTree)
{
	CMibNode *pNewNode = NULL;
	if (pPotentiallyParentNode->IsNameEqual(m_ParentName.GetText()))
	{
		pNewNode = pTree->CreateNewNode(m_iDecimalSpec, m_ObjectName.GetText(), pPotentiallyParentNode);
	}
	return pNewNode;
}

/////////////////////////////////////////////////////////////////////
// class CDelayedDescs

CDelayedDescs::CDelayedDescs()
{
}

//virtual
CDelayedDescs::~CDelayedDescs()
{
}

void CDelayedDescs::NewDesc(CTextString &ObjectName, CTextString &ParentName, unsigned int iDecimalSpec)
{
	CDelayedDesc NewDesc(ObjectName, ParentName, iDecimalSpec);
	push_back(NewDesc);
}

void CDelayedDescs::TryToCreate(CMibNode *pPotentiallyParentNode, CMibTree *pTree)
{
	CMibNode *pNewNode = NULL;
	do
	{
		pNewNode = NULL;
		for (iterator it = begin(); it != end(); it++)
		{
			CDelayedDesc &Desc = *it;
			pNewNode = Desc.TryToCreate(pPotentiallyParentNode, pTree);
			if (pNewNode)
			{ // created successfully -> remove that desc from the list
				erase(it);
				break;
			}
		}
	} while (pNewNode);
}

/////////////////////////////////////////////////////////////////////
// class CMibTree

CMibTree::CMibTree()
{
}

//virtual
CMibTree::~CMibTree()
{
	Clean();
}

void CMibTree::Clean()
{
	m_RootNode.Clean();
	for (CMibNodes::iterator it = m_Nodes.begin(); it != m_Nodes.end(); it++)
	{
		CMibNode *pNode = *it;
		delete pNode;
	}
	m_Nodes.resize(0);
	m_ParsedNodes.resize(0);
	m_ModuleName.Empty();
}

LOGICAL CMibTree::VisitAllNodes(CMibTreeNodeVisitor *pVisitor)
{
	return m_RootNode.Visit(pVisitor);
}

MIB_ERROR_CODE_ENUM CMibTree::NodeCreated(CMibNode *pNode)
{
	if (DoesNodeExist(pNode))
		return MIB_ERROR_CODE_OK; // exists already, do nothing
	
	m_Nodes.push_back(pNode);
	
	return MIB_ERROR_CODE_OK;
}

LOGICAL CMibTree::DoesNodeExist(CMibNode *pNode)
{
	return DoesNodeExist(pNode, &m_RootNode);
}

LOGICAL CMibTree::DoesNodeExist(CMibNode *pNode, CMibNode *pFindIn)
{
	if (!pFindIn)
		return LOGICAL_FALSE;
	if (pNode == pFindIn)
		return LOGICAL_TRUE;
	return pFindIn->DoesNodeExist(pNode);
}

CMibNode *CMibTree::FindNode(char *pszNodeName)
{
	// Look in parsed nodes first
	CMibNode *pNode = m_ParsedNodes.FindNode(pszNodeName);
	if (pNode)
		return pNode;
	return m_RootNode.FindNode(pszNodeName);
}

CMibNode *CMibTree::CreateNewNode(unsigned int iLastDigit, char *pszSymbolicName, CMibNode *pParent)
{
	CMibNode *pNewNode = new CMibNode(this, iLastDigit, pszSymbolicName, pParent);
	return pNewNode;
}

CMibNode *CMibTree::CreateNewNode(CMibNode *pNode)
{
	CMibNode *pNewNode = new CMibNode(this);
	pNewNode->ClonedFrom(pNode);
	return pNewNode;
}

// Create new node or returns existing one from string like { parent_node_name 1 }
CMibNode *CMibTree::CreateNewNode(CTextString &Name, CTextString &token)
{
	char *pszText = token.GetText();
	char *pszParentName = strtok(pszText, " \t");
	char *pszLastDigit = strtok(pszParentName+strlen(pszParentName)+1, " \t");
	CTextString ParentObjectName(pszParentName);
	CTextString LastDigit(pszLastDigit);
	unsigned int iLastDigit = atoi(pszLastDigit);
	CMibNode *pParentNode = FindNode(ParentObjectName.GetText());
	if (!pParentNode)
	{
		ASSERT(0);
		m_DelayedDescs.NewDesc(Name, ParentObjectName, iLastDigit);
		return NULL;
	}
	CMibNode *pNewNode = pParentNode->FindNode(Name.GetText());
	if (!pNewNode)
	{
		pNewNode = CreateNewNode(iLastDigit, Name.GetText(), pParentNode);
		m_ParsedNodes.push_back(pNewNode);
		// New node is created. Probably, some of "delayed" nodes are waiting for it...
		m_DelayedDescs.TryToCreate(pNewNode, this);
	}
	return pNewNode;
}


/////////////////////////////////////////////////////////////////////
// class CMibModule

CMibModule::CMibModule()
{
}

//virtual
CMibModule::~CMibModule()
{
}

/////////////////////////////////////////////////////////////////////
// class CMibModules

CMibModules::CMibModules()
{
}

//virtual
CMibModules::~CMibModules()
{
	for (iterator it = begin(); it != end(); it++)
	{
		CMibModule *pModule = *it;
		delete pModule;
	}
}

CMibModule *CMibModules::GetModuleByName(char *pszModuleName)
{
	// Look among loaded modules first
	for (iterator it = begin(); it != end(); it++)
	{
		CMibModule *pModule = *it;
		if (pModule->IsMyself(pszModuleName))
			return pModule;
	}
	// Cannot found, create new one
	CMibModule *pModule = NULL;
	if (CSNMPv2_SMI::IsMyName(pszModuleName))
		pModule = new CSNMPv2_SMI();
	else if (CSNMPv1_SMI::IsMyName(pszModuleName))
		pModule = new CSNMPv1_SMI();
	else
	{
	}

	if (pModule)
		push_back(pModule);

	return pModule;
}

// Opens file with name of module in specified path, and if succeeded, load that module
MIB_ERROR_CODE_ENUM CMibModules::LoadMibModule(char *pszPath, char *pszModuleName)
{
	CTextString FileName;
	FileName.MakeFilePathName(pszPath, pszModuleName);
	CMibFileParser parser;
	return parser.ParseMib(FileName.GetText());
	/*CMibModule *pModule = new CMibModuleFromFile();
	if (!pModule->ParseMib(FileName.GetText()))
	{
		delete pModule;
		pModule = NULL;
	}
	push_back(pModule);*/
	return MIB_ERROR_CODE_OK;	
}


/////////////////////////////////////////////////////////////////////
// class CMibModuleFromFile : public CMibModule

CMibModuleFromFile::CMibModuleFromFile()
{
}

//virtual
CMibModuleFromFile::~CMibModuleFromFile()
{
}


//virtual
LOGICAL CMibModuleFromFile::IsMyself(char *pszModuleName)
{
	return (m_ModuleName == pszModuleName);
}

/////////////////////////////////////////////////////////////////////
// class CSNMPv1_SMI : public CMibModule

CSNMPv1_SMI::CSNMPv1_SMI()
{
	m_pIsoNode = CreateNewNode(1, "iso", &m_RootNode);
		m_pOrgNode = CreateNewNode(3, "org", m_pIsoNode);
			m_pDodNode = CreateNewNode(6, "dod", m_pOrgNode);
				m_pInternetNode = CreateNewNode(1, "internet", m_pDodNode);
					m_pDirectoryNode = CreateNewNode(1, "directory", m_pInternetNode);
					m_pMgmtNode = CreateNewNode(2, "mgmt", m_pInternetNode);
					m_pPrivateNode = CreateNewNode(4, "private", m_pInternetNode);
						m_pEnterprisesNode = CreateNewNode(1, "enterprises", m_pPrivateNode);
}

//virtual
CSNMPv1_SMI::~CSNMPv1_SMI()
{
}

//static
LOGICAL CSNMPv1_SMI::IsMyName(char *pszModuleName)
{
	if (!stricmp(pszModuleName, "SNMPv1-SMI"))
		return LOGICAL_TRUE;
	if (!stricmp(pszModuleName, "RFC1155-SMI"))
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

//virtual
LOGICAL CSNMPv1_SMI::IsMyself(char *pszModuleName)
{
	return IsMyName(pszModuleName);
}


/////////////////////////////////////////////////////////////////////
// class CSNMPv2_SMI : public CMibModule

CSNMPv2_SMI::CSNMPv2_SMI()
{
	m_pIsoNode = CreateNewNode(1, "iso", &m_RootNode);
		m_pOrgNode = CreateNewNode(3, "org", m_pIsoNode);
			m_pDodNode = CreateNewNode(6, "dod", m_pOrgNode);
				m_pInternetNode = CreateNewNode(1, "internet", m_pDodNode);
					m_pDirectoryNode = CreateNewNode(1, "directory", m_pInternetNode);
					m_pMgmtNode = CreateNewNode(2, "mgmt", m_pInternetNode);
					m_pMib2Node = CreateNewNode(1, "mib-2", m_pMgmtNode);	
						m_pSystemNode = CreateNewNode(1, "system", m_pMib2Node);
							CreateNewNode(1, "sysDescr", m_pSystemNode);
							CreateNewNode(2, "sysObjectID", m_pSystemNode);
							CreateNewNode(3, "sysUpTime", m_pSystemNode);
							CreateNewNode(4, "sysContact", m_pSystemNode);
							CreateNewNode(5, "sysName", m_pSystemNode);
							CreateNewNode(6, "sysLocation", m_pSystemNode);
							CreateNewNode(7, "sysServices", m_pSystemNode);
						m_pInterfacesNode = CreateNewNode(2, "interfaces", m_pMib2Node);
							CreateNewNode(1, "ifNumber", m_pInterfacesNode);
							CreateNewNode(2, "ifTable", m_pInterfacesNode);
						m_pAtNode = CreateNewNode(3, "at", m_pMib2Node);
							CreateNewNode(1, "atTable", m_pAtNode);
						m_pIpNode = CreateNewNode(4, "ip", m_pMib2Node);
							CreateNewNode(1, "ipForwarding", m_pIpNode);
							CreateNewNode(2, "ipDefaultTTL", m_pIpNode);
							CreateNewNode(3, "ipInReceives", m_pIpNode);
							CreateNewNode(4, "ipInHdrErrors", m_pIpNode);
							CreateNewNode(5, "ipInAddrErrors", m_pIpNode);
							CreateNewNode(6, "ipForwDatagrams", m_pIpNode);
							CreateNewNode(7, "ipInUnknownProtos", m_pIpNode);
							CreateNewNode(8, "ipInDiscards", m_pIpNode);
							CreateNewNode(9, "ipInDelivers", m_pIpNode);
							CreateNewNode(10, "ipOutRequests", m_pIpNode);
							CreateNewNode(11, "ipOutDiscards", m_pIpNode);
							CreateNewNode(12, "ipOutNoRoutes", m_pIpNode);
							CreateNewNode(13, "ipReasmTimeout", m_pIpNode);
							CreateNewNode(14, "ipReasmReqds", m_pIpNode);
							CreateNewNode(15, "ipReasmOKs", m_pIpNode);
							CreateNewNode(16, "ipReasmFails", m_pIpNode);
							CreateNewNode(17, "ipFragOKs", m_pIpNode);
							CreateNewNode(18, "ipFragFails", m_pIpNode);
							CreateNewNode(19, "ipFragCreates", m_pIpNode);
							CreateNewNode(20, "ipAddrTable", m_pIpNode);
							CreateNewNode(21, "ipRouteTable", m_pIpNode);
							CreateNewNode(22, "ipNetToMediaTable", m_pIpNode);
							CreateNewNode(23, "ipRoutingDiscards", m_pIpNode);
						m_pIcmpNode = CreateNewNode(5, "icmp", m_pMib2Node);
							CreateNewNode(1, "icmpInMsgs", m_pIcmpNode);
							CreateNewNode(2, "icmpInErrors", m_pIcmpNode);
							CreateNewNode(3, "icmpInDestUnreachs", m_pIcmpNode);
							CreateNewNode(4, "icmpInTimeExcds", m_pIcmpNode);
							CreateNewNode(5, "icmpInParmProbs", m_pIcmpNode);
							CreateNewNode(6, "icmpInSrcQuenchs", m_pIcmpNode);
							CreateNewNode(7, "icmpInRedirects", m_pIcmpNode);
							CreateNewNode(8, "icmpInEchoes", m_pIcmpNode);
							CreateNewNode(9, "icmpInEchoReps", m_pIcmpNode);
							CreateNewNode(10, "icmpInTimestamps", m_pIcmpNode);
							CreateNewNode(11, "icmpInTimestampReps", m_pIcmpNode);
							CreateNewNode(12, "icmpInAddrMasks", m_pIcmpNode);
							CreateNewNode(13, "icmpInAddrMaskReps", m_pIcmpNode);
							CreateNewNode(14, "icmpOutMsgs", m_pIcmpNode);
							CreateNewNode(15, "icmpOutErrors", m_pIcmpNode);
							CreateNewNode(16, "icmpOutDestUnreachs", m_pIcmpNode);
							CreateNewNode(17, "icmpOutTimeExcds", m_pIcmpNode);
							CreateNewNode(18, "icmpOutParmProbs", m_pIcmpNode);
							CreateNewNode(19, "icmpOutSrcQuenchs", m_pIcmpNode);
							CreateNewNode(20, "icmpOutRedirects", m_pIcmpNode);
							CreateNewNode(21, "icmpOutEchoes", m_pIcmpNode);
							CreateNewNode(22, "icmpOutEchoReps", m_pIcmpNode);
							CreateNewNode(23, "icmpOutTimestamps", m_pIcmpNode);
							CreateNewNode(24, "icmpOutTimestampReps", m_pIcmpNode);
							CreateNewNode(25, "icmpOutAddrMasks", m_pIcmpNode);
							CreateNewNode(26, "icmpOutAddrMaskReps", m_pIcmpNode);
						m_pTcpNode = CreateNewNode(6, "tcp", m_pMib2Node);
							CreateNewNode(1, "tcpRtoAlgorithm", m_pTcpNode);
							CreateNewNode(2, "tcpRtoMin", m_pTcpNode);
							CreateNewNode(3, "tcpRtoMax", m_pTcpNode);
							CreateNewNode(4, "tcpMaxConn", m_pTcpNode);
							CreateNewNode(5, "tcpActiveOpens", m_pTcpNode);
							CreateNewNode(6, "tcpPassiveOpens", m_pTcpNode);
							CreateNewNode(7, "tcpAttemptFails", m_pTcpNode);
							CreateNewNode(8, "tcpEstabResets", m_pTcpNode);
							CreateNewNode(9, "tcpCurrEstab", m_pTcpNode);
							CreateNewNode(11, "tcpInSegs", m_pTcpNode);
							CreateNewNode(11, "tcpOutSegs", m_pTcpNode);
							CreateNewNode(12, "tcpRetransSegs", m_pTcpNode);
							CreateNewNode(13, "tcpConnTable", m_pTcpNode);
							CreateNewNode(14, "tcpInnErrs", m_pTcpNode);
							CreateNewNode(15, "tcpOutRsts", m_pTcpNode);
						m_pUdpNode = CreateNewNode(7, "udp", m_pMib2Node);
							CreateNewNode(1, "udpInDatagrams", m_pUdpNode);
							CreateNewNode(2, "udpNoPorts", m_pUdpNode);
							CreateNewNode(3, "udpInErrors", m_pUdpNode);
							CreateNewNode(4, "udpOutDatagrams", m_pUdpNode);
							CreateNewNode(5, "udpTable", m_pUdpNode);
						m_pEgpNode = CreateNewNode(8, "egp", m_pMib2Node);
						m_pTransmissionNode = CreateNewNode(10, "transmission", m_pMib2Node);
						m_pSnmpNode = CreateNewNode(11, "snmp", m_pMib2Node);
					m_pExperimentalNode = CreateNewNode(3, "experimental", m_pInternetNode);
					m_pPrivateNode = CreateNewNode(4, "private", m_pInternetNode);
						m_pEnterprisesNode = CreateNewNode(1, "enterprises", m_pPrivateNode);
					m_pSecurityNode = CreateNewNode(5, "security", m_pInternetNode);
					m_pSnmpV2Node = CreateNewNode(6, "snmpV2", m_pInternetNode);
						m_pSnmpDomainsNode = CreateNewNode(1, "snmpDomains", m_pSnmpV2Node);
						m_pSnmpProxiesNode = CreateNewNode(2, "snmpProxys", m_pSnmpV2Node);
						m_pSnmpModulesNode = CreateNewNode(3, "snmpModules", m_pSnmpV2Node);
}

//virtual
CSNMPv2_SMI::~CSNMPv2_SMI()
{
}

//static
LOGICAL CSNMPv2_SMI::IsMyName(char *pszModuleName)
{
	if (!stricmp(pszModuleName, "SNMPv2-SMI"))
		return LOGICAL_TRUE;
	if (!stricmp(pszModuleName, "RFC1213-MIB"))
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

//virtual
LOGICAL CSNMPv2_SMI::IsMyself(char *pszModuleName)
{
	return IsMyName(pszModuleName);
}


/////////////////////////////////////////////////////////////////////
// class CMibFileParser

CMibFileParser::CMibFileParser()
{
	m_pFile = NULL;
	m_CurrentKeyWord = MIB_KEYWORD_UNKNOWN_YET;
	m_nFileLineNo = 0;
	m_pCurrentModule = NULL;
}

//virtual
CMibFileParser::~CMibFileParser()
{
}

MIB_ERROR_CODE_ENUM CMibFileParser::ParseMib(const char *pszFileName)
{
	m_pCurrentModule = NULL;
	m_FilePath = pszFileName;
	m_FilePath.LeaveDirNameOnly();
	
	m_pFile = fopen(pszFileName, "r");
	if (!m_pFile)
	{
		if (!strstr(pszFileName, PSZ_MIB_EXTENSION))
		{
			CTextString MibFileName = pszFileName;
			MibFileName += PSZ_MIB_EXTENSION;
			m_pFile = fopen(MibFileName.GetText(), "r");
		}
		if (!m_pFile)
			return MIB_ERROR_CODE_CANNOT_OPEN_FILE;
	}
	MIB_ERROR_CODE_ENUM ErrorCode = ParseMib(m_pFile);
	fclose(m_pFile);
	m_pFile = NULL;
	return ErrorCode;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ParseMib(FILE *pFile)
{
	m_nFileLineNo = 1;
	m_pFile = pFile;
	TRACE("---------------------------------------------\n");
	MIB_ERROR_CODE_ENUM EC;
	while (1)
	{
		EC = ParseNextToken();
		if (EC == MIB_ERROR_CODE_EOF)
			break;
		if (EC != MIB_ERROR_CODE_OK)
			return EC;
		if (strlen(m_TokenStr.GetText()) < 512)
		{
			TRACE("%s\n", m_TokenStr.GetText());
		//	Sleep(100);
		}
		ProcessToken();
	}
	TRACE("---------------------------------------------\n");
	return MIB_ERROR_CODE_OK;
}

static SMibKeyWord *FindKeyWord(CTextString &token)
{
	for (int i = 0; i < sizeof(KeyWords)/sizeof(KeyWords[0]); i++)
	{
		SMibKeyWord *pKeyWord = KeyWords+i;
		if (!stricmp(token.GetText(), pKeyWord->pszText))
			return pKeyWord;
	}
	return NULL;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessToken()
{
	SMibKeyWord *pKeyWord = FindKeyWord(m_TokenStr);
	if (pKeyWord)
	{
		MIB_ERROR_CODE_ENUM EC = ProcessKeyWord(pKeyWord);
		if (EC != MIB_ERROR_CODE_NOT_KEYWORD)
			return EC; // else continue processing, this is not keyword but object identifier, for example
	}

	m_SymbolicName2 = m_SymbolicName1;
	m_SymbolicName1 = m_TokenStr;
	return MIB_ERROR_CODE_OK;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessKeyWord(SMibKeyWord *pKeyWord)
{
	MIB_ERROR_CODE_ENUM EC = MIB_ERROR_CODE_OK;

	switch (pKeyWord->KeyWord)
	{
	case MIB_KEYWORD_DEFINITIONS:
		EC = ProcessDefinitionsKeyWord();
		break;
	case MIB_KEYWORD_BEGIN:
		EC = ProcessBeginKeyWord();
		break;
	case MIB_KEYWORD_END:
		EC = ProcessEndKeyWord();
		break;
	case MIB_KEYWORD_IMPORTS:
		EC = ProcessImportsKeyWord();
		break;
	case MIB_KEYWORD_OBJECT:
		EC = ProcessObjectKeyWord();
		break;
	case MIB_KEYWORD_IDENTIFIER:
		EC = ProcessIdentifierKeyWord();
		break;
	case MIB_KEYWORD_OBJECT_TYPE:
		EC = ProcessObjectTypeKeyWord();
		break;
	case MIB_KEYWORD_MODULE_IDENTITY:
		EC = ProcessModuleIdentityKeyWord();
	case MIB_KEYWORD_STATUS:
		EC = ProcessStatusKeyWord();
		break;
	case MIB_KEYWORD_ACCESS:
		EC = ProcessAccessKeyWord();
		break;
	} // end of switch
	m_CurrentKeyWord = pKeyWord->KeyWord;
	
	return EC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessDefinitionsKeyWord()
{
	// Deal with optional OID_VALUE token between DEFINITION clause
	if (m_SymbolicName1.IsEmpty())
		m_pCurrentModule->SetModuleName(m_SymbolicName2.GetText());
	else
		m_pCurrentModule->SetModuleName(m_SymbolicName1.GetText());

	m_SymbolicName1.Empty();
	m_SymbolicName2.Empty();
	return MIB_ERROR_CODE_OK;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessBeginKeyWord()
{
	return MIB_ERROR_CODE_OK;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessEndKeyWord()
{
	return MIB_ERROR_CODE_OK;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessImportsKeyWord()
{
	CListOfStrings ImportedNames;
	MIB_ERROR_CODE_ENUM EC = MIB_ERROR_CODE_OK;
	LOGICAL bNeedGoAhead = LOGICAL_TRUE;
	while (1)
	{
		EC = ParseNextToken();
		if (EC == MIB_ERROR_CODE_EOF)
			break;
		if (m_CurrentKeyWord == MIB_KEYWORD_FROM)
		{
			// m_TokenStr contains module name
			while (strchr(m_TokenStr.GetText(), ';'))
			{
				m_TokenStr.TrimRight(1);
				bNeedGoAhead = LOGICAL_FALSE;  // semicolon symbol ';' ends IMPORTS clause
			}
			if (ImportedNames.size())
			{
				// remove latest semicolon at the end of module name
				CMibModule *pModule = g_MibModules.GetModuleByName(m_TokenStr.GetText());
				if (!pModule)
				{ // does not exist yet. Load it from somewhere
					g_MibModules.LoadMibModule(m_FilePath.GetText(), m_TokenStr.GetText());
					pModule = g_MibModules.GetModuleByName(m_TokenStr.GetText());
				}
				ASSERT(pModule);
				for (CListOfStrings::iterator it = ImportedNames.begin(); it != ImportedNames.end(); it++)
				{
					CTextString str = *it;
					if (LoadFromModule(str.GetText(), pModule) != MIB_ERROR_CODE_OK)
					{
						TRACE("Cannot inport %s from module %s\n", str.GetText(), m_TokenStr.GetText());
					}
				}
			}
			ImportedNames.resize(0);

			m_CurrentKeyWord = MIB_KEYWORD_IMPORTS;
		}
		else if (m_TokenStr == "FROM")
		{
			m_CurrentKeyWord = MIB_KEYWORD_FROM;	
		}
		else if (FindKeyWord(m_TokenStr))
		{
			// do nothing. Keywords like Counter and TimeTicks are hardcoded and ignored here
		}
		else
		{			
			ImportedNames.push_back(m_TokenStr);
		}

		if (!bNeedGoAhead)
			break;
	}
	return EC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessObjectKeyWord()
{
	MIB_ERROR_CODE_ENUM EC = MIB_ERROR_CODE_OK;
	m_ObjectName = m_SymbolicName1; // store object name
	return EC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessIdentifierKeyWord()
{
	if (m_CurrentKeyWord == MIB_KEYWORD_OBJECT)
		return ProcessObjectIdentifierKeyWord();
	ASSERT(0);
	return MIB_ERROR_CODE_GENERIC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ParseIdentifier(CTextString &ObjectName)
{
	MIB_ERROR_CODE_ENUM EC = MIB_ERROR_CODE_OK;

	m_TmpForDescriptions.Empty();
	while (m_TokenStr != "::=")
	{
		if (m_TokenStr == "LAST-UPDATED" ||
			m_TokenStr == "REVISION")
		{
			EC = ParseNextToken();
			// TODO: add data conversion here
			m_TmpForDescriptions += m_TokenStr;
			m_TmpForDescriptions += "\n";
		}
		else if (m_TokenStr == "DESCRIPTION")
		{
			EC = ParseNextToken();
			m_TmpForDescriptions += m_TokenStr;
			m_TmpForDescriptions += "\n";
		}

		EC = ParseNextToken();
	}

	if (EC == MIB_ERROR_CODE_OK)
	{
		EC = ParseNextToken();
		CMibNode *pNewNode = m_pCurrentModule->CreateNewNode(ObjectName, m_TokenStr);
	}
	return EC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessObjectIdentifierKeyWord()
{
	MIB_ERROR_CODE_ENUM EC = ParseIdentifier(m_ObjectName);

	return EC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessObjectTypeKeyWord()
{
	MIB_ERROR_CODE_ENUM EC = MIB_ERROR_CODE_OK;

	m_ObjectName = m_SymbolicName1; // store object name
	m_CurrentKeyWord = MIB_KEYWORD_OBJECT_TYPE;
	m_TmpForDescriptions.Empty();
	CMibObjectSyntax *pSyntax = NULL;
	CMibObjectType *pObjectType = new CMibObjectType();

	while (1)
	{
		EC = ParseNextToken();
		if (m_TokenStr == "::=")
			break;
		SMibKeyWord *pKeyWord = FindKeyWord(m_TokenStr);
		if (pKeyWord)
		{
			//CurrentKeyWord = pKeyWord->KeyWord;
			switch (pKeyWord->KeyWord)
			{
			case MIB_KEYWORD_SYNTAX:
				EC = ParseNextToken();
				pSyntax = ProcessSyntaxKeyWord(m_TokenStr);
				pObjectType->SetSyntax(pSyntax);
				break;
			case MIB_KEYWORD_DESCRIPTION:
				EC = ParseNextToken();
				m_TmpForDescriptions += m_TokenStr;
				break;
			case MIB_KEYWORD_STATUS:
				EC = ProcessStatusKeyWord();
				break;
			case MIB_KEYWORD_ACCESS:
				EC = ProcessAccessKeyWord();
				break;
			case MIB_KEYWORD_DEFVAL:
				EC = ParseNextToken();
				break;
			case MIB_KEYWORD_INDEX:
				EC = ParseNextToken();
				break;
			case MIB_KEYWORD_REFERENCE:
				EC = ParseNextToken();
				break;
			default:
				ASSERT(0);
				break;
			}
		}
		else
		{
			// token is not keyword, probably that is syntax additional specification like (0..65535) or something like that
			if (pSyntax)
				pSyntax->ProcessSpecificationToken(m_TokenStr);
			else
			{
				ASSERT(0);
			}
		}
	}
	pObjectType->SetDescription(m_TmpForDescriptions);

	if (EC == MIB_ERROR_CODE_OK)
	{
		EC = ParseNextToken();
		CMibNode *pNewNode = m_pCurrentModule->CreateNewNode(m_ObjectName, m_TokenStr);
		pNewNode->SetObjectType(pObjectType);
	}
	return EC;
}

CMibObjectSyntax *CMibFileParser::ProcessSyntaxKeyWord(CTextString &token)
{
	CMibObjectSyntax *pSyntax = NULL;
	SMibKeyWord *pKeyWord = FindKeyWord(token);
	if (pKeyWord)
	{
		switch (pKeyWord->KeyWord)
		{
		case MIB_KEYWORD_INTEGER:
			pSyntax = ProcessSyntaxIntegerKeyWord();
			break;
		case MIB_KEYWORD_INTEGER32:
			break;
		case MIB_KEYWORD_COUNTER:
			break;
		case MIB_KEYWORD_COUNTER32:
			break;
		case MIB_KEYWORD_GAUGE:
			break;
		case MIB_KEYWORD_GAUGE32:
			break;
		case MIB_KEYWORD_IPADDRESS:
			break;
		case MIB_KEYWORD_SEQUENCE:
			pSyntax = ProcessSyntaxSequenceKeyWord();
			break;
		case MIB_KEYWORD_OCTETSTRING:
			pSyntax = ProcessSyntaxOctetStringKeyWord();
			break;
		case MIB_KEYWORD_OCTET:
			// STRING after OCTET is expected
			ParseNextToken();
			pSyntax = ProcessSyntaxOctetStringKeyWord();
			break;
		case MIB_KEYWORD_STRING:
			pSyntax = ProcessSyntaxOctetStringKeyWord();
			break;
		default:
			pKeyWord = pKeyWord;//debug
			ASSERT(0);
		}
	}
	else
	{ // Later declaration expected

	}

	return pSyntax;
}

CMibObjectSyntax *CMibFileParser::ProcessSyntaxIntegerKeyWord()
{
	CMibObjectSyntaxInteger *pSyntax = new CMibObjectSyntaxInteger;

	return pSyntax;
}

CMibObjectSyntax *CMibFileParser::ProcessSyntaxSequenceKeyWord()
{
	CMibObjectSyntaxSequence *pSyntax = new CMibObjectSyntaxSequence;

	return pSyntax;
}

CMibObjectSyntax *CMibFileParser::ProcessSyntaxOctetStringKeyWord()
{
	CMibObjectSyntax *pSyntax = new CMibObjectSyntaxOctetString;

	return pSyntax;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessModuleIdentityKeyWord()
{
	m_pCurrentModule->SetModuleIdentity(m_SymbolicName1.GetText());
	MIB_ERROR_CODE_ENUM EC = ParseIdentifier(m_SymbolicName1);

	return EC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessStatusKeyWord()
{
	if (m_CurrentKeyWord != MIB_KEYWORD_OBJECT_TYPE)
		return MIB_ERROR_CODE_NOT_KEYWORD;

	MIB_ERROR_CODE_ENUM EC = ParseNextToken();
	return EC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ProcessAccessKeyWord()
{
	if (m_CurrentKeyWord != MIB_KEYWORD_OBJECT_TYPE)
		return MIB_ERROR_CODE_NOT_KEYWORD;

	MIB_ERROR_CODE_ENUM EC = ParseNextToken();
	return EC;
}

MIB_ERROR_CODE_ENUM CMibFileParser::LoadFromModule(char *pszName, CMibModule *pModule)
{
	if (!pModule)
		return MIB_ERROR_CODE_INVALID_MODULE;
	CMibNode *pNode = pModule->FindNode(pszName);
	if (!pNode)
		return MIB_ERROR_CODE_CANNOT_IMPORT;

	// Node and all its parents (father, grandfather, grandgrandfather etc)
	CMibNode *pNewNode = m_pCurrentModule->CreateNewNode(pNode);
	CMibNode *pParentNodeInModule = pNode->GetParent();
	while (pParentNodeInModule && strlen(pParentNodeInModule->GetSymbolicName()) != 0)
	{
		CMibNode *pParentNode = m_pCurrentModule->FindNode(pParentNodeInModule->GetSymbolicName());
		if (!pParentNode)
		{
			pParentNode = pParentNodeInModule->Clone(m_pCurrentModule);
			pNewNode = pParentNode;
			pParentNodeInModule = pParentNodeInModule->GetParent();
		}
		else
		{
			pParentNode->NewChild(pNewNode);
			break;
		}
	}

	if (!pNewNode->GetParent())
		m_pCurrentModule->m_RootNode.NewChild(pNewNode);
	return MIB_ERROR_CODE_OK;
}

MIB_ERROR_CODE_ENUM CMibFileParser::ParseNextToken()
{
	return ParseNextToken(m_TokenStr);
}

MIB_ERROR_CODE_ENUM CMibFileParser::ParseNextToken(CTextString &token)
{
	const char *pszDelimeters = " \t\r\n,";
	token.Empty();
	LOGICAL bDash = LOGICAL_FALSE, bDoubleDash = LOGICAL_FALSE;
	LOGICAL bQuote = LOGICAL_FALSE;
	int iBracketLevel = 0;
	while (1)
	{
		int ch = fgetc(m_pFile);
		if (ch == EOF)
		{
			if (token.IsEmpty())
				return MIB_ERROR_CODE_EOF;
			else
				return MIB_ERROR_CODE_OK;
		}
		char c = LOBYTE(LOWORD(ch));
		if (ch == 0x0D || ch == 0x0A)
		{
			bDash = bDoubleDash = LOGICAL_FALSE;
			if (ch == 0x0A)
				++m_nFileLineNo; // inc line counter
		}
		if (!bQuote && !iBracketLevel && strchr(pszDelimeters, ch))
		{
			if (!token.IsEmpty())
				return MIB_ERROR_CODE_OK;
		}
		else
		{
			if (!bQuote)
			{
				if (bDoubleDash)
					continue; // skip everything till CRLF
				if (ch == '-')
				{
					if (bDash)
					{
						bDoubleDash = LOGICAL_TRUE;
						token.TrimRight(1);
						continue;
					}
					bDash = LOGICAL_TRUE;
				}
				else
					bDash = bDoubleDash = LOGICAL_FALSE;
			}

			if (ch == '\"')
			{
				if (bQuote)
					return MIB_ERROR_CODE_OK;
				bQuote = LOGICAL_TRUE;
			}
			
			if (!bQuote)
			{
				if (ch == '(' || ch == '{')
				{
					++iBracketLevel;
					c = ' ';
				}
				else if (ch == '}' || ch == ')')
				{
					--iBracketLevel;
					if (!iBracketLevel)
						return MIB_ERROR_CODE_OK;
					c = ' ';
				}
			}
			if (c != '"')
				token += c;
		}
	}

	return MIB_ERROR_CODE_OK;
}


