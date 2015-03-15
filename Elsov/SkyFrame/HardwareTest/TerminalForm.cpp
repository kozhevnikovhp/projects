// TerminalForm.cpp : implementation file
//

#include "stdafx.h"
#include "TerminalForm.h"
#include "Attached.h"
#include "Device.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char CR=0x0D;
static char LF=0x0A;
static char ESC=0x1B;

/////////////////////////////////////////////////////////////////////////////
// CTerminalForm

IMPLEMENT_DYNCREATE(CTerminalForm, CAbstractForm)

CTerminalForm::CTerminalForm()
	: CAbstractForm(CTerminalForm::IDD)
{
	//{{AFX_DATA_INIT(CTerminalForm)
	m_Reply = _T("");
	m_Command = _T("");
	m_RepeatInterval = 3;
	m_bRepeat = FALSE;
	//}}AFX_DATA_INIT
	m_pDevice=NULL;
}

CTerminalForm::~CTerminalForm()
{
}

void CTerminalForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerminalForm)
	DDX_Control(pDX, IDC_REPLY_EDIT, m_ReplyCtrl);
	DDX_Text(pDX, IDC_REPLY_EDIT, m_Reply);
	DDX_Text(pDX, IDC_COMMAND_EDIT, m_Command);
	DDX_Text(pDX, IDC_REPEAT_EDIT, m_RepeatInterval);
	DDV_MinMaxUInt(pDX, m_RepeatInterval, 0, 100);
	DDX_Check(pDX, IDC_REPEAT_CHECK, m_bRepeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerminalForm, CAbstractForm)
	//{{AFX_MSG_MAP(CTerminalForm)
	ON_BN_CLICKED(IDC_SEND_BUTTON, OnSendButton)
	ON_BN_CLICKED(IDC_SENDCRLF_BUTTON, OnSendCRLFButton)
	ON_BN_CLICKED(IDC_SENDESC_BUTTON, OnSendEscButton)
	ON_BN_CLICKED(IDC_SENDFILE_BUTTON, OnSendFileButton)
	ON_BN_CLICKED(IDC_REPEAT_CHECK, OnRepeatCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalForm diagnostics

#ifdef _DEBUG
void CTerminalForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CTerminalForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG


BOOL CTerminalForm::SetupControls()
{
	m_LastCommandTime=timeGetTime();
	m_bRepeat=FALSE;
	m_pDevice=m_pAttached->m_pDevice;
	UpdateData(FALSE);
	return TRUE;
}

BOOL CTerminalForm::DoTimer()
{
	static char pszBuffer[4092];
	unsigned int NewSymbols = m_pDevice->PumpMC(pszBuffer, sizeof(pszBuffer));
	if (!NewSymbols)
		return TRUE; // do nothing if no symbols are received
	if (m_ReplyCtrl.GetLineCount() >= 300)
	{
		m_ReplyCtrl.SetSel(0, -1);
		m_ReplyCtrl.Clear();
	}

	UpdateData(TRUE);
	for (unsigned int i = 0; i<NewSymbols; i++)
	{
		if (pszBuffer[i] == LF)
			continue;
		if (pszBuffer[i] == CR)
		{
			m_Reply += CR;
			m_Reply += LF;
			continue;
		}
		if (pszBuffer[i] == 0)
			pszBuffer[i] = ' '; // because some evices like SkyLane modems insert zeros into its reply strings (I do not know why)
		m_Reply += pszBuffer[i];
	}

	UpdateData(FALSE);
	m_ReplyCtrl.LineScroll(1000);

/*	if (m_bRepeat)
	{
		DWORD CurrentTime=timeGetTime();
		if (CurrentTime-m_LastCommandTime>m_RepeatInterval*1000)
		{
			OnSendButton();
			char pszCRLF[3];
			pszCRLF[0]=CR;
			pszCRLF[1]=LF;
			pszCRLF[2]=0;
			m_pDevice->m_MCPort.Send(pszCRLF, 2);

			m_LastCommandTime=CurrentTime;
		}
	}
*/
	return TRUE;
}

//virtual
void CTerminalForm::OnMcSymbolReceived(char cSymbol)
{
	m_Reply += cSymbol;
	m_ReplyCtrl.LineScroll(1000);
}

/////////////////////////////////////////////////////////////////////////////
// CTerminalForm message handlers

void CTerminalForm::OnSendButton() 
{
	CWaitCursor cursor;
	UpdateData(TRUE);
	static char pszBuffer[1024];
	strcpy(pszBuffer, LPCTSTR(m_Command));
	int len=strlen(pszBuffer);
	pszBuffer[len] = CR;
	pszBuffer[len+1] = LF;

	m_Reply.Empty();
	m_pDevice->SendMC(pszBuffer, len+2);
}

void CTerminalForm::OnSendCRLFButton() 
{
	m_pDevice->SendMC(&CR, 1);
	m_pDevice->SendMC(&LF, 1);
}

void CTerminalForm::OnSendEscButton() 
{
	m_pDevice->SendMC(&ESC, 1);
}

void CTerminalForm::OnSendFileButton() 
{
	CFileDialog fd(TRUE, _T("scr"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Все файлы (*.*)|*.*||"));
	if (fd.DoModal() == IDCANCEL) return;

	CWaitCursor cursor;

	HANDLE hFile = CreateFile(fd.GetPathName(), GENERIC_READ, 0, NULL,
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
		if (!ReadFile(hFile, &ch, 1, &ReadBytes, NULL) || ReadBytes==0) break;
		m_pDevice->SendMC(&ch, 1);
	}

	CloseHandle(hFile);
	UpdateData(FALSE);	
}

void CTerminalForm::OnRepeatCheck() 
{
	// TODO: Add your control notification handler code here
	
}
