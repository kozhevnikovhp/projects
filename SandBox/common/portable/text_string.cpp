#include "text_string.h"
#include <string.h>
#include <stdlib.h>

namespace common {

namespace portable {

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

bool CTextString::AllocBuffer(int nBufferLen)
{
        char *pszNewBuffer = (char *)realloc(m_pszText, nBufferLen*sizeof(char));
        if (!pszNewBuffer)
                return false;
        m_pszText = pszNewBuffer;
        m_nAllocatedBytes = nBufferLen;
        return true;
}

bool CTextString::ReallocBuffer(int nRequiredBytes)
{
        if ((nRequiredBytes + 1) < m_nAllocatedBytes)
                return true; // do nothing as existing buffer is sufficient
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
		char *pszSlash = strrchr(GetText(), SUBDIRECTORIES_SEPARATOR);
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
		m_pszText[nPathLen] = SUBDIRECTORIES_SEPARATOR; // slash
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

CTextString &CTextString::operator = (char *pszSrc)
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

bool CTextString::operator == (const CTextString &AnotherString)
{
        if (m_nTextLen != AnotherString.m_nTextLen)
                return false;
        return (strcmp(m_pszText, AnotherString.m_pszText) == 0);
}

bool CTextString::operator == (const char *pszString)
{
        return (strcmp(m_pszText, pszString) == 0);
}

bool CTextString::operator != (const CTextString &AnotherString)
{
        if (m_nTextLen != AnotherString.m_nTextLen)
                return true;
        return (strcmp(m_pszText, AnotherString.m_pszText) != 0);
}

bool CTextString::operator != (const char *pszString)
{
        return (strcmp(m_pszText, pszString) != 0);
}

bool CTextString::IsEqualNoCase(const char *pszString)
{
    return (strcmp(GetText(), pszString) == 0); // TODO!!!! strIcmp must be here
}


} // namespace portable

} // namespace common

