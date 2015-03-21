#include "design_text_string.h"
#include <string.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// CDesignTextString

static const char *PSZ_EQUALS_KEYWORD = " = ";
extern const char *PSZ_RGBA_FORMAT_STRING;



void CDesignTextString::Set(const char *pszParamName, const char *pszTextValue)
{
    Init();
    *this += pszParamName;
    *this += PSZ_EQUALS_KEYWORD;
    const char *pcBlank = strchr(pszTextValue, ' ');
    if (pcBlank)
        *this += '"';
    *this += pszTextValue;
    if (pcBlank)
        *this += '"';
}

void CDesignTextString::Set(const char *pszParamName, common::portable::SRGBA *pColor)
{
    Init();
    *this += pszParamName;
    *this += PSZ_EQUALS_KEYWORD;
    char szColor[64];
    *this += '"';
    sprintf(szColor, PSZ_RGBA_FORMAT_STRING, pColor->R(), pColor->G(), pColor->B(), pColor->A());
    *this += szColor;
    *this += '"';
}

void CDesignTextString::Set(const char *pszParamName, double fValue)
{
    Init();
    *this += pszParamName;
    *this += PSZ_EQUALS_KEYWORD;
    char szValue[64];
    sprintf(szValue, "%e", fValue);
    *this += szValue;
}

