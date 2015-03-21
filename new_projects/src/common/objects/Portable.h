/* Portable.h - header file for portable system API

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	30 Aug 2006 - initial creation
	30 Aug 2006 - critical sections, sleep
	20 Sep 2006 - LOGICAL data type, current time
	08 Feb 2008 - Locale-dependent date/time formats
	06 Jun 2008 - SRGBA structure
*/


#ifndef __PORTABLE_H_INCLUDED__
#define __PORTABLE_H_INCLUDED__

#include <vector>
#include <wtypes.h>

typedef long LOGICAL;
const LOGICAL LOGICAL_TRUE  = 1;
const LOGICAL LOGICAL_FALSE  = 0;

typedef unsigned long IPADDRESS_TYPE;
const char C_SLASH = '\\'; // Windows!

///////////////////////////////////////////////////////////////////
// CPortableCriticalSection - class for critical sections

class CPortableCriticalSection
{
//Constuctors/destructors
public:
	CPortableCriticalSection();
	virtual ~CPortableCriticalSection();

// Public methods
public:
	void Enter(char *pszText = NULL);
	void Leave(char *pszText = NULL);

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CRITICAL_SECTION m_CriticalSection;
	int m_Depth;

// Private methods
private:

// Private overridables
private:

// Private members
private:
};

class CStackCriticalSection
{
//Constuctors/destructors
public:
	CStackCriticalSection(CPortableCriticalSection *pRealSection, char *pszText/* = NULL*/);
	virtual ~CStackCriticalSection();

// Public methods
public:

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

// Private methods
private:

// Private overridables
private:

// Private members
private:
	CPortableCriticalSection *m_pRealSection;
};

///////////////////////////////////////////////////////////////////
// CPortableLocale - class for critical sections

class CPortableLocale
{
//Constuctors/destructors
public:
	CPortableLocale();
	virtual ~CPortableLocale();

// Public methods
public:
	char *GetShortDateFormatStr();
	char *GetTimeFormatStr();
	char *GetShortDateAndTimeFormatStr();

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void Init();
	void SystemLocaleChanged(); // should be called from "locale changed" hook
	char GetShortDateDelimeter();
	char GetTimeDelimeter();

// Protected overridables
protected:

// Protected members
protected:
	char m_szShortDateFormat[80], m_szLongDateFormat[80];
	char m_szTimeFormat[80];
	char m_szShortDateAndTimeFormat[80];
	char m_szTmpBuffer[80];

// Private methods
private:

// Private overridables
private:

// Private members
private:
};

typedef union SRGBA
{
//Constuctors/destructors
public:
	SRGBA();
	
// Public methods
public:
	void Init(SRGBA *pRGBA) { m_uiRGBA = pRGBA->m_uiRGBA; }
	void Init(SRGBA &RGBA) { Init(&RGBA); }
	void InitAndHighlite(SRGBA *pRGBA, double fFactor);
	void InitAndHighlite(SRGBA &RGBA, double fFactor) { InitAndHighlite(&RGBA, fFactor); }
	void Init(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
//	void Init(double r, double g, double b, double a = 1.);
	void Init(unsigned int RGBA) { m_uiRGBA = RGBA; }
	unsigned int GetRGBA() { return m_uiRGBA; }
	unsigned char *GetRGBAptr() { return m_ucRGBA; }
	unsigned char R() { return m_ucRGBA[0]; }
	unsigned char G() { return m_ucRGBA[1]; }
	unsigned char B() { return m_ucRGBA[2]; }
	unsigned char A() { return m_ucRGBA[3]; }

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	unsigned char m_ucRGBA[4];
	unsigned int m_uiRGBA;

// Private methods
private:

// Private overridables
private:

// Private members
private:
} SRGBA;

class CTextString
{
//Constuctors/destructors
public:
	CTextString();
	CTextString(const CTextString &AnotherString);
	CTextString(const char *pszAnotherString);
	virtual ~CTextString();

// Public methods
public:
	char *GetText() const { return m_pszText; }
	void Empty();
	LOGICAL IsEmpty() const { return (m_nTextLen == 0); }
	void SetLength(int nNewLength);
	void TrimRight(int nSymbols);
	void LeaveDirNameOnly();
	void MakeFilePathName(char *pszPath, char *pszFileName);
	CTextString &operator = (const CTextString &Src);
	CTextString &operator = (const char *pszSrc);
	const CTextString &operator += (char ch);
	const CTextString &operator += (const CTextString &Src);
	const CTextString &operator += (const char *pszString);
	LOGICAL operator == (const CTextString &AnoterString);
	LOGICAL operator == (const char *pszString);
	LOGICAL operator != (const CTextString &AnoterString);
	LOGICAL operator != (const char *pszString);
	LOGICAL IsEqualNoCase(const char *pszString) { return (stricmp(GetText(), pszString) == 0); }

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void Init();
	LOGICAL AllocBuffer(int nBufferLen);
	LOGICAL ReallocBuffer(int nRequiredBytes);

// Protected overridables
protected:

// Protected members
protected:
	char *m_pszText;
	int m_nTextLen;
	int m_nAllocatedBytes;

// Private methods
private:

// Private overridables
private:

// Private members
private:
};

class CListOfStrings : public std::vector<CTextString>
{
};

///////////////////////////////////////////////////////////////////
// Misc

void portableSleep(unsigned long uSleepTime);
void portableSwitchContext();
unsigned long portableGetCurrentTimeMsec();
COLORREF RGBAtoCOLORREF(SRGBA *pRGBA);


#endif //__PORTABLE_H_INCLUDED__
