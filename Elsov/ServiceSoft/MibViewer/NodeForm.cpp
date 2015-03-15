// NodeForm.cpp : implementation file
//

#include "stdafx.h"
#include "mibviewer.h"
#include "NodeForm.h"
#include "MibParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNodeForm

IMPLEMENT_DYNCREATE(CNodeForm, CFormView)

CNodeForm::CNodeForm()
	: CFormView(CNodeForm::IDD)
{
	//{{AFX_DATA_INIT(CNodeForm)
	m_FullSpec = _T("");
	m_FullDecimalSpec = _T("");
	m_Description = _T("");
	//}}AFX_DATA_INIT
	m_pNode = NULL;
}

CNodeForm::~CNodeForm()
{
}

void CNodeForm::SetupControls(CMibNode *pNode)
{
	m_pNode = pNode;
	m_FullSpec = pNode->GetFullTextSpec();
	m_FullDecimalSpec = pNode->GetFullDecimalSpec();
	m_Description.Empty();
	CMibObjectType *pType = pNode->GetObjectType();
	if (pType)
	{
		const char *pszDescription = pType->GetDescription();
		if (pszDescription)
		{
			char *pcSymbol = (char *)pszDescription;
			while (*pcSymbol)
			{
				if (*pcSymbol == 0x0A)
					m_Description += "\r\n";
				else if (*pcSymbol == 0x0D)
				{
				}
				else
					m_Description += *pcSymbol;

				++pcSymbol;
			}
		}
	}
	UpdateData(FALSE);
}

void CNodeForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNodeForm)
	DDX_Text(pDX, IDC_FULL_SPEC_EDIT, m_FullSpec);
	DDX_Text(pDX, IDC_FULL_DECIMAL_SPEC_EDIT, m_FullDecimalSpec);
	DDX_Text(pDX, IDC_DESCRIPTION_EDIT, m_Description);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNodeForm, CFormView)
	//{{AFX_MSG_MAP(CNodeForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodeForm diagnostics

#ifdef _DEBUG
void CNodeForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CNodeForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNodeForm message handlers
