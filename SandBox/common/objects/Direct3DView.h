#if !defined(AFX_DIRECT3DVIEW_H__5A56DF22_9120_11D3_ABE9_008048EEE0E8__INCLUDED_)
#define AFX_DIRECT3DVIEW_H__5A56DF22_9120_11D3_ABE9_008048EEE0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Direct3DView.h : header file
//

#include "3DView.h"
#include "Direct3D.h"

/////////////////////////////////////////////////////////////////////////////
// CDirect3DView view

class CDirect3DView : public C3DView
{
protected:
	inline CDirect3D *GetDirect3D() {return (CDirect3D *)GetRenderer();}
	LPDIRECT3DDEVICE3 GetDevice();
	CDirect3DView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirect3DView)

// Attributes
public:
	float m_CameraAngle;
	float m_CameraFarPlane, m_CameraNearPlane;

// Operations
public:
	void SetPerspectiveProjection(float CameraAngle, float NearPlane, float FarPlane);
	void SetBackGround(double Red, double Green, double Blue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirect3DView)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDirect3DView();
	virtual C3DRenderer *CreateRenderer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDirect3DView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECT3DVIEW_H__5A56DF22_9120_11D3_ABE9_008048EEE0E8__INCLUDED_)
