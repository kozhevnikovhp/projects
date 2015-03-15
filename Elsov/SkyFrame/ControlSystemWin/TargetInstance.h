#ifndef __TARGET_INSTANCE_H_INCLUDED__
#define __TARGET_INSTANCE_H_INCLUDED__

#include "DynamicLibrary.h"
#include "ControlSystemDatabase.h"
#include <vector>

//Forward declarations
class IControlTargetAPI;

class CTargetInstance : public CObject
{
// Constuction/destruction
public:
	CTargetInstance();
	CTargetInstance(CString &DllFileName);
	virtual ~CTargetInstance();

// Public methods
public:
	IControlTargetAPI *GetTarget() const { return m_pTarget; }
	CControlSystemDatabase *GetDatabase() { return &m_DB; }
	void WriteToRegistry(CString &SectionName);
	void LoadFromRegistry(CString &SectionName);
	CString &GetTargetName() { return m_TargetName; }
	void SetTargetName(CString &Name) { m_TargetName = Name; }
	CString &GetDbConnectionString() { return m_DbConnectionString; }
	BOOL CreateDB();
	void GetFullName(CString &FullName);
	void GetObjectFullName(CString &ObjectName, int ObjectType, CString &FullName);
	// Control objects
	CControlObject *CreateControlObject(CControlObject *pParent);
	CControlObject *CreateControlObject(CControlObject *pParent, long ID, long Type, CString Name, long bDeleted);
	void DeleteControlObject(CControlObject *pObject);
	void DestroyControlObject(CControlObject *pObjectToDelete);
	void RemoveObject(CControlObject *pObjectToDelete);
	void SetObjectParent(CControlObject *pChild, CControlObject *pParent);

// Protected methods
protected:
	void Initialize();
	void Create(CString &DllFileName);
	void Free();

// Protected members
protected:
	IControlTargetAPI *m_pTarget;
	CDynamicLibrary m_DLL;
	CString m_TargetName;
	CString m_DbConnectionString;
	CString m_DllFileName;
	CControlSystemDatabase m_DB;
	std::vector<CControlObject *> m_Objects;
};

#endif //__TARGET_INSTANCE_H_INCLUDED__