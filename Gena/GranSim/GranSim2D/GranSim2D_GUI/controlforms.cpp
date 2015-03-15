/* ControlForms.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	17 Jul 2008 - initial creation
	15 Sep 2008 - line form
	23 Sep 2008 - object common form
	16 Oct 2008 - splitted "object common form" and "geometry object common form"; Particle size and shape form
	17 Oct 2008 - particle type material form introduced
	10 Nov 2008 - particle type (size and shape)
	13 Nov 2008 - material constants form
	14 Nov 2008 - particle source intensity form
	15 Nov 2008 - particle type material form, reflecting object material form, particle types for sources implemented
	04 Jan 2009 - rectangle geometry form
	25 Aug 2009 - Simulation settings form
*/

#include "stdafx.h"
#include "ControlForms.h"
#include "GranSim2DRenderer.h"
#include "GranSim2DProjectView.h"
#include "..\GuiAPI.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////
// CParticleSizeRenderer

CParticleSizeRenderer::CParticleSizeRenderer()
{
	m_pType = NULL;
}

//virtual
CParticleSizeRenderer::~CParticleSizeRenderer()
{
}

//virtual
void CParticleSizeRenderer::RenderCurves()
{
	if (!m_pType)
		return;
	glColor4ubv(ucBrightRedColor);
	switch (m_pType->m_SizeType)
	{
	case PARTICLE_SIZE_FIXED:
		RenderFixedSize();
		break;
	case PARTICLE_SIZE_UNIFORM:
		RenderUniformDistribution();
		break;
	case PARTICLE_SIZE_GAUSS:
		RenderNormalDistribution();
		break;
	default:
		ASSERT(0);
		break;
	}
}

void CParticleSizeRenderer::RenderFixedSize()
{
	glBegin(GL_LINES);
		glVertex2d(GetMinXValue(), 0);
		glVertex2d(GetMaxXValue(), 0);
		glVertex2d(m_pType->m_SizeFixed, 0);
		glVertex2d(m_pType->m_SizeFixed, 1);
	glEnd();
}

void CParticleSizeRenderer::RenderUniformDistribution()
{
	double MaxDensity = m_pType->CalcMaxDistributionDensity();
	glBegin(GL_LINE_STRIP);
		glVertex2d(GetMinXValue(), 0);
		glVertex2d(m_pType->m_SizeFrom, 0);
		glVertex2d(m_pType->m_SizeFrom, MaxDensity);
		glVertex2d(m_pType->m_SizeTo, MaxDensity);
		glVertex2d(m_pType->m_SizeTo, 0);
		glVertex2d(GetMaxXValue(), 0);
	glEnd();
}

void CParticleSizeRenderer::RenderNormalDistribution()
{
	double step = GetWorldSizeX()/50;

	glBegin(GL_LINE_STRIP);
		double x = GetMinXValue();
		while (x <= GetMaxXValue())
		{
			double y = Gauss(x, m_pType->m_SizeMedian, m_pType->m_SizeSigma);
			glVertex2d(x, y);
			x += step;
		}
	glEnd();
}

//virtual
double CParticleSizeRenderer::GetMinXValue()
{
	if (m_pType)
	{
		switch (m_pType->m_SizeType)
		{
		case PARTICLE_SIZE_FIXED:
			return 0;
		case PARTICLE_SIZE_UNIFORM:
			return m_pType->m_SizeFrom - 0.2*(m_pType->m_SizeTo - m_pType->m_SizeFrom);
		case PARTICLE_SIZE_GAUSS:
			return max(0, m_pType->m_SizeMedian - 4*m_pType->m_SizeSigma);
		default:
			ASSERT(0);
			break;
		}
	}
	return 0;
}

//virtual
double CParticleSizeRenderer::GetMaxXValue()
{
	if (m_pType)
	{
		switch (m_pType->m_SizeType)
		{
		case PARTICLE_SIZE_FIXED:
			return m_pType->m_SizeFixed*2;
		case PARTICLE_SIZE_UNIFORM:
			return m_pType->m_SizeTo + 0.2*(m_pType->m_SizeTo - m_pType->m_SizeFrom);
		case PARTICLE_SIZE_GAUSS:
			return m_pType->m_SizeMedian + 4*m_pType->m_SizeSigma;
		default:
			ASSERT(0);
			break;
		}
	}
	return 1;
}

//virtual
double CParticleSizeRenderer::GetMinYValue()
{
	return -0.2*GetMaxYValue();
}

//virtual
double CParticleSizeRenderer::GetMaxYValue()
{
	if (m_pType)
		return 1.2*m_pType->CalcMaxDistributionDensity();
	return 1;
}

///////////////////////////////////////////////////////////////////////////
// CParticleShapeRenderer

CParticleShapeRenderer::CParticleShapeRenderer()
{
	m_pType = NULL;
}

//virtual
CParticleShapeRenderer::~CParticleShapeRenderer()
{
}

//virtual
void CParticleShapeRenderer::RenderCurves()
{
	if (!m_pType)
		return;
	FineRenderParticleType(m_pType);
}

//virtual
double CParticleShapeRenderer::GetMinXValue()
{
	return -GetMaxXValue();
}

//virtual
double CParticleShapeRenderer::GetMaxXValue()
{
	return 0.6;
}

//virtual
double CParticleShapeRenderer::GetMinYValue()
{
	return -GetMaxYValue();
}

//virtual
double CParticleShapeRenderer::GetMaxYValue()
{
	return 0.6;
}

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm

IMPLEMENT_DYNCREATE(CAbstractForm, CFormView)

CAbstractForm::CAbstractForm()
	: CFormView(IDD_ABSTRACT_FORM)
{
	//{{AFX_DATA_INIT(CAbstractForm)
	//}}AFX_DATA_INIT
	m_pAttached = NULL;
	m_bModified = LOGICAL_FALSE;
	m_pProjectView = NULL;
	m_pDesign = NULL;
}

CAbstractForm::CAbstractForm(UINT nIDTemplate)
	: CFormView(nIDTemplate)
{
	m_pAttached = NULL;
	m_bModified = LOGICAL_FALSE;
	m_pProjectView = NULL;
	m_pDesign = NULL;
}

CAbstractForm::~CAbstractForm()
{
}

void CAbstractForm::DoDataExchange(CDataExchange *pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbstractForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbstractForm, CFormView)
	//{{AFX_MSG_MAP(CAbstractForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
LOGICAL CAbstractForm::CanLeave()
{
	if (!IsModified())
		return LOGICAL_TRUE;
	
	int ID = MessageBox("Do you want to save the changes (press YES), or discard them (press NO), or continue editing (press CANCEL)?",
		"Attention", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (ID)
	{
	case IDYES:
		return LOGICAL_TRUE;
		break;
	case IDNO:
		ClearModifiedFlag(); // prevent from saving
		return LOGICAL_TRUE;
		break;
	case IDCANCEL:
		return LOGICAL_FALSE;
		break;
	}
	
	return LOGICAL_FALSE;
}

void CAbstractForm::ApplyButtonPressed(CGeometry2D *pGeometry)
{
	g_pGui->DesignChanged();
	m_pRenderer->UpdateExtents();
	m_pRenderer->RebuildDisplayList(pGeometry);
	m_pRenderer->Redraw();
	Entering();
}

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm diagnostics


#ifdef _DEBUG
void CAbstractForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CAbstractForm::Dump(CDumpContext &dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm message handlers

/////////////////////////////////////////////////////////////////////////////
// CSimSettingsForm

IMPLEMENT_DYNCREATE(CSimSettingsForm, CAbstractForm)

CSimSettingsForm::CSimSettingsForm()
	: CAbstractForm(IDD_SIM_SETTINGS_FORM)
{
	//{{AFX_DATA_INIT(CSimSettingsForm)
	m_Time = 0.0;
	m_SimMaxTime = 0.0;
	//}}AFX_DATA_INIT
	m_pDesign = NULL;
}

CSimSettingsForm::~CSimSettingsForm()
{
}

void CSimSettingsForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimSettingsForm)
	DDX_Text(pDX, IDC_TIME_EDIT, m_SimMaxTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSimSettingsForm, CAbstractForm)
	//{{AFX_MSG_MAP(CSimSettingsForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
LOGICAL CSimSettingsForm::Entering()
{
	m_pDesign = m_pAttached->m_pDesign;

	m_SimMaxTime = m_pDesign->GetSimulateUntilTime();

	UINT ID = IDC_TRAPEZOID_RADIO;
	IntergationMethodEnum method = m_pDesign->GetIntegrationMethod();

	switch(method)
	{
	case IMPLICIT_EULER:
		ID = IDC_IMPLICIT_EULER_RADIO;
		break;
	case VELOCITY_VERLET:
		ID = IDC_VERLET_RADIO;
		break;
	case TRAPEZOID:
		ID = IDC_TRAPEZOID_RADIO;
		break;
	case PREDICTOR_CORRECTOR:
		ID = IDC_PREDICTOR_CORRECTOR_RADIO;
		break;
	case RUNGE_KUTTA_2:
		ID = IDC_RUNGE_KUTTA2_RADIO;
		break;
	case RUNGE_KUTTA_4:
		ID = IDC_RUNGE_KUTTA4_RADIO;
		break;
	}
	CheckRadioButton(IDC_IMPLICIT_EULER_RADIO, IDC_RUNGE_KUTTA4_RADIO, ID);
	UpdateData(FALSE);
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CSimSettingsForm::Leaving()
{
	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSimSettingsForm diagnostics

#ifdef _DEBUG
void CSimSettingsForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CSimSettingsForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSimSettingsForm message handlers

void CSimSettingsForm::OnApplyButton() 
{
	UpdateData();

	m_pDesign->SetSimulateUntilTime(+m_SimMaxTime);
	
	UINT ID = GetCheckedRadioButton(IDC_IMPLICIT_EULER_RADIO, IDC_RUNGE_KUTTA4_RADIO);
	IntergationMethodEnum method = UNKNOWN_TIME_INTEGRATION_METHOD;
	switch(ID)
	{
	case IDC_IMPLICIT_EULER_RADIO:
		method = IMPLICIT_EULER;
		break;
	case IDC_VERLET_RADIO:
		method = VELOCITY_VERLET;
		break;
	case IDC_TRAPEZOID_RADIO:
		method = TRAPEZOID;
		break;
	case IDC_PREDICTOR_CORRECTOR_RADIO:
		method = PREDICTOR_CORRECTOR;
		break;
	case IDC_RUNGE_KUTTA2_RADIO:
		method = RUNGE_KUTTA_2;
		break;
	case IDC_RUNGE_KUTTA4_RADIO:
		method = RUNGE_KUTTA_4;
		break;
	}
	m_pDesign->SetIntegrationMethod(method);
	
	ClearModifiedFlag();
	Entering();
}

void CSimSettingsForm::OnCancelButton() 
{
	Entering();
}

/////////////////////////////////////////////////////////////////////////////
// CObjectCommonForm

IMPLEMENT_DYNCREATE(CObjectCommonForm, CAbstractForm)

CObjectCommonForm::CObjectCommonForm()
	: CAbstractForm(IDD_OBJECT_COMMON_FORM)
{
	//{{AFX_DATA_INIT(CObjectCommonForm)
	m_GivenName = _T("");
	//}}AFX_DATA_INIT
	m_pObject = NULL;
}

CObjectCommonForm::~CObjectCommonForm()
{
}

void CObjectCommonForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectCommonForm)
	DDX_Text(pDX, IDC_GIVEN_NAME_EDIT, m_GivenName);
	DDV_MaxChars(pDX, m_GivenName, 128);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CObjectCommonForm, CAbstractForm)
	//{{AFX_MSG_MAP(CObjectCommonForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
LOGICAL CObjectCommonForm::Entering()
{
	m_pObject = (CGranSim2DObject *)m_pAttached->m_pObject;
	m_GivenName = m_pObject->GetGivenName();
	UpdateData(FALSE);
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CObjectCommonForm::Leaving()
{
	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectCommonForm diagnostics

#ifdef _DEBUG
void CObjectCommonForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CObjectCommonForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjectCommonForm message handlers

void CObjectCommonForm::OnApplyButton() 
{
	UpdateData();
	m_pObject->SetGivenName(m_GivenName);
	m_pProjectView->UpdateSelectedItem();
	ClearModifiedFlag();
	Entering();
}

void CObjectCommonForm::OnCancelButton() 
{
	Entering();
}



/////////////////////////////////////////////////////////////////////////////
// CGeometryObjectCommonForm

IMPLEMENT_DYNCREATE(CGeometryObjectCommonForm, CAbstractForm)

CGeometryObjectCommonForm::CGeometryObjectCommonForm()
	: CAbstractForm(IDD_GEOMETRY_OBJECT_COMMON_FORM)
{
	//{{AFX_DATA_INIT(CGeometryObjectCommonForm)
	m_GivenName = _T("");
	//}}AFX_DATA_INIT
	m_pObject = NULL;
}

CGeometryObjectCommonForm::~CGeometryObjectCommonForm()
{
}

void CGeometryObjectCommonForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeometryObjectCommonForm)
	DDX_Text(pDX, IDC_GIVEN_NAME_EDIT, m_GivenName);
	DDV_MaxChars(pDX, m_GivenName, 128);
	DDX_Control(pDX, IDC_COLOR_BUTTON, m_ColorButton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGeometryObjectCommonForm, CAbstractForm)
	//{{AFX_MSG_MAP(CGeometryObjectCommonForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_BN_CLICKED(IDC_COLOR_BUTTON, OnColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
LOGICAL CGeometryObjectCommonForm::Entering()
{
	m_pObject = (CGeometry2DObject *)m_pAttached->m_pObject;
	m_GivenName = m_pObject->GetGivenName();
	m_ColorButton.SetColor(RGBAtoCOLORREF(m_pObject->GetRGBA()));
	m_Color.Init(m_pObject->GetRGBA());
	UpdateData(FALSE);
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CGeometryObjectCommonForm::Leaving()
{
	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGeometryObjectCommonForm diagnostics

#ifdef _DEBUG
void CGeometryObjectCommonForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CGeometryObjectCommonForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGeometryObjectCommonForm message handlers

void CGeometryObjectCommonForm::OnApplyButton() 
{
	UpdateData();
	m_pObject->SetGivenName(m_GivenName);
	m_pObject->SetColor(&m_Color);
	m_pRenderer->RebuildDisplayList(m_pObject->GetGeometry());
	m_pRenderer->Redraw();
	m_pProjectView->UpdateSelectedItem();
	ClearModifiedFlag();
	Entering();
}

void CGeometryObjectCommonForm::OnCancelButton() 
{
	Entering();
}

void CGeometryObjectCommonForm::OnColorButton() 
{
	m_ColorButton.ChangeColor(&m_Color);
}


/////////////////////////////////////////////////////////////////////////////
// CLineGeometryForm

IMPLEMENT_DYNCREATE(CLineGeometryForm, CAbstractForm)

CLineGeometryForm::CLineGeometryForm()
	: CAbstractForm(IDD_LINE_GEOMETRY_FORM)
{
	//{{AFX_DATA_INIT(CLineGeometryForm)
	m_X1 = 0.0;
	m_X2 = 0.0;
	m_Y1 = 0.0;
	m_Y2 = 0.0;
	m_XCenter = 0.0;
	m_YCenter = 0.0;
	m_Length = 0.0;
	m_Angle = 0.0;
	//}}AFX_DATA_INIT
}

CLineGeometryForm::~CLineGeometryForm()
{
}

void CLineGeometryForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLineGeometryForm)
	DDX_Text(pDX, IDC_X1_EDIT, m_X1);
	DDX_Text(pDX, IDC_X2_EDIT, m_X2);
	DDX_Text(pDX, IDC_Y1_EDIT, m_Y1);
	DDX_Text(pDX, IDC_Y2_EDIT, m_Y2);
	DDX_Text(pDX, IDC_XCENTER_EDIT, m_XCenter);
	DDX_Text(pDX, IDC_YCENTER_EDIT, m_YCenter);
	DDX_Text(pDX, IDC_LENGTH_EDIT, m_Length);
	DDX_Text(pDX, IDC_ANGLE_EDIT, m_Angle);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLineGeometryForm, CAbstractForm)
	//{{AFX_MSG_MAP(CLineGeometryForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_EN_CHANGE(IDC_X1_EDIT, OnChangeX1Edit)
	ON_EN_CHANGE(IDC_X2_EDIT, OnChangeX2Edit)
	ON_EN_CHANGE(IDC_XCENTER_EDIT, OnChangeXcenterEdit)
	ON_EN_CHANGE(IDC_Y1_EDIT, OnChangeY1Edit)
	ON_EN_CHANGE(IDC_Y2_EDIT, OnChangeY2Edit)
	ON_EN_CHANGE(IDC_YCENTER_EDIT, OnChangeYcenterEdit)
	ON_EN_CHANGE(IDC_LENGTH_EDIT, OnChangeLengthEdit)
	ON_EN_CHANGE(IDC_ANGLE_EDIT, OnChangeAngleEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
LOGICAL CLineGeometryForm::Entering()
{
	CGeometry2D *pGeometry = ((CGeometry2DObject *)m_pAttached->m_pObject)->GetGeometry();
	m_pLine = (CLine2D *)pGeometry;
	m_bPoint1Changed = m_bPoint2Changed = m_bCenterChanged = m_bLengthAngleChanged = LOGICAL_FALSE;
	OnDesignChanged();
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CLineGeometryForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual
void CLineGeometryForm::OnDesignChanged()
{
	m_X1 = m_pLine->X1();
	m_Y1 = m_pLine->Y1();
	m_X2 = m_pLine->X2();
	m_Y2 = m_pLine->Y2();
	m_XCenter = m_pLine->XCenter();
	m_YCenter = m_pLine->YCenter();
	m_Length = m_pLine->GetLength();
	m_Angle = m_pLine->GetAngleDegrees();
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CLineGeometryForm diagnostics

#ifdef _DEBUG
void CLineGeometryForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CLineGeometryForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLineGeometryForm message handlers

void CLineGeometryForm::OnApplyButton() 
{
	UpdateData(TRUE);
	LOGICAL bSomethingChanged = LOGICAL_FALSE;
	if (m_bPoint1Changed || m_bPoint2Changed)
	{
		m_pLine->SetPoint1(m_X1, m_Y1);
		m_pLine->SetPoint2(m_X2, m_Y2);
		bSomethingChanged = LOGICAL_TRUE;
	}
	else if (m_bCenterChanged || m_bLengthAngleChanged)
	{
		double fShiftX = m_XCenter - m_pLine->XCenter();
		double fShiftY = m_YCenter - m_pLine->YCenter();
		m_pLine->Move(fShiftX, fShiftY);
		m_pLine->SetLengthAngleDegrees(m_Length, m_Angle);
		bSomethingChanged = LOGICAL_TRUE;
	}

	if (bSomethingChanged)
		ApplyButtonPressed(m_pLine);
}

void CLineGeometryForm::OnCancelButton() 
{
	Entering();	
}

void CLineGeometryForm::OnChangeX1Edit() 
{
	m_bPoint1Changed = LOGICAL_TRUE;
}

void CLineGeometryForm::OnChangeX2Edit() 
{
	m_bPoint2Changed = LOGICAL_TRUE;
}

void CLineGeometryForm::OnChangeXcenterEdit() 
{
	m_bCenterChanged = LOGICAL_TRUE;
}

void CLineGeometryForm::OnChangeY1Edit() 
{
	m_bPoint1Changed = LOGICAL_TRUE;
}

void CLineGeometryForm::OnChangeY2Edit() 
{
	m_bPoint2Changed = LOGICAL_TRUE;
}

void CLineGeometryForm::OnChangeYcenterEdit() 
{
	m_bCenterChanged = LOGICAL_TRUE;
}


void CLineGeometryForm::OnChangeLengthEdit() 
{
	m_bLengthAngleChanged = LOGICAL_TRUE;
}

void CLineGeometryForm::OnChangeAngleEdit() 
{
	m_bLengthAngleChanged = LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPointGeometryForm

IMPLEMENT_DYNCREATE(CPointGeometryForm, CAbstractForm)

CPointGeometryForm::CPointGeometryForm()
	: CAbstractForm(IDD_POINT_GEOMETRY_FORM)
{
	//{{AFX_DATA_INIT(CPointGeometryForm)
	m_X = 0.0;
	m_Y = 0.0;
	//}}AFX_DATA_INIT
	m_pPoint = NULL;
}

CPointGeometryForm::~CPointGeometryForm()
{
}

void CPointGeometryForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointGeometryForm)
	DDX_Text(pDX, IDC_X_EDIT, m_X);
	DDX_Text(pDX, IDC_Y_EDIT, m_Y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPointGeometryForm, CAbstractForm)
	//{{AFX_MSG_MAP(CPointGeometryForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_EN_CHANGE(IDC_X_EDIT, OnChangeXEdit)
	ON_EN_CHANGE(IDC_Y_EDIT, OnChangeYEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
LOGICAL CPointGeometryForm::Entering()
{
	CGeometry2D *pGeometry = ((CGeometry2DObject *)m_pAttached->m_pObject)->GetGeometry();
	m_pPoint = (CPoint2D *)pGeometry;
	m_bChanged = LOGICAL_FALSE;
	OnDesignChanged();
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CPointGeometryForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual
void CPointGeometryForm::OnDesignChanged()
{
	m_X = m_pPoint->X();
	m_Y = m_pPoint->Y();
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CPointGeometryForm diagnostics

#ifdef _DEBUG
void CPointGeometryForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CPointGeometryForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPointGeometryForm message handlers

void CPointGeometryForm::OnApplyButton() 
{
	if (m_bChanged)
	{
		UpdateData(TRUE);
		m_pPoint->Set(m_X, m_Y);
		ApplyButtonPressed(m_pPoint);
	}
}

void CPointGeometryForm::OnCancelButton() 
{
	Entering();	
}

void CPointGeometryForm::OnChangeXEdit() 
{
	m_bChanged = LOGICAL_TRUE;
}

void CPointGeometryForm::OnChangeYEdit() 
{
	m_bChanged = LOGICAL_TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMaterialConstantsForm

IMPLEMENT_DYNCREATE(CMaterialConstantsForm, CAbstractForm)

CMaterialConstantsForm::CMaterialConstantsForm()
	: CAbstractForm(IDD_MATERIAL_CONSTANTS_FORM)
{
	//{{AFX_DATA_INIT(CMaterialConstantsForm)
	m_Nu = 0.0;
	m_E = 0.0;
	m_Ro = 0.0;
	//}}AFX_DATA_INIT
}

CMaterialConstantsForm::~CMaterialConstantsForm()
{
}

//virtual
LOGICAL CMaterialConstantsForm::Entering()
{
	m_pMaterial = (CMaterial *)m_pAttached->m_pObject;

	m_Nu = m_pMaterial->m_Nu;
	m_E = m_pMaterial->m_E/1.E+6; // Pa -> MPa
	m_Ro = m_pMaterial->m_Ro;
	UpdateData(FALSE);

	return LOGICAL_TRUE;
}

//virtual
LOGICAL CMaterialConstantsForm::Leaving()
{
	return LOGICAL_TRUE;
}

void CMaterialConstantsForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMaterialConstantsForm)
	DDX_Text(pDX, IDC_NU_EDIT, m_Nu);
	DDX_Text(pDX, IDC_E_EDIT, m_E);
	DDX_Text(pDX, IDC_RO_EDIT, m_Ro);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMaterialConstantsForm, CAbstractForm)
	//{{AFX_MSG_MAP(CMaterialConstantsForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMaterialConstantsForm diagnostics

#ifdef _DEBUG
void CMaterialConstantsForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CMaterialConstantsForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMaterialConstantsForm message handlers

void CMaterialConstantsForm::OnApplyButton() 
{
	UpdateData();
	m_pMaterial->m_Nu = m_Nu;
	m_pMaterial->m_E = m_E*1.E+6; // MPa -> Pa;
	m_pMaterial->m_Ro = m_Ro;
	g_pGui->DesignChanged();
}

void CMaterialConstantsForm::OnCancelButton() 
{
	Entering();
}

/////////////////////////////////////////////////////////////////////////////
// CParticleSizeOpenGLWnd

CParticleSizeOpenGLWnd::CParticleSizeOpenGLWnd()
{
}

//virtual
CParticleSizeOpenGLWnd::~CParticleSizeOpenGLWnd()
{
}

//inline
void CParticleSizeOpenGLWnd::SetParticleType(CParticleType *pType)
{
	m_pRenderer->SetParticleType(pType);
}

BEGIN_MESSAGE_MAP(CParticleSizeOpenGLWnd, COpenGLStatic)
	//{{AFX_MSG_MAP(CParticleSizeOpenGLWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
CRenderer *CParticleSizeOpenGLWnd::GetRenderer()
{
	return m_pRenderer;
}

//virtual
void CParticleSizeOpenGLWnd::CreateRenderer()
{
	m_pRenderer = new CParticleSizeRenderer;
}

//virtual
void CParticleSizeOpenGLWnd::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
}


/////////////////////////////////////////////////////////////////////////////
// CParticleSizeForm

IMPLEMENT_DYNCREATE(CParticleSizeForm, CAbstractForm)

CParticleSizeForm::CParticleSizeForm()
	: CAbstractForm(IDD_PARTICLE_SIZE_FORM)
{
	//{{AFX_DATA_INIT(CParticleSizeForm)
	m_FixedSize = 0.0;
	m_UniformFrom = 0.0;
	m_UniformTo = 0.0;
	m_NormalMedian = 0.0;
	m_NormalSigma = 0.0;
	//}}AFX_DATA_INIT
}

CParticleSizeForm::~CParticleSizeForm()
{
}

//virtual
LOGICAL CParticleSizeForm::Entering()
{
	m_pType = (CParticleType *)m_pAttached->m_pObject;
	m_SizeCtrl.SetParticleType(m_pType);

	UINT ID = IDC_FIXED_SIZE_RADIO;
	switch (m_pType->m_SizeType)
	{
	case PARTICLE_SIZE_FIXED:
		ID = IDC_FIXED_SIZE_RADIO;
		OnFixedSizeRadio();
		break;
	case PARTICLE_SIZE_UNIFORM:
		ID = IDC_UNIFORM_SIZE_RADIO;
		OnUniformSizeRadio();
		break;
	case PARTICLE_SIZE_GAUSS:
		ID = IDC_NORMAL_SIZE_RADIO;
		OnNormalSizeRadio();
		break;
	default:
		break;
	}
	CheckRadioButton(IDC_FIXED_SIZE_RADIO, IDC_NORMAL_SIZE_RADIO, ID);

	m_FixedSize = m_pType->m_SizeFixed;
	m_UniformFrom = m_pType->m_SizeFrom;
	m_UniformTo = m_pType->m_SizeTo;
	m_NormalMedian = m_pType->m_SizeMedian;
	m_NormalSigma = m_pType->m_SizeSigma;
	UpdateData(FALSE);

	return LOGICAL_TRUE;
}

//virtual
LOGICAL CParticleSizeForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual 
void CParticleSizeForm::OnInitialUpdate()
{
	CAbstractForm::OnInitialUpdate();
	m_SizeCtrl.Setup();
}

void CParticleSizeForm::EnableFixedSizeCtrls(BOOL bEnable)
{
	m_FixedSizeCtrl.EnableWindow(bEnable);
}

void CParticleSizeForm::EnableUniformSizeCtrls(BOOL bEnable)
{
	m_UniformFromCtrl.EnableWindow(bEnable);
	m_UniformToCtrl.EnableWindow(bEnable);
}

void CParticleSizeForm::EnableNormalSizeCtrls(BOOL bEnable)
{
	m_NormalMedianCtrl.EnableWindow(bEnable);
	m_NormalSigmaCtrl.EnableWindow(bEnable);
}

void CParticleSizeForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticleSizeForm)
	DDX_Control(pDX, IDC_NORMAL_SIGMA_EDIT, m_NormalSigmaCtrl);
	DDX_Control(pDX, IDC_NORMAL_MEDIAN_EDIT, m_NormalMedianCtrl);
	DDX_Control(pDX, IDC_UNIFORM_TO_EDIT, m_UniformToCtrl);
	DDX_Control(pDX, IDC_UNIFORM_FROM_EDIT, m_UniformFromCtrl);
	DDX_Control(pDX, IDC_FIXED_SIZE_EDIT, m_FixedSizeCtrl);
	DDX_Control(pDX, IDC_SIZE_STATIC, m_SizeCtrl);
	DDX_Text(pDX, IDC_FIXED_SIZE_EDIT, m_FixedSize);
	DDX_Text(pDX, IDC_UNIFORM_FROM_EDIT, m_UniformFrom);
	DDX_Text(pDX, IDC_UNIFORM_TO_EDIT, m_UniformTo);
	DDX_Text(pDX, IDC_NORMAL_MEDIAN_EDIT, m_NormalMedian);
	DDX_Text(pDX, IDC_NORMAL_SIGMA_EDIT, m_NormalSigma);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParticleSizeForm, CAbstractForm)
	//{{AFX_MSG_MAP(CParticleSizeForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_BN_CLICKED(IDC_FIXED_SIZE_RADIO, OnFixedSizeRadio)
	ON_BN_CLICKED(IDC_NORMAL_SIZE_RADIO, OnNormalSizeRadio)
	ON_BN_CLICKED(IDC_UNIFORM_SIZE_RADIO, OnUniformSizeRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleSizeForm diagnostics

#ifdef _DEBUG
void CParticleSizeForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CParticleSizeForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParticleSizeForm message handlers

void CParticleSizeForm::OnApplyButton() 
{
	UpdateData();
	m_pType->m_SizeFixed = m_FixedSize;
	m_pType->m_SizeFrom = m_UniformFrom;
	m_pType->m_SizeTo = m_UniformTo;
	m_pType->m_SizeMedian = m_NormalMedian;
	m_pType->m_SizeSigma = m_NormalSigma;

	UINT ID = GetCheckedRadioButton(IDC_FIXED_SIZE_RADIO, IDC_NORMAL_SIZE_RADIO);
	switch (ID)
	{
	case IDC_FIXED_SIZE_RADIO:
		m_pType->m_SizeType = PARTICLE_SIZE_FIXED;
		break;
	case IDC_UNIFORM_SIZE_RADIO:
		m_pType->m_SizeType = PARTICLE_SIZE_UNIFORM;
		break;
	case IDC_NORMAL_SIZE_RADIO:
		m_pType->m_SizeType = PARTICLE_SIZE_GAUSS;
		break;
	default:
		ASSERT(0);
	}
	
	g_pGui->DesignChanged();
	m_SizeCtrl.SetDefaultView();
}

void CParticleSizeForm::OnCancelButton() 
{
	Entering();
}

void CParticleSizeForm::OnFixedSizeRadio() 
{
	EnableFixedSizeCtrls(TRUE);
	EnableUniformSizeCtrls(FALSE);
	EnableNormalSizeCtrls(FALSE);
	m_pType->m_SizeType = PARTICLE_SIZE_FIXED;
	m_SizeCtrl.SetDefaultView();
}

void CParticleSizeForm::OnNormalSizeRadio() 
{
	EnableFixedSizeCtrls(FALSE);
	EnableUniformSizeCtrls(FALSE);
	EnableNormalSizeCtrls(TRUE);
	m_pType->m_SizeType = PARTICLE_SIZE_GAUSS;
	m_SizeCtrl.SetDefaultView();
}

void CParticleSizeForm::OnUniformSizeRadio() 
{
	EnableFixedSizeCtrls(FALSE);
	EnableUniformSizeCtrls(TRUE);
	EnableNormalSizeCtrls(FALSE);
	m_pType->m_SizeType = PARTICLE_SIZE_UNIFORM;
	m_SizeCtrl.SetDefaultView();
}



/////////////////////////////////////////////////////////////////////////////
// CParticleShapeOpenGLWnd

CParticleShapeOpenGLWnd::CParticleShapeOpenGLWnd()
{
}

//virtual
CParticleShapeOpenGLWnd::~CParticleShapeOpenGLWnd()
{
}

//inline
void CParticleShapeOpenGLWnd::SetParticleType(CParticleType *pType)
{
	m_pRenderer->SetParticleType(pType);
}

BEGIN_MESSAGE_MAP(CParticleShapeOpenGLWnd, COpenGLStatic)
	//{{AFX_MSG_MAP(CParticleShapeOpenGLWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
CRenderer *CParticleShapeOpenGLWnd::GetRenderer()
{
	return m_pRenderer;
}

//virtual
void CParticleShapeOpenGLWnd::CreateRenderer()
{
	m_pRenderer = new CParticleShapeRenderer;
}

//virtual
void CParticleShapeOpenGLWnd::DestroyRenderer()
{
	if (m_pRenderer)
		delete m_pRenderer;
}


/////////////////////////////////////////////////////////////////////////////
// CParticleShapeForm

IMPLEMENT_DYNCREATE(CParticleShapeForm, CAbstractForm)

CParticleShapeForm::CParticleShapeForm()
	: CAbstractForm(IDD_PARTICLE_SHAPE_FORM)
{
	//{{AFX_DATA_INIT(CParticleShapeForm)
	m_Squareness = 0;
	m_AspectRatio = 0.0;
	//}}AFX_DATA_INIT
}

CParticleShapeForm::~CParticleShapeForm()
{
}

//virtual
LOGICAL CParticleShapeForm::Entering()
{
	m_pType = (CParticleType *)m_pAttached->m_pObject;
	m_ShapeCtrl.SetParticleType(m_pType);
	m_AspectRatio = m_pType->GetShapeAspectRatio();
	m_Squareness = m_pType->GetShapeSquareness();
	UpdateData(FALSE);
	m_SquarenessSpin.SetPos(m_Squareness);
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CParticleShapeForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual 
void CParticleShapeForm::OnInitialUpdate()
{
	CAbstractForm::OnInitialUpdate();
	m_SquarenessSpin.SetBuddy(GetDescendantWindow(IDC_SQUARENESS_EDIT));
	m_SquarenessSpin.SetRange32(2, 65535);
	m_ShapeCtrl.Setup();
}

void CParticleShapeForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticleShapeForm)
	DDX_Control(pDX, IDC_SQUARENESS_SPIN, m_SquarenessSpin);
	DDX_Control(pDX, IDC_SHAPE_STATIC, m_ShapeCtrl);
	DDX_Text(pDX, IDC_SQUARENESS_EDIT, m_Squareness);
	DDV_MinMaxUInt(pDX, m_Squareness, 2, 65535);
	DDX_Text(pDX, IDC_ASPECT_RATIO_EDIT, m_AspectRatio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParticleShapeForm, CAbstractForm)
	//{{AFX_MSG_MAP(CParticleShapeForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleShapeForm diagnostics

#ifdef _DEBUG
void CParticleShapeForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CParticleShapeForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParticleShapeForm message handlers


void CParticleShapeForm::OnApplyButton() 
{
	UpdateData();
	m_pType->SetShapeAspectRatio(m_AspectRatio);
	m_pType->SetShapeSquareness(m_Squareness);
	
	m_pType->RecalcShapeVertices();
	g_pGui->DesignChanged();
	m_ShapeCtrl.SetDefaultView();
}

void CParticleShapeForm::OnCancelButton() 
{
	Entering();
}


/////////////////////////////////////////////////////////////////////
// CObjectComboBox

void CObjectComboBox::SelectObject(CGranSim2DObject *pObject)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (GetItemDataPtr(i) == pObject)
		{
			SetCurSel(i);
			break;
		}
	}
}

CGranSim2DObject *CObjectComboBox::GetSelectedObject()
{
	int index = GetCurSel();
	if (index == CB_ERR)
		return NULL;

	return (CGranSim2DObject *)GetItemDataPtr(index);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectComboBoxVisitor

//virtual
LOGICAL CObjectComboBoxVisitor::Visit(CGranSim2DObject *pObject)
{
	int index = m_pCombo->AddString(pObject->GetGivenName());
	m_pCombo->SetItemDataPtr(index, pObject);
	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CParticleMaterialForm

IMPLEMENT_DYNCREATE(CParticleMaterialForm, CAbstractForm)

CParticleMaterialForm::CParticleMaterialForm()
	: CAbstractForm(IDD_PARTICLE_MATERIAL_FORM)
{
	//{{AFX_DATA_INIT(CParticleMaterialForm)
	//}}AFX_DATA_INIT
}

CParticleMaterialForm::~CParticleMaterialForm()
{
}

//virtual
LOGICAL CParticleMaterialForm::Entering()
{
	m_pType = (CParticleType *)m_pAttached->m_pObject;
	m_MaterialCombo.ResetContent();
	
	CObjectComboBoxVisitor visitor(&m_MaterialCombo);
	m_pType->GetDesign()->VisitAllMaterials(&visitor);
	m_MaterialCombo.SelectObject(m_pType->GetMaterial());
	
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CParticleMaterialForm::Leaving()
{
	return LOGICAL_TRUE;
}

void CParticleMaterialForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticleMaterialForm)
	DDX_Control(pDX, IDC_MATERIAL_COMBO, m_MaterialCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParticleMaterialForm, CAbstractForm)
	//{{AFX_MSG_MAP(CParticleMaterialForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleMaterialForm diagnostics

#ifdef _DEBUG
void CParticleMaterialForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CParticleMaterialForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CParticleMaterialForm message handlers

void CParticleMaterialForm::OnApplyButton() 
{
	m_pType->SetMaterial((CMaterial *)m_MaterialCombo.GetSelectedObject());
	g_pGui->DesignChanged();
}

void CParticleMaterialForm::OnCancelButton() 
{
	Entering();
}


/////////////////////////////////////////////////////////////////////////////
// CParticleSourceTypeForm

IMPLEMENT_DYNCREATE(CParticleSourceTypeForm, CAbstractForm)

CParticleSourceTypeForm::CParticleSourceTypeForm()
	: CAbstractForm(IDD_PARTICLE_SOURCE_TYPE_FORM)
{
	//{{AFX_DATA_INIT(CParticleSourceTypeForm)
	//}}AFX_DATA_INIT
}

CParticleSourceTypeForm::~CParticleSourceTypeForm()
{
}

//virtual
LOGICAL CParticleSourceTypeForm::Entering()
{
	m_pSource = (CParticleSource *)m_pAttached->m_pObject;
	m_ParticleTypeCombo.ResetContent();
	
	CObjectComboBoxVisitor visitor(&m_ParticleTypeCombo);
	m_pSource->GetDesign()->VisitAllParticleTypes(&visitor);
	m_ParticleTypeCombo.SelectObject(m_pSource->GetParticleType());
	
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CParticleSourceTypeForm::Leaving()
{
	return LOGICAL_TRUE;
}

void CParticleSourceTypeForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticleSourceTypeForm)
	DDX_Control(pDX, IDC_PARTICLE_TYPE_COMBO, m_ParticleTypeCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParticleSourceTypeForm, CAbstractForm)
	//{{AFX_MSG_MAP(CParticleSourceTypeForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleSourceTypeForm diagnostics

#ifdef _DEBUG
void CParticleSourceTypeForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CParticleSourceTypeForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CParticleSourceTypeForm message handlers

void CParticleSourceTypeForm::OnApplyButton() 
{
	m_pSource->SetParticleType((CParticleType *)m_ParticleTypeCombo.GetSelectedObject());
	g_pGui->DesignChanged();
}

void CParticleSourceTypeForm::OnCancelButton() 
{
	Entering();
}


/////////////////////////////////////////////////////////////////////////////
// CParticleSourceIntensityForm

IMPLEMENT_DYNCREATE(CParticleSourceIntensityForm, CAbstractForm)

CParticleSourceIntensityForm::CParticleSourceIntensityForm()
	: CAbstractForm(IDD_PARTICLE_SOURCE_INTENSITY_FORM)
{
	//{{AFX_DATA_INIT(CParticleSourceIntensityForm)
	m_Intensity = 0.0;
	//}}AFX_DATA_INIT
}

CParticleSourceIntensityForm::~CParticleSourceIntensityForm()
{
}

//virtual
LOGICAL CParticleSourceIntensityForm::Entering()
{
	m_pSource = (CParticleSource *)m_pAttached->m_pObject;

	UINT ID = IDC_UNIFORM_FLOW_RADIO;
	switch (m_pSource->m_FlowType)
	{
	case PARTICLE_SOURCE_FLOW_TYPE_UNIFORM:
		ID = IDC_UNIFORM_FLOW_RADIO;
		break;
	case PARTICLE_SOURCE_FLOW_TYPE_POISSON:
		ID = IDC_POISSON_FLOW_RADIO;
		break;
	}
	CheckRadioButton(IDC_UNIFORM_FLOW_RADIO, IDC_POISSON_FLOW_RADIO, ID);
	m_Intensity = m_pSource->m_fIntensity;
	UpdateData(FALSE);

	return LOGICAL_TRUE;
}

//virtual
LOGICAL CParticleSourceIntensityForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual 
void CParticleSourceIntensityForm::OnInitialUpdate()
{
	CAbstractForm::OnInitialUpdate();
}

void CParticleSourceIntensityForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticleSourceIntensityForm)
	DDX_Text(pDX, IDC_INTENSITY_EDIT, m_Intensity);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CParticleSourceIntensityForm, CAbstractForm)
	//{{AFX_MSG_MAP(CParticleSourceIntensityForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleSourceIntensityForm diagnostics

#ifdef _DEBUG
void CParticleSourceIntensityForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CParticleSourceIntensityForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParticleSourceIntensityForm message handlers

void CParticleSourceIntensityForm::OnApplyButton() 
{
	UpdateData();
	m_pSource->m_fIntensity = m_Intensity;
	UINT ID = GetCheckedRadioButton(IDC_UNIFORM_FLOW_RADIO, IDC_POISSON_FLOW_RADIO);
	switch (ID)
	{
	case IDC_UNIFORM_FLOW_RADIO:
		m_pSource->m_FlowType = PARTICLE_SOURCE_FLOW_TYPE_UNIFORM;
		break;
	case IDC_POISSON_FLOW_RADIO:
		m_pSource->m_FlowType = PARTICLE_SOURCE_FLOW_TYPE_POISSON;
		break;
	default:
		ASSERT(0);
	}
	g_pGui->DesignChanged();
}

void CParticleSourceIntensityForm::OnCancelButton() 
{
	Entering();
}


/////////////////////////////////////////////////////////////////////////////
// CReflectingObjectMaterialForm

IMPLEMENT_DYNCREATE(CReflectingObjectMaterialForm, CAbstractForm)

CReflectingObjectMaterialForm::CReflectingObjectMaterialForm()
	: CAbstractForm(IDD_REFLECTING_OBJECT_MATERIAL_FORM)
{
	//{{AFX_DATA_INIT(CReflectingObjectMaterialForm)
	//}}AFX_DATA_INIT
}

CReflectingObjectMaterialForm::~CReflectingObjectMaterialForm()
{
}

//virtual
LOGICAL CReflectingObjectMaterialForm::Entering()
{
	m_pReflectingObject = (CReflectingObject *)m_pAttached->m_pObject;
	m_MaterialCombo.ResetContent();
	
	CMaterial *pMaterial = m_pReflectingObject->m_pMaterial;
	CObjectComboBoxVisitor visitor(&m_MaterialCombo);
	m_pReflectingObject->GetDesign()->VisitAllMaterials(&visitor);
	m_MaterialCombo.SelectObject(pMaterial);
	
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CReflectingObjectMaterialForm::Leaving()
{
	return LOGICAL_TRUE;
}

void CReflectingObjectMaterialForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReflectingObjectMaterialForm)
	DDX_Control(pDX, IDC_MATERIAL_COMBO, m_MaterialCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReflectingObjectMaterialForm, CAbstractForm)
	//{{AFX_MSG_MAP(CReflectingObjectMaterialForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReflectingObjectMaterialForm diagnostics

#ifdef _DEBUG
void CReflectingObjectMaterialForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CReflectingObjectMaterialForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CReflectingObjectMaterialForm message handlers

void CReflectingObjectMaterialForm::OnApplyButton() 
{
	m_pReflectingObject->m_pMaterial = (CMaterial *)m_MaterialCombo.GetSelectedObject();
	g_pGui->DesignChanged();
}

void CReflectingObjectMaterialForm::OnCancelButton() 
{
	Entering();
}


/////////////////////////////////////////////////////////////////////////////
// CRectangleGeometryForm

IMPLEMENT_DYNCREATE(CRectangleGeometryForm, CAbstractForm)

CRectangleGeometryForm::CRectangleGeometryForm()
	: CAbstractForm(IDD_RECTANGLE_GEOMETRY_FORM)
{
	//{{AFX_DATA_INIT(CRectangleGeometryForm)
	m_XCenter = 0.0;
	m_YCenter = 0.0;
	m_Height = 0.0;
	m_Width = 0.0;
	//}}AFX_DATA_INIT
}

CRectangleGeometryForm::~CRectangleGeometryForm()
{
}

//virtual
LOGICAL CRectangleGeometryForm::Entering()
{
	CGeometry2D *pGeometry = ((CGeometry2DObject *)m_pAttached->m_pObject)->GetGeometry();
	m_pRectangle = (CRectangle2D *)pGeometry;
	OnDesignChanged();
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CRectangleGeometryForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual
void CRectangleGeometryForm::OnDesignChanged()
{
	m_XCenter = m_pRectangle->XCenter();
	m_YCenter = m_pRectangle->YCenter();
	m_Width = m_pRectangle->Width();
	m_Height = m_pRectangle->Height();
	UpdateData(FALSE);
}

void CRectangleGeometryForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRectangleGeometryForm)
	DDX_Text(pDX, IDC_XCENTER_EDIT, m_XCenter);
	DDX_Text(pDX, IDC_YCENTER_EDIT, m_YCenter);
	DDX_Text(pDX, IDC_HEIGHT_EDIT, m_Height);
	DDX_Text(pDX, IDC_WIDTH_EDIT, m_Width);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRectangleGeometryForm, CAbstractForm)
	//{{AFX_MSG_MAP(CRectangleGeometryForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRectangleGeometryForm diagnostics

#ifdef _DEBUG
void CRectangleGeometryForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CRectangleGeometryForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRectangleGeometryForm message handlers

void CRectangleGeometryForm::OnApplyButton() 
{
	UpdateData(TRUE);
	m_pRectangle->Init(m_XCenter, m_YCenter, m_Width, m_Height);
	ApplyButtonPressed(m_pRectangle);
}

void CRectangleGeometryForm::OnCancelButton() 
{
	Entering();	
}

