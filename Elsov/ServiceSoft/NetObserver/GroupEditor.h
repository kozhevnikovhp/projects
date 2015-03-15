/* GroupEditor.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	10 Dec 2007 - initial creation
	25 Dec 2007 - form switching from trre view
	20 Feb 2008 - group creation/deletion
	14 Mar 2008 - storing group params in DB, tree-view update while group name is changed
	20 Mar 2008 - ping group creation, event handling
	21 Mar 2008 - default groups are dynamically allocated as ordinary ones
	21 Mar 2008 - default groups are dynamically allocated as ordinary ones; TreeView: visit all items functionality
	06 Jun 2008 - appearance groups introduced (not completed)
	08 Jul 2008 - color schemes for appearance groups
	26 Aug 2008 - SNMP groups
*/


#ifndef __GROUP_EDITOR_H_INCLUDED__
#define __GROUP_EDITOR_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetworkDocAPI.h"
#include "NetworkFrameAPI.h"
#include "Groups.h"
#include "ColorButton.h"

// Forward declarations
class CGroupEditorForm;
class CGroupEditorTreeView;

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorDoc document

class CGroupEditorDoc : public CNetworkDocAPI
{
protected:
	CGroupEditorDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGroupEditorDoc)

// Public methods
public:
	void CreateForms(CSplitterWnd &Splitter);
	CGroupEditorForm *GetControlForm(int nViewIndex) { return m_pForms[nViewIndex]; }

// Public overridabled
public:
	virtual void UpdateWindowCaption() { SetTitle("Group Editor"); }

	//{{AFX_VIRTUAL(CGroupEditorDoc)
	public:
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGroupEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	void AddForm(int IndexInViewArray, CGroupEditorForm *pForm, CWnd *pParent);


// Protected members
protected:
	CGroupEditorForm *m_pForms[STOP_GROUP_INDEX];

protected:
	//{{AFX_MSG(CGroupEditorDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorFrame frame

class CGroupEditorFrame : public CNetworkFrameAPI
{
	friend class CGroupEditorDoc;
	DECLARE_DYNCREATE(CGroupEditorFrame)
protected:
	CGroupEditorFrame();           // protected constructor used by dynamic creation

// Public methods
public:
	CGroupEditorForm *GetCurrentForm();

// Public overridables
public:
	virtual void Refresh() { m_Splitter.RecalcLayout(); }
	virtual void NewHostCreated(CHost *pHost);
	virtual void NewGroupCreated(CGroup *pGroup);
	virtual void GroupChanged(CGroup *pGroup);
	virtual void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup);

	//{{AFX_VIRTUAL(CGroupEditorFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGroupEditorFrame();
// Protected methods
protected:

protected:
	CNetworkSplitter m_Splitter;
	CGroupEditorTreeView *m_pTreeView;
	
	//{{AFX_MSG(CGroupEditorFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditDeleteGroup();
	afx_msg void OnUpdateEditDeleteGroup(CCmdUI *pCmdUI);
	afx_msg void OnEditCreatePingGroup();
	afx_msg void OnEditCreateSNMPGroup();
	afx_msg void OnEditCreateAppearanceGroup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////
// CGroupEditorTreeItemVisitor

class CGroupEditorTreeItemVisitor : public CNetworkTreeItemVisitor
{
//Constuctors/destructors
public:
	CGroupEditorTreeItemVisitor(CGroupEditorTreeView *pTreeView, CGroup *pGroup);
	virtual ~CGroupEditorTreeItemVisitor();

// Public overridables
public:

// Protected overridables
protected:

	// Protected members
protected:
	CGroupEditorTreeView *m_pTreeView;
	CTreeCtrl *m_pTreeCtrl;
	CGroup *m_pGroup;
};

/////////////////////////////////////////////////////////////////////
// CGroupCreatedTreeItemVisitor

class CGroupCreatedTreeItemVisitor : public CGroupEditorTreeItemVisitor
{
//Constuctors/destructors
public:
	CGroupCreatedTreeItemVisitor(CGroupEditorTreeView *pTreeView, CGroup *pGroup);
	virtual ~CGroupCreatedTreeItemVisitor();

// Public overridables
public:
	virtual LOGICAL Visit(HTREEITEM hItem);

// Protected overridables
protected:

	// Protected members
protected:
};

/////////////////////////////////////////////////////////////////////
// CGroupChangedTreeItemVisitor

class CGroupChangedTreeItemVisitor : public CGroupEditorTreeItemVisitor
{
//Constuctors/destructors
public:
	CGroupChangedTreeItemVisitor(CGroupEditorTreeView *pTreeView, CGroup *pGroup);
	virtual ~CGroupChangedTreeItemVisitor();

// Public overridables
public:
	virtual LOGICAL Visit(HTREEITEM hItem);

// Protected overridables
protected:

	// Protected members
protected:
};

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorTreeView view

class CGroupEditorTreeView : public CNetworkTreeView
{
protected:
	CGroupEditorTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGroupEditorTreeView)

// Public methods
public:
	void NewHostCreated(CHost *pHost);
	void NewGroupCreated(CGroup *pGroup);
	void GroupChanged(CGroup *pGroup);
	void AddGroup(HTREEITEM hItem, CGroup *pGroup);

// Public overridables
public:

	//{{AFX_VIRTUAL(CGroupEditorTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGroupEditorTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	void AddGroups(HTREEITEM hItem, CGroup *pDefaultGroup);

// Protected overridables
protected:
	virtual CNetworkForm *GetDefaultForm() { return GetForm(DEFAULT_GROUP_INDEX); }
	virtual CNetworkForm *GetForm(int nViewIndex);

// Protected members
protected:

protected:
	//{{AFX_MSG(CGroupEditorTreeView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CGroupHostsListBox : public CListBox
{
//Constuctors/destructors
public:

// Public methods
public:
	void Init(CGroup *pGroup);
	int AddHost(CHost *pHost);
	void RemoveHost(CHost *pHost);

// Public overridables
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
};

class CGroupEditorForm : public CNetworkForm
{
protected:
	CGroupEditorForm();           // protected constructor used by dynamic creation
	CGroupEditorForm(UINT nIDTemplate);           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGroupEditorForm)

// Form Data
public:
	//{{AFX_DATA(CGroupEditorForm)
	//}}AFX_DATA

// Public methods
public:
	void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup);

// Public overridables
public:
	virtual CGroup *GetGroup() { return NULL; }
	virtual void SetGroupName(char *pszName) { ASSERT(FALSE); }
	virtual const char *GetGroupName() { ASSERT(FALSE); return NULL; }

// Protected overridables
protected:

	//{{AFX_VIRTUAL(CGroupEditorForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGroupEditorForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:

	//{{AFX_MSG(CGroupEditorForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CPingGroupEditorForm

class CPingGroupEditorForm : public CGroupEditorForm
{
protected:
	DECLARE_DYNCREATE(CPingGroupEditorForm)

// Form Data
public:
	//{{AFX_DATA(CPingGroupEditorForm)
	CString	m_GroupName;
	BOOL	m_bKeepAliveEnabled;
	UINT	m_uKeepAlivePeriod;
	//}}AFX_DATA

// Constructors/destructors
public:
	CPingGroupEditorForm();
	virtual ~CPingGroupEditorForm();

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual CGroup *GetGroup() { return m_pGroup; }
	virtual void SetGroupName(char *pszName) { m_GroupName = pszName; UpdateData(FALSE); }
	virtual const char *GetGroupName() { return m_GroupName; }

// Protected overridables
protected:

	//{{AFX_VIRTUAL(CPingGroupEditorForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CPingGroup *m_pGroup;

	//{{AFX_MSG(CPingGroupEditorForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CAppearanceGroupEditorForm

class CAppearanceGroupEditorForm : public CGroupEditorForm
{
protected:
	DECLARE_DYNCREATE(CAppearanceGroupEditorForm)

// Form Data
public:
	//{{AFX_DATA(CAppearanceGroupEditorForm)
	enum { IDD = IDD_APPEARANCE_GROUP_EDITOR_FORM };
	CColorButton	m_TextColorButton;
	CColorButton	m_AlarmColorButton;
	CColorButton	m_WarningColorButton;
	CColorButton	m_OrdinaryColorButton;
	CString	m_GroupName;
	//}}AFX_DATA

// Constructors/destructors
public:
	CAppearanceGroupEditorForm();
	virtual ~CAppearanceGroupEditorForm();

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual CGroup *GetGroup() { return m_pGroup; }
	virtual void SetGroupName(char *pszName) { m_GroupName = pszName; UpdateData(FALSE); }
	virtual const char *GetGroupName() { return m_GroupName; }

// Protected overridables
protected:

	//{{AFX_VIRTUAL(CAppearanceGroupEditorForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Protected methods
protected:

// Protected members
protected:
	CAppearanceGroup *m_pGroup;
	SRGBA m_OrdinaryColor;
	SRGBA m_WarningColor;
	SRGBA m_AlarmColor;
	SRGBA m_TextColor;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CAppearanceGroupEditorForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	afx_msg void OnOrdinaryColorButton();
	afx_msg void OnWarningColorButton();
	afx_msg void OnAlarmColorButton();
	afx_msg void OnTextColorButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSnmpGroupEditorForm

class CSnmpGroupEditorForm : public CGroupEditorForm
{
protected:
	DECLARE_DYNCREATE(CSnmpGroupEditorForm)

// Form Data
public:
	//{{AFX_DATA(CSnmpGroupEditorForm)
	enum { IDD = IDD_SNMP_GROUP_EDITOR_FORM };
	CSpinButtonCtrl	m_RetriesSpin;
	CString	m_GroupName;
	CString m_CommunityName;
	CString m_Password;
	BOOL	m_bEnabled;
	int		m_nRetries;
	//}}AFX_DATA

// Constructors/destructors
public:
	CSnmpGroupEditorForm();
	virtual ~CSnmpGroupEditorForm();

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual CGroup *GetGroup() { return m_pGroup; }
	virtual void SetGroupName(char *pszName) { m_GroupName = pszName; UpdateData(FALSE); }
	virtual const char *GetGroupName() { return m_GroupName; }

// Protected overridables
protected:

	//{{AFX_VIRTUAL(CSnmpGroupEditorForm)
	protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CSnmpGroup *m_pGroup;

	//{{AFX_MSG(CSnmpGroupEditorForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CGroupEditorDocTemplate : public CNetworkDocTemplate
{
// Constructors/destructors
public:
	CGroupEditorDocTemplate(UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass * pViewClass);

// public methods
public:
	CNetworkDocAPI *CreateNewWindow(CGroup *pGroup);

// public overridables
public:
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#endif //__GROUP_EDITOR_H_INCLUDED__
