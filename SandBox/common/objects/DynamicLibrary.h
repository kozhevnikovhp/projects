#ifndef __DYNAMIC_LIBRARY_H_INCLUDED__
#define __DYNAMIC_LIBRARY_H_INCLUDED__

#define WINDOWS 1

#if defined(WINDOWS) // Windows-related header files
#include "Windows.h"
#define LIBRARY_HANDLE HMODULE
#endif	// End of Windows related port header files

#if defined(LINUX) // Linux-related header files
#define LIBRARY_HANDLE int
#endif	// End of Linux related header files

// Forward declarations
class CSharedDynamicLibrary;

class CDynamicLibrary
{
	friend class CSharedDynamicLibrary;
// public methods
public:
	CDynamicLibrary();
	virtual ~CDynamicLibrary();
	virtual void Unload();

//	void SetFileName(const char *pszFileName);
	inline char *GetFileName() { return m_pszFileName; }
	LIBRARY_HANDLE GetHandle();
	void SetPortDesc(LIBRARY_HANDLE hLibrary) { m_Handle = hLibrary; }
	BOOL Load(const char *pszFileName);
	BOOL IsLoaded();
// Protected methods
	void ClearLibraryHandle();
static void NewInstanceLoaded(CDynamicLibrary *pLibrary);
static CSharedDynamicLibrary *FindSharedLibrary(const char *pszFileName);
// protected members
protected:
	char m_pszFileName[256];
	LIBRARY_HANDLE m_Handle;
	static CSharedDynamicLibrary *m_pAllSharedLibs;
	CSharedDynamicLibrary *m_pSharedLib;
};

class CSharedDynamicLibrary
{
	friend class CDynamicLibrary;
// public methods
public:
	CSharedDynamicLibrary(CDynamicLibrary *pLibrary);
	virtual ~CSharedDynamicLibrary();
	CSharedDynamicLibrary *Find(const char *pszFileName);
	LIBRARY_HANDLE GetHandle() { return m_Handle; }
	void Load() { ++m_nInstances; }
	void Unload();
// Protected methods
// protected members
protected:
	CDynamicLibrary *m_pLibrary;
	CSharedDynamicLibrary *m_pPrev;
	CSharedDynamicLibrary *m_pNext;
	int m_nInstances;
	LIBRARY_HANDLE m_Handle;
};

#endif //__DYNAMIC_LIBRARY_H_INCLUDED__