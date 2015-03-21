/* opengl_widget.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
*/

#include "opengl_widget.h"
#include <stdio.h>

namespace common {

namespace ogl {

/////////////////////////////////////////////////////////////////
// cMouseDrag

cMouseDrag::cMouseDrag()
{
    m_bDragging = false;
    m_ButtonPressedAtX = m_LastProcessedX = 0;
    m_ButtonPressedAtY = m_LastProcessedY = 0;
}

void cMouseDrag::buttonPressed(QMouseEvent *pEvent)
{
    m_ButtonPressedAtX = pEvent->x();
    m_ButtonPressedAtY = pEvent->y();
    processed(pEvent);

    m_bDragging = true;
}

void cMouseDrag::buttonReleased()
{
    m_bDragging = false;
}

void cMouseDrag::processed(QMouseEvent *pEvent)
{
    m_LastProcessedX = pEvent->x();
    m_LastProcessedY = pEvent->y();
}


/////////////////////////////////////////////////////////////////
// cOpenGLWidget

cOpenGL_Widget::cOpenGL_Widget(QWidget *parent) :
	inherited(parent)
{
	m_WindowWidth = m_WindowHeight = 0;		// window sizes
    m_BackgroundColor = Qt::black;
}

//virtual
void cOpenGL_Widget::initializeGL()
{
	//makeCurrent();
	onInitialized();
	setDefaultView();
	setMouseTracking(isMouseTrackingNeeded());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

//virtual
void cOpenGL_Widget::resizeGL(int width, int height)
{
	m_WindowHeight = height;
	m_WindowWidth = width;
}

//virtual
void cOpenGL_Widget::mousePressEvent(QMouseEvent *pEvent)
{
	const int buttons(pEvent->buttons());
	const Qt::KeyboardModifiers mods(pEvent->modifiers());
	if (buttons & Qt::LeftButton)
	{
        m_LMouseButtonDrag.buttonPressed(pEvent);
    }
	if (buttons & Qt::RightButton)
	{
        m_RMouseButtonDrag.buttonPressed(pEvent);
    }

	inherited::mousePressEvent(pEvent);
}

//virtual
void cOpenGL_Widget::mouseReleaseEvent(QMouseEvent *pEvent)
{
	if (isLDragging())
	{
        m_LMouseButtonDrag.buttonReleased();
    }
	if (isRDragging())
	{
        m_RMouseButtonDrag.buttonReleased();
    }
	inherited::mouseReleaseEvent(pEvent);
}

//virtual
double cOpenGL_Widget::getAspectRatio() const
{
    return (m_WindowHeight == 0) ? m_WindowHeight : (double)m_WindowWidth/(double)m_WindowHeight;
}

/*unsigned int cOpenGL_Widget::create2DRasterFont(const char *pszFontName, int height)
{
	// Here we generate the lists for each character we want to use.
	unsigned int fontListID = glGenLists(256);					// Generate the list for the font

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
}*/

/*unsigned int cOpenGL_Widget::createVectorFont(const char *pszFontName, int height, GLYPHMETRICSFLOAT *pGlyphMetrics)
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
}*/

void cOpenGL_Widget::draw2DText(const char *pszText)
{
	glCallLists(strlen(pszText), GL_UNSIGNED_BYTE, pszText);
}

} // namespace ogl

} // namespace common

