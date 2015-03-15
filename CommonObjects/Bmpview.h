// BmpView.h : header file
//
#ifndef __BMPVIEW__
#define __BMPVIEW__

//DECLARE_HANDLE(HDIB);

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.

//#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

#include "Bmp.h"

/////////////////////////////////////////////////////////////////////////////
// CBitmapView view

class CBitmapView : public CScrollView
{
protected:
	CPoint ClientToBitmap(CPoint ClientPoint);
	CRect ClientToBitmap(CRect ClientRect);
	CPoint m_Origin;
	int m_ZoomPercent;
	CBmp m_Bmp;
	void DoRealizePalette();
	void InitDIB();
	CSize m_TotalSize;
	DECLARE_DYNCREATE(CBitmapView)
public: 
// Attributes
	//char fname[MAX_PATH];
	CString FileName;
public:
	CPoint GetMousePlacement();
	BOOL SetOrigin(CPoint Origin, BOOL bRedraw);
	BOOL SetZoom(int ZoomPercent, BOOL bRedraw=TRUE);
	BOOL InvertImage() { m_Bmp.InvertImage(); Invalidate(); UpdateWindow(); return TRUE; }
	BOOL InvertPalette() { m_Bmp.InvertPalette(); 	DoRealizePalette(); return TRUE; }
	virtual BOOL Free();
	CBitmapView();
	LPSTR GetBits() { return m_Bmp.GetBits(); };
	WORD GetBitCount()  { return m_Bmp.GetBitCount(); };
	WORD GetNumColors() { return m_Bmp.GetNumColors(); };
	CBmp *GetBmp() { return &m_Bmp; }
	DWORD GetImageHeight() { return m_Bmp.GetImageHeight(); };
	DWORD GetImageWidth() { return m_Bmp.GetImageWidth(); };
	COLORREF GetPixel(int x,int y)  { return m_Bmp.GetPixel(x, y); };
	COLORREF GetPixel(CPoint p) { return GetPixel(p.x,p.y); }
	void SetPixel(int x,int y,int c)  { m_Bmp.SetPixel(x, y, c); }
	BOOL SaveAs(CString FileFullPath="") { return m_Bmp.SaveAs(FileFullPath); }
	BOOL LoadFromFile(CString FileFullPath="");
// Operations
public:
	BOOL SetImage(HBITMAP hBitMap);
	BOOL SetImage(CBmp &Bmp);
	BOOL SetImage(CBmp *pBmp) { return SetImage(*pBmp);}
	void UpdateScrollSizes();
// Overrides
	//{{AFX_VIRTUAL(CBitmapView)
	protected:
	virtual void OnDraw(CDC* pDC); 
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBitmapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CBitmapView)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // __BMPVIEW__
