// ControlSystemDoc.h : interface of the CControlSystemDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLSYSTEMDOC_H__902F2692_5F82_4A34_BA9B_8C7C6487573E__INCLUDED_)
#define AFX_CONTROLSYSTEMDOC_H__902F2692_5F82_4A34_BA9B_8C7C6487573E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declarations
class CAbstractForm;
class CControlSystemView;

enum FI_FormIndex
{
	ABSTRACT_INDEX,
	TARGET_FORM_INDEX,
	OBJECT_FORM_INDEX,
	EVENT_LOG_FORM_INDEX,
	MONITOR_AND_CONTROL_FORM_INDEX,
	STOP_FORM_INDEX
};

class CControlSystemDoc : public CDocument
{
protected: // create from serialization only
	CControlSystemDoc();
	DECLARE_DYNCREATE(CControlSystemDoc)

// Attributes
public:

// Operations
public:
	void AddNewForm(FI_FormIndex IndexInViewArray, CView *pViewToAdd, CWnd *pParent);
	BOOL IsAbstractFormUpdateTimerEnabled() { return m_bAbstractFormUpdateTimerEnabled;	}

// Overrides
	//{{AFX_VIRTUAL(CControlSystemDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlSystemDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
	CPtrList m_TargetInstanceList;
	CAbstractForm *m_pForms[STOP_FORM_INDEX+1];
	CControlSystemView *m_pTreeView;
	BOOL m_bAbstractFormUpdateTimerEnabled;
protected:

protected:
	//{{AFX_MSG(CControlSystemDoc)
	afx_msg void OnTargetsNewControlTarget();
	afx_msg void OnTargetsDelete();
	afx_msg void OnUpdateTargetsDelete(CCmdUI* pCmdUI);
	afx_msg void OnTargetsNewObject();
	afx_msg void OnUpdateTargetsNewObject(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSYSTEMDOC_H__902F2692_5F82_4A34_BA9B_8C7C6487573E__INCLUDED_)
