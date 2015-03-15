// NetworkFrameAPI.h : interface of the CNetworkFrameAPI class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_FRAME_API_H_INCLUDED__
#define __NETWORK_FRAME_API_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declaraions
class CNetworkDocAPI;

class CNetworkSplitter : public CSplitterWnd
{
	DECLARE_DYNAMIC(CNetworkSplitter)

// Construction
public:
	CNetworkSplitter();

// Attributes
public:
// Operations
public:

// Overridables
protected:

public:

// Implementation
public:
	virtual ~CNetworkSplitter();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	//{{AFX_MSG(CNetworkSplitter)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CNetworkFrameAPI : public CFrameWnd
{
	
protected: // create from serialization only
	CNetworkFrameAPI();
	DECLARE_DYNCREATE(CNetworkFrameAPI)

// Attributes
public:

// Operations
public:
	virtual void NewHostCreated(CHost *pHost) {}
	virtual void HostChanged(CHost *pHost) {}
	virtual void NewSubnetCreated(CSubnet *pSubnet) {}
	virtual void SubnetChanged(CSubnet *pSubnet) {}
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet) {}
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet) {}
	virtual void GuiCreated(IGuiAPI *pGui) {}
	virtual void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged) {}
	
	virtual BOOL OnIdleProcess() { return FALSE; }

// Overrides
	//{{AFX_VIRTUAL(CNetworkFrameAPI)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetworkFrameAPI();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	CNetworkDocAPI *m_pDoc;

protected:
	//{{AFX_MSG(CNetworkFrameAPI)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __NETWORK_FRAME_API_H_INCLUDED__
