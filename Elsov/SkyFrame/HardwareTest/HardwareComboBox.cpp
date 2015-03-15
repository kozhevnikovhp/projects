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
// cHardwareComboBox

cHardwareComboBox::cHardwareComboBox()
{
}

cHardwareComboBox::~cHardwareComboBox()
{
}

void cHardwareComboBox::SelectByDataValue(int iDataValue)
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

int cHardwareComboBox::getSelectedMode() const
{
	int index = GetCurSel();
	if (index == CB_ERR)
		return -1;
	return GetItemData(index);
}

BEGIN_MESSAGE_MAP(cHardwareComboBox, CComboBox)
	//{{AFX_MSG_MAP(cHardwareComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cHardwareComboBox message handlers
