// AnimateIcon.h : interface of the CAnimatedIcon class
/*Usage: 

1. Add AnimateIcon.cpp and AnimateIcon.h files in your project. 

2. Create a bitmap resource and put all the images (16 pixels x 16 pixels) in the desired sequence. 



3. Add #include "AnimatedIcon.h" at the beginning of your mainframe.h file.

4. Add the following protected members to your mainframe class 


    CAnimatedIcon m_animIcon;
    UINT m_timerID;
5. Add the following code in OnCreate member of your frame class 

    m_animIcon.SetImageList(IDB_ANIM_IMGLIST,4,RGB(0,0,0));
    m_timerID = this->SetTimer(99,500,NULL);
SetImageList takes 3 parameters pass ID of your bitmap resource created in step 2 as first parameter. The second parameter is the no of (16x16) images in your bitmap, which is 4 in our case. The last parameter is the RGB value of color you want to make transparent. Second line of code sets a new timer. Here I have made it to fire every 500 ms. you can change it to suit your animation speed. 
6. Create a OnDestroy()handler for your main frame class and add the following code. This is very important because if you do not kill the timer created you will lose system resources. 


    CFrameWnd::OnDestroy();
    if(m_timerID != 0)
        KillTimer(m_timerID);
7. Add WM_TIMERhandler to your frame class and add the following code 


    m_animIcon.ShowNextImage();
*/
#if !defined(AFX_ANIMATEICON_H__47E058AD_6F69_11D2_B59C_86DD54033006__INCLUDED_)
#define      AFX_ANIMATEICON_H__47E058AD_6F69_11D2_B59C_86DD54033006__INCLUDED_
#include <afxcmn.h>

class CAnimatedIcon
{
protected :
	// variable to hold the image list containing a series
	// of icons
	CImageList  m_ImageList;
	// keep track of which image is current
	int m_ImageCounter;
	// store the max nos of images
	int m_MaxNoOfImages;
	HICON hIcon;
	HICON hPrevIcon;
		
public:
	CAnimatedIcon();
	~CAnimatedIcon();
	BOOL SetImageList(int IDOfImgListResource,int numberOfImages,COLORREF transparentColor);
	BOOL ShowNextImage();
};

#endif //#define AFX_ANIMATEICON_H__47E058AD_6F69_11D2_B59C_86DD54033006__INCLUDED_