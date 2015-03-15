#ifndef __LOG_FILE_H_INCLUDED__
#define __LOG_FILE_H_INCLUDED__

class CLogFile
{
public:
	CLogFile(CString FileName);
	~CLogFile();
	void Format(const char *pszFormat...);
	void WriteString(char *pszString);
protected:
	void WriteTimeStamp();
	FILE *m_pFileDesc;
};

#endif //__LOG_FILE_H_INCLUDED__