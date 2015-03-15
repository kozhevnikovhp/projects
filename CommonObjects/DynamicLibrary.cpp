#include "stdafx.h"
#include <string.h>
#include <stdio.h>

#if defined(UNIX) //UNIX-related header files
#include <unistd.h>
#include <errno.h>
#endif // End of Unix-related header files

#include "DynamicLibrary.h"


/////////////////////////////////////////////////////////////////////////////////////
// CDynamicLibrary class implementation

CSharedDynamicLibrary *CDynamicLibrary::m_pAllSharedLibs = NULL;

CDynamicLibrary::CDynamicLibrary()
{
	m_pszFileName[0] = 0;
	m_pSharedLib = NULL;
	ClearLibraryHandle();
}

CDynamicLibrary::~CDynamicLibrary()
{
	Unload();
}

BOOL CDynamicLibrary::IsLoaded()
{
#if defined(WINDOWS) // Windows-related code
	return (GetHandle() != NULL);
#endif // End of Windows-related code

#if defined(UNIX) // UNIX-related code
	return (GetHandle() > -1);
#endif // End of UNIX-related code
}


BOOL CDynamicLibrary::Load(const char *pszFileName)
{
	strcpy(m_pszFileName, pszFileName);
	m_pSharedLib = FindSharedLibrary(pszFileName);
	if (m_pSharedLib)
		m_pSharedLib->Load();
	else
	{
#if defined(WINDOWS) // Windows-related code
		m_Handle = LoadLibrary(m_pszFileName);
#endif // End of Windows-related code

#if defined(LINUX) // UNIX-related code

#endif // End of UNIX-related code
		NewInstanceLoaded(this);
	}
	return IsLoaded();
}

void CDynamicLibrary::Unload()
{
	m_pszFileName[0] = '\0';
	if (m_pSharedLib)
		m_pSharedLib->Unload();

	ClearLibraryHandle();
	m_pSharedLib = NULL;
}

LIBRARY_HANDLE CDynamicLibrary::GetHandle()
{
	if (m_pSharedLib)
		return m_pSharedLib->GetHandle();		
	else
		return m_Handle;
}

void CDynamicLibrary::ClearLibraryHandle()
{
#if defined(WINDOWS) // Windows-related code
	m_Handle = NULL;
#endif // End of Windows-related code

#if defined(LINUX) // UNIX-related code
	m_Handle = -1;
#endif // End of UNIX-related code
}

//static
void CDynamicLibrary::NewInstanceLoaded(CDynamicLibrary *pLibrary)
{
	CSharedDynamicLibrary *pNewLibrary = new CSharedDynamicLibrary(pLibrary);
	pLibrary->m_pSharedLib = pNewLibrary;
	// link it to the list (to the head)
	if (m_pAllSharedLibs)
		m_pAllSharedLibs->m_pPrev = pNewLibrary;
	pNewLibrary->m_pNext = m_pAllSharedLibs;
	m_pAllSharedLibs = pNewLibrary;
}

//static
CSharedDynamicLibrary *CDynamicLibrary::FindSharedLibrary(const char *pszFileName)
{
	if (!m_pAllSharedLibs)
		return NULL;
	return m_pAllSharedLibs->Find(pszFileName);
}


/////////////////////////////////////////////////////////////////////////////////////
// CSharedDynamicLibrary class implementation

CSharedDynamicLibrary::CSharedDynamicLibrary(CDynamicLibrary *pLibrary)
{
	m_pLibrary = pLibrary;
	m_Handle = pLibrary->GetHandle();
	m_nInstances = 1;
	m_pPrev = NULL;
	m_pNext = NULL;
}

//virtual
CSharedDynamicLibrary::~CSharedDynamicLibrary()
{
}

CSharedDynamicLibrary *CSharedDynamicLibrary::Find(const char *pszFileName)
{
	if (!strcmp(pszFileName, m_pLibrary->GetFileName()))
		return this;
	if (m_pNext)
		return m_pNext->Find(pszFileName);
	return NULL;
}

void CSharedDynamicLibrary::Unload()
{
	--m_nInstances;
	if (m_nInstances == 0)
	{ // close handle, remove itself from the list and suicide
#if defined(WINDOWS) // Windows-related code
		if (m_Handle != NULL)
			FreeLibrary(m_Handle);
		m_Handle = NULL;
#endif // End of Windows-related code

#if defined(LINUX) // UNIX-related code
#endif // End of UNIX-related code

		if (m_pPrev)
			m_pPrev->m_pNext = m_pNext;
		if (m_pNext)
			m_pNext->m_pPrev = m_pPrev;
		if (!m_pPrev)
			CDynamicLibrary::m_pAllSharedLibs = m_pNext;
		delete this;
	}
}



