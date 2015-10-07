// HardwareComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "hardwaretest.h"
#include "HardwareComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HardwareComboBox

HardwareComboBox::HardwareComboBox()
{
}

HardwareComboBox::~HardwareComboBox()
{
}

void HardwareComboBox::addMode(const char *pszModeName, int iData)
{
	if (!strlen(pszModeName))
		return;
	int index = AddString(pszModeName);
	SetItemData(index, iData);
}

void HardwareComboBox::SelectByDataValue(int iDataValue)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (GetItemData(i) == iDataValue)
		{
			SetCurSel(i);
			return;
		}
	}
}

int HardwareComboBox::getSelectedMode() const
{
	int index = GetCurSel();
	if (index == CB_ERR)
		return -1;
	return GetItemData(index);
}

BEGIN_MESSAGE_MAP(HardwareComboBox, CComboBox)
	//{{AFX_MSG_MAP(HardwareComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HardwareComboBox message handlers
