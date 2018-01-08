/* 3DRenderer.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	?? ??? ???? - initial creation
	03 0ct 2008 - Cursors

*/

#include "stdafx.h"
#include <math.h>
#include "3DRenderer.h"

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
}

///////////////////////////////////////////////////////
// class C3DRenderer implementation

C3DRenderer::C3DRenderer() : CRenderer()
{
	m_bTrackballEnabled=TRUE;
	m_TrackballSensitivity = 1.f;				// 0...1

	m_CameraX=m_CameraY=0.f;
	m_CameraZ=-30.f;
	m_CameraTilt=0.f;	
	m_CameraAzimuth=0.f;
	m_WorldX=m_WorldY=0.f;
	m_WorldZ=0.f;
	m_WorldRx=m_WorldRy=0.f;
	m_WorldRz=0.f;
	CameraToWorldCenter();

}

//virtual
C3DRenderer::~C3DRenderer()
{
}

void C3DRenderer::MoveCameraForward(double Distance, BOOL bRedraw)
{
	double CosAngle = cos(m_CameraAngle);
	double SinAngle = sin(m_CameraAngle);
	double CosAzimuth = cos(m_CameraAzimuth);
	double SinAzimuth = sin(m_CameraAzimuth);
	double dx = -Distance*CosAngle*SinAzimuth;
	double dy = Distance*SinAngle;
	double dz = Distance*CosAngle*CosAzimuth;
	m_CameraX += dx;
	m_CameraY += dy;
	m_CameraZ += dz;
	OnCameraPosChanged();
	if (bRedraw)	
		Redraw();
}

void C3DRenderer::MoveCamera(double dX, double dY, double dZ, BOOL bRedraw)
{
	m_CameraX += dX;
	m_CameraY += dY;
	m_CameraZ += dZ;
	OnCameraPosChanged();
	if (bRedraw)	
		Redraw();
}

void C3DRenderer::SetCameraPos(double CameraX, double CameraY, double CameraZ, BOOL bRedraw)
{
	m_CameraX = CameraX;
	m_CameraY = CameraY;
	m_CameraZ = CameraZ;
	RecalcCameraAngles();
	OnCameraPosChanged();
}

void C3DRenderer::SetCameraDir(double CameraAzimuth, double CameraAngle, double CameraTilt, BOOL bRedraw)
{
	m_CameraAzimuth = CameraAzimuth;
	m_CameraAngle = CameraAngle;
	m_CameraTilt = CameraTilt;
	OnCameraDirChanged();
}

void C3DRenderer::SetWorldPos(double WorldX, double WorldY, double WorldZ, BOOL bRedraw)
{
	m_WorldX=WorldX;
	m_WorldY=WorldY;
	m_WorldZ=WorldZ;
	OnWorldPosChanged();
}

void C3DRenderer::SetWorldDir(double WorldRx, double WorldRy, double WorldRz, BOOL bRedraw)
{
	m_WorldRx=WorldRx;
	m_WorldRy=WorldRy;
	m_WorldRz=WorldRz;
	OnWorldDirChanged();
}

void C3DRenderer::CameraToWorldCenter(BOOL bRedraw)
{
	RecalcCameraAngles();
	OnCameraPosChanged();
	OnCameraDirChanged();
}

void C3DRenderer::RecalcCameraAngles()
{
	double dX = m_WorldX-m_CameraX;
	double dY = m_WorldY-m_CameraY;
	double dZ = m_WorldZ-m_CameraZ;
	double dR = sqrt(dX*dX+dZ*dZ);
	m_CameraAzimuth = atan2(-dX, dZ);
	m_CameraAngle = atan2(dY, dR);
}

void C3DRenderer::NormalizeCameraDir()
{
	while (m_CameraAzimuth>180.f) m_CameraAzimuth-=360.f;
	while (m_CameraAzimuth<-180.f) m_CameraAzimuth+=360.f;
	while (m_CameraAngle>180.f) m_CameraAngle-=360.f;
	while (m_CameraAngle<-180.f) m_CameraAngle+=360.f;
	while (m_CameraTilt>180.f) m_CameraTilt-=360.f;
	while (m_CameraTilt<-180.f) m_CameraTilt+=360.f;
}

void C3DRenderer::NormalizeWorldDir()
{
	while (m_WorldRx>180.f) m_WorldRx-=360.f;
	while (m_WorldRx<-180.f) m_WorldRx+=360.f;
	while (m_WorldRy>180.f) m_WorldRy-=360.f;
	while (m_WorldRy<-180.f) m_WorldRy+=360.f;
	while (m_WorldRz>180.f) m_WorldRz-=360.f;
	while (m_WorldRz<-180.f) m_WorldRz+=360.f;
}

//void C3DRenderer::OnLButtonUp(UINT nFlags, POINT point)
//{
//	m_bLDragging=FALSE;
//}

/*void C3DRenderer::OnMouseMove(UINT nFlags, POINT point)
{
	if (m_bLDragging)
	{
		double dx=m_CameraX-m_WorldX;
		double dy=m_CameraY-m_WorldY;
		double dz=m_CameraZ-m_WorldZ;
		double distance=sqrt(dx*dx+dy*dy+dz*dz);
		double speed=fabs(distance);
		if (speed<1.f)
			speed=1.f;
		if (nFlags & MK_SHIFT)
		{ // fast camera moving
			double Distance = sqrt(speed)*(m_Ly-point.y)/m_WindowHeight*GetStepForward();
			MoveCameraForward(Distance);
		}
		else
		{
			double RotationDir; // To work with left and right coordinate systems
			RotationDir=-1.;
			
			double CosAngle=cos(m_CameraAngle*grad2rad);
			double SinAngle=sin(m_CameraAngle*grad2rad);
			double CosAzimuth=cos(m_CameraAzimuth*grad2rad);
			double SinAzimuth=sin(m_CameraAzimuth*grad2rad);
			if (nFlags & MK_CONTROL)
			{
				double SpeedLeftRight=25.f*(point.x-m_Lx)*GetStepForward()/m_WindowWidth;
				double SpeedUpDown=25.f*(m_Ly-point.y)*GetStepForward()/m_WindowHeight;
				m_WorldX += SpeedLeftRight*CosAzimuth + SpeedUpDown*SinAzimuth*SinAngle;
				m_WorldY +=							    SpeedUpDown*CosAngle;
				m_WorldZ += RotationDir*(SpeedLeftRight*SinAzimuth-SpeedUpDown*SinAngle*CosAzimuth);
			}
			else
			{
				if (IsTrackballEnabled())
				{
					// Сначала сверху вниз
					double AngleUpDown=(point.y-m_Ly)*PI/m_WindowHeight*GetTrackballSensitivity(); // сразу в радианах
					double dRx=AngleUpDown;//*CosAzimuth;
					double dRy=0.f;
					double dRz=0.f;//AngleUpDown*SinAzimuth;

					// Теперь справа налево
					double AngleLeftRight=(point.x-m_Lx)*PI/m_WindowWidth*GetTrackballSensitivity(); // сразу в радианах
					dRx+=0.f;//AngleLeftRight;//*SinAngle*SinAzimuth;
					dRy+=AngleLeftRight;//*CosAngle;
					dRz+=0.f;//AngleLeftRight*(-SinAngle*CosAzimuth);

					m_WorldRx+=RotationDir*dRx*rad2grad;
					m_WorldRy+=-RotationDir*dRy*rad2grad;
					m_WorldRz+=RotationDir*dRz*rad2grad;
				} //if (IsTrackballEnabled())
			} // else
		} // else

		m_Lx=point.x;
		m_Ly=point.y;
		Update();
	}

} */

/*void C3DRenderer::OnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags)
{
	if (!IsKbdNavigationEnabled()) return;
	switch (nChar)
	{
	case VK_LEFT:
		m_CameraAzimuth+=GetStepAngles();
		OnCameraDirChanged();
		Update();
		break;
	case VK_RIGHT:
		m_CameraAzimuth-=GetStepAngles();
		OnCameraDirChanged();
		Update();
		break;
	case VK_UP:
		m_CameraAngle+=GetStepAngles();
		OnCameraDirChanged();
		Update();
		break;
	case VK_DOWN:
		m_CameraAngle-=GetStepAngles();
		OnCameraDirChanged();
		Update();
		break;
	case VK_MULTIPLY:
		MoveCameraForward(GetStepForward());
		break;
	case VK_DIVIDE:
		MoveCameraForward(-GetStepForward());
		break;
	default:
		break;
	} // end of switch
} */


