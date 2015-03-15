/* OpenGLRenderer.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	?? ??? ???? - initial creation
	21 Jun 2007 - COpenGL_Chart2D class added

*/

#include "stdafx.h"
#include <math.h>
#include "OpenGLRenderer.h"

#define COS(x) ((float)cos(3.1415926f*(x)/180.f))
#define SIN(x) ((float)sin(3.1415926f*(x)/180.f))

unsigned char ucWhiteColor[4]			= { 255, 255, 255, 0 };
unsigned char ucBlackColor[4]			= { 000, 000, 000, 0 };
unsigned char ucGrayColor[4]			= { 128, 128, 128, 0 };
unsigned char ucBrightRedColor[4]		= { 255, 000, 000, 0 };
unsigned char ucRedColor[4]				= { 128, 000, 000, 0 };
unsigned char ucBrightGreenColor[4]		= { 000, 255, 000, 0 };
unsigned char ucGreenColor[4]			= { 000, 128, 000, 0 };
unsigned char ucBrightBlueColor[4]		= { 000, 000, 255, 0 };
unsigned char ucBlueColor[4]			= { 000, 000, 128, 0 };
unsigned char ucYellowColor[4]			= { 255, 255, 000, 0 };


///////////////////////////////////////////////////////////////////////////
// COpenGL

COpenGL::COpenGL()
{
}

COpenGL::~COpenGL()
{
}

/*void COpenGL::CalculateNormals(D3DVERTEX *pVertex)
{
	double nx, ny, nz;
	double ax,ay,az,bx,by,bz;
	ax=pVertex[1].x-pVertex[0].x;
	bx=pVertex[2].x-pVertex[0].x;
	ay=pVertex[1].y-pVertex[0].y;
	by=pVertex[2].y-pVertex[0].y;
	az=pVertex[1].z-pVertex[0].z;
	bz=pVertex[2].z-pVertex[0].z;
	nx=ay*bz-az*by;
	ny=az*bx-ax*bz;
	nz=ax*by-ay*bx;
	double normer=sqrt(nx*nx+ny*ny+nz*nz);
	if (fabs(normer)<1.e-6) return;
	nx/=normer;
	ny/=normer;
	nz/=normer;
	pVertex[0].nx=pVertex[1].nx=pVertex[2].nx=(D3DVALUE)nx;
	pVertex[0].ny=pVertex[1].ny=pVertex[2].ny=(D3DVALUE)ny;
	pVertex[0].nz=pVertex[1].nz=pVertex[2].nz=(D3DVALUE)nz;
}*/

LOGICAL COpenGL::InitGL(HDC hDC)
{
	m_hDC = hDC;
	if (SetWindowPixelFormat() == LOGICAL_FALSE)
		return LOGICAL_FALSE;
	
	if (CreateViewGLContext() == LOGICAL_FALSE)
		return LOGICAL_FALSE;

	MakeGLContextCurrent();
	OnSetDefaultOptions();
	OnInitialized();
	MakeGLContextNotCurrent();

	return LOGICAL_TRUE;
}

void COpenGL::FinitGL()
{
	if (wglGetCurrentContext())
		MakeGLContextNotCurrent();
	
	if (m_hGLContext)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
	}
}

LOGICAL COpenGL::SetWindowPixelFormat()
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;
	
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	int GLPixelIndex = ChoosePixelFormat(m_hDC,&pixelDesc);
	if (GLPixelIndex == 0) // Choose default
	{
		GLPixelIndex = 1;
		if (DescribePixelFormat(m_hDC, GLPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc)==0)
			return LOGICAL_FALSE;
	}
	
	if (!SetPixelFormat(m_hDC, GLPixelIndex, &pixelDesc))
		return LOGICAL_FALSE;

	return LOGICAL_TRUE;
}

LOGICAL COpenGL::CreateViewGLContext()
{
	m_hGLContext = wglCreateContext(m_hDC);
	
	if (m_hGLContext == NULL)
		return LOGICAL_FALSE;
	
	return LOGICAL_TRUE;
}

// !!! Do not forget to make a call of MakeGLContextNotCurrent every time after you call this method
LOGICAL COpenGL::MakeGLContextCurrent()
{
	return wglMakeCurrent(m_hDC, m_hGLContext);
}

LOGICAL COpenGL::MakeGLContextNotCurrent()
{
	return wglMakeCurrent(NULL, NULL);
}

unsigned int COpenGL::Create2DRasterFont(const char *pszFontName, int height)
{
	// Here we generate the lists for each character we want to use.
	UINT fontListID = glGenLists(256);					// Generate the list for the font

	// Now we actually need to create the font.  We use a windows function called:
	// CreateFont() that returns a handle to a font (HFONT).  Our CreateOpenGLFont()
	// function allows us to pass in a name and height.  For simplistic reasons, I left
	// other options out, but feel free to add them to your function (like bold, italic, width..)

	HFONT hFont = CreateFont(height,							// Our desired HEIGHT of the font
						0,								// The WIDTH (If we leave this zero it will pick the best width depending on the height)
						0,								// The angle of escapement
						0,								// The angle of orientation
						FW_BOLD,						// The font's weight (We want it bold)
						FALSE,							// Italic - We don't want italic
						FALSE,							// Underline - We don't want it underlined
						FALSE,							// Strikeout - We don't want it strikethrough
						ANSI_CHARSET,					// This is the type of character set
						OUT_TT_PRECIS,					// The Output Precision
						CLIP_DEFAULT_PRECIS,			// The Clipping Precision
						ANTIALIASED_QUALITY,			// The quality of the font - We want anitaliased fonts
						FF_DONTCARE|DEFAULT_PITCH,		// The family and pitch of the font.  We don't care.
						pszFontName);					// The font name (Like "Arial", "Courier", etc...)

	// Now that we have created a new font, we need to select that font into our global HDC.
	// We store the old font so we can select it back in when we are done to avoid memory leaks.
	SelectObject(m_hDC, hFont);

	// This function does the magic.  It takes the current font selected in
	// the hdc and makes bitmaps out of each character.  These are called glyphs.
	// The first parameter is the HDC that holds the font to be used.
	// The second parameters is the ASCII value to start from, which is zero in our case.
	// The third parameters is the ASCII value to end on (255 is the last of the ASCII values so we minus 1 from MAX_CHARS)
	// The last parameter is the base pointer for the display lists being used.  This should be 1.

	BOOL b = wglUseFontBitmaps(m_hDC, 0, 255, fontListID);	// Builds 255 bitmap characters

	DeleteObject(hFont);
	return fontListID;									// Return the ID to the display list to use later
}

unsigned int COpenGL::CreateVectorFont(const char *pszFontName, int height, GLYPHMETRICSFLOAT *pGlyphMetrics)
{
	// Here we generate the lists for each character we want to use.
	UINT fontListID = glGenLists(256);					// Generate the list for the font

	// Now we actually need to create the font.  We use a windows function called:
	// CreateFont() that returns a handle to a font (HFONT).  Our CreateOpenGLFont()
	// function allows us to pass in a name and height.  For simplistic reasons, I left
	// other options out, but feel free to add them to your function (like bold, italic, width..)

	HFONT hFont = CreateFont(height,							// Our desired HEIGHT of the font
						0,								// The WIDTH (If we leave this zero it will pick the best width depending on the height)
						0,								// The angle of escapement
						0,								// The angle of orientation
						FW_BOLD,						// The font's weight (We want it bold)
						FALSE,							// Italic - We don't want italic
						FALSE,							// Underline - We don't want it underlined
						FALSE,							// Strikeout - We don't want it strikethrough
						ANSI_CHARSET,					// This is the type of character set
						OUT_TT_PRECIS,					// The Output Precision
						CLIP_DEFAULT_PRECIS,			// The Clipping Precision
						ANTIALIASED_QUALITY,			// The quality of the font - We want anitaliased fonts
						FF_DONTCARE|DEFAULT_PITCH,		// The family and pitch of the font.  We don't care.
						pszFontName);					// The font name (Like "Arial", "Courier", etc...)

	// Now that we have created a new font, we need to select that font into our global HDC.
	// We store the old font so we can select it back in when we are done to avoid memory leaks.
	SelectObject(m_hDC, hFont);

	// This function does the magic.  It takes the current font selected in
	// the hdc and makes bitmaps out of each character.  These are called glyphs.
	// The first parameter is the HDC that holds the font to be used.
	// The second parameters is the ASCII value to start from, which is zero in our case.
	// The third parameters is the ASCII value to end on (255 is the last of the ASCII values so we minus 1 from MAX_CHARS)
	// The last parameter is the base pointer for the display lists being used.  This should be 1.

	BOOL b = wglUseFontOutlines(m_hDC, 0, 255, fontListID,
		0, 0,
		WGL_FONT_POLYGONS,
		pGlyphMetrics
		);	// Builds 255 bitmap characters

	DeleteObject(hFont);

	return fontListID;									// Return the ID to the display list to use later
}

void COpenGL::Draw2DText(const char *pszText)
{
	glCallLists(strlen(pszText), GL_UNSIGNED_BYTE, pszText);
}

///////////////////////////////////////////////////////////////////////////
// COpenGL_2D

COpenGL_2D::COpenGL_2D()
{
	m_WindowWidth = m_WindowHeight = 0;		// window sizes
	m_fAspectRatio = 1;
	m_fWorldShiftX = m_fWorldShiftY = 0.;
	m_fWorldShiftDeltaX = m_fWorldShiftDeltaY = 0.;
	m_fWorldZoomX = m_fWorldZoomY = 1;
	m_bLDragging = LOGICAL_FALSE;
	m_MouseClickDragType = MouseClickDrag_Select;
}

COpenGL_2D::~COpenGL_2D()
{
}

//virtual
LOGICAL COpenGL_2D::Init(HDC hDC)
{
	if (!InitGL(hDC))
		return LOGICAL_FALSE;
	SetDefaultView();
	return LOGICAL_TRUE;
}

//virtual
void COpenGL_2D::Finit()
{
	FinitGL();
}

//virtual
void COpenGL_2D::SetDefaultView()
{
	m_fWorldShiftX = -GetDefaultWorldCenterX();
	m_fWorldShiftY = -GetDefaultWorldCenterY();
	m_fWorldZoomX = m_fWorldZoomY =	1;
	Redraw();
}

//virtual
void COpenGL_2D::Redraw()
{
	if (m_WindowHeight <= 0)
		return; // not initialized yet
	
	MakeGLContextCurrent();
	unsigned char *pcBackColor = GetBackgroundColor();
	glClearColor(pcBackColor[0]/255.f, pcBackColor[1]/255.f, pcBackColor[2]/255.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawBuffer(GL_BACK);
	SetupCamera();
	StoreModelViewMatrix();  // store MODELVIEW matrix (user can forget to restore it inside OnRenderScene)
	OnRenderScene();
	RestoreModelViewMatrix();
	if (IsInZoomRectangleMode() && m_bLDragging)
	{
		glPushAttrib(GL_ENABLE_BIT); // we will disable depth test therefore it must be restored
		glColor4ubv(GetZoomRectangleColor());
		glDisable(GL_DEPTH_TEST);
		double x1 = ScreenToWorldX(m_LButtonPressedAtX);
		double x2 = ScreenToWorldX(m_MouseCurrentPosX);
		double y1 = ScreenToWorldY(m_LButtonPressedAtY);
		double y2 = ScreenToWorldY(m_MouseCurrentPosY);
		glBegin(GL_LINE_LOOP);
			glVertex2d(x1, y1);
			glVertex2d(x2, y1);
			glVertex2d(x2, y2);
			glVertex2d(x1, y2);
		glEnd();
		glPopAttrib();
	}
	
	glFinish();
	SwapBuffers(m_hDC);
	MakeGLContextNotCurrent();
}

//virtual
unsigned char *COpenGL_2D::GetZoomRectangleColor()
{
	return ucGrayColor;
}

//virtual
void COpenGL_2D::OnRenderScene()
{
	glBegin(GL_LINES );
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(0.1f, 0.1f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.9f, 0.1f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.5f, 0.9f);
	glEnd();
}


//virtual
void COpenGL_2D::OnSize(int cx, int cy)
{
	m_WindowWidth = cx;
	m_WindowHeight = cy;
	if (IsIsotropic() == LOGICAL_TRUE)
		m_fAspectRatio = (m_WindowWidth == 0) ? m_WindowWidth : (double)m_WindowHeight/(double)m_WindowWidth;
	else
		m_fAspectRatio = 1;
	MakeGLContextCurrent();
	OnEvent_SizeChanged(cx, cy);
	MakeGLContextNotCurrent();
}

//virtual
void COpenGL_2D::OnLButtonDown(unsigned int uFlags, int x, int y)
{
	m_LButtonPressedAtX = x;
	m_LButtonPressedAtY = y;
	m_bLDragging = LOGICAL_TRUE;
	m_fWorldShiftDeltaX = m_fWorldShiftDeltaY = 0.;
	OnEvent_LeftMouseButtonPressed(uFlags, x, y);
}

//virtual
void COpenGL_2D::OnLButtonUp(unsigned int uFlags, int x, int y)
{
	if (IsInZoomRectangleMode() && m_bLDragging)
	{
		int DeltaX = abs(m_LButtonPressedAtX-x);
		int DeltaY = abs(m_LButtonPressedAtY-y);
		if (DeltaX > 10 && DeltaY > 10)
		{
			m_fWorldShiftX = -ScreenToWorldX((m_LButtonPressedAtX+x)/2);
			m_fWorldShiftY = -ScreenToWorldY((m_LButtonPressedAtY+y)/2);
			m_fWorldZoomX *= (double)m_WindowWidth/DeltaX;
			m_fWorldZoomY *= (double)m_WindowHeight/DeltaY;
			if (IsIsotropic())
			{
				if (m_fWorldZoomX > m_fWorldZoomY)
					m_fWorldZoomX = m_fWorldZoomY;
				else
					m_fWorldZoomY = m_fWorldZoomX;
			}
			m_bLDragging = LOGICAL_FALSE;
			Redraw();
		}
	}

	m_bLDragging = LOGICAL_FALSE;
	m_fWorldShiftX += m_fWorldShiftDeltaX;
	m_fWorldShiftY += m_fWorldShiftDeltaY;
	m_fWorldShiftDeltaX = m_fWorldShiftDeltaY = 0.;
	OnEvent_LeftMouseButtonReleased(uFlags, x, y);
}

//virtual
void COpenGL_2D::OnRButtonDown(unsigned int uFlags, int x, int y)
{
	OnEvent_RightMouseButtonPressed(uFlags, x, y);
}

//virtual
void COpenGL_2D::OnRButtonUp(unsigned int uFlags, int x, int y)
{
	OnEvent_RightMouseButtonReleased(uFlags, x, y);
}

//virtual
void COpenGL_2D::OnMouseMove(unsigned int uFlags, int x, int y)
{
	m_MouseCurrentPosX = x;
	m_MouseCurrentPosY = y;
	if (m_bLDragging)
	{
		if (IsInDragScrollMode())
		{
			if (IsMouseMovePanXEnabled())
				m_fWorldShiftDeltaX = ScreenToWorldX(x)-ScreenToWorldX(m_LButtonPressedAtX);
			if (IsMouseMovePanYEnabled())
				m_fWorldShiftDeltaY = ScreenToWorldY(y)-ScreenToWorldY(m_LButtonPressedAtY);
			Redraw();
		}
		else if (IsInZoomRectangleMode())
		{
			Redraw();
		}
	}
	OnEvent_MouseMoved(uFlags, x, y);
}

void COpenGL_2D::SetupCamera()
{
	// Set OpenGL perspective, viewport and mode
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	m_Frustrum.left = ScreenToWorldX(0);
	m_Frustrum.right = ScreenToWorldX(m_WindowWidth);
	m_Frustrum.top = ScreenToWorldY(0);
	m_Frustrum.bottom = ScreenToWorldY(m_WindowHeight);
	m_Frustrum.height = m_Frustrum.top-m_Frustrum.bottom;
	m_Frustrum.width = m_Frustrum.right - m_Frustrum.left;
	m_1Pixel2WorldX = (m_Frustrum.right - m_Frustrum.left)/m_WindowWidth;
	m_1Pixel2WorldY = (m_Frustrum.top - m_Frustrum.bottom)/m_WindowHeight;
	glOrtho(m_Frustrum.left, m_Frustrum.right, m_Frustrum.bottom, m_Frustrum.top, -1, 1);
}

void COpenGL_2D::IncreaseZoomX(double fIncreaseBy, LOGICAL bRedraw)
{
	m_fWorldZoomX *= fIncreaseBy;
	if (bRedraw)
		Redraw();
}

void COpenGL_2D::IncreaseZoomY(double fIncreaseBy, LOGICAL bRedraw)
{
	m_fWorldZoomY *= fIncreaseBy;
	if (bRedraw)
		Redraw();
}

void COpenGL_2D::IncreaseZoom(double fIncreaseBy, LOGICAL bRedraw)
{
	IncreaseZoomX(fIncreaseBy, LOGICAL_FALSE);
	IncreaseZoomY(fIncreaseBy, bRedraw);
}

double COpenGL_2D::MostLeftVisibleX()
{
	return ScreenToWorldX(0);
}

double COpenGL_2D::MostRightVisibleX()
{
	return ScreenToWorldX(m_WindowWidth);
}

double COpenGL_2D::MostTopVisibleY()
{
	return ScreenToWorldY(m_WindowHeight);
}

double COpenGL_2D::MostBottomVisibleY()
{
	return ScreenToWorldY(0);
}

void COpenGL_2D::CalcWorld2ScreenTranslationX(double &a, double &b)
{
	double scale = GetWorldScaleX()*m_fWorldZoomX;
	a = m_WindowWidth*scale/GetWorldSizeX();
	b = m_WindowWidth*(0.5+(m_fWorldShiftX+m_fWorldShiftDeltaX)*scale/GetWorldSizeX());
}

void COpenGL_2D::CalcWorld2ScreenTranslationY(double &a, double &b)
{
	double scale = GetWorldScaleY()*m_fWorldZoomY;
	a = m_WindowHeight*scale/(GetWorldSizeY()*m_fAspectRatio);
	b = m_WindowHeight*(0.5+(m_fWorldShiftY+m_fWorldShiftDeltaY)*scale/(GetWorldSizeY()*m_fAspectRatio));
}

int COpenGL_2D::WorldToScreenX(double WorldCoord)
{
	double a, b;
	CalcWorld2ScreenTranslationX(a, b);
	return ((int)(a*WorldCoord + b + 0.49999));
}

int COpenGL_2D::WorldToScreenY(double WorldCoord)
{
	double a, b;
	CalcWorld2ScreenTranslationY(a, b);
	return (int)(a*WorldCoord + b + 0.49999);
}

double COpenGL_2D::ScreenToWorldX(int ScreenCoord)
{
	double a, b;
	CalcWorld2ScreenTranslationX(a, b);
	return (ScreenCoord-b)/a;
}

double COpenGL_2D::ScreenToWorldY(int ScreenCoord)
{
	double a, b;
	CalcWorld2ScreenTranslationY(a, b);
	return (m_WindowHeight-ScreenCoord-b)/a;
}

///////////////////////////////////////////////////////////////////////////
// COpenGL_3D

COpenGL_3D::COpenGL_3D()
{
	m_WindowWidth = m_WindowHeight = 0;		// window sizes
	SetCameraPos(0, 0, -3);
	CameraToWorldCenter();
}

COpenGL_3D::~COpenGL_3D()
{
}

//virtual
LOGICAL COpenGL_3D::Init(HDC hDC)
{
	if (!InitGL(hDC))
		return LOGICAL_FALSE;
	SetDefaultView();
	return LOGICAL_TRUE;
}

//virtual
void COpenGL_3D::Finit()
{
	FinitGL();
}

//virtual
void COpenGL_3D::SetDefaultView()
{

}

//virtual
void COpenGL_3D::OnRenderScene()
{
	glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-0.3f, 0.3f, 0.3f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, -0.3f, 0.3f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.3f, 0.3f, 0.3f);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(0.3f, -0.3f, 0.3f);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(0.3f, 0.3f, -0.3f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.3f, -0.3f, -0.3f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.3f, 0.3f, -0.3f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, -0.3f,  -0.3f);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-0.3f, 0.3f, 0.3f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, -0.3f, 0.3f);
	glEnd();
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-0.3f, 0.3f, 0.3f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.3f, 0.3f, 0.3f);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(0.3f, 0.3f, -0.3f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.3f, 0.3f, -0.3f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, -0.3f, 0.3f);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(0.3f, -0.3f, 0.3f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.3f, -0.3f, -0.3f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, -0.3f, -0.3f);
	glEnd();

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES );
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES );
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES );
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();
	return;
/*	// Light
	GLfloat AmbientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
	GLfloat DiffuseLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat SpecularLight[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat PositionLight[] = {0.0f, 1.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, PositionLight);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Materials
	GLfloat AmbientMaterialBox1[] = {0.0f, 0.8f, 0.0f, 1.0f};
	GLfloat SpecularMaterialBox1[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat AmbientMaterialBox2[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat SpecularMaterialBox2[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat AmbientMaterialSphere[] = {0.0f, 0.0f, 0.8f, 1.0f};
	GLfloat SpecularMaterialSphere[] = {1.0f, 1.0f, 1.0f, 1.0f};

//	glColor3f(0.f, 0.f, 1.f);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AmbientMaterialBox1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, SpecularMaterialBox1);
	auxSolidBox(1.0, 0.5, 1.5);

//	glColor3f(0.f, 1.f, 0.f);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AmbientMaterialSphere);
	glMaterialfv(GL_FRONT, GL_SPECULAR, SpecularMaterialSphere);
	glTranslated(-2, 0, -2);
	auxSolidSphere(1);

//	glColor3f(1.f, 0.f, 1.f);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AmbientMaterialBox2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, SpecularMaterialBox2);
	glTranslated(3, 3, 3);
	auxSolidBox(1, 2, 0.6);*/
}

//virtual
void COpenGL_3D::OnSetDefaultOptions()
{
	glEnable(GL_DEPTH_TEST);
}

//virtual
void COpenGL_3D::Redraw()
{
	MakeGLContextCurrent();
	unsigned char *pcBackColor = GetBackgroundColor();
	glClearColor(pcBackColor[0]/255.f, pcBackColor[1]/255.f, pcBackColor[2]/255.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Position / translation / scale
	StoreModelViewMatrix();
//	glLoadIdentity();
	double d = sqrt((m_WorldX-m_CameraX)*(m_WorldX-m_CameraX) + 
					(m_WorldY-m_CameraY)*(m_WorldY-m_CameraY) + 
					(m_WorldZ-m_CameraZ)*(m_WorldZ-m_CameraZ));
	double dx = m_CameraX-d*sin(m_CameraAzimuth);
	double dz = m_CameraZ-d*(1.-cos(m_CameraAzimuth));
	glTranslated(m_WorldX-m_CameraX+dx, m_WorldY-m_CameraY, m_WorldZ-m_CameraZ+dz);

	glRotated(m_WorldRx-m_CameraAngle, 1.0, 0.0, 0.0);
	glRotated(m_WorldRy+m_CameraAzimuth, 0.0, 1.0, 0.0);
	glRotated(m_WorldRz, 0.0, 0.0, 1.0);
	glScaled(1,1,1);

	OnRenderScene();

	RestoreModelViewMatrix();

	glFinish();
	SwapBuffers(m_hDC);
	MakeGLContextNotCurrent();
}


//virtual
void COpenGL_3D::OnCameraPosChanged()
{
	SetupCamera();
}

//virtual
void COpenGL_3D::OnCameraDirChanged()
{
	SetupCamera();
}

void COpenGL_3D::SetupCamera()
{
	// Set OpenGL perspective, viewport and mode
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double aspect = (m_WindowWidth == 0) ? m_WindowWidth : (double)m_WindowWidth/(double)m_WindowHeight;
	gluPerspective(30, aspect, 0.01, 100.0);
	TRACE("Camera: %lf\t%lf\t%lf\n",  m_CameraX, m_CameraY, m_CameraZ);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_CameraX, m_CameraY, m_CameraZ,
		m_WorldX, m_WorldY, m_WorldZ,
		0, 1, 0);
}

//virtual
void COpenGL_3D::OnSize(int cx, int cy)
{
	m_WindowWidth = cx;
	m_WindowHeight = cy;
	MakeGLContextCurrent();

	// Set OpenGL perspective, viewport and mode
	SetupCamera();
	
	glDrawBuffer(GL_BACK);
	OnEvent_SizeChanged(cx, cy);
	MakeGLContextNotCurrent();
}

//virtual
void COpenGL_3D::OnLButtonDown(UINT nFlags, int x, int y)
{
/*	m_Lx = x;
	m_Ly = y;
	m_bLDragging = LOGICAL_TRUE;*/
//	OnEvent_LeftMouseButtonPressed(uFlags, x, y);
}

//virtual
void COpenGL_3D::OnLButtonUp(unsigned int uFlags, int x, int y)
{
//	m_bLDragging = LOGICAL_FALSE;
//	OnEvent_LeftMouseButtonReleased(uFlags, x, y);
}

//virtual
void COpenGL_3D::OnRButtonDown(UINT nFlags, int x, int y)
{
/*	m_Lx = x;
	m_Ly = y;
	m_bLDragging = LOGICAL_TRUE;*/
//	OnEvent_LeftMouseButtonPressed(uFlags, x, y);
}

//virtual
void COpenGL_3D::OnRButtonUp(unsigned int uFlags, int x, int y)
{
//	m_bLDragging = LOGICAL_FALSE;
//	OnEvent_LeftMouseButtonReleased(uFlags, x, y);
}

//virtual
void COpenGL_3D::OnMouseMove(unsigned int uFlags, int x, int y)
{
}


///////////////////////////////////////////////////////////////////////////
// COpenGL_Chart2D

COpenGL_Chart2D::COpenGL_Chart2D() : COpenGL_2D()
{
	m_MouseClickDragType = MouseClickDrag_DragScroll;
}

COpenGL_Chart2D::~COpenGL_Chart2D()
{
}


//virtual
void COpenGL_Chart2D::OnRenderScene()
{
///	COpenGL_2D::OnRenderScene();
	RenderVerticalGrid();
	RenderHorizontalGrid();
	RenderCurves();
	RenderMarksForVertScale();
	RenderMarksForHorzScale();
}

//virtual
void COpenGL_Chart2D::OnInitialized()
{
	m_RasterFontListBaseID = Create2DRasterFont("Arial", GetGridFontSize());
}

//virtual
void COpenGL_Chart2D::OnSetDefaultOptions()
{
}

// Rounds grid_step to (0.1, 0.2, 0.25, 0.4, 0.5, 0.75) * 10**N
static void RoundGridStep(double &grid_step, double &subgrid_step)
{
	int i, iBest = 0;
	double normalized_steps[] = { 1, 2, 2.5, 4, 5, 7.5, 10 };
	double subgrid_factor[]   = { 4, 4, 5.0, 4, 5, 3.0, 4 };
	int nPower = (int)floor(log10(grid_step));
	if (nPower > 0)
	{
		for (i = 0; i < nPower; i++)
			grid_step /= 10.;
	}
	else if (nPower < 0)
	{
		for (i = 0; i < -nPower; i++)
			grid_step *= 10.;

	}

	for (i = 1; i < sizeof(normalized_steps)/sizeof(normalized_steps[0]); i++)
	{
		if (grid_step < normalized_steps[i] &&
			grid_step > normalized_steps[i-1])
		{
			if ((grid_step - normalized_steps[i-1]) < (normalized_steps[i] - grid_step))
				grid_step = normalized_steps[i-1];
			else
				grid_step = normalized_steps[i];
			iBest = i;
			break;
		}
	}

	if (nPower > 0)
	{
		for (i = 0; i < nPower; i++)
			grid_step *= 10.;
	}
	else if (nPower < 0)
	{
		for (i = 0; i < -nPower; i++)
			grid_step /= 10.;
	}
	subgrid_step = grid_step/subgrid_factor[iBest];
}

//virtual
void COpenGL_Chart2D::RenderVerticalGrid()
{
	if (m_WindowWidth <= 0)
		return;
	glColor4ubv(GetGridColor());
	if (m_WindowHeight <= 0)
		return;
	double subgrid_step;
	m_fVertGridStep = GetFrustrumWidth()/(m_WindowWidth/(2*GetVertGridMinStep()));
	RoundGridStep(m_fVertGridStep, subgrid_step);

	// subgrid
	glColor4ubv(GetSubgridColor());
	glLineStipple(1, 0x8888);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		for (int i = (int)floor(GetFrustrumLeft()/subgrid_step); i < ceil(GetFrustrumRight()/subgrid_step); i++)
		{
			double value = i*subgrid_step;
			glVertex2d(value, GetFrustrumBottom());
			glVertex2d(value, GetFrustrumTop());
		}
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	
	// grid
	glColor4ubv(GetGridColor());
	glBegin(GL_LINES);
		for (i = (int)floor(GetFrustrumLeft()/m_fVertGridStep); i < ceil(GetFrustrumRight()/m_fVertGridStep); i++)
		{
			double value = i*m_fVertGridStep;
			glVertex2d(value, GetFrustrumBottom());
			glVertex2d(value, GetFrustrumTop());
		}
	glEnd();
}

//virtual
void COpenGL_Chart2D::RenderHorizontalGrid()
{
	if (m_WindowHeight <= 0)
		return;
	double subgrid_step;
	m_fHorzGridStep = GetFrustrumHeight()/(m_WindowHeight/(3*GetGridFontSize()));
	RoundGridStep(m_fHorzGridStep, subgrid_step);
	
	// subgrid
	glColor4ubv(GetSubgridColor());
	glLineStipple(1, 0x8888);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		for (int i = (int)floor(GetFrustrumBottom()/subgrid_step); i < ceil(GetFrustrumTop()/subgrid_step); i++)
		{
			double value = i*subgrid_step;
			glVertex2d(GetFrustrumRight(), value);
			glVertex2d(GetFrustrumLeft(), value);
		}
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	
	// grid
	glColor4ubv(GetGridColor());
	glBegin(GL_LINES);
		for (i = (int)floor(GetFrustrumBottom()/m_fHorzGridStep); i < ceil(GetFrustrumTop()/m_fHorzGridStep); i++)
		{
			double value = i*m_fHorzGridStep;
			glVertex2d(GetFrustrumRight(), value);
			glVertex2d(GetFrustrumLeft(), value);
		}
	glEnd();
}

// default function
double func(double x)
{
	return 0.5*(x*sin(x*x*x*20)+1);
}

//virtual
void COpenGL_Chart2D::RenderCurves() //default curve
{
	glColor4ubv(ucBrightRedColor);
	double step = GetWorldSizeX()/100;
	glBegin(GL_LINE_STRIP);
		double x1 = GetMinXValue();
		double y1 = func(x1);
		glVertex2d(x1, y1);
		double x2 = x1+step;
		while (x2 <= GetMaxXValue())
		{
			double y2 = func(x2);
			glVertex2d(x2, y2);
			x1 = x2;
			x2 += step;
		}
	glEnd();
}

//virtual
void COpenGL_Chart2D::RenderMarksForVertScale()
{
	glColor4ubv(GetGridMarksColor());
	glListBase(m_RasterFontListBaseID);
	double MarkX = ScreenToWorldX(3);
	char szValue[32];
	double Shift = (ScreenToWorldY(GetGridFontSize()) - ScreenToWorldY(0))/2;
	for (int i = (int)floor(GetFrustrumBottom()/m_fHorzGridStep); i < ceil(GetFrustrumTop()/m_fHorzGridStep); i++)
	{
		double fValue = i*m_fHorzGridStep;
		FormatYMark(szValue, sizeof(szValue)/sizeof(szValue[0]), fValue);
		glRasterPos2d(MarkX, fValue+Shift);
		Draw2DText(szValue);
	}
}

//virtual
void COpenGL_Chart2D::RenderMarksForHorzScale()
{
	glColor4ubv(GetGridMarksColor());
	glListBase(m_RasterFontListBaseID);
	double MarkY = ScreenToWorldY(m_WindowHeight-2);
	char szValue[32];
	for (int i = (int)floor(GetFrustrumLeft()/m_fVertGridStep); i < ceil(GetFrustrumRight()/m_fVertGridStep); i++)
	{
		double fValue = i*m_fVertGridStep;
		FormatXMark(szValue, sizeof(szValue)/sizeof(szValue[0]), fValue);
		double Shift = ScreenToWorldX(GetGridFontSize()/2*strlen(szValue)/2) - ScreenToWorldX(0);
		glRasterPos2d(fValue-Shift, MarkY);
		Draw2DText(szValue);
	}
}

//virtual
unsigned char *COpenGL_Chart2D::GetGridColor()
{
	return ucGrayColor;
}

//virtual
unsigned char *COpenGL_Chart2D::GetSubgridColor()
{
	return ucGrayColor;
}

//virtual
unsigned char *COpenGL_Chart2D::GetGridMarksColor()
{
	return ucYellowColor;
}

//virtual
void COpenGL_Chart2D::FormatXMark(char *pszMark, int MarkMaxLen, double value)
{
	sprintf(pszMark, "%g", value);
}

//virtual
void COpenGL_Chart2D::FormatYMark(char *pszMark, int MarkMaxLen, double value)
{
	sprintf(pszMark, "%g", value);
}


