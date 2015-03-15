// TmiAnalizerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TmiAnalizer.h"
#include "TmiAnalizerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiAnalizerDlg dialog

CTmiAnalizerDlg::CTmiAnalizerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTmiAnalizerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTmiAnalizerDlg)
	m_bSplitByDates = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTmiAnalizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiAnalizerDlg)
	DDX_Check(pDX, IDC_SPLIT_BY_DATES_CHECK, m_bSplitByDates);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTmiAnalizerDlg, CDialog)
	//{{AFX_MSG_MAP(CTmiAnalizerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NEW_FILE_BUTTON, OnNewFileButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiAnalizerDlg message handlers

BOOL CTmiAnalizerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTmiAnalizerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTmiAnalizerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTmiAnalizerDlg::OnNewFileButton() 
{
	UpdateData();
	CFileDialog fd(TRUE, _T("txt"), NULL,OFN_HIDEREADONLY,
	_T("Все файлы (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle="Укажите имя открываемого файла";
	if (fd.DoModal()!=IDOK)
		return;
	CString in=fd.GetPathName();
	CStdioFile FileIn;
	if (!FileIn.Open(in, CFile::modeRead))
	{
		MessageBox("Не могу открыть файл", "Ошибка!", MB_ICONEXCLAMATION);
		return;
	}

	CFileDialog fd1(FALSE, _T("txt"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Все файлы (*.*)|*.*||"));
	fd1.m_ofn.lpstrTitle="Укажите имя создаваемого файла";
	if (fd1.DoModal()!=IDOK)
		return;
	CString out=fd1.GetPathName();
	CStdioFile FileOut;
	if (!FileOut.Open(out, CFile::modeWrite | CFile::modeCreate))
	{
		MessageBox("Не могу открыть файл", "Ошибка!", MB_ICONEXCLAMATION);
		return;
	}

	char pszBuffer[1024], pszHeader[1024], pszDate[64], pszTime[64], pszTmpBuffer[64];
	int Var[15];
	int nDay, nMonth, nYear;
	int nHour, nMin, nSec;
	CTime PrevTime;
	BOOL bFirst = TRUE;
	while (1)
	{
		if (!FileIn.ReadString(pszBuffer, sizeof(pszBuffer)))
			break;

		int nVars = sscanf(pszBuffer, "%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t", pszDate, pszTime,
			&Var[0],
			&Var[1],
			&Var[2],
			&Var[3],
			&Var[4],
			&Var[5],
			&Var[6],
			&Var[7],
			&Var[8],
			&Var[9],
			&Var[10],
			&Var[11],
			&Var[12],
			&Var[13],
			&Var[14]);
		if (nVars < 3)
		{
			strcpy(pszHeader, pszBuffer);
			continue;
		}

		sscanf(pszDate, "%d.%d.%d)", &nDay, &nMonth, &nYear);
		sscanf(pszTime, "%d:%d:%d)", &nHour, &nMin, &nSec);
		CTime ThisTime = CTime(nYear, nMonth, nDay, nHour, nMin, nSec);

		if (!bFirst)
		{
			if (m_bSplitByDates && ThisTime.GetDay() != PrevTime.GetDay())
			{
				FileOut.Close();
				bFirst = TRUE;
				CString AddString=ThisTime.Format("_%B%d");
				int nPoint=out.Find('.');
				CString OutNewString = out.Left(nPoint);
				OutNewString += AddString;
				OutNewString += ".txt";
				if (!FileOut.Open(OutNewString, CFile::modeWrite | CFile::modeCreate))
				{
					MessageBox("Не могу открыть файл", "Ошибка!", MB_ICONEXCLAMATION);
					return;
				}
				FileOut.WriteString(pszHeader);
			}
			CTimeSpan DeltaTime = ThisTime - PrevTime;
			int nTotalSecs = DeltaTime.GetTotalSeconds();
			if (nTotalSecs > 0 && Var[0] > 0)
			{
				sprintf(pszBuffer, "%s\t%s", pszDate, pszTime);
				for (int i=0; i<nVars; i++)
				{
					double tmp = Var[i];
					tmp *= 60./nTotalSecs;
					Var[i] = (int)(tmp+0.5);
					sprintf(pszTmpBuffer, "\t%d", Var[i]);
					strcat(pszBuffer, pszTmpBuffer);
				}
				strcat(pszBuffer, "\n");

				FileOut.WriteString(pszBuffer);
			}
		}

		bFirst = FALSE;
		PrevTime = ThisTime;
	}
}
