/* NetworkDocAPI.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	07 Sep 2006 - initial creation
*/

#ifndef __NETWORK_DOC_API_H_INCLUDED__
#define __NETWORK_DOC_API_H_INCLUDED__

// Forward declarations
class CNetworkDocAPI;
class CNetworkFrameAPI;
class CNetObject;
class CHost;
class CSubnet;
class IGuiAPI;

// Visitors

class CDocVisitorAPI
{
// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc) = 0;

// Protected overridables
protected:

};

class COnIdleVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	COnIdleVisitor();
	virtual ~COnIdleVisitor();

// Public methods
public:
	BOOL IsToBeContinued() { return m_bNeedToBecontinued; }
	
// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	BOOL m_bNeedToBecontinued;
};

class CGuiCreatedVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CGuiCreatedVisitor(IGuiAPI *pGui);
	virtual ~CGuiCreatedVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	IGuiAPI *m_pGui;
};

class CNewHostCreatedVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CNewHostCreatedVisitor(CHost *pHost);
	virtual ~CNewHostCreatedVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CHost *m_pHost;
};

class CHostChangedVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CHostChangedVisitor(CHost *pHost);
	virtual ~CHostChangedVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CHost *m_pHost;
};

class CNewSubnetCreatedVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CNewSubnetCreatedVisitor(CSubnet *pSubnet);
	virtual ~CNewSubnetCreatedVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CSubnet *m_pSubnet;
};

class CSubnetChangedVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CSubnetChangedVisitor(CSubnet *pSubnet);
	virtual ~CSubnetChangedVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CSubnet *m_pSubnet;
};

class CHostAddedToSubnetVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CHostAddedToSubnetVisitor(CHost *pHost, CSubnet *pSubnet);
	virtual ~CHostAddedToSubnetVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CHost *m_pHost;
	CSubnet *m_pSubnet;
};

class CHostRemovedFromSubnetVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CHostRemovedFromSubnetVisitor(CHost *pHost, CSubnet *pSubnet);
	virtual ~CHostRemovedFromSubnetVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CHost *m_pHost;
	CSubnet *m_pSubnet;
};


class CObjectSelectionChangedVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CObjectSelectionChangedVisitor(CNetObject *pObject, void *pWhereChanged);
	virtual ~CObjectSelectionChangedVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CNetObject *m_pObject;
	void *m_pWhereChanged;
};


class CUpdateGuiVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CUpdateGuiVisitor();
	virtual ~CUpdateGuiVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
};


class CNetworkDocAPI : public CDocument
{
	friend class CNetworkFrameAPI;
protected: // create from serialization only
	CNetworkDocAPI();
	DECLARE_DYNCREATE(CNetworkDocAPI)

// Attributes
public:

// Operations
public:
	void NewHostCreated(CHost *pHost);
	void HostChanged(CHost *pHost);
	void NewSubnetCreated(CSubnet *pSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	void GuiCreated(IGuiAPI *pGui);
	void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);

	LONG OnIdleProcess();

// Overrides
	//{{AFX_VIRTUAL(CNetworkDocAPI)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetworkDocAPI();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// protected methods
protected:
	void SetFrame(CNetworkFrameAPI *pFrame) { m_pFrame = pFrame; }

protected:
	//{{AFX_MSG(CNetworkDocAPI)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// protected members
protected:
	CNetworkFrameAPI *m_pFrame;
};

#endif //__NETWORK_DOC_API_H_INCLUDED__