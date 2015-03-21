#include "stdafx.h"
#include "Bmp.h"

CBmp::CBmp()
{
//	m_RefsCount=0;
	ImageSize=0;
	m_hBitMap=NULL;
	lpDib=NULL;
	lpBits=NULL;
	m_palDIB = NULL;
	lpPalette=NULL;
}

WORD CBmp::GetBitCount()
{
	if (!lpDib) return 0;
	LPSTR lpbi=(LPSTR)lpDib;
	if (IS_WIN30_DIB(lpbi))
		return ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		return ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
}

/*************************************************************************
 *
 * GetNumColors()
 *
 * Description:
 *
 * This function calculates the number of colors in the DIB's color table
 * by finding the bits per pixel for the DIB (whether Win3.0 or other-style
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
 * if 24, no colors in color table.
 *
 ************************************************************************/
WORD CBmp::GetNumColors()
{
	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	if (!lpDib) return 0;
	LPSTR lpbi=(LPSTR)lpDib;
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 
	 return number of colors based on bits per pixel */
	switch (GetBitCount())
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}

/*************************************************************************
 *
 * CreateDIBPalette()
 *
 * Parameter:
 *
 * HDIB hDIB        - specifies the DIB
 *
 * Return Value:
 *
 * HPALETTE         - specifies the palette
 *
 * Description:
 *
 * This function creates a palette from a DIB by allocating memory for the
 * logical palette, reading and storing the colors from the DIB's color table
 * into the logical palette, creating a palette from this logical palette,
 * and then returning the palette's handle. This allows the DIB to be
 * displayed using the best possible colors (important for DIBs with 256 or
 * more colors).
 *
 ************************************************************************/


BOOL CBmp::CreateDIBPalette()
{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	HPALETTE hPal = NULL;    // handle to a palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPSTR lpbi;              // pointer to packed-DIB
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	BOOL bResult = FALSE;

	/* if handle to DIB is invalid, return FALSE */

	if (m_hBitMap== NULL)
	  return FALSE;

	if (lpPalette)
	{
		delete lpPalette;
		lpPalette=NULL;
	}
	
   lpbi = (LPSTR)lpDib;

   /* get pointer to BITMAPINFO (Win 3.0) */
   lpbmi = (LPBITMAPINFO)lpbi;

   /* get pointer to BITMAPCOREINFO (old 1.x) */
   lpbmc = (LPBITMAPCOREINFO)lpbi;

   /* get the number of colors in the DIB */
   wNumColors = GetNumColors();

   if (wNumColors != 0)
   {
		/* allocate memory block for logical palette */
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors);

		/* if not enough memory, clean up and return NULL */
		if (hLogPal == 0)
		{
			//::GlobalUnlock((HGLOBAL)m_hBitMap);
			return FALSE;
		}

		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);

		/* set version and number of palette entries */
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		/* is this a Win 3.0 DIB? */
		bWinStyleDIB = IS_WIN30_DIB(lpbi);
		lpPalette = new COLORREF[wNumColors];
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags =0;
			}
			lpPalette[i] = RGB(lpPal->palPalEntry[i].peRed,
							lpPal->palPalEntry[i].peGreen,
							lpPal->palPalEntry[i].peBlue);
		}
		/* create the palette and get handle to it */ 
		bResult = m_palDIB->CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
	}

	return bResult;
}

/*************************************************************************
 *
 * PaletteSize()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - size of the color palette of the DIB
 *
 * Description:
 *
 * This function gets the size required to store the DIB's palette by
 * multiplying the number of colors by the size of an RGBQUAD (for a
 * Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an other-
 * style DIB).
 *
 ************************************************************************/


 WORD CBmp::PaletteSize()
{
	if (!lpDib) return 0;
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpDib))
	  return (WORD)(GetNumColors() * sizeof(RGBQUAD));
   else
	  return (WORD)(GetNumColors() * sizeof(RGBTRIPLE));
}

/*************************************************************************
 *
 * FindDIBBits()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * LPSTR            - pointer to the DIB bits
 *
 * Description:
 *
 * This function calculates the address of the DIB's bits and returns a
 * pointer to the DIB bits.
 *
 ************************************************************************/


LPSTR CBmp::GetBits()
{
	if (!lpDib) return NULL;
	 LPSTR pStr=(LPSTR)lpDib;
	 long PalSize=PaletteSize();
 	 lpBits=pStr + *(LPDWORD)pStr +PalSize;
	 ImageBytes=ImageSize-*(LPDWORD)pStr-PalSize;
	 return lpBits;
}

/*************************************************************************
 *
 * GetImageWidth()
 *
 * This function gets the width of the DIB from the BITMAPINFOHEADER
 * width field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * width field if it is an other-style DIB.
 *
 ************************************************************************/
DWORD CBmp::GetImageWidth()
{
	if (!lpDib) return 0;
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether Win 3.0 and old) */

	lpbmi = (LPBITMAPINFOHEADER)lpDib;
	lpbmc = (LPBITMAPCOREHEADER)lpDib;

	/* return the DIB width if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDib))
		return lpbmi->biWidth;
	else  /* it is an other-style DIB, so return its width */
		return (DWORD)lpbmc->bcWidth;
}


/*************************************************************************
 *
 * DIBHeight()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - height of the DIB
 *
 * Description:
 *
 * This function gets the height of the DIB from the BITMAPINFOHEADER
 * height field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * height field if it is an other-style DIB.
 *
 ************************************************************************/


DWORD CBmp::GetImageHeight()
{
	if (!lpDib) return 0;
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether old or Win 3.0 */

	lpbmi = (LPBITMAPINFOHEADER)lpDib;
	lpbmc = (LPBITMAPCOREHEADER)lpDib;

	/* return the DIB height if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDib))
		return lpbmi->biHeight;
	else  /* it is an other-style DIB, so return its height */
		return (DWORD)lpbmc->bcHeight;
}

/*************************************************************************
 *
 * PaintDIB()
 *
 * Parameters:
 *
 * HDC hDC          - DC to do output to
 *
 * LPRECT lpDCRect  - rectangle on DC to do output to
 *
 * HDIB hDIB        - handle to global memory with a DIB spec
 *                    in it followed by the DIB bits
 *
 * LPRECT lpDIBRect - rectangle of DIB to output into lpDCRect
 *
 * CPalette* pPal   - pointer to CPalette containing DIB's palette
 *
 * Return Value:
 *
 * BOOL             - TRUE if DIB was drawn, FALSE otherwise
 *
 * Description:
 *   Painting routine for a DIB.  Calls StretchDIBits() or
 *   SetDIBitsToDevice() to paint the DIB.  The DIB is
 *   output to the specified DC, at the coordinates given
 *   in lpDCRect.  The area of the DIB to be output is
 *   given by lpDIBRect.
 *
 ************************************************************************/
BOOL CBmp::PaintDIB(CDC *pDC,
					LPRECT  lpDCRect,
					LPRECT  lpDIBRect)
{
	BOOL     bSuccess=FALSE;      // Success/fail flag
	CPalette *pOldPalette=NULL;        // Previous palette

	/* Check for valid DIB handle */
	if (m_hBitMap == NULL)
		return FALSE;

	// Get the DIB's palette, then select it into DC
	if (m_palDIB != NULL)
	{
		// Select as background since we have
		// already realized in forground if needed
		pOldPalette=pDC->SelectPalette(m_palDIB, TRUE);
		pDC->RealizePalette();
	}

	/* Make sure to use the stretching mode best for color pictures */
	pDC->SetStretchBltMode(COLORONCOLOR);

	/* Determine whether to call StretchDIBits() or SetDIBitsToDevice() */
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
	/*{
	bSuccess = ::SetDIBitsToDevice(pDC->m_hDC,                    // hDC
	   lpDCRect->left,             // DestX
	   lpDCRect->top,              // DestY
	   RECTWIDTH(lpDCRect),        // nDestWidth
	   RECTHEIGHT(lpDCRect),       // nDestHeight
	   lpDIBRect->left,            // SrcX
		0,
		0,                          // nStartScan
	   (WORD)GetImageHeight(),  // nNumScans
	   GetBits(),                  // lpBits
	   (LPBITMAPINFO)lpDib,     // lpBitsInfo
	   DIB_RGB_COLORS);            // wUsage 
	}*/
	{ //Specially for OCRUS
		int Width=GetImageWidth();
		int Height=GetImageHeight();
		CDC CompDC;
		CompDC.CreateCompatibleDC(pDC);
		lpDib->biHeight=-lpDib->biHeight;
		HBITMAP hBmp = CreateDIBitmap(pDC->m_hDC, lpDib, CBM_INIT, GetBits(),
			(LPBITMAPINFO)lpDib, DIB_RGB_COLORS);
		HBITMAP old = (HBITMAP)SelectObject(CompDC, hBmp);
		pDC->BitBlt(0, 0,	Width, Height, &CompDC, 0, 0, SRCCOPY);
		SelectObject(CompDC, old);
		DeleteObject(hBmp);
		DeleteDC(CompDC);
		lpDib->biHeight=-lpDib->biHeight;
	}
	else 
		bSuccess = ::StretchDIBits(pDC->m_hDC,                          // hDC
			lpDCRect->left,                 // DestX
			lpDCRect->bottom,                  // DestY
			RECTWIDTH(lpDCRect),            // nDestWidth
			-RECTHEIGHT(lpDCRect),           // nDestHeight
			lpDIBRect->left,                // SrcX
			lpDIBRect->top,                 // SrcY
			RECTWIDTH(lpDIBRect),           // wSrcWidth
			RECTHEIGHT(lpDIBRect),          // wSrcHeight
			GetBits(),                      // lpBits
			(LPBITMAPINFO)lpDib,         // lpBitsInfo
			DIB_RGB_COLORS,                 // wUsage
			SRCCOPY);                       // dwROP

	if (pOldPalette)
		pDC->SelectPalette(pOldPalette, TRUE);

	return bSuccess;
}

void CBmp::InitDIB()
{
	if (m_hBitMap == NULL) return;
	ImageSize=GlobalSize(m_hBitMap);
	delete m_palDIB;
	m_palDIB=NULL;
	delete lpPalette;
	lpPalette=NULL;

	// Set up document size
	lpDib = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL)m_hBitMap);
	GetBits();
	nHeight=(int)GetImageHeight();
	nWidth=(int)GetImageWidth();
	nBitCount=GetBitCount();
	DWORD w = nWidth*GetBitCount();
	if (w%16) w = (w/16+1)*16;
	if (w%32) w = (w/32+1)*32;
	nLine=w/8;
	//::GlobalUnlock((HGLOBAL)m_hBitMap); // Do not decrement lock count !!!
	// Create copy of palette
	m_palDIB = new CPalette;
	if (m_palDIB == NULL)
	{
		// we must be really low on memory
		::GlobalUnlock((HGLOBAL)m_hBitMap);
		::GlobalFree((HGLOBAL)m_hBitMap);
		m_hBitMap = NULL;
		return;
	}
	if (CreateDIBPalette() == NULL)
	{
		// DIB may not have a palette
		delete m_palDIB;
		m_palDIB = NULL;
		return;
	}
//	InvertImage(); // for Ocrus only
}

COLORREF CBmp::GetPixel(int x, int y)
{
	if (!lpDib) return 0;
	if (x<0 || x>=nWidth || y<0 || y>=nHeight) return 0xFFFFFFFF;
	BYTE nByte;//  = lpMask[(nHeight-1-y)*nLine+x/8];
	BYTE nIndex;// = (nByte>>(7-(x%8)))&0x01;
	switch (GetBitCount())
	{
		case 1:
			nByte = lpBits[(nHeight-1-y)*nLine+x/8];
			nIndex = (BYTE)((nByte>>(7-(x%8)))&0x01);
			break;
		case 4:
			nByte = lpBits[(nHeight-1-y)*nLine+x/2];
			nIndex = (BYTE)(x%2?(0x0F&nByte):nByte>>4);
			break;
		case 8:
			nIndex = (BYTE)(lpBits[(nHeight-1-y)*nLine+x]);
			break;
		case 24:
			/*return	lpBits[(nHeight-1-y)*nLine+x*3]<<16+
					lpBits[(nHeight-1-y)*nLine+x*3+1]<<8+
					lpBits[(nHeight-1-y)*nLine+x*3+2];*/
			return RGB(lpBits[(nHeight-1-y)*nLine+x*3+2],
				lpBits[(nHeight-1-y)*nLine+x*3+1],
				lpBits[(nHeight-1-y)*nLine+x*3]);
	}
	COLORREF RetValue=lpPalette[nIndex];
	return RetValue;
}

void CBmp::SetPixel(int x, int y, COLORREF c)
{
	if (x<0 || x>=nWidth || y<0 || y>=nHeight) return;
	switch (nBitCount)
	{
		case 1:
			if (c)  lpBits[(nHeight-1-y)*nLine+x/8] |=  (1<<(7-(x%8)));
			else	lpBits[(nHeight-1-y)*nLine+x/8] &= ~(1<<(7-(x%8)));
			break;
		case 4:
			if (c)  lpBits[(nHeight-1-y)*nLine+x/2] |=  (x%2?0x0F:0xF0);
			else	lpBits[(nHeight-1-y)*nLine+x/2] &= ~(x%2?0x0F:0xF0);
			break;
		case 8:
			lpBits[(nHeight-1-y)*nLine+x] = LOBYTE(LOWORD(c));//c?0xFF:0x00;
			break;
		case 24:
			{
			int shift = (nHeight-1-y)*nLine+x*3;
			lpBits[shift+2] = GetRValue(c);
			lpBits[shift+1] = GetGValue(c);
			lpBits[shift+0] = GetBValue(c);
			}
			break;
	}
};

BOOL CBmp::SaveAs(CString FileFullPath)
{
	if (!lpDib) return FALSE;
	BITMAPFILEHEADER BMPFileHeader;
	BMPFileHeader.bfType=('M'<<8)|'B';
	BMPFileHeader.bfReserved1=0;
	BMPFileHeader.bfReserved2=0;
	BMPFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+
		sizeof(RGBQUAD)*GetNumColors();
	BMPFileHeader.bfSize=BMPFileHeader.bfOffBits+ImageSize;
	if (FileFullPath.IsEmpty())
	{
		CFileDialog fd(FALSE, _T(".bmp"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Файлы битовых матриц (*.bmp)|*.bmp|Все файлы (*.*)|*.*||"));
		if (fd.DoModal()==IDCANCEL) return FALSE;
		FileFullPath=fd.GetPathName();
	}
	CFile file;
	file.Open(FileFullPath, CFile::modeCreate|CFile::modeWrite);
	file.Write(&BMPFileHeader, sizeof(BMPFileHeader));
//	file.WriteHuge(lpDib, ImageSize);
// Перешел на хранение изображения с выравниванием по WORD16!
	{
		file.Write(lpDib, sizeof(BITMAPINFOHEADER));
		file.Write((LPBYTE)lpDib+sizeof(BITMAPINFOHEADER), GetNumColors()*sizeof(PALETTEENTRY));
		DWORD Wbits=GetImageWidth()*GetBitCount(); //bits
		if (Wbits%32) Wbits = (Wbits/32+1)*32; //bits aligned
		nLine=Wbits/8; //bytes aligned
		lpBits=(LPSTR)lpDib+sizeof(BITMAPINFOHEADER)+GetNumColors()*sizeof(PALETTEENTRY)+nLine*(lpDib->biHeight-1);
		for (int i=0; i<lpDib->biHeight; i++)
		{
			file.WriteHuge(lpBits, nLine);
			lpBits-=nLine;   // Bonch
		}
	}
	file.Close();
	return TRUE;
}

#ifdef _DEBUG
void CBmp::AssertValid() const
{
//	CScrollView::AssertValid();
}

void CBmp::Dump(CDumpContext& dc) const
{
//	CScrollView::Dump(dc);
}
#endif //_DEBUG

BOOL CBmp::LoadFromFile(CString FileFullPath)
{
	Free();
	if (FileFullPath.IsEmpty())
	{
		CFileDialog fd(TRUE, _T(".bmp"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Файлы битовых матриц (*.bmp)|*.bmp|Все файлы (*.*)|*.*||"));

		if (fd.DoModal()==IDCANCEL) return FALSE;
		FileFullPath=fd.GetPathName();
	}
	CFile file;
	if (!file.Open(FileFullPath, CFile::modeRead)) return FALSE;
	BITMAPFILEHEADER BMPFileHeader;
	file.Read(&BMPFileHeader, sizeof(BITMAPFILEHEADER));
	if ( BMPFileHeader.bfType !=(('M'<<8)|'B'))
	{
		MessageBox(NULL, "Неправильный формат файла", "", MB_ICONEXCLAMATION);
		return FALSE;
	}
	ImageSize=file.SeekToEnd()-sizeof(BITMAPFILEHEADER);  // может быть больше!
	file.Seek(sizeof(BITMAPFILEHEADER),CFile::begin);

	m_hBitMap=(HBITMAP)GlobalAlloc(GHND, ImageSize);
	lpDib=(LPBITMAPINFOHEADER)GlobalLock(m_hBitMap);
	if ( lpDib==NULL ) return FALSE;
//	file.ReadHuge(lpDib, ImageSize);
	// For OCRUS ONLY !!!
// Перешел на хранение изображения с выравниванием по WORD16!
	{
		file.Read(lpDib, sizeof(BITMAPINFOHEADER));
		file.Read((LPBYTE)lpDib+sizeof(BITMAPINFOHEADER), GetNumColors()*sizeof(PALETTEENTRY));
		DWORD Wbits=GetImageWidth()*GetBitCount(); //bits
		if (Wbits%32) Wbits = (Wbits/32+1)*32; //bits aligned
		nLine=Wbits/8; //bytes aligned
		lpBits=(LPSTR)lpDib+sizeof(BITMAPINFOHEADER)+GetNumColors()*sizeof(PALETTEENTRY)+nLine*(lpDib->biHeight-1);
		for (int i=0; i<lpDib->biHeight; i++)
		{
			file.ReadHuge(lpBits, nLine);
			lpBits-=nLine;   // Bonch
		}
	}
	GlobalUnlock(m_hBitMap);
	InitDIB();
	return TRUE;
}

BOOL CBmp::Free()
{
//	MessageBox(NULL, NULL, "CBmp::Free()", MB_ICONINFORMATION);
	if (m_hBitMap==NULL) return TRUE;
	GlobalUnlock(m_hBitMap); // Decrement Lock Count
//	MessageBox(NULL, "GlobalUnlock-OK", "CBmp::Free()", MB_ICONINFORMATION);
	UINT RefCount=GlobalFlags(m_hBitMap) & GMEM_LOCKCOUNT;
	BOOL BitmapLocked=(RefCount!=0);
	if (!BitmapLocked) 
	{
		GlobalFree(m_hBitMap);
//		MessageBox(NULL, "GlobalFree-OK", "CBmp::Free()", MB_ICONINFORMATION);
	}
	m_hBitMap=NULL;
	lpDib=NULL;
	lpBits=NULL;
	if (!BitmapLocked && m_palDIB)
	{
		delete m_palDIB;
//		MessageBox(NULL, "Delete m_PalDIB-OK", "CBmp::Free()", MB_ICONINFORMATION);
	}

	m_palDIB=NULL;
	if (!BitmapLocked && lpPalette)
	{
		delete lpPalette;
//		MessageBox(NULL, "Delete lpPalette-OK", "CBmp::Free()", MB_ICONINFORMATION);
	}

	lpPalette=NULL;
	return TRUE;
}

BOOL CBmp::InvertPalette()
{
	int Colors=m_palDIB->GetEntryCount();//	Retrieves the number of palette entries in a logical palette.
	PALETTEENTRY *lpPalEntries=new PALETTEENTRY[Colors];
	UINT Success=m_palDIB->GetPaletteEntries(0, Colors, lpPalEntries);//	Retrieves a range of palette entries in a logical palette.
	if (!Success) return FALSE;
	for (int i=0; i<Colors; i++)
	{
		lpPalEntries[i].peRed=~lpPalEntries[i].peRed;
		lpPalEntries[i].peGreen=~lpPalEntries[i].peGreen;
		lpPalEntries[i].peBlue=~lpPalEntries[i].peBlue;
	}
	Success=m_palDIB->SetPaletteEntries(0, Colors, lpPalEntries);//	Sets RGB color values and flags in a range of entries in a logical
	delete lpPalEntries;
	return TRUE;
}

BOOL CBmp::InvertImage()
{
	for (long i=0; i<ImageBytes; i++)
		lpBits[i]=~lpBits[i];
	return TRUE;
}

BOOL CBmp::SetImage(CBmp &Bmp)
{
	Free();
	GlobalLock(Bmp.m_hBitMap); // Increment lock count
	m_hBitMap=Bmp.m_hBitMap;
//	GlobalLock(m_hBitMap); // Increment lock count
	ImageBytes=Bmp.ImageBytes;
	nBitCount=Bmp.nBitCount;
	nLine=Bmp.nLine;
	nHeight=Bmp.nHeight;
	nWidth=Bmp.nWidth;
	lpDib=Bmp.lpDib;
	lpBits=Bmp.lpBits;
	ImageSize=Bmp.ImageSize;
	m_palDIB=Bmp.m_palDIB;
	lpPalette=Bmp.lpPalette;
	return TRUE;
}

BOOL CBmp::SetImage(HBITMAP hBmp)
{
	Free();
	m_hBitMap=hBmp;
	InitDIB();
	return TRUE;
}

BOOL CBmp::AllocImage(int Width, int Height, int Bits)
{
	int rgb;
	Free();
	// память для зоны
	// sxx*syy - полутон, sxx*syy/8 - бинар
	// (с учетом выровненных на 8 значений xx, yy, sxx, syy)
	ImageSize=sizeof(BITMAPINFOHEADER);
	DWORD Wbits = Width*Bits, nLine;
	// Specially for OCRUS
	//if (Height % 8) Height = (Height/8+1)*8; //8 strings aligned

	if (Bits == 1)
	{
		Wbits = Width;
		ImageSize += 2*sizeof(PALETTEENTRY);
	}
	
	if (Bits == 8)
	{
		Wbits = Width*8;
		ImageSize += 256*sizeof(PALETTEENTRY);
	}

	if (Wbits % 32) Wbits = (Wbits/32+1)*32; //bits aligned
	nLine=Wbits/8; //bytes aligned
	if (Bits==1)
		Width=Wbits;
	if (Bits==8)
		Width=Wbits/8;
	ImageSize += Height*nLine;
	m_hBitMap = (HBITMAP)GlobalAlloc(GHND, ImageSize);
	if (m_hBitMap == NULL) return FALSE;
	lpDib = (LPBITMAPINFOHEADER)GlobalLock(m_hBitMap);
	if (lpDib==NULL) return NULL;
	LPBYTE pBuffer;
	lpDib->biSize=sizeof(BITMAPINFOHEADER);
	lpDib->biWidth=Width; 
	lpDib->biHeight=Height; 
	lpDib->biPlanes=1; 
	lpDib->biCompression=BI_RGB; 
	lpDib->biSizeImage=0; 
	lpDib->biXPelsPerMeter=0; 
	lpDib->biYPelsPerMeter=0; 
	lpDib->biClrUsed=0; 
	lpDib->biClrImportant=0;
	LPBYTE pPalette=(LPBYTE)lpDib+sizeof(BITMAPINFOHEADER);
	switch (Bits)
	{
		case 1:
			lpDib->biBitCount=1;
			pPalette[0]=255;
			pPalette[1]=255;
			pPalette[2]=255;
			pBuffer=pPalette+2*sizeof(PALETTEENTRY);
			break;
		case 8:
			lpDib->biBitCount=8;
			for (rgb=0; rgb<256; rgb++)
			{
				pPalette[sizeof(PALETTEENTRY)*rgb]=rgb;
				pPalette[sizeof(PALETTEENTRY)*rgb+1]=rgb;
				pPalette[sizeof(PALETTEENTRY)*rgb+2]=rgb;
			}
			pBuffer=pPalette+256*sizeof(PALETTEENTRY);
			break;
		case 24:
			lpDib->biBitCount=24;
			break;
		default:
			MessageBeep(0);
	}

	GlobalUnlock(m_hBitMap);
	InitDIB();
	return (BOOL)m_hBitMap;
}

static WORD tiffHeader[61] =			//длинной  122 байта.
{
	0x4949, 0x2a, 8, 0,   8,
	0x00FF,    3, 1, 0,   1,                        0,
	0x0100,    3, 1, 0,   0/*points in line [15]*/, 0,
	0x0101,    3, 1, 0,   0/*number of lines[21]*/, 0,
	0x0102,    3, 1, 0,   1,                        0,
	0x0106,    3, 1, 0,   1,                        0,
	0x0111,    4, 1, 0, 122,                        0,
	0x011A,    5, 1, 0, 106,                        0,
	0x011B,    5, 1, 0, 114,                        0,
	200, 0, 1, 0,
	200, 0, 1, 0			//, data ...
};

BOOL CBmp::SaveAsTiff(CString FileFullPath)
{
	if (!lpDib) return FALSE;
	if (FileFullPath.IsEmpty())
	{
		CFileDialog fd(FALSE, _T(".tif"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Файлы tiff (*.tiff)|*.tiff|Все файлы (*.*)|*.*||"));
		if (fd.DoModal()==IDCANCEL) return FALSE;
		FileFullPath=fd.GetPathName();
	}
	CFile file;
	file.Open(FileFullPath, CFile::modeCreate|CFile::modeWrite);
	tiffHeader[15] = (WORD)GetImageWidth();
	tiffHeader[21] = (WORD)GetImageHeight();
	file.Write(tiffHeader, sizeof(tiffHeader));
	file.WriteHuge(lpBits, ImageBytes);
	file.Close();
	return TRUE;
}

int CBmp::AlignDWORD(int ToAlign, int Bits)
{
	int Aligned, Wbits;
	switch (Bits)
	{
		case 1:
			Wbits=ToAlign;
			break;
		case 8:
			Wbits=ToAlign*8;
			break;
		default :
			MessageBeep(0);
	}
	
	if (Wbits%32) Wbits = (Wbits/32+1)*32; //bits aligned
	switch (Bits)
	{
		case 1:
			Aligned=Wbits;
			break;
		case 8:
			Aligned=Wbits/8;
			break;
		default :
			Aligned=0;
	}
	return Aligned;
}

void CBmp::XOR(CBmp &Bmp1, CBmp &Bmp2)
{
	int w = max(Bmp1.nWidth, Bmp2.nWidth);
	int h = max(Bmp1.nHeight, Bmp2.nHeight);
	int bits = max(Bmp1.nBitCount, Bmp2.nBitCount);
	AllocImage(w, h, bits);

	for (int i = 0; i < (w-1); i++)
	{
		for (int j = 0; j < (h-1); j++)
		{
			COLORREF pixel1 = Bmp1.GetPixel(i, j);
			COLORREF pixel2 = Bmp2.GetPixel(i, j);
			COLORREF xor = pixel1 ^ pixel2;
			SetPixel(i, j, xor);
		}
	}
}
