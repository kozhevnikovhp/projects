#if !defined(AFX_GOODFORMVIEW_H__CE2A0E61_0B69_11D2_ABE8_008048EEE0E8__INCLUDED_)
#define AFX_GOODFORMVIEW_H__CE2A0E61_0B69_11D2_ABE8_008048EEE0E8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Forms.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm form view
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CGoodFormView : public CFormView
{
protected:
	CGoodFormView(UINT ID=0);
	DECLARE_DYNCREATE(CGoodFormView)
	virtual ~CGoodFormView();

// Form Data
public:
	//{{AFX_DATA(CGoodFormView)
	//}}AFX_DATA

// Attributes
public:

	// Operations
public:
// Overrides
	virtual void UpdateTexts() {};
	//{{AFX_VIRTUAL(CGoodFormView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CGoodFormView)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	BOOL NeedToolTipsHandler(UINT id, NMHDR *pTTTStruct, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOODFORMVIEW_H__CE2A0E61_0B69_11D2_ABE8_008048EEE0E8__INCLUDED_)
