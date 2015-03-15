// HardwareTestView.h : interface of the CHardwareTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HARDWARETESTVIEW_H__1C2CEF00_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
#define AFX_HARDWARETESTVIEW_H__1C2CEF00_300A_11D4_BBDF_E46DF892F25A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHardwareTestDoc;
class CDevice;
class CModem;
class CAbstractForm;

class CHardwareTestView : public CTreeView
{
protected: // create from serialization only
	CHardwareTestView();
	DECLARE_DYNCREATE(CHardwareTestView)

// Attributes
public:
	CHardwareTestDoc* GetDocument();

// Operations
public:
	void Clear();
	void Refresh();
	void AddDemodulator(CDevice *pDevice);
	void AddModulator(CDevice *pDevice);
	void AddModem(CDevice *pDevice);
	void AddStation(CDevice *pDevice);
	void AddSkyLaneSL512(CDevice *pDevice);
	void AddNsgFrad(CDevice *pDevice);
	void AddTranceiver(CDevice *pDevice);
	void AddUpDownConverter(CDevice *pDevice);
	void AddDeviceSubstitutor(CDevice *pDevice);
	void AddPowerMeter(CDevice *pDevice);
	void AddFrequencyCounter(CDevice *pDevice);
	CDevice *DeleteSelectedDevice();
	void RefreshCurrentForm();

// Overrides
	//{{AFX_VIRTUAL(CHardwareTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHardwareTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HTREEITEM InsertTerminalItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertConnectionItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertSoftwareVersionItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertStartupConfigItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertEthernetPortItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertEthernetStatusItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertSerialPortItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertSerialStatusItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertDemodulatorCarrierItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice);
	HTREEITEM InsertModulatorCarrierItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice);
	HTREEITEM InsertDemodulatorDataItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice);
	HTREEITEM InsertModulatorDataItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice);
	HTREEITEM InsertBucStatusItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice);
	HTREEITEM InsertLnbStatusItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice);
	HTREEITEM InsertModulatorBerTestItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice);
	HTREEITEM InsertDemodulatorBerTestItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice);
	HTREEITEM InsertTranceiverItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertNsgScriptItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertPhysicalPortsItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertFrameRelayStationsItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertEthernetStationsItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertTelnetStationsItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertIPInterfacesItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertIPStaticRoutesItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertIPRoutingTableItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertX25RoutingItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertPVCsItem(HTREEITEM ParentItem, CDevice *pDevice);
	HTREEITEM InsertSubstitutorItem(HTREEITEM ParentItem, CDevice *pDevice);
	void InsertModem(HTREEITEM ParentItem, CDevice *pDevice);
	void TurnToAbstractPane(CAbstractForm *pOld);
	
	// Protected members
	HTREEITEM m_PrevSelection;
	CImageList m_ImageList;
	CTreeCtrl *m_pTreeCtrl;
	CAbstractForm *m_pAbstractPane;
	CAbstractForm *m_pCurrentlyVisibleForm;
	UINT m_TimerID;
//	CFont font;

// Generated message map functions
protected:
	//{{AFX_MSG(CHardwareTestView)
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in HardwareTestView.cpp
inline CHardwareTestDoc* CHardwareTestView::GetDocument()
   { return (CHardwareTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HARDWARETESTVIEW_H__1C2CEF00_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
