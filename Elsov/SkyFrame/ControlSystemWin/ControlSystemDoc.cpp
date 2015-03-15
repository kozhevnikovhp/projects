// ControlSystemDoc.cpp : implementation of the CControlSystemDoc class
//

#include "stdafx.h"
#include "ControlSystemWin.h"

#include "ControlSystemDoc.h"
#include "ControlSystemView.h"
#include "NewControlTargetDialog.h"
#include "ControlTargetAPI.h"
#include "TargetInstance.h"
#include "AbstractForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlSystemDoc

IMPLEMENT_DYNCREATE(CControlSystemDoc, CDocument)

BEGIN_MESSAGE_MAP(CControlSystemDoc, CDocument)
	//{{AFX_MSG_MAP(CControlSystemDoc)
	ON_COMMAND(ID_TARGETS_NEWCONTROLTARGET, OnTargetsNewControlTarget)
	ON_COMMAND(ID_TARGETS_DELETE, OnTargetsDelete)
	ON_UPDATE_COMMAND_UI(ID_TARGETS_DELETE, OnUpdateTargetsDelete)
	ON_COMMAND(ID_TARGETS_NEWOBJECT, OnTargetsNewObject)
	ON_UPDATE_COMMAND_UI(ID_TARGETS_NEWOBJECT, OnUpdateTargetsNewObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlSystemDoc construction/destruction

CControlSystemDoc::CControlSystemDoc()
{
	m_bAbstractFormUpdateTimerEnabled = TRUE;
	m_pTreeView = NULL;
	SetModifiedFlag();

}

CControlSystemDoc::~CControlSystemDoc()
{
	for (POSITION pos = m_TargetInstanceList.GetHeadPosition(); pos; m_TargetInstanceList.GetNext(pos))
	{
		CTargetInstance *pInstance = (CTargetInstance *)m_TargetInstanceList.GetAt(pos);
		delete pInstance;
	}
	m_TargetInstanceList.RemoveAll();
}

const char *pszCommonSectionName = "Common";
const char *pszInstanceCountParamName = "InstanceCount";

CString GetInstanceSectionName(int iInstance)
{
	CString str;
	str.Format("ControlTarget#%d", iInstance);
	return str;
}

BOOL CControlSystemDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	int nInstances = AfxGetApp()->GetProfileInt(pszCommonSectionName, pszInstanceCountParamName, 0);
	for (int iInstance = 0; iInstance < nInstances; iInstance++)
	{
		CString SectionName = GetInstanceSectionName(iInstance);
		CTargetInstance *pNewInstance = new CTargetInstance;
		pNewInstance->LoadFromRegistry(SectionName);
		m_TargetInstanceList.AddTail(pNewInstance);
		m_pTreeView->AddNewTarget(pNewInstance);
	}

	return TRUE;
}

void CControlSystemDoc::AddNewForm(FI_FormIndex IndexInViewArray, CView *pViewToAdd, CWnd *pParent)
{
	m_pForms[IndexInViewArray] = (CAbstractForm *)pViewToAdd;
	m_pForms[IndexInViewArray]->SetDocument(this);
	pViewToAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CFrameWnd::rectDefault, pParent,
		AFX_IDW_PANE_FIRST+IndexInViewArray+1, NULL);
	AddView(pViewToAdd);
}



/////////////////////////////////////////////////////////////////////////////
// CControlSystemDoc serialization

void CControlSystemDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CControlSystemDoc diagnostics

#ifdef _DEBUG
void CControlSystemDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CControlSystemDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CControlSystemDoc commands

void CControlSystemDoc::OnTargetsNewControlTarget() 
{
	CNewControlTargetDialog dialog;
	CString FileName = dialog.GetLibraryFileName();
	if (FileName.IsEmpty())
		return;

	CTargetInstance *pNewInstance = new CTargetInstance(FileName);
	m_TargetInstanceList.AddTail(pNewInstance);
	m_pTreeView->AddNewTarget(pNewInstance);
}

BOOL CControlSystemDoc::SaveModified() 
{
	int nInstances = m_TargetInstanceList.GetCount();
	AfxGetApp()->WriteProfileInt(pszCommonSectionName, pszInstanceCountParamName, nInstances);
	int iInstance = 0;
	for (POSITION pos = m_TargetInstanceList.GetHeadPosition(); pos; m_TargetInstanceList.GetNext(pos))
	{
		CTargetInstance *pInstance = (CTargetInstance *)m_TargetInstanceList.GetAt(pos);
		CString SectionName = GetInstanceSectionName(iInstance);
		pInstance->WriteToRegistry(SectionName);
		++iInstance;
	}
	return CDocument::SaveModified();
}

void CControlSystemDoc::OnTargetsDelete() 
{
	if (AfxMessageBox(IDS_ARE_YOU_SURE_TO_DELETE, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
	CTargetInstance *pDeletedInstance = m_pTreeView->DeleteSelectedTarget();
	if (pDeletedInstance)
	{
		POSITION pos = m_TargetInstanceList.Find(pDeletedInstance);
		ASSERT(pos);
		m_TargetInstanceList.RemoveAt(pos);
		delete pDeletedInstance;
	}
}

void CControlSystemDoc::OnUpdateTargetsDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pTreeView->CanBeDeletedSelectedItem());
}

void CControlSystemDoc::OnTargetsNewObject() 
{
	m_pTreeView->AddNewObject();
}

void CControlSystemDoc::OnUpdateTargetsNewObject(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pTreeView->CanAddChildToSelectedItem());
}
