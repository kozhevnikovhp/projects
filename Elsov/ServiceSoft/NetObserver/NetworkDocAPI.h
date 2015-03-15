/* NetworkDocAPI.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	07 Sep 2006 - initial creation
	09 Dec 2007 - doc templates: bringing existing docs of the same netobject to top, etc.
	10 Dec 2007 - CNetworkTreeView class (base class for all tree views)
	15 Jan 2008 - splitting MFC- and NonMFC-visitors to avoid multithread problems with MFC-controls 
	13 Mar 2008 - HostChanged event handling; asking user to save/discard/continue editing of modified data at tree-ctrl selection changing
	20 Mar 2008 - moving host between groups event handling
	21 Mar 2008 - NetworkTreeView: visit all items functionality
	28 Aug 2008 - Windows messages instead of method calls for more safe multythreading
	01 Sep 2008 - Window captions reflects host/subnet name and document type
*/

#ifndef __NETWORK_DOC_API_H_INCLUDED__
#define __NETWORK_DOC_API_H_INCLUDED__

// Forward declarations
class CNetworkDocAPI;
class CNetworkFrameAPI;
class CNetObject;
class CHost;
class CSubnet;
class CGroup;
class IGuiAPI;
class CAttachedBlock;

// Visitors

class CDocVisitorAPI
{
// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc) = 0;

// Protected overridables
protected:
};

class COnIdleMFCVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	COnIdleMFCVisitor();
	virtual ~COnIdleMFCVisitor();

// Public methods
public:
	
// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
};

class COnIdleNonMFCVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	COnIdleNonMFCVisitor();
	virtual ~COnIdleNonMFCVisitor();

// Public methods
public:
	
// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
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

class CNewGroupCreatedVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CNewGroupCreatedVisitor(CGroup *pGroup);
	virtual ~CNewGroupCreatedVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CGroup *m_pGroup;
};


class CGroupChangedVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CGroupChangedVisitor(CGroup *pGroup);
	virtual ~CGroupChangedVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CGroup *m_pGroup;
};


class CHostMovedBetweenGroupsVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CHostMovedBetweenGroupsVisitor(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup);
	virtual ~CHostMovedBetweenGroupsVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CHost *m_pHost;
	CGroup *m_pOldGroup, *m_pNewGroup;
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

class CNetObjectIgnoredVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CNetObjectIgnoredVisitor(CNetObject *pObject);
	virtual ~CNetObjectIgnoredVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CNetObject *m_pObject;
};

class CNetObjectHiddenVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CNetObjectHiddenVisitor(CNetObject *pObject);
	virtual ~CNetObjectHiddenVisitor();

// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
	CNetObject *m_pObject;
};

class CShowHiddenObjectsVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CShowHiddenObjectsVisitor();
	virtual ~CShowHiddenObjectsVisitor();

// Public methods
public:
	
// Public overridables
public:
	virtual void Visit(CNetworkDocAPI *pDoc);

// Protected members
protected:
};

class CShowIgnoredObjectsVisitor : public CDocVisitorAPI
{
//Constuctors/destructors
public:
	CShowIgnoredObjectsVisitor();
	virtual ~CShowIgnoredObjectsVisitor();

// Public methods
public:
	
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

// Public methods
public:
	void BringToTop();
	void NewHostCreated(CHost *pHost);
	void HostChanged(CHost *pHost);
	void NewSubnetCreated(CSubnet *pSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);
	void NewGroupCreated(CGroup *pGroup);
	void GroupChanged(CGroup *pGroup);
	void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup);
	void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);
	void NetObjectIgnored(CNetObject *pObject);
	void NetObjectHidden(CNetObject *pObject);
	void ShowHiddenObjects();
	void ShowIgnoredObjects();

	LONG OnIdleMFC();
	LONG OnIdleNonMFCThread();

// Public overridabled
public:
	virtual LOGICAL IsMine(CNetObject *pObject) { return (m_pNetObject == pObject); }
	virtual void Initialize(CHost *pHost) {}
	virtual void Initialize(CSubnet *pSubnet) {}
	virtual void UpdateWindowCaption();
	virtual char *GetDocTypeDescription() { return "<unknown doc type>"; }

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
	void CreateForm(int IndexInViewArray, CView *pViewToAdd, CWnd *pParent);

protected:
	//{{AFX_MSG(CNetworkDocAPI)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// protected members
protected:
	CNetworkFrameAPI *m_pFrame;
	CNetObject *m_pNetObject;
};

class CNetworkDocTemplate : public CMultiDocTemplate
{
// Constructors/destructors
public:
	CNetworkDocTemplate(UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass * pViewClass);

// public methods
public:

// public overridables
public:
	virtual void InitialUpdateFrame(CFrameWnd *pFrame, CDocument *pDoc, BOOL bMakeVisible = TRUE);

// protected methods
	CNetworkDocAPI *FindDocument(CNetObject *pObject);
};

class CHostDocTemplate : public CNetworkDocTemplate
{
// Constructors/destructors
public:
	CHostDocTemplate(UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass * pViewClass);

// public methods
public:
	CNetworkDocAPI *CreateNewWindow(CHost *pHost);

// public overridables
public:
};

class CSubnetDocTemplate : public CNetworkDocTemplate
{
// Constructors/destructors
public:
	CSubnetDocTemplate(UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass * pViewClass);

// public methods
public:
	CNetworkDocAPI *CreateNewWindow(CSubnet *pSubnet);

// public overridables
public:
};

class CNetworkForm : public CFormView
{
protected:
	CNetworkForm();           // protected constructor used by dynamic creation
	CNetworkForm(UINT nIDTemplate);           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNetworkForm)

// Form Data
public:
	//{{AFX_DATA(CNetworkForm)
	//}}AFX_DATA

// Public methods
public:
	void SetModifiedFlag() { m_bModified = LOGICAL_TRUE; }
	void ClearModifiedFlag() { m_bModified = LOGICAL_FALSE; }
	LOGICAL IsModified() { return m_bModified; }
	void SetAttachedBlock(CAttachedBlock *pAttached) { m_pAttached = pAttached; }
	void UpdateStatus();

// Public overridables
public:
	virtual LOGICAL Entering() { return LOGICAL_TRUE; }
	virtual LOGICAL UpdateReadOnlyFields() { return LOGICAL_TRUE; }
	virtual LOGICAL Leaving() { return LOGICAL_TRUE; }
	virtual LOGICAL CanLeave();
	virtual unsigned long GetUpdateStatusPeriod() { return 1000; } // 1 sec
	virtual void OnUpdateStatus() {}

	//{{AFX_VIRTUAL(CNetworkForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CNetworkForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:
	LOGICAL m_bModified;
	CAttachedBlock *m_pAttached;
	unsigned long m_LastUpdateTime;

	//{{AFX_MSG(CNetworkForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CAttachedBlock
{
	// Constructors-destructors
public:
	CAttachedBlock(long ViewIndex, void *pObject);
	~CAttachedBlock();

	// Public members
public:
	long m_ViewIndex;
	void *m_pObject;
};

/////////////////////////////////////////////////////////////////////
// CNetworkTreeViewItemVisitor

class CNetworkTreeItemVisitor
{
// Public overridables
public:
	virtual LOGICAL Visit(HTREEITEM hItem) = 0;

// Protected overridables
protected:
};

/////////////////////////////////////////////////////////////////////
// CNetTreeView

class CNetworkTreeView : public CTreeView
{
protected:
	CNetworkTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNetworkTreeView)

// Public methods
public:
	void SetEnv(CNetworkFrameAPI *pFrame, CNetworkForm *pForm) { m_pFrame = pFrame; m_pCurrentlyVisibleForm = pForm; }

// Public overridables
public:

	//{{AFX_VIRTUAL(CNetworkTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Protected methods
protected:
	HTREEITEM NewItem(char *pszText, int nImage, long FormIndex, void *pObject, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	void VisitAllItems(CNetworkTreeItemVisitor *pVisitor);
	LOGICAL VisitChildItems(CNetworkTreeItemVisitor *pVisitor, HTREEITEM hItem);
	LOGICAL VisitItem(CNetworkTreeItemVisitor *pVisitor, HTREEITEM hItem);

// Protected methods
protected:
	void SwitchToDefaultForm(CNetworkForm *pOldForm);
	void RefreshCurrentForm();

// Protected overridables
protected:
	virtual CNetworkForm *GetDefaultForm() { return NULL; }
	virtual CNetworkForm *GetForm(int nViewIndex) { return NULL; }

// Protected members
protected:
	CImageList m_ImageList;
	HTREEITEM m_hPrevSelection;
	CNetworkForm *m_pCurrentlyVisibleForm;
	CNetworkFrameAPI *m_pFrame;
	
// Implementation
protected:
	virtual ~CNetworkTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	//{{AFX_MSG(CNetworkTreeView)
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif //__NETWORK_DOC_API_H_INCLUDED__/////////////////////////////////////////////////////////////////////////////
