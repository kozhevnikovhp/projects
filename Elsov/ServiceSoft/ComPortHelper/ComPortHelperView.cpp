// ComPortHelperView.cpp : implementation of the CComPortHelperView class
//

#include "stdafx.h"
#include "ComPortHelper.h"

#include "ComPortHelperDoc.h"
#include "ComPortHelperView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char szPortName[32];
char szPortConfigureString[32];
const char *pszIniFile = "TermHex.ini";
const char *pszSection = "Port";
const char *pszPortName = "Name";
const char *pszPortConfig = "Configuration";

//virtual
char *CTargetPort::GetControlString()
{
	return szPortConfigureString;
}


/////////////////////////////////////////////////////////////////////////////
// CComPortHelperView

IMPLEMENT_DYNCREATE(CComPortHelperView, CFormView)

BEGIN_MESSAGE_MAP(CComPortHelperView, CFormView)
	//{{AFX_MSG_MAP(CComPortHelperView)
	ON_BN_CLICKED(IDC_OPEN_BUTTON, OnOpenButton)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CLEAR_FROM_BUTTON, OnClearFromButton)
	ON_BN_CLICKED(IDC_FROM_SCROLL_TO_BOTTOM_CHECK, OnFromScrollToBottomCheck)
	ON_BN_CLICKED(IDC_0_BUTTON, On0Button)
	ON_BN_CLICKED(IDC_1_BUTTON, On1Button)
	ON_BN_CLICKED(IDC_2_BUTTON, On2Button)
	ON_BN_CLICKED(IDC_3_BUTTON, On3Button)
	ON_BN_CLICKED(IDC_4_BUTTON, On4Button)
	ON_BN_CLICKED(IDC_5_BUTTON, On5Button)
	ON_BN_CLICKED(IDC_6_BUTTON, On6Button)
	ON_BN_CLICKED(IDC_7_BUTTON, On7Button)
	ON_BN_CLICKED(IDC_8_BUTTON, On8Button)
	ON_BN_CLICKED(IDC_9_BUTTON, On9Button)
	ON_BN_CLICKED(IDC_A_BUTTON, OnAButton)
	ON_BN_CLICKED(IDC_B_BUTTON, OnBButton)
	ON_BN_CLICKED(IDC_C_BUTTON, OnCButton)
	ON_BN_CLICKED(IDC_D_BUTTON, OnDButton)
	ON_BN_CLICKED(IDC_E_BUTTON, OnEButton)
	ON_BN_CLICKED(IDC_F_BUTTON, OnFButton)
	ON_BN_CLICKED(IDC_BACKSPACE_BUTTON, OnBackspaceButton)
	ON_BN_CLICKED(IDC_TO_CLEAR_BUTTON, OnToClearButton)
	ON_BN_CLICKED(IDC_CONFIGURE_BUTTON, OnConfigureButton)
	ON_BN_CLICKED(IDC_SEND_BUTTON, OnSendButton)
	ON_BN_CLICKED(IDC_SKIP_INPUT_CHECK, OnSkipInputCheck)
	ON_BN_CLICKED(IDC_CLOSE_BUTTON, OnCloseButton)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperView construction/destruction

CComPortHelperView::CComPortHelperView()
	: CFormView(CComPortHelperView::IDD)
{
	//{{AFX_DATA_INIT(CComPortHelperView)
	m_FromPortString = _T("");
	m_ToPortString = _T("");
	m_ModeString = _T("");
	m_PortStatus = _T("");
	//}}AFX_DATA_INIT

	m_nByte = 0;
	m_nBytesToSend = 0;
	m_bHalfByte = FALSE;
}

CComPortHelperView::~CComPortHelperView()
{
}

void CComPortHelperView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComPortHelperView)
	DDX_Control(pDX, IDC_SKIP_INPUT_CHECK, m_SkipInputButton);
	DDX_Control(pDX, IDC_TO_PORT_EDIT, m_ToPortCtrl);
	DDX_Control(pDX, IDC_FROM_SCROLL_TO_BOTTOM_CHECK, m_FromScrollToBottom);
	DDX_Control(pDX, IDC_FROM_PORT_EDIT, m_FromPortCtrl);
	DDX_Control(pDX, IDC_PORT_COMBO, m_PortCombo);
	DDX_Text(pDX, IDC_FROM_PORT_EDIT, m_FromPortString);
	DDX_Text(pDX, IDC_TO_PORT_EDIT, m_ToPortString);
	DDX_Text(pDX, IDC_MODE_EDIT, m_ModeString);
	DDX_Text(pDX, IDC_STATUS_EDIT, m_PortStatus);
	//}}AFX_DATA_MAP
}

BOOL CComPortHelperView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CComPortHelperView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	GetPrivateProfileString(pszSection, pszPortName, "Com1", szPortName, sizeof(szPortName)/sizeof(szPortName[0]), pszIniFile);
	GetPrivateProfileString(pszSection, pszPortConfig, "9600,n,8,1", szPortConfigureString, sizeof(szPortConfigureString)/sizeof(szPortConfigureString[0]), pszIniFile);

	m_PortCombo.ResetContent();
	CString str;
	for (int i=1; i <= 256; i++)
	{
		str.Format("Com%d", i);
		int index = m_PortCombo.AddString(str);
		m_PortCombo.SetItemData(index, i);
		if (!str.CompareNoCase(szPortName))
			m_PortCombo.SetCurSel(index);
	}

	CFont *pFont = m_FromPortCtrl.GetFont();
	LOGFONT LogFont;
	pFont->GetLogFont(&LogFont);
	strcpy(LogFont.lfFaceName, "Courier");
	m_Font.CreateFontIndirect(&LogFont);
	m_FromPortCtrl.SetFont(&m_Font);
	m_ToPortCtrl.SetFont(&m_Font);

	m_FromScrollToBottom.SetCheck(1);
	m_bFromScrollToBottom = m_FromScrollToBottom.GetCheck();
	m_SkipInputButton.SetCheck(0);
	m_bSkipInput = m_SkipInputButton.GetCheck();

	m_ModeString = szPortConfigureString;
	UpdateData(FALSE);

	CComPortHelperApp *pApp = (CComPortHelperApp *)AfxGetApp();
	pApp->m_pView = this;
	OnOpenButton();
}

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperView printing

BOOL CComPortHelperView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CComPortHelperView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CComPortHelperView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CComPortHelperView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperView diagnostics

#ifdef _DEBUG
void CComPortHelperView::AssertValid() const
{
	CFormView::AssertValid();
}

void CComPortHelperView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CComPortHelperDoc* CComPortHelperView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CComPortHelperDoc)));
	return (CComPortHelperDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperView message handlers

void CComPortHelperView::OnOpenButton() 
{
	int index = m_PortCombo.GetCurSel();
	if (index == CB_ERR)
		MessageBox("Specify correct port, please!", "Incorrect parameter", MB_ICONEXCLAMATION);
	else
	{
		m_PortCombo.GetLBText(index, szPortName);
		m_Port.Open(szPortName);
	}
	m_PortStatus = m_Port.GetStatusString();
	UpdateData(FALSE);

	WritePrivateProfileString(pszSection, pszPortName, szPortName, pszIniFile);
}

void CComPortHelperView::OnDestroy() 
{
	m_Port.Close();
	CFormView::OnDestroy();
}

void CComPortHelperView::OnClearFromButton() 
{
	m_FromPortString = &afxChNil;
	UpdateData(FALSE);
	m_nByte = 0;
}

int CComPortHelperView::OnIdle()
{
	char szBuffer[64];
	int nFirstVisibleLine = 0;
	if (m_Port.IsOpened())
	{
		unsigned char c;
		BOOL bSuccess = FALSE;
		unsigned int nReadBytes = 0;
		BOOL bChanged = FALSE;;
		if (!m_bFromScrollToBottom)
			nFirstVisibleLine = m_FromPortCtrl.GetFirstVisibleLine();
		do
		{
			bSuccess = m_Port.Read((char *)&c, 1, nReadBytes);
			if (bSuccess && nReadBytes && !m_bSkipInput)
			{
				if ((m_nByte % 10) == 0)
				{
					sprintf(szBuffer, "%04d...%04d:  ", m_nByte, m_nByte+10);
					m_FromPortString += szBuffer;
				}
				sprintf(szBuffer, "%02hX ", c);
				m_FromPortString += szBuffer;
				UpdateData(FALSE);
				if (((m_nByte+1) % 10) == 0)
				{
					m_FromPortString += "\r\n";
				}
				++m_nByte;
				bChanged = TRUE;
			}
		} while (bSuccess && nReadBytes);
		if (bChanged)
		{
			if (m_bFromScrollToBottom)
			{
				m_FromPortCtrl.LineScroll(m_FromPortCtrl.GetLineCount());		
			}
			else
			{
				m_FromPortCtrl.LineScroll(-m_FromPortCtrl.GetLineCount());		
				m_FromPortCtrl.LineScroll(nFirstVisibleLine);		
			}
		}


		return 1;
	}
	else
		return 0;
}

void CComPortHelperView::OnFromScrollToBottomCheck() 
{
	m_bFromScrollToBottom = m_FromScrollToBottom.GetCheck();
}

void CComPortHelperView::DigitPressed(unsigned char half_octet)
{
	m_bHalfByte = !m_bHalfByte;
	if (m_bHalfByte)
	{
		m_cHalfOctet = half_octet & 0x0F;
	}
	else
	{
		m_szBuffer[m_nBytesToSend] = m_cHalfOctet << 4;
		m_szBuffer[m_nBytesToSend] |= half_octet;
		++m_nBytesToSend;
	}
	UpdateToString();
}

void CComPortHelperView::UpdateToString()
{
	char szBuffer[64];
	m_ToPortString = &afxChNil;
	for (int i = 0; i < m_nBytesToSend; i++)
	{
		if ((i % 10) == 0)
		{
			sprintf(szBuffer, "%04d...%04d:  ", i, i+10);
			m_ToPortString += szBuffer;
		}
		sprintf(szBuffer, "%02hX ", m_szBuffer[i]);
		m_ToPortString += szBuffer;
		if (((i+1) % 10) == 0)
		{
			m_ToPortString += "\r\n";
		}
	}
	
	char symbols[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	if (m_bHalfByte)
	{
		if (m_nBytesToSend % 10 == 0)
		{
			sprintf(szBuffer, "%04d...%04d:  ", m_nBytesToSend, m_nBytesToSend+10);
			m_ToPortString += szBuffer;
		}
		m_ToPortString += symbols[m_cHalfOctet];
	}

	UpdateData(FALSE);
	m_ToPortCtrl.LineScroll(m_ToPortCtrl.GetLineCount());		
}

void CComPortHelperView::On0Button() 
{
	DigitPressed(0x00);
}

void CComPortHelperView::On1Button() 
{
	DigitPressed(0x01);
}

void CComPortHelperView::On2Button() 
{
	DigitPressed(0x02);
}

void CComPortHelperView::On3Button() 
{
	DigitPressed(0x03);
}

void CComPortHelperView::On4Button() 
{
	DigitPressed(0x04);
}

void CComPortHelperView::On5Button() 
{
	DigitPressed(0x05);
}

void CComPortHelperView::On6Button() 
{
	DigitPressed(0x06);
}

void CComPortHelperView::On7Button() 
{
	DigitPressed(0x07);
}

void CComPortHelperView::On8Button() 
{
	DigitPressed(0x08);
}

void CComPortHelperView::On9Button() 
{
	DigitPressed(0x09);
}

void CComPortHelperView::OnAButton() 
{
	DigitPressed(0x0A);
}

void CComPortHelperView::OnBButton() 
{
	DigitPressed(0x0B);
}

void CComPortHelperView::OnCButton() 
{
	DigitPressed(0x0C);
}

void CComPortHelperView::OnDButton() 
{
	DigitPressed(0x0D);
}

void CComPortHelperView::OnEButton() 
{
	DigitPressed(0x0E);
}

void CComPortHelperView::OnFButton() 
{
	DigitPressed(0x0F);
}

void CComPortHelperView::OnBackspaceButton() 
{
	if (!m_bHalfByte)
		--m_nBytesToSend;

	m_bHalfByte = FALSE;
	UpdateToString();
}

void CComPortHelperView::OnToClearButton() 
{
	m_nBytesToSend = 0;
	m_bHalfByte = FALSE;
	UpdateToString();
}

void CComPortHelperView::OnConfigureButton() 
{
	int index = m_PortCombo.GetCurSel();
	if (index == CB_ERR)
		return;

	m_PortCombo.GetLBText(index, szPortName);

	if (!Comm_GuiUpdateConfigureString(szPortName, szPortConfigureString))
		return;  // canceled => do nothing

	
	m_ModeString = szPortConfigureString;
	UpdateData(FALSE);

	WritePrivateProfileString(pszSection, pszPortConfig, szPortConfigureString, pszIniFile);
	OnOpenButton();
}

void CComPortHelperView::OnSendButton() 
{
	unsigned int nWrittenBytes;
	m_Port.Write((char *)m_szBuffer, m_nBytesToSend, nWrittenBytes);	
}

void CComPortHelperView::OnSkipInputCheck() 
{
	m_bSkipInput = m_SkipInputButton.GetCheck();
}

void CComPortHelperView::OnCloseButton() 
{
	m_Port.Close();
	m_PortStatus = m_Port.GetStatusString();
	UpdateData(FALSE);
}
