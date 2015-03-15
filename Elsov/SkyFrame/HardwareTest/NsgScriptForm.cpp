// NsgScriptForm.cpp : implementation file
//

#include "stdafx.h"
#include "NsgScriptForm.h"
#include "Attached.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char pszScriptBody[0xFFFF]; //64K size

/////////////////////////////////////////////////////////////////////////////
// CNsgScriptForm

IMPLEMENT_DYNCREATE(CNsgScriptForm, CAbstractForm)

CNsgScriptForm::CNsgScriptForm()
	: CAbstractForm(CNsgScriptForm::IDD)
{
	//{{AFX_DATA_INIT(CNsgScriptForm)
	m_ScriptBody = _T("");
	//}}AFX_DATA_INIT
	m_pNsgFrad=NULL;
}

CNsgScriptForm::~CNsgScriptForm()
{
}

void CNsgScriptForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNsgScriptForm)
	DDX_Text(pDX, IDC_SCRIPT_EDIT, m_ScriptBody);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNsgScriptForm, CAbstractForm)
	//{{AFX_MSG_MAP(CNsgScriptForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_MAKE_SCRIPT_BUTTON, OnMakeScriptButton)
	ON_BN_CLICKED(IDC_SAVE_AS_BUTTON, OnSaveAsButton)
	ON_BN_CLICKED(IDC_LOAD_BUTTON, OnLoadButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNsgScriptForm diagnostics

#ifdef _DEBUG
void CNsgScriptForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CNsgScriptForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CNsgScriptForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pNsgFrad=(CNsgFrad *)pDevice;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNsgScriptForm message handlers

void CNsgScriptForm::OnApplyButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pNsgFrad->ApplyScript((char *)LPCTSTR(m_ScriptBody));
}

void CNsgScriptForm::OnMakeScriptButton() 
{
	CWaitCursor cursor;
	m_pNsgFrad->MakeScript(pszScriptBody);
	m_ScriptBody=pszScriptBody;
	UpdateData(FALSE);
}

void CNsgScriptForm::OnSaveAsButton() 
{
	UpdateData();

	CFileDialog fd(FALSE, _T("scr"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Файлы скриптов(*scr)|*scr|Все файлы (*.*)|*.*||"));
	if (fd.DoModal()==IDCANCEL) return;

	CWaitCursor cursor;

	HANDLE hFile=CreateFile(fd.GetPathName(), GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox("Файл не создался", "Внимание", MB_ICONEXCLAMATION);
		return;
	}

	DWORD WrittenBytes;
	WriteFile(hFile, LPCTSTR(m_ScriptBody), m_ScriptBody.GetLength(),
		&WrittenBytes, NULL);

	CloseHandle(hFile);	
}

void CNsgScriptForm::OnLoadButton() 
{
	CFileDialog fd(TRUE, _T("scr"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Файлы скриптов(*scr)|*scr|Все файлы (*.*)|*.*||"));
	if (fd.DoModal()==IDCANCEL) return;

	CWaitCursor cursor;

	m_ScriptBody.Empty();

	HANDLE hFile=CreateFile(fd.GetPathName(), GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox("Файл не открылся", "Внимание", MB_ICONEXCLAMATION);
		return;
	}

	DWORD ReadBytes;
	char ch;
	while (1)
	{
		if (!ReadFile(hFile, &ch, 1, &ReadBytes, NULL)
			|| ReadBytes==0) break;
		m_ScriptBody+=ch;
	}

	CloseHandle(hFile);

	UpdateData(FALSE);	
}
