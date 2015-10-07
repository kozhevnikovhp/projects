#if !defined(AFX_HARDWARECOMBOBOX_H__776066EF_4B35_4229_A9A1_0370D8B8B276__INCLUDED_)
#define AFX_HARDWARECOMBOBOX_H__776066EF_4B35_4229_A9A1_0370D8B8B276__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HardwareComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HardwareComboBox window

class HardwareComboBox : public CComboBox
{
// Construction
public:
	HardwareComboBox();
	virtual ~HardwareComboBox();

public:
	void addMode(const char *pszModeName, int iData);
	void SelectByDataValue(int iDataValue);
	int getSelectedMode() const;

// Overrides
	//{{AFX_VIRTUAL(HardwareComboBox)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(HardwareComboBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HARDWARECOMBOBOX_H__776066EF_4B35_4229_A9A1_0370D8B8B276__INCLUDED_)
