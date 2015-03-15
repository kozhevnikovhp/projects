#include "stdafx.h"
#include <string.h>
#include <stdio.h>

#if defined(UNIX) //UNIX-related header files
#include <unistd.h>
#include <errno.h>
#endif // End of Unix-related header files

#include "Port.h"

CSharedPort *CPort::m_pAllSharedPorts = NULL;

CPort::CPort()
{
	m_pszName[0] = 0;
	m_LastErrorCode = 0;
	m_pszStatusString[0] = 0;
	m_pSharedPort = NULL;
	m_nSentBytes = m_nReceivedBytes = 0;
	ClearPortDesc();
}

CPort::~CPort()
{
	Close();
}

BOOL CPort::IsOpened()
{
#if defined(WINDOWS) // Windows-related code
	return (GetPortDesc() != INVALID_HANDLE_VALUE);
#endif // End of Windows-related code

#if defined(UNIX) // UNIX-related code
	return (GetPortDesc() > -1);
#endif // End of UNIX-related code
}


void CPort::SetName(const char *pszName)
{
	strcpy(m_pszName, pszName);
}

void CPort::Close()
{
	m_pszName[0]='\0';
	if (m_pSharedPort)
		m_pSharedPort->Close();

#if defined(WINDOWS) // Windows-related code
	if (m_PortDesc != INVALID_HANDLE_VALUE)
	    CloseHandle(m_PortDesc);
	m_PortDesc = INVALID_HANDLE_VALUE;
#endif // End of Windows-related code

#if defined(LINUX) || defined(RTEMS) // UNIX-related code
	if (m_PortDesc != -1)
	    close(m_PortDesc);
	m_PortDesc = -1;
#endif // End of UNIX-related code
	m_pSharedPort = NULL;
}

void CPort::ReportLastError(const char *pszName, ErrorReportAction Action)
{
	if (Action == NoMessage) return;

#if defined(WINDOWS) // Windows-related code
	m_LastErrorCode = GetLastError();
#endif // End of Windows-related code

#if defined(LINUX) || defined(RTEMS) // UNIX-related code
	m_LastErrorCode = errno;
#endif // End of UNIX-related code

	char pszString[256];
	strcpy(pszString, "Port ");
	strcat(pszString, pszName);
	char pszErrorCode[32];
	sprintf(pszErrorCode, " error %d !\n", m_LastErrorCode);
	strcat(pszString, pszErrorCode);
	switch (Action)
	{
	case ErrorToScreen:
#if defined(WINDOWS) // Windows-related code
		MessageBox(NULL, pszString, "Port Error", MB_ICONEXCLAMATION);
#endif // End of Windows-related code

#if defined(LINUX) // Linux-related code
		printf("Port Error: %s\n", pszString);
#endif // End of Linux-related code

#if defined(RTEMS) // RTEMS-related code
		printf("Port Error: %s\n", pszString);
#endif // End of RTEMS-related code
		//no break!!!
	case ErrorToDebugWindow:
//		TRACE0(pszString);
		break;
	}
}

char *CPort::GetStatusString()
{
	if (IsOpened())
		strcpy(m_pszStatusString, "Port opened successfully");
	else
		if (m_LastErrorCode)
			sprintf(m_pszStatusString, "Port not opened due to error %d", m_LastErrorCode);
		else
			strcpy(m_pszStatusString, "Port not opened");
	return m_pszStatusString;
}

PORT_DESC CPort::GetPortDesc()
{
	if (m_pSharedPort)
		return m_pSharedPort->GetPortDesc();		
	else
		return m_PortDesc;
}

void CPort::ClearPortDesc()
{
#if defined(WINDOWS) // Windows-related code
	m_PortDesc = INVALID_HANDLE_VALUE;
#endif // End of Windows-related code

#if defined(LINUX) || defined(RTEMS) // UNIX-related code
	m_PortDesc = -1;
#endif // End of UNIX-related code
}

//static
void CPort::NewPortOpened(CPort *pPort)
{
	if (pPort->CanBeShared())
	{
		CSharedPort *pNewPort = new CSharedPort(pPort);
		pPort->m_pSharedPort = pNewPort;
		// link it to the list (to the head)
		if (m_pAllSharedPorts)
			m_pAllSharedPorts->m_pPrev = pNewPort;
		pNewPort->m_pNext = m_pAllSharedPorts;
		m_pAllSharedPorts = pNewPort;
		pPort->ClearPortDesc();
	}
}

//static
CSharedPort *CPort::FindSharedPort(const char *pszName)
{
	if (!m_pAllSharedPorts)
		return NULL;
	return m_pAllSharedPorts->Find(pszName);
}

// CSharedPort implementation

CSharedPort::CSharedPort(CPort *pPort)
{
	m_pPort = pPort;
	m_PortDesc = pPort->GetPortDesc();
	m_nInstances = 1;
	m_pPrev = NULL;
	m_pNext = NULL;
}

//virtual
CSharedPort::~CSharedPort()
{
}

CSharedPort *CSharedPort::Find(const char *pszName)
{
	if (!strcmp(pszName, m_pPort->GetName()))
		return this;
	if (m_pNext)
		return m_pNext->Find(pszName);
	return NULL;
}

void CSharedPort::Close()
{
	--m_nInstances;
	if (m_nInstances == 0)
	{ // close handle, remove itself from the list and suicide
#if defined(WINDOWS) // Windows-related code
		if (m_PortDesc != INVALID_HANDLE_VALUE)
			CloseHandle(m_PortDesc);
		m_PortDesc = INVALID_HANDLE_VALUE;
#endif // End of Windows-related code

#if defined(LINUX) || defined(RTEMS) // UNIX-related code
		if (m_PortDesc != -1)
			close(m_PortDesc);
		m_PortDesc = -1;
#endif // End of UNIX-related code

		if (m_pPrev)
			m_pPrev->m_pNext = m_pNext;
		if (m_pNext)
			m_pNext->m_pPrev = m_pPrev;
		if (!m_pPrev)
			CPort::m_pAllSharedPorts = m_pNext;
		delete this;
	}
}



