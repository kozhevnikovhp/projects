// TmiDatabaseDoc.h : interface of the CTmiDatabaseDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMIDATABASEDOC_H__6AE72ACC_99FD_43E9_BFA9_194CC66C6D4F__INCLUDED_)
#define AFX_TMIDATABASEDOC_H__6AE72ACC_99FD_43E9_BFA9_194CC66C6D4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TmiStationsSet.h"
#include "TmiSet.h"

class CActionsSet;
class CTmiFromTimeToTimeQuerySet;
class CWorkingHoursSet;

class CTmiDatabaseDoc : public CDocument
{
protected: // create from serialization only
	CTmiDatabaseDoc();
	DECLARE_DYNCREATE(CTmiDatabaseDoc)

// Attributes
public:
	CStationSet m_StationSet;

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTmiDatabaseDoc)
	public:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTmiDatabaseDoc();
	int Import(CString &Path, CTmiSet *pSet, CActionsSet *pActionsSet);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString CalcWorkingTimeReport(long StationID, CTime CurrentDayStart, CTime NextDayStart, CWorkingHoursSet *pAzsUzsSet, CTmiFromTimeToTimeQuerySet *pUzsSet, BOOL bHoursNeeded);
	int HowManyRegulations(long StationID, CTime FromTime, CTime ToTime, CActionsSet *pActionsSet);
	
protected:
	//{{AFX_MSG(CTmiDatabaseDoc)
	afx_msg void OnFileImport();
	afx_msg void OnQueryFromTimeToTime();
	afx_msg void OnQueryRequlationsFromTimeToTime();
	afx_msg void OnQueryWorkingHours();
	afx_msg void OnQueryDeleteTmiFromTimeToTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMIDATABASEDOC_H__6AE72ACC_99FD_43E9_BFA9_194CC66C6D4F__INCLUDED_)
