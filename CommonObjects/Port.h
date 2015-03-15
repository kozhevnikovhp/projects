#ifndef __PORT_H_INCLUDED__
#define __PORT_H_INCLUDED__

#define WINDOWS 1

#if defined(WINDOWS) // Windows-related header files
#include "Windows.h"
#define PORT_DESC HANDLE
#endif	// End of Windows related port header files

#if defined(LINUX) // Linux-related header files
#define PORT_DESC int
#endif	// End of Linux related header files

#if defined(RTEMS) // RTEMS-related header files
#define PORT_DESC int
#endif	// End of RTEMS related port header files

enum ErrorReportAction  { ErrorToScreen, ErrorToDebugWindow, NoMessage };

// Forward declarations
class CSharedPort;

class CPort
{
	friend class CSharedPort;
// public methods
public:
	CPort();
	virtual ~CPort();
	virtual void Close();
	virtual BOOL CanBeShared() { return FALSE; }

	void SetName(const char *pszName);
	inline char *GetName() { return m_pszName; }
	PORT_DESC GetPortDesc();
	void SetPortDesc(PORT_DESC PortDesc) { m_PortDesc = PortDesc; }
	void ReportLastError(const char *pszName, ErrorReportAction Action);
	BOOL IsOpened();
	char *GetStatusString();
	unsigned long GetStatusCode() { return m_LastErrorCode; }
	unsigned long GetSentBytesCount() { return m_nSentBytes; }
	unsigned long GetReceivedBytesCount() { return m_nReceivedBytes; }
// Protected methods
	void ClearPortDesc();

	static void NewPortOpened(CPort *pPort);
	static CSharedPort *FindSharedPort(const char *pszName);

	// protected members
protected:
	char m_pszName[16];
	PORT_DESC m_PortDesc;
	unsigned long m_LastErrorCode;
	char m_pszStatusString[64];
	static CSharedPort *m_pAllSharedPorts;
	CSharedPort *m_pSharedPort;
	unsigned long m_nSentBytes, m_nReceivedBytes;
};

class CSharedPort
{
	friend class CPort;
// public methods
public:
	CSharedPort(CPort *pPort);
	virtual ~CSharedPort();
	CSharedPort *Find(const char *pszName);
	PORT_DESC GetPortDesc() { return m_PortDesc; }
	void Open() { ++m_nInstances; }
	void Close();
// Protected methods
// protected members
protected:
	CPort *m_pPort;
	CSharedPort *m_pPrev;
	CSharedPort *m_pNext;
	int m_nInstances;
	PORT_DESC m_PortDesc;
};

#endif //__PORT_H_INCLUDED__