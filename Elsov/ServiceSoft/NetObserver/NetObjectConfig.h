/* NetObjectConfig.h 
	
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Oct 2007 - initial creation
	14 Nov 2007 - templates for host and subnet configure windows
	15 Nov 2007 - switching between control forms
	16 Nov 2007 - control form initialization
	10 Jan 2008 - host group created and changed functionality
	13 Mar 2008 - HostChanged event processing
	08 Jun 2008 - host appearance control form
	26 Aug 2008 - host SNMP control form
	28 Aug 2008 - Windows messages instead of method calls for more safe multythreading
*/

#ifndef __NET_OBJECT_CONFIG_H_INCLUDED__
#define __NET_OBJECT_CONFIG_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetworkDocAPI.h"
#include "NetworkFrameAPI.h"

// Forward declarations
class CNetObjectConfigFrame;
class CControlForm;
class CHostControlForm;
class CSubnetControlForm;

enum {
	DEFAULT_CONTROL_INDEX,
	HOST_NAMES_CONTROL_INDEX,
	SUBNET_NAMES_CONTROL_INDEX,
	HOST_ADDRESSES_CONTROL_INDEX,
	SUBNET_ADDRESS_CONTROL_INDEX,
	HOST_APPEARANCE_CONTROL_INDEX,
	SUBNET_APPEARANCE_CONTROL_INDEX,
	HOST_PING_CONTROL_INDEX,
	HOST_SNMP_CONTROL_INDEX,
	STOP_CONTROL_INDEX
};


/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigDoc document

class CNetObjectConfigDoc : public CNetworkDocAPI
{
protected:
	CNetObjectConfigDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNetObjectConfigDoc)

// Attributes
public:

// Public methods
public:

// Public overridables
public:
	virtual void CreateControlForms(CSplitterWnd &Splitter) {}
	virtual CControlForm *GetControlForm(int nViewIndex) { return NULL; }
	virtual char *GetDocTypeDescription() { return "config/view"; }

	//{{AFX_VIRTUAL(CHostConfigDoc)
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetObjectConfigDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:

protected:
	//{{AFX_MSG(CNetObjectConfigDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CHostConfigDoc document

class CHostConfigDoc : public CNetObjectConfigDoc
{
protected:
	CHostConfigDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHostConfigDoc)

// Attributes
public:

// Public methods
public:

// Public overridables
public:
	virtual void Initialize(CHost *pHost);
	virtual void CreateControlForms(CSplitterWnd &Splitter);
	virtual CControlForm *GetControlForm(int nViewIndex);

// Overrides
	//{{AFX_VIRTUAL(CHostConfigDoc)
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHostConfigDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	void AddControlForm(int IndexInViewArray, CHostControlForm *pForm, CWnd *pParent);

// Protected members
protected:
	CHostControlForm *m_pControlForms[STOP_CONTROL_INDEX];

protected:
	//{{AFX_MSG(CHostConfigDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigDoc document

class CSubnetConfigDoc : public CNetObjectConfigDoc
{
protected:
	CSubnetConfigDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSubnetConfigDoc)

// Attributes
public:

// Public methods
public:

// Public overridables
public:
	void Initialize(CSubnet *pSubnet);
	virtual void CreateControlForms(CSplitterWnd &Splitter);
	virtual CControlForm *GetControlForm(int nViewIndex);

// Overrides
	//{{AFX_VIRTUAL(CSubnetConfigDoc)
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSubnetConfigDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	void AddControlForm(int IndexInViewArray, CSubnetControlForm *pForm, CWnd *pParent);

// Protected members
protected:
	CSubnetControlForm *m_pControlForms[STOP_CONTROL_INDEX];

protected:
	//{{AFX_MSG(CSubnetConfigDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigTreeView view

class CNetObjectConfigTreeView : public CNetworkTreeView
{
protected:
	CNetObjectConfigTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNetObjectConfigTreeView)

// Public methods
public:
    void ObjectChanged(CNetObject *pObject);

// Public overridables
public:
	virtual void Init(CNetObject *pObject) { ASSERT(FALSE); }

	//{{AFX_VIRTUAL(CNetObjectConfigTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CNetObjectConfigTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

// Protected overridables
protected:
	virtual CNetworkForm *GetDefaultForm() { return GetForm(DEFAULT_CONTROL_INDEX); }
	virtual CNetworkForm *GetForm(int nViewIndex);

	// Protected members
protected:

protected:
	//{{AFX_MSG(CNetObjectConfigTreeView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHostConfigTreeView view

class CHostConfigTreeView : public CNetObjectConfigTreeView
{
protected:
	CHostConfigTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHostConfigTreeView)

// Public methods
public:

// Public overridables
public:
	virtual void Init(CNetObject *pObject) { m_pHost = (CHost *)pObject; }

	//{{AFX_VIRTUAL(CHostConfigTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHostConfigTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CHost *m_pHost;

protected:
	//{{AFX_MSG(CHostConfigTreeView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigTreeView view

class CSubnetConfigTreeView : public CNetObjectConfigTreeView
{
protected:
	CSubnetConfigTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSubnetConfigTreeView)

// Public methods
public:

// Public overridables
public:
	virtual void Init(CNetObject *pObject) { m_pSubnet = (CSubnet *)pObject; }

	//{{AFX_VIRTUAL(CSubnetConfigTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSubnetConfigTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CSubnet *m_pSubnet;

protected:
	//{{AFX_MSG(CSubnetConfigTreeView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigFrame frame

class CNetObjectConfigFrame : public CNetworkFrameAPI
{
	friend class CHostConfigDoc;
	DECLARE_DYNCREATE(CNetObjectConfigFrame)
protected:
	CNetObjectConfigFrame();           // protected constructor used by dynamic creation

// Public methods
public:

// Public overridables
public:
	virtual BOOL OnIdleMFC();
	virtual void Refresh() { m_SplitterHor.RecalcLayout(); m_SplitterVert.RecalcLayout(); }

	//{{AFX_VIRTUAL(CNetObjectConfigFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CNetObjectConfigFrame();

// Protected methods
protected:
	CNetworkForm *GetCurrentForm();

// Protected overridables
protected:
	virtual CRuntimeClass *GetControlFormRunTime() { ASSERT(FALSE); return NULL; } //must be overridden

// Protected members
protected:
	CNetworkSplitter m_SplitterVert, m_SplitterHor;
	CNetObjectConfigTreeView *m_pTreeView;

	//{{AFX_MSG(CNetObjectConfigFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CHostConfigFrame frame

class CHostConfigFrame : public CNetObjectConfigFrame
{
	friend class CHostConfigDoc;
	DECLARE_DYNCREATE(CHostConfigFrame)
protected:
	CHostConfigFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Public overridables
public:
	virtual void HostChanged(CHost *pHost);
	virtual void NewGroupCreated(CGroup *pGroup);
	virtual void GroupChanged(CGroup *pGroup);

	//{{AFX_VIRTUAL(CHostConfigFrame)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHostConfigFrame();

// Protected methods
protected:
	void Init(CHost *pHost);
	CHostControlForm *GetCurrentForm();

// Protected overridables
protected:
	virtual CRuntimeClass *GetControlFormRunTime();

// Protected members
protected:

	//{{AFX_MSG(CHostConfigFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigFrame frame

class CSubnetConfigFrame : public CNetObjectConfigFrame
{
	friend class CSubnetConfigDoc;
	DECLARE_DYNCREATE(CSubnetConfigFrame)
protected:
	CSubnetConfigFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CSubnetConfigFrame)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSubnetConfigFrame();

// Protected methods
protected:
	void Init(CSubnet *pSubnet);

// Protected overridables
protected:
	virtual CRuntimeClass *GetControlFormRunTime();

// Protected members
protected:

	//{{AFX_MSG(CSubnetConfigFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __NET_OBJECT_CONFIG_H_INCLUDED__
