/* Portable.cpp - implementation file for portable system API

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	30 Aug 2006 - initial creation
	30 Aug 2006 - critical sections
	20 Sep 2006 - LOGICAL data type, current time
	08 Feb 2008 - Locale-dependent date/time formats
	06 Jun 2008 - SRGBA structure

*/

#include "stdafx.h"
#include "Portable.h"

///////////////////////////////////////////////////////////////////
// CPortableCriticalSection - class for critical sections

CPortableCriticalSection::CPortableCriticalSection()
{
	InitializeCriticalSection(&m_CriticalSection);
	m_Depth = 0;
}

//virtual
CPortableCriticalSection::~CPortableCriticalSection()
{
//	Leave();
///	TRACE("~CPortableCriticalSection\n");
	DeleteCriticalSection(&m_CriticalSection);
}

void CPortableCriticalSection::Enter(char *pszText)
{
	if (pszText)
	{
//		TRACE("Entering %s....", pszText);
	}

	EnterCriticalSection(&m_CriticalSection);
	++m_Depth;
	if (pszText)
	{
//		TRACE("%s done (depth = %d)\n", pszText, m_Depth);
	}
}

void CPortableCriticalSection::Leave(char *pszText)
{
	if (pszText)
	{
//		TRACE("Leaving %s....", pszText);
	}
	LeaveCriticalSection(&m_CriticalSection);
	--m_Depth;
	if (pszText)
	{
//		TRACE("%s done (depth = %d)\n", pszText, m_Depth);
	}
}


///////////////////////////////////////////////////////////////////
// CStackCriticalSection - class for critical sections

CStackCriticalSection::CStackCriticalSection(CPortableCriticalSection *pRealSection, char *pszText)
{
	m_pRealSection = pRealSection;
//	ASSERT(m_pRealSection);
	m_pRealSection->Enter();
}

//virtual
CStackCriticalSection::~CStackCriticalSection()
{
	m_pRealSection->Leave();
}

/////////////////////////////////////////////////////////////////////
// CPortableLocale

CPortableLocale::CPortableLocale()
{
	Init();
}

//virtual
CPortableLocale::~CPortableLocale()
{
}

void CPortableLocale::Init()
{
	m_szTimeFormat[0] = 0;
	m_szLongDateFormat[0] = m_szShortDateFormat[0] = 0;
	m_szShortDateAndTimeFormat[0] = 0;
}

void CPortableLocale::SystemLocaleChanged()
{
	Init();
}

char CPortableLocale::GetShortDateDelimeter()
{
	char cDelimeter = '.';
	int nSymbols = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SDATE,
		m_szTmpBuffer, sizeof(m_szTmpBuffer));
	if (nSymbols)
	{
		cDelimeter = m_szTmpBuffer[0];
	}
	return cDelimeter;
}

char CPortableLocale::GetTimeDelimeter()
{
	char cDelimeter = ':';
	int nSymbols = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_STIME,
		m_szTmpBuffer, sizeof(m_szTmpBuffer));
	if (nSymbols)
	{
		cDelimeter = m_szTmpBuffer[0];
	}
	return cDelimeter;
}

char *CPortableLocale::GetShortDateFormatStr()
{
	if (m_szShortDateFormat[0] != 0)
		return m_szShortDateFormat;
	char cDelimeter = GetShortDateDelimeter();
	char c1 = 'd', c2 = 'm', c3 = 'Y';
	int nSymbols = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDATE,
		m_szTmpBuffer, sizeof(m_szTmpBuffer));
	if (nSymbols)
	{
		if (m_szTmpBuffer[0] == '0')
		{
			c1 = 'm'; c2 = 'd'; c3 = 'Y';
		}
		else if (m_szTmpBuffer[0] == '1')
		{
			c1 = 'd'; c2 = 'm'; c3 = 'Y';
		}
		else if (m_szTmpBuffer[0] == '2')
		{
			c1 = 'Y'; c2 = 'm'; c3 = 'd';
		}
	}
	
	sprintf(m_szShortDateFormat, "%%%c%c%%%c%c%%%c", c1, cDelimeter, c2, cDelimeter, c3);
	return m_szShortDateFormat;
}

char *CPortableLocale::GetTimeFormatStr()
{
	if (m_szTimeFormat[0] != 0)
		return m_szShortDateFormat;
	char cDelimeter = GetTimeDelimeter();

	LOGICAL b24Hour = LOGICAL_TRUE;
	int nSymbols = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_ITIME,
		m_szTmpBuffer, sizeof(m_szTmpBuffer));
	if (nSymbols)
	{
		if (m_szTmpBuffer[0] == '0')
			b24Hour = LOGICAL_FALSE;	
	}
	
	if (b24Hour)
		sprintf(m_szTimeFormat, "%%H%c%%M%c%%S", cDelimeter, cDelimeter);
	else
		sprintf(m_szTimeFormat, "%%I%c%%M%c%%S %%p", cDelimeter, cDelimeter);

	return m_szTimeFormat;
}

char *CPortableLocale::GetShortDateAndTimeFormatStr()
{
	if (m_szShortDateAndTimeFormat[0] != 0)
		return m_szShortDateAndTimeFormat;
	sprintf(m_szShortDateAndTimeFormat, "%s %s", GetShortDateFormatStr(), GetTimeFormatStr());
	return m_szShortDateAndTimeFormat;
}

///////////////////////////////////////////////////////////////////
// SRGBA

SRGBA::SRGBA()
{
	m_uiRGBA = 0;
}
	
void SRGBA::InitAndHighlite(SRGBA *pRGBA, double fFactor)
{
	unsigned char r, g, b, a;

	if (pRGBA->R() * fFactor > 0xFF)
		r = 0xFF;
	else
		r = (unsigned char)(pRGBA->R()*fFactor);

	if (pRGBA->G() * fFactor > 0xFF)
		g = 0xFF;
	else
		g = (unsigned char)(pRGBA->G()*fFactor);

	if (pRGBA->B() * fFactor > 0xFF)
		b = 0xFF;
	else
		b = (unsigned char)(pRGBA->B()*fFactor);

	if (pRGBA->A() * fFactor > 0xFF)
		a = 0xFF;
	else
		a = (unsigned char)(pRGBA->A()*fFactor);

	Init(r, g, b, a);
}

void SRGBA::Init(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (r > 0xFF)
		r = 0xFF;
	m_ucRGBA[0] = r;

	if (g > 0xFF)
		g = 0xFF;
	m_ucRGBA[1] = g;

	if (b > 0xFF)
		b = 0xFF;
	m_ucRGBA[2] = b;

	if (a > 0xFF)
		a = 0xFF;
	m_ucRGBA[3] = a;
}

/*unsigned char double2char(double x)
{
	if (x < 0.)
		x = 0.;
	unsigned char _x = (unsigned char)(x*256+0.49999);
	return _x;
}

void SRGBA::Init(double r, double g, double b, double a)
{
	Init(double2char(r), double2char(g), double2char(b), double2char(a));
}*/


/////////////////////////////////////////////////////////////////////
// CTextString

CTextString::CTextString()
{
	Init();
}

CTextString::CTextString(const CTextString &AnothetString)
{
	Init();
	*this = AnothetString;
}

CTextString::CTextString(const char *pszAnotherString)
{
	Init();
	*this = pszAnotherString;
}

void CTextString::Init()
{
	m_pszText = NULL;
	AllocBuffer(8);
	Empty();
}

//virtual
CTextString::~CTextString()
{
	if (m_pszText)
		free(m_pszText);
}

LOGICAL CTextString::AllocBuffer(int nBufferLen)
{
	char *pszNewBuffer = (char *)realloc(m_pszText, nBufferLen*sizeof(char));
	if (!pszNewBuffer)
		return LOGICAL_FALSE;
	m_pszText = pszNewBuffer;
	m_nAllocatedBytes = nBufferLen;
	return LOGICAL_TRUE;
}

LOGICAL CTextString::ReallocBuffer(int nRequiredBytes)
{
	if ((nRequiredBytes + 1) < m_nAllocatedBytes)
		return LOGICAL_TRUE; // do nothing as existing buffer is sufficient
	do
	{
		m_nAllocatedBytes *= 2;
	} while ((nRequiredBytes + 1) >= m_nAllocatedBytes);
	return AllocBuffer(m_nAllocatedBytes);
}

void CTextString::Empty()
{
	m_pszText[0] = 0;
	m_nTextLen = 0;
}

void CTextString::SetLength(int nNewLength)
{
	if (nNewLength > m_nTextLen)
	{ // just realloc, but no text operations
		ReallocBuffer(nNewLength);
	}
	else
	{
		m_nTextLen = nNewLength;
		m_pszText[m_nTextLen] = 0;
	}
}

void CTextString::TrimRight(int nSymbols)
{
	m_nTextLen -= nSymbols;
	if (m_nTextLen < 0)
		m_nTextLen = 0;
	m_pszText[m_nTextLen] = 0;
}

void CTextString::LeaveDirNameOnly()
{
	char *pszSlash = strrchr(GetText(), C_SLASH);
	if (!pszSlash)
		return;
	int nNewLength = pszSlash - m_pszText;
//	ASSERT(nNewLength >= 0);
	SetLength(nNewLength);
}

void CTextString::MakeFilePathName(char *pszPath, char *pszFileName)
{
	int nPathLen = strlen(pszPath);
	int nLen = nPathLen+1+strlen(pszFileName)+1;
	ReallocBuffer(nLen);
	strcpy(m_pszText, pszPath); // path
	m_pszText[nPathLen] = C_SLASH; // slash
	strcpy(m_pszText+nPathLen+1, pszFileName); // file name
}

CTextString &CTextString::operator = (const CTextString &Src)
{
	ReallocBuffer(Src.m_nTextLen);
	strcpy(m_pszText, Src.m_pszText);
	m_nTextLen = Src.m_nTextLen;
	return *this;
}

CTextString &CTextString::operator = (const char *pszSrc)
{
	int nTextLen = strlen(pszSrc);
	ReallocBuffer(nTextLen);
	strcpy(m_pszText, pszSrc);
	m_nTextLen = nTextLen;
	return *this;
}

const CTextString &CTextString::operator += (char ch)
{
	ReallocBuffer(m_nTextLen+1);
	m_pszText[m_nTextLen] = ch;
	++m_nTextLen;
	m_pszText[m_nTextLen] = 0;
	return *this;
}

const CTextString &CTextString::operator += (const CTextString &Src)
{
	ReallocBuffer(m_nTextLen + Src.m_nTextLen + 1);
	strcpy(m_pszText+m_nTextLen, Src.m_pszText);
	m_nTextLen += Src.m_nTextLen;
	return *this;
}

const CTextString &CTextString::operator += (const char *pszString)
{
	int nAddedTextLen = strlen(pszString);
	ReallocBuffer(m_nTextLen + nAddedTextLen + 1);
	strcpy(m_pszText+m_nTextLen, pszString);
	m_nTextLen += nAddedTextLen;
	return *this;
}

LOGICAL CTextString::operator == (const CTextString &AnotherString)
{
	if (m_nTextLen != AnotherString.m_nTextLen)
		return LOGICAL_FALSE;
	return (strcmp(m_pszText, AnotherString.m_pszText) == 0);
}

LOGICAL CTextString::operator == (const char *pszString)
{
	return (strcmp(m_pszText, pszString) == 0);
}

LOGICAL CTextString::operator != (const CTextString &AnotherString)
{
	if (m_nTextLen != AnotherString.m_nTextLen)
		return LOGICAL_TRUE;
	return (strcmp(m_pszText, AnotherString.m_pszText) != 0);
}

LOGICAL CTextString::operator != (const char *pszString)
{
	return (strcmp(m_pszText, pszString) != 0);
}

///////////////////////////////////////////////////////////////////
// Misc

void portableSleep(unsigned long uSleepTime)
{
	Sleep(uSleepTime);
}

void portableSwitchContext()
{
	Sleep(0);
}

unsigned long portableGetCurrentTimeMsec()
{
	return GetTickCount();
}

COLORREF RGBAtoCOLORREF(SRGBA *pRGBA)
{
	COLORREF color = RGB(pRGBA->R(), pRGBA->G(), pRGBA->B());
	return color;
}



