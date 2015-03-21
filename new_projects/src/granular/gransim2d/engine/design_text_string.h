#ifndef DESIGN_TEXT_STRING_H
#define DESIGN_TEXT_STRING_H

#include "portable/text_string.h"
#include "portable/rgba.h"

class CDesignTextString : public common::portable::CTextString
{
// Public methods
public:
    void Set(const char *pszParamName, const char *pszTextValue);
    void Set(const char *pszParamName, common::portable::SRGBA *pColor);
    void Set(const char *pszParamName, double fValue);

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
};


#endif // DESIGN_TEXT_STRING_H
