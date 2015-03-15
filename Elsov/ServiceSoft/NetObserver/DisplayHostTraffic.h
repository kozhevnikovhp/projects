/* DisplayHostTraffic.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	15 Jun 2006 - initial creation
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables

*/


#ifndef __DISPLAY_HOST_TRAFFIC_H_INCLUDED__
#define __DISPLAY_HOST_TRAFFIC_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetworkDocAPI.h"
#include "NetworkFrameAPI.h"
#include "OpenGLView.h"
#include "HostTrafficRenderer.h"

// Forward declarations
class CDisplayHostTrafficView;
class CDisplayHostTrafficCtrls;
class CHostTrafficRenderer;
class CHost;
class IDatabaseAPI;

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficDoc document

class CDisplayHostTrafficDoc : public CNetworkDocAPI
{
protected:
	CDisplayHostTrafficDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDisplayHostTrafficDoc)

// Public methods
public:

// Public overridabled
public:
	virtual void Initialize(CHost *pHost);
	virtual char *GetDocTypeDescription() { return "traffic"; }

	//{{AFX_VIRTUAL(CDisplayHostTrafficDoc)
	public:
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDisplayHostTrafficDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDisplayHostTrafficDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficFrame frame

class CDisplayHostTrafficFrame : public CNetworkFrameAPI
{
	friend class CDisplayHostTrafficDoc;
	DECLARE_DYNCREATE(CDisplayHostTrafficFrame)
protected:
	CDisplayHostTrafficFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDisplayHostTrafficFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDisplayHostTrafficFrame();
// Protected methods
protected:
	void Init(CHost *pHost);

protected:
	CStatusBar  m_wndStatusBar;
	CToolBar m_NavigationToolbar;
	CNetworkSplitter m_SplitterWnd;
	CDisplayHostTrafficCtrls *m_pControlView;
	CDisplayHostTrafficView *m_pChartView;
	CHostTrafficRenderer *m_pChart;
	
	//{{AFX_MSG(CDisplayHostTrafficFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomRectangle();
	afx_msg void OnUpdateViewZoomRectangle(CCmdUI *pCmdUI);
	afx_msg void OnViewInitialView();
	afx_msg void OnViewDragScroll();
	afx_msg void OnUpdateViewDragScroll(CCmdUI *pCmdUI);
	afx_msg void OnViewZoomInTraffic();
	afx_msg void OnViewZoomOutTraffic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateDateTime(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTraffic(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficCtrls form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CDisplayHostTrafficCtrls : public CFormView
{
protected:
	CDisplayHostTrafficCtrls();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDisplayHostTrafficCtrls)

// Form Data
public:
	//{{AFX_DATA(CDisplayHostTrafficCtrls)
	enum { IDD = IDD_DISPLAY_HOST_TRAFFIC_CTRLS };
	BOOL	m_bTcpOut;
	BOOL	m_bTcpIn;
	BOOL	m_bUdpOut;
	BOOL	m_bUdpIn;
	BOOL	m_bIcmpOut;
	BOOL	m_bIcmpIn;
	BOOL	m_bIgmpOut;
	BOOL	m_bTotalOut;
	BOOL	m_bTotalTo;
	CString	m_FromTime;
	CString	m_ToTime;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void SetChart(CHostTrafficRenderer *pChart) { m_pChart = pChart; }

// Overrides
	//{{AFX_VIRTUAL(CDisplayHostTrafficCtrls)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDisplayHostTrafficCtrls();
	void UpdateFromToTimes();
	void RefreshChart();

// Protected members
	CHostTrafficRenderer *m_pChart;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CDisplayHostTrafficCtrls)
	afx_msg void OnIcmpFromCheck();
	afx_msg void OnIcmpToCheck();
	afx_msg void OnIgmpFromCheck();
	afx_msg void OnTcpFromCheck();
	afx_msg void OnTcpToCheck();
	afx_msg void OnTotalFromCheck();
	afx_msg void OnTotalToCheck();
	afx_msg void OnUdpFromCheck();
	afx_msg void OnUdpToCheck();
	afx_msg void OnOctetsRadio();
	afx_msg void OnPacketsRadio();
	afx_msg void OnRefreshButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CDisplayHostTrafficView view

class CDisplayHostTrafficView : public COpenGLView
{
protected:
	CDisplayHostTrafficView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDisplayHostTrafficView)

// Attributes
public:

// Operations
public:
	void ZoomTimeIn();
	void ZoomTimeOut();
	void ZoomTrafficIn();
	void ZoomTrafficOut();
	void InitialView();
	void SetDragScrollMode();
	void OnUpdateDragScrollMode(CCmdUI *pCmdUI);
	void SetZoomRectangleMode();
	void OnUpdateZoomRectangleMode(CCmdUI *pCmdUI);
	CHostTrafficRenderer *GetChart() { return m_pHostTrafficRenderer; }
	LOGICAL GetDateTimeUnderMouse(time_t &t);
	LOGICAL GetTrafficUnderMouse(double &traffic);

// Overrides
	virtual CRenderer *GetRenderer() { return m_pHostTrafficRenderer; }
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

// Protected methods
protected:
	LOGICAL GetMousePosition(int &x, int &y);


	//{{AFX_VIRTUAL(CDisplayHostTrafficView)
	protected:
	virtual void OnDraw(CDC *pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDisplayHostTrafficView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
	CHostTrafficRenderer *m_pHostTrafficRenderer;

protected:
	//{{AFX_MSG(CDisplayHostTrafficView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __DISPLAY_HOST_TRAFFIC_H_INCLUDED__
