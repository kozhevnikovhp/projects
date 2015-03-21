#ifndef TEXT_STRING_H
#define TEXT_STRING_H

#include "types.h"

namespace common {

namespace portable {

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
	bool IsEmpty() const { return (m_nTextLen == 0); }
	void SetLength(int nNewLength);
	void TrimRight(int nSymbols);
	void LeaveDirNameOnly();
	void MakeFilePathName(char *pszPath, char *pszFileName);
	CTextString &operator = (const CTextString &Src);
	CTextString &operator = (const char *pszSrc);
	CTextString &operator = (char *pszSrc);
	const CTextString &operator += (char ch);
	const CTextString &operator += (const CTextString &Src);
	const CTextString &operator += (const char *pszString);
	bool operator == (const CTextString &AnoterString);
	bool operator == (const char *pszString);
	bool operator != (const CTextString &AnoterString);
	bool operator != (const char *pszString);
	bool IsEqualNoCase(const char *pszString);

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void Init();
	bool AllocBuffer(int nBufferLen);
	bool ReallocBuffer(int nRequiredBytes);

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

} // namespace portable

} // namespace common


#endif // TEXT_STRING_H
