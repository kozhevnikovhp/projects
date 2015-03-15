// ObjectForm.cpp : implementation file
//

#include "stdafx.h"
#include "ControlSystemWin.h"
#include "ControlSystemView.h"
#include "ControlSystemFrame.h"
#include "ControlSystemDoc.h"
#include "TargetInstance.h"
#include "ObjectForm.h"
#include "Attached.h"
#include "ControlTargetAPI.h"
#include "ControlObject.h"
#include "DeleteObjectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectForm

IMPLEMENT_DYNCREATE(CObjectForm, CAbstractForm)

CObjectForm::CObjectForm()
	: CAbstractForm(CObjectForm::IDD)
{
	//{{AFX_DATA_INIT(CObjectForm)
	m_NameEdit = _T("");
	m_NameStatic = _T("");
	m_TypeStatic = _T("");
	//}}AFX_DATA_INIT
	m_pObject = NULL;
	m_pTarget = NULL;
	m_pTargetInstance = NULL;
}

CObjectForm::~CObjectForm()
{
}

void CObjectForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectForm)
	DDX_Control(pDX, IDC_DISPLAY_TAB, m_DisplayTab);
	DDX_Control(pDX, IDC_TYPE_COMBO, m_TypeComboBox);
	DDX_Text(pDX, IDC_NAME_EDIT, m_NameEdit);
	DDX_Text(pDX, IDC_NAME_STATIC, m_NameStatic);
	DDX_Text(pDX, IDC_TYPE_STATIC, m_TypeStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectForm, CAbstractForm)
	//{{AFX_MSG_MAP(CObjectForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_NOTIFY(TCN_SELCHANGE, IDC_DISPLAY_TAB, OnSelChangeDisplayTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_DISPLAY_TAB, OnSelChangingDisplayTab)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectForm diagnostics

#ifdef _DEBUG
void CObjectForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CObjectForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjectForm message handlers

void CObjectForm::OnInitialUpdate() 
{
	m_NameStatic.LoadString(IDS_NAME_STATIC);
	m_TypeStatic.LoadString(IDS_TYPE_STATIC);
	CString str;
	// Apply button
	str.LoadString(IDS_APPLY_BUTTON);
	GetDescendantWindow(IDC_APPLY_BUTTON)->SetWindowText(str);
	
	CAbstractForm::OnInitialUpdate();
}

//virtual
CTargetInstance *CObjectForm::DeleteSelectedObject(CAttachedBlock *pAttached)
{
	if (!CanBeDeleted())
		return NULL;

	CDeleteObjectDialog dialog(m_pObject);
	if (dialog.DoModal() != IDOK)
		return NULL;
	if (dialog.GetDeletionType() == DT_DELETE)
		m_pTargetInstance->DeleteControlObject(m_pObject);
	else if (dialog.GetDeletionType() == DT_DESTROY)
		m_pTargetInstance->DestroyControlObject(m_pObject);
	
	return NULL;
}

//virtual
BOOL CObjectForm::SetupControls()
{
	CAttachedBlockForObject *pAttachedForObject = (CAttachedBlockForObject *)m_pAttached;
	m_pObject = pAttachedForObject->GetObject();
	ASSERT(m_pObject);
	m_pTarget = m_pAttached->GetTarget();
	ASSERT(m_pTarget);
	m_pTargetInstance = m_pAttached->GetTargetInstance();
	ASSERT(m_pTargetInstance);

	m_NameEdit = m_pObject->GetName();
	UpdateData(FALSE);

	m_TypeComboBox.ResetContent();
	int index;
	for (int type = 1; type <= m_pTarget->GetNumberOfSupportedObjectTypes(); type++)
	{
		index = m_TypeComboBox.AddString(m_pTarget->GetObjectTypeDescription(type));
		m_TypeComboBox.SetItemData(index, type);
		if (type == m_pObject->GetType())
		{
			m_TypeComboBox.SetCurSel(index);
		}
	}

	LoadDisplaySwitcher();

	return TRUE;
}

//virtual
BOOL CObjectForm::BeforeHiding()
{
	TurnToAbstractPane();
	return TRUE;
}

void CObjectForm::LoadDisplaySwitcher()
{
	TC_ITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = TCIF_TEXT | TCIF_PARAM;
	m_DisplayTab.DeleteAllItems();
	CString DisplayedPageStr;

	if (m_pObject->HasEventLog())
	{
		DisplayedPageStr.LoadString(IDS_EVENT_LOG);
		item.lParam = EVENT_LOG_FORM_INDEX;
		item.pszText = (char *)(LPCTSTR)DisplayedPageStr;
		m_DisplayTab.InsertItem(m_DisplayTab.GetItemCount(), &item);
	}

	if (m_pObject->MC_HasMonitorAndControl())
	{
		DisplayedPageStr.LoadString(IDS_MONITOR_AND_CONTROL);
		item.lParam = MONITOR_AND_CONTROL_FORM_INDEX;
		item.pszText = (char *)(LPCTSTR)DisplayedPageStr;
		m_DisplayTab.InsertItem(m_DisplayTab.GetItemCount(), &item);
	}

	if (m_DisplayTab.GetItemCount() > 0)
	{
		m_DisplayTab.SetCurSel(0);
		OnSelChangeDisplayTab(NULL, NULL);
		m_DisplayTab.ShowWindow(SW_SHOW);
	}
	else
	{
		TurnToAbstractPane();
		m_DisplayTab.ShowWindow(SW_HIDE);
	}
}

//virtual
CControlObject *CObjectForm::CreateNewObject(CTargetInstance *pTargetInstance)
{
	CControlObject *pNewObject = pTargetInstance->CreateControlObject(m_pObject);
	return pNewObject;
}

void CObjectForm::OnApplyButton() 
{
	UpdateData(TRUE);
	int index = m_TypeComboBox.GetCurSel();
	if (index == CB_ERR)
		return;

	long type = m_TypeComboBox.GetItemData(index);
	m_pObject->SetNameAndType(m_NameEdit, type);
	CString NewItemText = m_pObject->GetFullName();
	GetTreeView()->UpdateSelectedText(NewItemText);
	GetTreeView()->UpdateSelectedImage(m_pObject);
	LoadDisplaySwitcher();
}

void CObjectForm::OnSelChangeDisplayTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CWaitCursor cursor;
	int index = m_DisplayTab.GetCurSel();
	if (index != -1)
	{
		TC_ITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = TCIF_PARAM;
		m_DisplayTab.GetItem(index, &item);

		CControlSystemFrame *pFrame = (CControlSystemFrame *)GetParentFrame();
		CAbstractForm *pOld = (CAbstractForm *)(pFrame->GetBottomAbstractPane());

		CAbstractForm *pNew = GetDocument()->m_pForms[item.lParam];

		UINT OldIDD = pOld->GetDlgCtrlID();
		UINT NewIDD = pNew->GetDlgCtrlID();
		if (OldIDD == NewIDD) return;
		if (pOld)
		{
			if (!pOld->BeforeHiding())
			{
			}
		}

		pNew->m_pAttached = m_pAttached;
		if (pNew->SetupControls())
		{
			//m_pCurrentlyVisibleForm = pNew;
			if (OldIDD != NewIDD)
			{
				pOld->ShowWindow(SW_HIDE);
				pNew->ShowWindow(SW_SHOW);
				pOld->SetDlgCtrlID(NewIDD);
				pNew->SetDlgCtrlID(OldIDD);
				pFrame->Refresh();
			}
		}
		else
		{
			MessageBeep(0);
		//	m_pTreeCtrl->SelectItem(m_PrevSelection);
		}
	}
	else
		TurnToAbstractPane();

	if (pResult)
		*pResult = 0;
}

void CObjectForm::OnSelChangingDisplayTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CObjectForm::TurnToAbstractPane()
{
	CControlSystemFrame *pFrame = (CControlSystemFrame *)GetParentFrame();

	CWnd *pOld = pFrame->GetBottomAbstractPane();
	UINT OldIDD = pOld->GetDlgCtrlID();
	CWnd *pNew = GetDocument()->m_pForms[ABSTRACT_INDEX];
	UINT NewIDD = pNew->GetDlgCtrlID();
	pOld->ShowWindow(SW_HIDE);
	pNew->ShowWindow(SW_SHOW);
	pOld->SetDlgCtrlID(NewIDD);
	pNew->SetDlgCtrlID(OldIDD);
	pFrame->Refresh();
}


void CObjectForm::OnSize(UINT nType, int cx, int cy) 
{
	CAbstractForm::OnSize(nType, cx, cy);
	if (!IsWindow(m_DisplayTab.m_hWnd))
		return;

	CRect ClientRect(0,0,0,0);
	GetClientRect(&ClientRect);
	
	CRect WindowRect(0,0,0,0);
	m_DisplayTab.GetWindowRect(&WindowRect);

	m_DisplayTab.SetWindowPos(NULL, 0, ClientRect.Height()-WindowRect.Height(), ClientRect.Width(), WindowRect.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);

	InvalidateRect(NULL);
	UpdateWindow();
	
	m_DisplayTab.InvalidateRect(NULL);
	m_DisplayTab.UpdateWindow();
}
