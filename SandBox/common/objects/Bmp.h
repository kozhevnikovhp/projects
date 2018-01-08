#ifndef __CBMP__
#define __CBMP__
#define new DEBUG_NEW

/* DIB constants */
#define PALVERSION   0x300

/* DIB Macros*/
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// этот класс полезен для хранения изображений в dib-формате
// он использует некоторые объекты MFC (CDC, CPalette, CFileDialog etc.)
// При компиляции в СОМ - сервер код, использующий эти объекты, можно смело удалять
// (там это и не требуется - проверено-мин нет)
// Похожий класс есть в примере DIBLOOK - там много комментариев

class CBmp
{
protected:
	long ImageBytes;
	int nBitCount;
	int nLine;
	int nHeight;
	int nWidth;
	LPBITMAPINFOHEADER lpDib;
	char *lpBits;
	UINT ImageSize;
	BOOL CreateDIBPalette();
	WORD PaletteSize();
	HBITMAP m_hBitMap;
	CPalette *m_palDIB;
	COLORREF *lpPalette;
public: 
// Attributes
public:
	CPalette *GetPalette() { return m_palDIB; }
	BOOL PaintDIB(CDC *pDC, LPRECT  lpDCRect, LPRECT  lpDIBRect);
	BOOL IsImage() { return (m_hBitMap!=0 && lpDib!=NULL); }
	void InitDIB();
	BOOL InvertImage();
	BOOL InvertPalette();
	BOOL Free();
	CBmp();
	virtual ~CBmp() { Free(); }
	LPSTR GetBits();
	WORD GetBitCount();
	WORD GetNumColors();
	DWORD GetImageHeight();
	DWORD GetImageWidth();
	COLORREF GetPixel(int x,int y);
	COLORREF GetPixel(CPoint p) { return GetPixel(p.x, p.y); }
	void SetPixel(int x, int y, COLORREF c);
	BOOL SaveAs(CString FileFullPath="");
	BOOL SaveAsTiff(CString FileFullPath="");
	BOOL LoadFromFile(CString FileFullPath="");
	BOOL AllocImage(int Width, int Height, int BitsPerPixel);
	BOOL SetImage(CBmp &Bmp);
	BOOL SetImage(CBmp *pBmp) { return SetImage(*pBmp);}
	BOOL SetImage(HBITMAP hBitMap);
	void XOR(CBmp &Bmp1, CBmp &Bmp2);

// Implementation
protected:
	int AlignDWORD(int ToAlign, int Bits);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif //__CBMP__
