#if !defined(AFX_HARDWARECOMBOBOX_H__776066EF_4B35_4229_A9A1_0370D8B8B276__INCLUDED_)
#define AFX_HARDWARECOMBOBOX_H__776066EF_4B35_4229_A9A1_0370D8B8B276__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HardwareComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cHardwareComboBox window

class cHardwareComboBox : public CComboBox
{
// Construction
public:
	cHardwareComboBox();

// Attributes
public:
	void SelectByDataValue(int iDataValue);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cHardwareComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cHardwareComboBox();

	int getSelectedMode() const;

	// Generated message map functions
protected:
	//{{AFX_MSG(cHardwareComboBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HARDWARECOMBOBOX_H__776066EF_4B35_4229_A9A1_0370D8B8B276__INCLUDED_)
