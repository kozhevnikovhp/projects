// AnimateIcon.cpp : implementation of the CAnimateIcon class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
#include "AnimatedIcon.h"

// default constructor
CAnimatedIcon::CAnimatedIcon()
{
	m_ImageCounter = -1;
	m_MaxNoOfImages = -99;
	m_ImageList.m_hImageList = NULL;
}

// default do nothing destructor
CAnimatedIcon::~CAnimatedIcon()
{
	if (hPrevIcon) 
		DestroyIcon(hPrevIcon);
}

// This is the first function which needs to be called in order
// to fill the image list
// Parameters :
// ------------
// int IDOfImgListResource - pass the Resource ID of a toolbar resource
//                           containing the image list
// int numberOfImages      - Number of images (16x16) in the toolbar resource
// transparentColor        - RGB value of color you want to be transparent
BOOL CAnimatedIcon::SetImageList(int IDOfImgListResource,int numberOfImages,COLORREF transparentColor)
{
	if(numberOfImages <= 0)
		return FALSE;
	m_MaxNoOfImages = numberOfImages;
	VERIFY(m_ImageList.Create(IDOfImgListResource,16,1,transparentColor));
	return TRUE;
}

// This function needs to be called repetatively to show next image
// Parameters :
// ------------
// NONE

BOOL CAnimatedIcon::ShowNextImage()
{
	if(m_ImageList.m_hImageList == NULL)
		return FALSE;
	m_ImageCounter++;
	m_ImageCounter %= m_MaxNoOfImages;
	// extract the icon from imagelist
	hIcon = m_ImageList.ExtractIcon(m_ImageCounter);
	// send the message to frame to update icon
	HICON hPrevIcon = (HICON) 	AfxGetMainWnd()->SendMessage(WM_SETICON,TRUE,(LPARAM)hIcon);	
	// Free the previous icon resource
	if (hPrevIcon) 	
	    DestroyIcon(hPrevIcon);
	return TRUE;
}