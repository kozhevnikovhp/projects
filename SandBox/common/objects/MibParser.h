/* MibParser.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	27 Sep 2008 - initial creation
	10 Nov 2008 - tree structure
	20 Nov 2008 - OBJECT_TYPE, SYNTAX clause started

*/

#ifndef __MIB_PARSER_H_INCLUDED__
#define __MIB_PARSER_H_INCLUDED__

#include "Portable.h"
#include <vector>

// Forward declarations
class CMibNode;
class CMibModule;
class CMibObjectType;

typedef enum
{
	MIB_KEYWORD_UNKNOWN_YET,
	MIB_KEYWORD_OBSOLETE,
	MIB_KEYWORD_OPAQUE,
	MIB_KEYWORD_OPTIONAL,
	MIB_KEYWORD_LAST_UPDATED,
	MIB_KEYWORD_ORGANIZATION,
	MIB_KEYWORD_CONTACT_INFO,
	MIB_KEYWORD_MODULE_IDENTITY,
	MIB_KEYWORD_MODULE_COMPLIANCE,
	MIB_KEYWORD_DEFINITIONS,
	MIB_KEYWORD_END,
	MIB_KEYWORD_AUGMENTS,
	MIB_KEYWORD_NOT_ACCESSIBLE,
	MIB_KEYWORD_WRITE_ONLY,
	MIB_KEYWORD_NSAP_ADDRESS,
	MIB_KEYWORD_UNITS,
	MIB_KEYWORD_REFERENCE,
	MIB_KEYWORD_NUM_ENTRIES,
	MIB_KEYWORD_BITSTRING,
	MIB_KEYWORD_BIT,
	MIB_KEYWORD_BITS,
	MIB_KEYWORD_COUNTER64,
	MIB_KEYWORD_TIMETICKS,
	MIB_KEYWORD_NOTIFICATION_TYPE,
	MIB_KEYWORD_OBJECT_GROUP,
	MIB_KEYWORD_OBJECT_IDENTITY,
	MIB_KEYWORD_IDENTIFIER,
	MIB_KEYWORD_OBJECT,
	MIB_KEYWORD_OBJECT_IDENTIFIER,
	MIB_KEYWORD_NETWORK_ADDRESS,
	MIB_KEYWORD_GAUGE,
	MIB_KEYWORD_GAUGE32,
	MIB_KEYWORD_UNSIGNED32,
	MIB_KEYWORD_READ_WRITE,
	MIB_KEYWORD_READ_CREATE,
	MIB_KEYWORD_OCTETSTRING,
	MIB_KEYWORD_OCTET,
	MIB_KEYWORD_STRING,
//	MIB_KEYWORD_OF,
	MIB_KEYWORD_SEQUENCE,
	MIB_KEYWORD_NULL,
	MIB_KEYWORD_IPADDRESS,
	MIB_KEYWORD_UINTEGER32,
	MIB_KEYWORD_INTEGER,
	MIB_KEYWORD_INTEGER32,
	MIB_KEYWORD_COUNTER,
	MIB_KEYWORD_COUNTER32,
	MIB_KEYWORD_READ_ONLY,
	MIB_KEYWORD_DESCRIPTION,
	MIB_KEYWORD_INDEX,
	MIB_KEYWORD_DEFVAL,
	MIB_KEYWORD_DEPRECATED,
	MIB_KEYWORD_SIZE,
	MIB_KEYWORD_MAX_ACCESS,
	MIB_KEYWORD_ACCESS,
	MIB_KEYWORD_MANDATORY,
	MIB_KEYWORD_CURRENT,
	MIB_KEYWORD_STATUS,
	MIB_KEYWORD_SYNTAX,
	MIB_KEYWORD_OBJECT_TYPE,
	MIB_KEYWORD_TRAP_TYPE,
	MIB_KEYWORD_ENTERPRISE,
	MIB_KEYWORD_BEGIN,
	MIB_KEYWORD_IMPORTS,
	MIB_KEYWORD_EXPORTS,
	MIB_KEYWORD_ACCESSIBLE_FOR_NOTIFY,
	MIB_KEYWORD_TEXTUAL_CONVENTION,
	MIB_KEYWORD_NOTIFICATION_GROUP,
	MIB_KEYWORD_DISPLAY_HINT,
	MIB_KEYWORD_DISPLAY_STRING,
	MIB_KEYWORD_PHYS_ADDRESS,
	MIB_KEYWORD_TRUTH_VALUE,
	MIB_KEYWORD_ROW_STATUS,
	MIB_KEYWORD_TIMESTAMP,
	MIB_KEYWORD_AUTONOMOUS_TYPE,
	MIB_KEYWORD_TEST_AND_INCR,
	MIB_KEYWORD_FROM,
	MIB_KEYWORD_AGENT_CAPABILITIES,
	MIB_KEYWORD_MACRO,
	MIB_KEYWORD_IMPLIED,
	MIB_KEYWORD_SUPPORTS,
	MIB_KEYWORD_INCLUDES,
	MIB_KEYWORD_VARIATION,
	MIB_KEYWORD_REVISION,
	MIB_KEYWORD_NOT_IMPLEMENTED,
	MIB_KEYWORD_OBJECTS,
	MIB_KEYWORD_NOTIFICATIONS,
	MIB_KEYWORD_MODULE,
	MIB_KEYWORD_MIN_ACCESS,
	MIB_KEYWORD_PRODUCT_RELEASE,
	MIB_KEYWORD_WRITE_SYNTAX,
	MIB_KEYWORD_CREATION_REQUIRES,
	MIB_KEYWORD_MANDATORY_GROUPS,
	MIB_KEYWORD_GROUP,
	MIB_KEYWORD_CHOICE,
	MIB_KEYWORD_IMPLICIT,
	MIB_KEYWORD_OBJECT_SYNTAX,
	MIB_KEYWORD_SIMPLE_SYNTAX,
	MIB_KEYWORD_APPLICATION_SYNTAX,
	MIB_KEYWORD_OBJECT_NAME,
	MIB_KEYWORD_NOTIFICATION_NAME,
	MIB_KEYWORD_VARIABLES
} MIB_KEYWORD_ENUM;

typedef struct SMibKeyWord
{
    const char *pszText;
	MIB_KEYWORD_ENUM KeyWord;
} SMibKeyWord;

class CMibNodes : public std::vector<CMibNode *>
{
// Constructors/destructors
public:
	CMibNodes();
	virtual ~CMibNodes();

// Public methods
public:
	CMibNode *FindNode(char *pszNodeName) const;

// Protected members
protected:
};

// interface class for all nodes of the tree visiting
class CMibTreeNodeVisitor
{
// Public overridables
public:
	virtual LOGICAL Visit(CMibNode *pNode) = 0;
	virtual void LowerLevel() {}
	virtual void UpperLevel() {}

// Protected overridables
protected:
};

typedef enum
{
	MIB_OBJECT_SYNTAX_NONE,			// for tree brunches, not for leaves
	MIB_OBJECT_SYNTAX_INTEGER,
	MIB_OBJECT_SYNTAX_COUNTER

} MIB_OBJECT_SYNTAX_ENUM;

class CMibObjectSyntax
{
// Constructors/destructors
public:
	CMibObjectSyntax();
	virtual ~CMibObjectSyntax();

// Public methods
public:

// Public overridables
public:
	virtual void ProcessSpecificationToken(CTextString &token) {}

// Protected methods
protected:

// Protected members
protected:
};

class CMibObjectSyntaxInteger : public CMibObjectSyntax
{
// Constructors/destructors
public:
	CMibObjectSyntaxInteger();
	virtual ~CMibObjectSyntaxInteger();

// Public methods
public:

// Public overridables
public:
	virtual void ProcessSpecificationToken(CTextString &token);

// Protected methods
protected:

// Protected members
protected:
};

class CMibObjectSyntaxSequence : public CMibObjectSyntax
{
// Constructors/destructors
public:
	CMibObjectSyntaxSequence();
	virtual ~CMibObjectSyntaxSequence();

// Public methods
public:

// Public overridables
public:
	virtual void ProcessSpecificationToken(CTextString &token);

// Protected methods
protected:

// Protected members
protected:
	LOGICAL m_bOf; // SEQUENCE or SEQUENCE OF
	CTextString m_strSequenceOfWhat;
	CMibObjectType *m_pSequenceOfWhat;
};

class CMibObjectSyntaxOctetString : public CMibObjectSyntax
{
// Constructors/destructors
public:
	CMibObjectSyntaxOctetString();
	virtual ~CMibObjectSyntaxOctetString();

// Public methods
public:

// Public overridables
public:
	virtual void ProcessSpecificationToken(CTextString &token);

// Protected methods
protected:

// Protected members
protected:
};

class CMibObjectType
{
// Constructors/destructors
public:
	CMibObjectType();
	virtual ~CMibObjectType();

// Public methods
public:
	void SetSyntax(CMibObjectSyntax *pSyntax) { m_pSyntax = pSyntax; }
	void SetDescription(char *pszDescription) { m_Description = pszDescription; }
	void SetDescription(CTextString &Description) { m_Description = Description; }
	const char *GetDescription() { return m_Description.GetText(); }
// Protected methods
protected:

// Protected members
protected:
	CMibObjectSyntax *m_pSyntax;
	CTextString m_Description;
};

class CMibNode
{
	friend class CMibTree;
// Constructors/destructors
public:
	CMibNode();
	CMibNode(CMibTree *pTree);
	CMibNode(CMibTree *pTree, unsigned int iLastDigit, char *pszSymbolicName, CMibNode *pParent);
	virtual ~CMibNode();

// Public methods
public:
	CMibNode *Clone(CMibTree *pTree);
	void ClonedFrom(CMibNode *pFrom) { m_pWasClonedFrom = pFrom; }
	char *GetSymbolicName();
	char *GetDecimalName();
	LOGICAL IsNameEqual(char *pszName);
	void NewChild(CMibNode *pNewChild);
	unsigned int GetLastOidDigit();
	CMibNode *GetParent() { return m_pParent; }
	CMibObjectType *GetObjectType() { return m_pObjectType; }
	void SetObjectType(CMibObjectType *pObjectType) { m_pObjectType = pObjectType; }
	LOGICAL DoesNodeExist(CMibNode *pNode);
	CMibNode *FindNode(char *pszNodeName);
	void Clean();
	char *GetFullTextSpec();
	char *GetFullDecimalSpec();

// Protected methods
protected:
	LOGICAL VisitChildren(CMibTreeNodeVisitor *pVisitor);
	LOGICAL Visit(CMibTreeNodeVisitor *pVisitor);
	void SetParent(CMibNode *pParent);

// Protected members
protected:
	CMibTree *m_pTree;
	CMibNodes m_Children;
	CMibNode *m_pParent;
	unsigned int m_iLastOidDigit;
	char *m_pszSymbolicName;
	char *m_pszDecimalName;
	CMibNode *m_pWasClonedFrom;
	CMibObjectType *m_pObjectType;
	CTextString m_FullTextSpec;
	CTextString m_FullDecimalSpec;
};

typedef enum
{
	MIB_ERROR_CODE_OK,
	MIB_ERROR_CODE_GENERIC,
	MIB_ERROR_CODE_EOF,
	MIB_ERROR_CODE_NOT_KEYWORD,
	MIB_ERROR_CODE_OUTOFMEMORY,
	MIB_ERROR_CODE_INVALID_MODULE,
	MIB_ERROR_CODE_CANNOT_IMPORT,
	MIB_ERROR_CODE_CANNOT_FIND_NODE,
	MIB_ERROR_CODE_CANNOT_OPEN_FILE
} MIB_ERROR_CODE_ENUM;

class CDelayedDesc
{
// Constructors/destructors
public:
	CDelayedDesc(CTextString &ObjectName, CTextString &ParentName, unsigned int nDecimalSpec);
	virtual ~CDelayedDesc();

// Public methods
public:
	CMibNode *TryToCreate(CMibNode *pPotentiallyParentNode, CMibTree *pTree);

// Protected members
protected:
	CTextString m_ObjectName;
	CTextString m_ParentName;
	unsigned int m_iDecimalSpec;
};

class CDelayedDescs : public std::vector<CDelayedDesc>
{
// Constructors/destructors
public:
	CDelayedDescs();
	virtual ~CDelayedDescs();

// Public methods
public:
	void NewDesc(CTextString &ObjectName, CTextString &ParentName, unsigned int iDecimalSpec);
	void TryToCreate(CMibNode *pPotentiallyParentNode, CMibTree *pTree);
};

class CMibTree
{
	friend class CMibFileParser;
	friend class CDelayedDesc;
// Constructors/destructors
public:
	CMibTree();
	virtual ~CMibTree();

// Public methods
public:
	void Clean();
	MIB_ERROR_CODE_ENUM NodeCreated(CMibNode *pNode);
	LOGICAL VisitAllNodes(CMibTreeNodeVisitor *pVisitor);

// Protected methods
protected:
	CMibNode *CreateNewNode(unsigned int iLastDigit, char *pszSymbolicName, CMibNode *pParent);
	CMibNode *CreateNewNode(CMibNode *pNode);
	CMibNode *CreateNewNode(CTextString &Name, CTextString &token);
	LOGICAL DoesNodeExist(CMibNode *pNode);
	LOGICAL DoesNodeExist(CMibNode *pNode, CMibNode *pFindIn);
	CMibNode *FindNode(char *pszNodeName);


protected:
// Protected members
	CMibNode m_RootNode;
	CDelayedDescs m_DelayedDescs;
	
	CTextString m_ModuleName;
	CTextString m_ModuleIdentity;
	CMibNodes m_Nodes;
	CMibNodes m_ParsedNodes;
};

class CMibFileParser
{
// Constructors/destructors
public:
	CMibFileParser();
	virtual ~CMibFileParser();

// Public methods
	MIB_ERROR_CODE_ENUM ParseMib(const char *pszFileName);
	MIB_ERROR_CODE_ENUM ParseMib(FILE *pFile);

// Protected methods
protected:
	MIB_ERROR_CODE_ENUM ParseNextToken();
	MIB_ERROR_CODE_ENUM ParseNextToken(CTextString &token);
	MIB_ERROR_CODE_ENUM ProcessToken();
	MIB_ERROR_CODE_ENUM ProcessKeyWord(SMibKeyWord *pKeyWord);

	MIB_ERROR_CODE_ENUM ProcessDefinitionsKeyWord();
	MIB_ERROR_CODE_ENUM ProcessBeginKeyWord();
	MIB_ERROR_CODE_ENUM ProcessEndKeyWord();
	MIB_ERROR_CODE_ENUM ProcessImportsKeyWord();
	MIB_ERROR_CODE_ENUM ProcessObjectKeyWord();
	MIB_ERROR_CODE_ENUM ProcessIdentifierKeyWord();
	MIB_ERROR_CODE_ENUM ProcessObjectIdentifierKeyWord();
	MIB_ERROR_CODE_ENUM ProcessObjectTypeKeyWord();
	CMibObjectSyntax *ProcessSyntaxKeyWord(CTextString &token);
	CMibObjectSyntax *ProcessSyntaxIntegerKeyWord();
	CMibObjectSyntax *ProcessSyntaxSequenceKeyWord();
	CMibObjectSyntax *ProcessSyntaxOctetStringKeyWord();
	MIB_ERROR_CODE_ENUM ProcessModuleIdentityKeyWord();
	MIB_ERROR_CODE_ENUM ProcessStatusKeyWord();
	MIB_ERROR_CODE_ENUM ProcessAccessKeyWord();
	MIB_ERROR_CODE_ENUM ParseIdentifier(CTextString &ObjectName);
	MIB_ERROR_CODE_ENUM LoadFromModule(char *pszName, CMibModule *pModule);
	
// Protected members
protected:
	FILE *m_pFile;
	CTextString m_FilePath;
	int m_nFileLineNo;
	
	CTextString m_TokenStr;
	CTextString m_SymbolicName1, m_SymbolicName2;
	CTextString m_ObjectName;
	CTextString m_TmpForDescriptions;
	MIB_KEYWORD_ENUM m_CurrentKeyWord;
	CMibNodes m_Nodes;
	CMibNodes m_ParsedNodes;
	CMibModule *m_pCurrentModule;
};

class CMibModule : public CMibTree
{
// Constructors/destructors
public:
	CMibModule();
	virtual ~CMibModule();

	void SetModuleIdentity(char *pszModuleIdentity) { m_ModuleIdentity = pszModuleIdentity; }
	void SetModuleName(char *pszModuleName) { m_ModuleName = pszModuleName; }

// Public overridables
public:
	virtual LOGICAL IsMyself(char *pszModuleName) = 0;

// Public methods
public:

// Protected members
protected:
};

class CMibModules : public std::vector<CMibModule *>
{
// Constructors/destructors
public:
	CMibModules();
	virtual ~CMibModules();

// Public methods
public:
	CMibModule *GetModuleByName(char *pszModuleName);
	MIB_ERROR_CODE_ENUM LoadMibModule(char *pszPath, char *pszModuleName);

// Protected members
protected:
};

class CMibModuleFromFile : public CMibModule
{
// Constructors/destructors
public:
	CMibModuleFromFile();
	virtual ~CMibModuleFromFile();

// Public overridables
public:
	virtual LOGICAL IsMyself(char *pszModuleName);

// Public methods
public:

// Protected members
protected:
};

class CSNMPv1_SMI : public CMibModule
{
// Constructors/destructors
public:
	CSNMPv1_SMI();
	virtual ~CSNMPv1_SMI();

// Public overridables
public:
	virtual LOGICAL IsMyself(char *pszModuleName);

// Public methods
public:
	static LOGICAL IsMyName(char *pszModuleName);

// Protected members
protected:
	CMibNode *m_pIsoNode;
	CMibNode *m_pOrgNode;
	CMibNode *m_pDodNode;
	CMibNode *m_pInternetNode;
	CMibNode *m_pDirectoryNode;
	CMibNode *m_pMgmtNode;
	CMibNode *m_pMib2Node;
	CMibNode *m_pSystemNode;
	CMibNode *m_pInterfacesNode;
	CMibNode *m_pAtNode;
	CMibNode *m_pIpNode;
	CMibNode *m_pIcmpNode;
	CMibNode *m_pTcpNode;
	CMibNode *m_pUdpNode;
	CMibNode *m_pEgpNode;
	CMibNode *m_pTransmissionNode;
	CMibNode *m_pSnmpNode;
	CMibNode *m_pExperimentalNode;
	CMibNode *m_pPrivateNode;
	CMibNode *m_pEnterprisesNode;
	CMibNode *m_pSecurityNode;
	CMibNode *m_pSnmpV2Node;
	CMibNode *m_pSnmpDomainsNode;
	CMibNode *m_pSnmpProxiesNode;
	CMibNode *m_pSnmpModulesNode;
};

class CSNMPv2_SMI : public CMibModule
{
// Constructors/destructors
public:
	CSNMPv2_SMI();
	virtual ~CSNMPv2_SMI();

// Public overridables
public:
	virtual LOGICAL IsMyself(char *pszModuleName);

// Public methods
public:
	static LOGICAL IsMyName(char *pszModuleName);

// Protected members
protected:
	CMibNode *m_pIsoNode;
	CMibNode *m_pOrgNode;
	CMibNode *m_pDodNode;
	CMibNode *m_pInternetNode;
	CMibNode *m_pDirectoryNode;
	CMibNode *m_pMgmtNode;
	CMibNode *m_pMib2Node;
	CMibNode *m_pSystemNode;
	CMibNode *m_pInterfacesNode;
	CMibNode *m_pAtNode;
	CMibNode *m_pIpNode;
	CMibNode *m_pIcmpNode;
	CMibNode *m_pTcpNode;
	CMibNode *m_pUdpNode;
	CMibNode *m_pEgpNode;
	CMibNode *m_pTransmissionNode;
	CMibNode *m_pSnmpNode;
	CMibNode *m_pExperimentalNode;
	CMibNode *m_pPrivateNode;
	CMibNode *m_pEnterprisesNode;
	CMibNode *m_pSecurityNode;
	CMibNode *m_pSnmpV2Node;
	CMibNode *m_pSnmpDomainsNode;
	CMibNode *m_pSnmpProxiesNode;
	CMibNode *m_pSnmpModulesNode;
};

#endif // __MIB_PARSER_H_INCLUDED__

