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

#ifndef __CONTROL_FORMS_H_INCLUDED__
#define __CONTROL_FORMS_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Forward declaration
class CMaterial;
class CParticleType;
class CParticleSource;

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "Resource.h"
#include "Portable.h"
#include "ColorButton.h"
#include "OpenGLView.h"
#include "..\GranSim2DDesign.h"

// Forward declarations

class CAttachedBlock;
class CGranSim2DObject;
class CGeometry2DObject;
class CGranSim2DProjectView;
class CGranSim2DRenderer;
class CPoint2D;
class CLine2D;

class CParticleSizeRenderer : public COpenGL_Chart2D
{	
// Construction/destruction
public:
	CParticleSizeRenderer();
	virtual ~CParticleSizeRenderer();

// Public overridable methods
public:
	virtual void RenderCurves();

// Protected overridable methods
protected:
	virtual double GetMinXValue();
	virtual double GetMaxXValue();
	virtual double GetMinYValue();
	virtual double GetMaxYValue();
	//virtual void FormatXMark(char *pszMark, int MarkMaxLen, double value);
	virtual int GetGridFontSize() { return 14; } // in pixels
	virtual int GetVertGridMinStep() { return 50; } // in pixels
	// Event handlers

// Public methods
public:
	void SetParticleType(CParticleType *pType) { m_pType = pType; SetDefaultView(); }

// Protected methods
protected:
	void RenderFixedSize();
	void RenderUniformDistribution();
	void RenderNormalDistribution();

// Private methods
private:

// Public members
public:

// Protected members
protected:
	CParticleType *m_pType;

// Private members
private:
};

class CParticleShapeRenderer : public COpenGL_Chart2D
{	
// Construction/destruction
public:
	CParticleShapeRenderer();
	virtual ~CParticleShapeRenderer();

// Public overridable methods
public:
	virtual void RenderCurves();

// Protected overridable methods
protected:
	virtual double GetMinXValue();
	virtual double GetMaxXValue();
	virtual double GetMinYValue();
	virtual double GetMaxYValue();
	//virtual void FormatXMark(char *pszMark, int MarkMaxLen, double value);
	virtual int GetGridFontSize() { return 14; } // in pixels
	virtual int GetVertGridMinStep() { return 50; } // in pixels
	// Event handlers

// Public methods
public:
	void SetParticleType(CParticleType *pType) { m_pType = pType; SetDefaultView(); }

// Protected methods
protected:

// Private methods
private:

// Public members
public:

// Protected members
protected:
	CParticleType *m_pType;

// Private members
private:
};

class CAbstractForm : public CFormView
{
// Constructors/destructors
protected:
	CAbstractForm();           // protected constructor used by dynamic creation
	CAbstractForm(UINT nIDTemplate);
	DECLARE_DYNCREATE(CAbstractForm)

// Form Data
public:
	//{{AFX_DATA(CAbstractForm)
	//}}AFX_DATA

// Public methods
	void SetModifiedFlag() { m_bModified = LOGICAL_TRUE; }
	void ClearModifiedFlag() { m_bModified = LOGICAL_FALSE; }
	LOGICAL IsModified() { return m_bModified; }
	void SetAttachedBlock(CAttachedBlock *pAttached) { m_pAttached = pAttached; }
	void SetProjectView(CGranSim2DProjectView *pProjectView) { m_pProjectView = pProjectView; }
	void SetDesign(CGranSim2DDesign *pDesign) { m_pDesign = pDesign; }
	void SetRenderer(CGranSim2DRenderer *pRenderer) { m_pRenderer = pRenderer; }

// Public overridables
public:
	virtual LOGICAL CanBeDeleted() { return FALSE; }
	virtual LOGICAL Entering() { return TRUE; }
	virtual LOGICAL Leaving() { return TRUE; }
	virtual LOGICAL CanLeave();
	virtual void OnDesignChanged() {}

	//{{AFX_VIRTUAL(CAbstractForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAbstractForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	void ApplyButtonPressed(CGeometry2D *pGeometry);

// Public members
public:
	
// Protected members
protected:
	CGranSim2DProjectView *m_pProjectView;
	CGranSim2DDesign *m_pDesign;
	CGranSim2DRenderer *m_pRenderer;
	CTime m_LastTimerTime;
	LOGICAL m_bModified;
	CAttachedBlock *m_pAttached;
	//{{AFX_MSG(CAbstractForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
};

/////////////////////////////////////////////////////////////////////////////
// CSimSettingsForm

class CSimSettingsForm : public CAbstractForm
{
// Constructors/destructors
public:
	CSimSettingsForm();
	virtual ~CSimSettingsForm();
	DECLARE_DYNCREATE(CSimSettingsForm)

// Form Data
public:
	//{{AFX_DATA(CSimSettingsForm)
	enum { IDD = IDD_SIM_SETTINGS_FORM };
	double	m_Time;
	double	m_SimMaxTime;
	//}}AFX_DATA
	CGranSim2DDesign *m_pDesign;

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

// Protected overridables
	//{{AFX_VIRTUAL(CSimSettingsForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CSimSettingsForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CObjectCommonForm

class CObjectCommonForm : public CAbstractForm
{
// Constructors/destructors
public:
	CObjectCommonForm();
	virtual ~CObjectCommonForm();
	DECLARE_DYNCREATE(CObjectCommonForm)

// Form Data
public:
	//{{AFX_DATA(CObjectCommonForm)
	enum { IDD = IDD_OBJECT_COMMON_FORM };
	CString	m_GivenName;
	//}}AFX_DATA
	CGranSim2DObject *m_pObject;

// Public overridables
public:
	virtual LOGICAL CanBeDeleted() { return TRUE; }
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

// Protected overridables
	//{{AFX_VIRTUAL(CObjectCommonForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CObjectCommonForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CGeometryObjectCommonForm

class CGeometryObjectCommonForm : public CAbstractForm
{
// Constructors/destructors
public:
	CGeometryObjectCommonForm();
	virtual ~CGeometryObjectCommonForm();
	DECLARE_DYNCREATE(CGeometryObjectCommonForm)

// Form Data
public:
	//{{AFX_DATA(CGeometryObjectCommonForm)
	enum { IDD = IDD_GEOMETRY_OBJECT_COMMON_FORM };
	CString	m_GivenName;
	CColorButton	m_ColorButton;
	//}}AFX_DATA
	CGeometry2DObject *m_pObject;
	SRGBA m_Color;

// Public overridables
public:
	virtual LOGICAL CanBeDeleted() { return TRUE; }
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

// Protected overridables
	//{{AFX_VIRTUAL(CGeometryObjectCommonForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CGeometryObjectCommonForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	afx_msg void OnColorButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CLineGeometryForm

class CLineGeometryForm : public CAbstractForm
{
// Constructors/destructors
public:
	CLineGeometryForm();
	virtual ~CLineGeometryForm();
	DECLARE_DYNCREATE(CLineGeometryForm)

// Form Data
public:
	//{{AFX_DATA(CLineGeometryForm)
	enum { IDD = IDD_LINE_GEOMETRY_FORM };
	double	m_X1;
	double	m_X2;
	double	m_Y1;
	double	m_Y2;
	double	m_XCenter;
	double	m_YCenter;
	double	m_Length;
	double	m_Angle;
	//}}AFX_DATA

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual void OnDesignChanged();

// Protected overridables
	//{{AFX_VIRTUAL(CLineGeometryForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
	CLine2D *m_pLine;
	LOGICAL m_bPoint1Changed, m_bPoint2Changed, m_bCenterChanged, m_bLengthAngleChanged;

	//{{AFX_MSG(CLineGeometryForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	afx_msg void OnChangeX1Edit();
	afx_msg void OnChangeX2Edit();
	afx_msg void OnChangeXcenterEdit();
	afx_msg void OnChangeY1Edit();
	afx_msg void OnChangeY2Edit();
	afx_msg void OnChangeYcenterEdit();
	afx_msg void OnChangeLengthEdit();
	afx_msg void OnChangeAngleEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CPointGeometryForm

class CPointGeometryForm : public CAbstractForm
{
// Constructors/destructors
public:
	CPointGeometryForm();
	virtual ~CPointGeometryForm();
	DECLARE_DYNCREATE(CPointGeometryForm)

// Form Data
public:
	//{{AFX_DATA(CPointGeometryForm)
	enum { IDD = IDD_POINT_GEOMETRY_FORM };
	double	m_X;
	double	m_Y;
	//}}AFX_DATA

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual void OnDesignChanged();

// Protected overridables
	//{{AFX_VIRTUAL(CPointGeometryForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
	CPoint2D *m_pPoint;
	LOGICAL m_bChanged;

	//{{AFX_MSG(CPointGeometryForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	afx_msg void OnChangeXEdit();
	afx_msg void OnChangeYEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CMaterialConstantsForm

class CMaterialConstantsForm : public CAbstractForm
{
// Constructors/destructors
public:
	CMaterialConstantsForm();
	virtual ~CMaterialConstantsForm();
	DECLARE_DYNCREATE(CMaterialConstantsForm)

// Form Data
public:
	//{{AFX_DATA(CMaterialConstantsForm)
	enum { IDD = IDD_MATERIAL_CONSTANTS_FORM };
	double	m_Nu;
	double	m_E;
	double	m_Ro;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CMaterialConstantsForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CMaterialConstantsForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
	CMaterial *m_pMaterial;
};

/////////////////////////////////////////////////////////////////////////////
// CParticleSizeOpenGLWnd

class CParticleSizeOpenGLWnd : public COpenGLStatic
{
// Construction
public:
	CParticleSizeOpenGLWnd();
	virtual ~CParticleSizeOpenGLWnd();

// Public methods
public:
	inline void SetParticleType(CParticleType *pType);

// Overrides
	//{{AFX_VIRTUAL(CParticleSizeOpenGLWnd)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual CRenderer *GetRenderer();
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

protected:
	//{{AFX_MSG(CParticleSizeOpenGLWnd)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	CParticleSizeRenderer *m_pRenderer;
};

/////////////////////////////////////////////////////////////////////////////
// CParticleShapeOpenGLWnd

class CParticleShapeOpenGLWnd : public COpenGLStatic
{
// Construction
public:
	CParticleShapeOpenGLWnd();
	virtual ~CParticleShapeOpenGLWnd();

// Public methods
public:
	inline void SetParticleType(CParticleType *pType);

// Overrides
	//{{AFX_VIRTUAL(CParticleShapeOpenGLWnd)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual CRenderer *GetRenderer();
	virtual void CreateRenderer();
	virtual void DestroyRenderer();

protected:
	//{{AFX_MSG(CParticleShapeOpenGLWnd)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	CParticleShapeRenderer *m_pRenderer;
};

/////////////////////////////////////////////////////////////////////////////
// CParticleSizeForm

class CParticleSizeForm : public CAbstractForm
{
// Constructors/destructors
public:
	CParticleSizeForm();
	virtual ~CParticleSizeForm();
	DECLARE_DYNCREATE(CParticleSizeForm)

// Form Data
public:
	//{{AFX_DATA(CParticleSizeForm)
	enum { IDD = IDD_PARTICLE_SIZE_FORM };
	CEdit	m_NormalSigmaCtrl;
	CEdit	m_NormalMedianCtrl;
	CEdit	m_UniformToCtrl;
	CEdit	m_UniformFromCtrl;
	CEdit	m_FixedSizeCtrl;
	CParticleSizeOpenGLWnd	m_SizeCtrl;
	double	m_FixedSize;
	double	m_UniformFrom;
	double	m_UniformTo;
	double	m_NormalMedian;
	double	m_NormalSigma;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CParticleSizeForm)
	protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	void EnableFixedSizeCtrls(BOOL bEnable);
	void EnableUniformSizeCtrls(BOOL bEnable);
	void EnableNormalSizeCtrls(BOOL bEnable);

	//{{AFX_MSG(CParticleSizeForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	afx_msg void OnFixedSizeRadio();
	afx_msg void OnNormalSizeRadio();
	afx_msg void OnUniformSizeRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
	CParticleType *m_pType;
};

/////////////////////////////////////////////////////////////////////////////
// CParticleShapeForm

class CParticleShapeForm : public CAbstractForm
{
// Constructors/destructors
public:
	CParticleShapeForm();
	virtual ~CParticleShapeForm();
	DECLARE_DYNCREATE(CParticleShapeForm)

// Form Data
public:
	//{{AFX_DATA(CParticleShapeForm)
	enum { IDD = IDD_PARTICLE_SHAPE_FORM };
	CSpinButtonCtrl	m_SquarenessSpin;
	CParticleShapeOpenGLWnd	m_ShapeCtrl;
	UINT	m_Squareness;
	double	m_AspectRatio;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CParticleShapeForm)
	protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CParticleShapeForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
	CParticleType *m_pType;
};


/////////////////////////////////////////////////////////////////////
// CObjectComboBox

class CObjectComboBox : public CComboBox
{
// Public methods
public:
	void SelectObject(CGranSim2DObject *pObject);
	CGranSim2DObject *GetSelectedObject();

// Public overridables
public:

// Protected overridables
protected:

// Protected members
};

/////////////////////////////////////////////////////////////////////
// CObjectComboBoxVisitor

class CObjectComboBoxVisitor : public IObjectVisitor
{
// Constructors/destructor
public:
	CObjectComboBoxVisitor(CComboBox *pCombo) { m_pCombo = pCombo; }

// Public overridables
public:
	virtual LOGICAL Visit(CGranSim2DObject *pObject);

// Protected overridables
protected:

// Protected members
	CComboBox *m_pCombo;
};


/////////////////////////////////////////////////////////////////////////////
// CParticleMaterialForm

class CParticleMaterialForm : public CAbstractForm
{
// Constructors/destructors
public:
	CParticleMaterialForm();
	virtual ~CParticleMaterialForm();
	DECLARE_DYNCREATE(CParticleMaterialForm)

// Form Data
public:
	//{{AFX_DATA(CParticleMaterialForm)
	enum { IDD = IDD_PARTICLE_MATERIAL_FORM };
	CObjectComboBox	m_MaterialCombo;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CParticleMaterialForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CParticleMaterialForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
	CParticleType *m_pType;
};

/////////////////////////////////////////////////////////////////////////////
// CParticleSourceTypeForm

class CParticleSourceTypeForm : public CAbstractForm
{
// Constructors/destructors
public:
	CParticleSourceTypeForm();
	virtual ~CParticleSourceTypeForm();
	DECLARE_DYNCREATE(CParticleSourceTypeForm)

// Form Data
public:
	//{{AFX_DATA(CParticleSourceTypeForm)
	enum { IDD = IDD_PARTICLE_SOURCE_TYPE_FORM };
	CObjectComboBox	m_ParticleTypeCombo;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CParticleSourceTypeForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CParticleSourceTypeForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
	CParticleSource *m_pSource;
};

/////////////////////////////////////////////////////////////////////////////
// CParticleSourceIntensityForm

class CParticleSourceIntensityForm : public CAbstractForm
{
// Constructors/destructors
public:
	CParticleSourceIntensityForm();
	virtual ~CParticleSourceIntensityForm();
	DECLARE_DYNCREATE(CParticleSourceIntensityForm)

// Form Data
public:
	//{{AFX_DATA(CParticleSourceIntensityForm)
	enum { IDD = IDD_PARTICLE_SOURCE_INTENSITY_FORM };
	double	m_Intensity;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CParticleSourceIntensityForm)
	protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

	//{{AFX_MSG(CParticleSourceIntensityForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
	CParticleSource *m_pSource;
};

/////////////////////////////////////////////////////////////////////////////
// CReflectingObjectMaterialForm

class CReflectingObjectMaterialForm : public CAbstractForm
{
// Constructors/destructors
public:
	CReflectingObjectMaterialForm();
	virtual ~CReflectingObjectMaterialForm();
	DECLARE_DYNCREATE(CReflectingObjectMaterialForm)

// Form Data
public:
	//{{AFX_DATA(CReflectingObjectMaterialForm)
	enum { IDD = IDD_REFLECTING_OBJECT_MATERIAL_FORM };
	CObjectComboBox	m_MaterialCombo;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CReflectingObjectMaterialForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CReflectingObjectMaterialForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Protected members
	CReflectingObject *m_pReflectingObject;
};


/////////////////////////////////////////////////////////////////////////////
// CRectangleGeometryForm form view

class CRectangleGeometryForm : public CAbstractForm
{
// Constructors/destructors
public:
	CRectangleGeometryForm();
	virtual ~CRectangleGeometryForm();
	DECLARE_DYNCREATE(CRectangleGeometryForm)

// Form Data
public:
	//{{AFX_DATA(CRectangleGeometryForm)
	enum { IDD = IDD_RECTANGLE_GEOMETRY_FORM };
	double	m_XCenter;
	double	m_YCenter;
	double	m_Height;
	double	m_Width;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual void OnDesignChanged();

	//{{AFX_VIRTUAL(CRectangleGeometryForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
	CRectangle2D *m_pRectangle;

	//{{AFX_MSG(CRectangleGeometryForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__CONTROL_FORMS_H_INCLUDED__ 

/////////////////////////////////////////////////////////////////////////////
