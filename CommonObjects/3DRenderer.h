/* 3DRenderer.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	?? ??? ???? - initial creation
	03 0ct 2008 - Cursors

*/

#ifndef __3D_RENDERER__
#define __3D_RENDERER__

#include "stdafx.h"
#include "Portable.h"
#include "Calculations.h"


typedef enum
{
	CURSOR_ARROW,		// Standard arrow cursor
	CURSOR_IBEAM,		// Standard text-insertion cursor
	CURSOR_WAIT,		// Hourglass cursor used when Windows performs a time-consuming task
	CURSOR_CROSS,		// Cross-hair cursor for selection
	CURSOR_UPARROW,		// Arrow that points straight up
	CURSOR_SIZEALL,		// A four-pointed arrow. The cursor to use to resize a window.
	CURSOR_SIZENWSE,	// Two-headed arrow with ends at upper left and lower right
	CURSOR_SIZENESW,	// Two-headed arrow with ends at upper right and lower left
	CURSOR_SIZEWE,		// Horizontal two-headed arrow
	CURSOR_SIZENS		// Vertical two-headed arrow 		
} CursorTypeEnum;

class CRenderer
{
// Constructors/destructors
public:
	CRenderer();
	virtual ~CRenderer();

// Public methods
public:
	// Drawing
	virtual LOGICAL Init(HDC hDC) { return LOGICAL_TRUE; }
	virtual void Finit() {}
	virtual void SetDefaultView() = 0;
	virtual void Redraw() = 0;	// only drawing, must be overriden

	// Cursor
	virtual CursorTypeEnum GetCursor() { return CURSOR_ARROW; }
	// Message handlers
	virtual void OnSize(int cx, int cy) = 0;
	virtual void OnLButtonDown(unsigned int uFlags, int x, int y) = 0;
	virtual void OnLButtonUp(unsigned int uFlags, int x, int y) = 0;
	virtual void OnRButtonDown(unsigned int uFlags, int x, int y) = 0;
	virtual void OnRButtonUp(unsigned int uFlags, int x, int y) = 0;
	virtual void OnMouseMove(unsigned int uFlags, int x, int y) = 0;
	virtual void OnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags) {}

// Protected methods

protected:
};

class C3DRenderer : public CRenderer
{
// Constructors/destructors
public:
	C3DRenderer();
	virtual ~C3DRenderer();

// Public methods
public:
	// Track ball
	inline void EnableTrackball(BOOL bEnabled) {m_bTrackballEnabled=bEnabled;}
	inline BOOL IsTrackballEnabled() {return m_bTrackballEnabled;}
	inline void SetTrackballSensitivity(double Sensitivity) { m_TrackballSensitivity=Sensitivity; }
	inline double GetTrackballSensitivity() { return m_TrackballSensitivity; }

	// Camera operations
	void MoveCameraForward(double Distance, BOOL bRedraw = TRUE);
	void MoveCamera(double dX, double dY, double dZ, BOOL bRedraw = TRUE);
	void GetCameraPos(double &CameraX, double &CameraY, double &CameraZ) { CameraX=m_CameraX; CameraY=m_CameraY; CameraZ=m_CameraZ; }
	void GetCameraDir(double &CameraAzimuth, double &CameraAngle, double &CameraTilt) { CameraAzimuth=m_CameraAzimuth; CameraAngle=m_CameraAngle; CameraTilt=m_CameraTilt; }
	void GetWorldPos(double &WorldX, double &WorldY, double &WorldZ) { WorldX=m_WorldX; WorldY=m_WorldY; WorldZ=m_WorldY; }
	void GetWorldDir(double &WorldRx, double &WorldRy, double &WorldRz) { WorldRx=m_WorldRx; WorldRy=m_WorldRy; WorldRz=m_WorldRz; }
	void SetCameraPos(double CameraX, double CameraY, double CameraZ, BOOL bRedraw=TRUE);
	void SetCameraDir(double CameraAzimuth, double CameraAngle, double CameraTilt, BOOL bRedraw=TRUE);
	void SetWorldPos(double WorldX, double WorldY, double WorldZ, BOOL bRedraw=TRUE);
	void SetWorldDir(double WorldRx, double WorldRy, double WorldRz, BOOL bRedraw=TRUE);
	void CameraToWorldCenter(BOOL bRedraw=TRUE);


// Overridable methods
public:
	virtual void OnCameraPosChanged() {};
	virtual void OnCameraDirChanged() {};
	virtual void OnWorldPosChanged() {};
	virtual void OnWorldDirChanged() {};


// Protected overridable methods
protected:
	// Event handlers


// Protected methods
protected:
	void NormalizeCameraDir();
	void NormalizeWorldDir();
	void RecalcCameraAngles();
	
// Protected members
protected:
	// Track ball
	BOOL m_bTrackballEnabled;
	double m_TrackballSensitivity;				// 0...1

	double m_CameraX, m_CameraY, m_CameraZ;	// camera position
	double m_CameraAngle, m_CameraAzimuth, m_CameraTilt;	// camera orientation
	double m_WorldX, m_WorldY, m_WorldZ;		// world position
	double m_WorldRx, m_WorldRy, m_WorldRz;		// world orientation;
};

#endif //__3D_RENDERER__