/* Terminal.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail ek538@mail.ru

Revision history:
	31 Jan 2003 - initial creation

*/

#include "stdafx.h"
#include "Terminal.h"

CTerminal::CTerminal()
{
}

//virtual
CTerminal::~CTerminal()
{
}

//virtual
LOGICAL CTerminal::Write(void *pszBuffer, unsigned int BytesToWrite)
{
	unsigned int nWrittenBytes = 0;
	return Write(pszBuffer, BytesToWrite, nWrittenBytes);
}

//virtual
LOGICAL CTerminal::Read(void *pszBuffer, unsigned int BytesToRead)
{
	unsigned int nReadBytes = 0;
	return Read(pszBuffer, BytesToRead, nReadBytes);
}

