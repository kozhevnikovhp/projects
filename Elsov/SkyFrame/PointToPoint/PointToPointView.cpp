// PointToPointView.cpp : implementation of the CPointToPointView class
//

#include "stdafx.h"
#include "PointToPoint.h"

#include "PointToPointDoc.h"
#include "PointToPointView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char *pszUnknown = "Unknown";
char *psz12Viterbi = "1/2";
char *psz34Viterbi = "3/4";
char *psz78Viterbi = "7/8";

BOOL ParseDataParams(CModemDataParams &DataParams, const char *pszString)
{
	char szCodeRate[256], szInfoType[256];
	if (sscanf(pszString, "%s = %ld %s", szInfoType, &DataParams.m_DataRate, szCodeRate) != 3)
		return FALSE;
	if (strcmp(szCodeRate, psz12Viterbi) == 0)
	{
		DataParams.m_CodeRate = FEC_12RViterbi;
		return TRUE;
	}
	else if (strcmp(szCodeRate, psz34Viterbi) == 0)
	{
		DataParams.m_CodeRate = FEC_34RViterbi;
		return TRUE;
	}
	else if (strcmp(szCodeRate, psz78Viterbi) == 0)
	{
		DataParams.m_CodeRate = FEC_78RViterbi;
		return TRUE;
	}

	return FALSE;
}

BOOL CCity::ParseString(const char *pszString)
{
	char szInfoType[256];
	if (sscanf(pszString, "%s", szInfoType) != 1)
		return FALSE;
	if (stricmp(szInfoType, "ModulatorIF") == 0)
	{
		return (sscanf(pszString, "%s = %ld", szInfoType, &m_ModIfParams.m_Frequency) == 2);
	}
	else if (stricmp(szInfoType, "ModulatorData") == 0)
	{
		return ParseDataParams(m_ModDataParams, pszString);
	}
	else if (stricmp(szInfoType, "DemodulatorIF") == 0)
	{
		return (sscanf(pszString, "%s = %ld", szInfoType, &m_DemIfParams.m_Frequency) == 2);
	}
	else if (stricmp(szInfoType, "DemodulatorData") == 0)
	{
		return ParseDataParams(m_DemDataParams, pszString);
	}
	else if (stricmp(szInfoType, "IP") == 0)
	{
		char szIpAddress[256];
		if (sscanf(pszString, "%s = %s", szInfoType, szIpAddress) != 2)
			return FALSE;
		m_IpAddress = StringToAddress(szIpAddress);
		return (m_IpAddress != 0);
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPointToPointView

IMPLEMENT_DYNCREATE(CPointToPointView, CFormView)

BEGIN_MESSAGE_MAP(CPointToPointView, CFormView)
	//{{AFX_MSG_MAP(CPointToPointView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CITY_TAB, OnSelChangeCityTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_CITY_TAB, OnSelChangingCityTab)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_POINT_TO_POINT_BUTTON, OnPointToPointButton)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointToPointView construction/destruction

CPointToPointView::CPointToPointView()
	: CFormView(CPointToPointView::IDD)
{
	//{{AFX_DATA_INIT(CPointToPointView)
	m_SkyFrameConfig = _T("");
	m_PointToPointConfig = _T("");
	m_CurrentConfig = _T("");
	//}}AFX_DATA_INIT
	m_pThisCity = NULL;
}

CPointToPointView::~CPointToPointView()
{
}

void CPointToPointView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointToPointView)
	DDX_Control(pDX, IDC_SF_CONFIG_EDIT, m_SkyFrameEdit);
	DDX_Control(pDX, IDC_PTOP_CONFIG_EDIT, m_PointToPointEdit);
	DDX_Control(pDX, IDC_SKY_FRAME_BUTTON, m_SkyFrameButton);
	DDX_Control(pDX, IDC_POINT_TO_POINT_BUTTON, m_PointToPointButton);
	DDX_Control(pDX, IDC_CITY_TAB, m_CityTabCtrl);
	DDX_Text(pDX, IDC_SF_CONFIG_EDIT, m_SkyFrameConfig);
	DDX_Text(pDX, IDC_PTOP_CONFIG_EDIT, m_PointToPointConfig);
	DDX_Text(pDX, IDC_CURRENT_CONFIG_EDIT, m_CurrentConfig);
	//}}AFX_DATA_MAP
}

BOOL CPointToPointView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CPointToPointView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CStdioFile file;
	if (!file.Open("Config.txt", CFile::modeRead))
	{
		MessageBox("Нет файла конфигурации - выполнение программы невозможно!", "Ошибка!", MB_ICONSTOP);
		return;
	}
	char szString[256];
	while (file.ReadString(szString, sizeof(szString)))
	{
		for (unsigned int i = 0; i < strlen(szString); i++)
			if (szString[i] == 0x0A || szString[i] == 0x0D)
				szString[i] = 0;
		if (!strlen(szString))
			continue;
		
		CCity *pCity = new CCity();
		strcpy(pCity->m_szName, szString);

		for (i = 0; i < 5; i++)
		{
			if (!file.ReadString(szString, sizeof(szString)))
			{
				MessageBox("Недостаточно информации - проверьте файл конфигурации", "Ошибка!", MB_ICONSTOP);
				return;
			}
			if (!pCity->ParseString(szString))
			{
				CString str = "Ошибка в файле конфигурации:\n";
				str += szString;
				MessageBox(str, "Ошибка!", MB_ICONSTOP);
				return;
			}
		}
		if (m_pThisCity == NULL)
			m_pThisCity = pCity;
		
		TCITEM tcItem;
		tcItem.mask = TCIF_TEXT | TCIF_PARAM;
		tcItem.pszText = pCity->m_szName;
		tcItem.lParam = (DWORD)pCity;
		m_CityTabCtrl.InsertItem(m_CityTabCtrl.GetItemCount(), &tcItem);
	}
	m_CityTabCtrl.SetCurSel(0);
	InitCurrentTab();
}

char *GetCodeRateString(FEC_CodeRate CodeRate)
{
	switch (CodeRate)
	{
	case FEC_12RViterbi:
		return psz12Viterbi;
	case FEC_34RViterbi:
		return psz34Viterbi;
	case FEC_78RViterbi:
		return psz78Viterbi;
	}
	return pszUnknown;
}

void FormatConfig
(
	CString &str,
	CModIfParams &ModIf,
	CModemDataParams &ModData,
	CDemIfParams &DemIf,
	CModemDataParams &DemData
)
{
	str.Format("Модулятор:\r\n  Частота %ld КГц\r\n  Скорость %ld Бит/сек\r\n  Кодирование %s\r\nДемодулятор:\r\n  Частота %ld КГц\r\n  Скорость %ld Бит/сек\r\n  Кодирование %s\r\n",
		ModIf.m_Frequency,
		ModData.m_DataRate, GetCodeRateString(ModData.m_CodeRate),
		DemIf.m_Frequency,
		DemData.m_DataRate, GetCodeRateString(DemData.m_CodeRate));
}

void CPointToPointView::InitCurrentTab()
{
	int index = m_CityTabCtrl.GetCurSel();
	if (index == -1)
		return;

	CCity *pCity = GetCity(index);
	if (!pCity)
		return;
	if (!m_pThisCity)
		return;

	FormatConfig(m_SkyFrameConfig, pCity->m_ModIfParams, pCity->m_ModDataParams, pCity->m_DemIfParams, pCity->m_DemDataParams);
	FormatConfig(m_PointToPointConfig, pCity->m_ModIfParams, pCity->m_ModDataParams, m_pThisCity->m_DemIfParams, m_pThisCity->m_DemDataParams);

	BOOL bThisCity = (pCity == m_pThisCity);
	m_PointToPointButton.EnableWindow(!bThisCity);
	m_PointToPointEdit.EnableWindow(!bThisCity);
	m_SkyFrameButton.EnableWindow(!bThisCity);
	m_SkyFrameEdit.EnableWindow(!bThisCity);

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CPointToPointView printing

BOOL CPointToPointView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPointToPointView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPointToPointView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CPointToPointView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CPointToPointView diagnostics

#ifdef _DEBUG
void CPointToPointView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPointToPointView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPointToPointDoc* CPointToPointView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPointToPointDoc)));
	return (CPointToPointDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPointToPointView message handlers

void CPointToPointView::OnSelChangeCityTab(NMHDR *pNMHDR, LRESULT *pResult) 
{
	InitCurrentTab();
	*pResult = 0;
}

void CPointToPointView::OnSelChangingCityTab(NMHDR *pNMHDR, LRESULT *pResult) 
{
	*pResult = 0;
}

void CPointToPointView::OnDestroy() 
{
	for (int i = 0; i < m_CityTabCtrl.GetItemCount(); i++)
	{
		CCity *pCity = GetCity(i);
		delete pCity;
	}
	CFormView::OnDestroy();
}

CCity *CPointToPointView::GetCity(int index)
{
	TCITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	m_CityTabCtrl.GetItem(index, &tcItem);
	CCity *pCity = (CCity *)tcItem.lParam;
	return pCity;
}


void CPointToPointView::OnPointToPointButton() 
{
	
}
