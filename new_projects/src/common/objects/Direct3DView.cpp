// Direct3DView.cpp : implementation file
//

#include "stdafx.h"
#include "Direct3DView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirect3DView

IMPLEMENT_DYNCREATE(CDirect3DView, C3DView)

CDirect3DView::CDirect3DView()
{
	m_CameraAngle=45.f;
	m_CameraNearPlane=0.1f;
	m_CameraFarPlane=100000.f;
}

CDirect3DView::~CDirect3DView()
{
}

BEGIN_MESSAGE_MAP(CDirect3DView, C3DView)
	//{{AFX_MSG_MAP(CDirect3DView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

C3DRenderer *CDirect3DView::CreateRenderer()
{
	return new CDirect3D((DWORD)m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CDirect3DView diagnostics

#ifdef _DEBUG
void CDirect3DView::AssertValid() const
{
	C3DView::AssertValid();
}

void CDirect3DView::Dump(CDumpContext& dc) const
{
	C3DView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirect3DView message handlers

LPDIRECT3DDEVICE3 CDirect3DView::GetDevice()
{
	if (!GetDirect3D())
		return NULL;
	return GetDirect3D()->GetDevice();
}

void CDirect3DView::SetBackGround(double Red, double Green, double Blue)
{
	GetDirect3D()->SetBackGround(Red, Green, Blue);
}

void CDirect3DView::SetPerspectiveProjection(float CameraAngle, float NearPlane, float FarPlane)
{
	m_CameraAngle=CameraAngle;
	m_CameraNearPlane=NearPlane;
	m_CameraFarPlane=FarPlane;
	GetDirect3D()->SetPerspectiveProjection(CameraAngle, NearPlane, FarPlane);
}


