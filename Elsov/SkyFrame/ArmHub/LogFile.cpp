#include "stdafx.h"
#include "LogFile.h"

CLogFile::CLogFile(CString FileName)
{
	m_pFileDesc = fopen(FileName, "a+t");
}

CLogFile::~CLogFile()
{
	fclose(m_pFileDesc);
}

void CLogFile::Format(const char *pszFormat...)
{
	WriteTimeStamp();
	fprintf(m_pFileDesc, pszFormat);
}

void CLogFile::WriteString(char *pszString)
{
	WriteTimeStamp();
	fprintf(m_pFileDesc, "%s\n", pszString);
}

void CLogFile::WriteTimeStamp()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	fprintf(m_pFileDesc, "%02d.%02d.%d %02d:%02d:%02d\t", time.wDay, time.wMonth, time.wYear,
		time.wHour,	time.wMinute, time.wSecond);
}

