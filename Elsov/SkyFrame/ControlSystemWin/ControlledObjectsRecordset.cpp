// ControlledObjectsRecordset.cpp : implementation file
//

#include "stdafx.h"
#include "ControlSystemWin.h"
#include "ControlledObjectsRecordset.h"
#include "ControlSystemDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlledObjectsRecordset

IMPLEMENT_DYNAMIC(CControlledObjectsRecordset, CControlSystemRecordset)

CControlledObjectsRecordset::CControlledObjectsRecordset(CDatabase *pdb)
	: CControlSystemRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CControlledObjectsRecordset)
	m_ID = 0;
	m_PARENT_ID = 0;
	m_NAME = _T("");
	m_TYPE = 0;
	m_bDeleted = 0;
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_Param1 = 0;
	m_Param2 = 0;
	m_nParams = 2;
}


CString CControlledObjectsRecordset::GetDefaultSQL()
{
	return _T("[ControlObjects]");
}

void CControlledObjectsRecordset::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CControlledObjectsRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Long(pFX, _T("[PARENT_ID]"), m_PARENT_ID);
	RFX_Text(pFX, _T("[NAME]"), m_NAME);
	RFX_Long(pFX, _T("[TYPE]"), m_TYPE);
	RFX_Long(pFX, _T("[DELETED]"), m_bDeleted);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
	RFX_Long(pFX,"", m_Param2);
}

long CControlledObjectsRecordset::QueryTargetInstanceID()
{
	m_strFilter = "TYPE = ?";
	m_strSort = "ID";
	m_Param1 = UNDEFINED_TYPE;
	if (IsOpen())
		Requery();
	else
		Open();

	if (IsEOF())
		return -1; // no target instance yet
	return m_ID;
}

long CControlledObjectsRecordset::CreateTargetInstance()
{
	if (!IsOpen())
		Open();
	AddNew();
	m_PARENT_ID = UNDEFINED_PARENT;
	m_TYPE = UNDEFINED_TYPE;
	m_NAME = "TargetInstance";
	m_bDeleted = 0;
	Update();

	m_strFilter = "PARENT_ID = ? AND TYPE = ?";
	m_strSort = "ID";
	m_Param1 = UNDEFINED_PARENT;
	m_Param2 = UNDEFINED_TYPE;
	Requery();
	if (!IsEOF())
		MoveLast();
	return m_ID;
}

long CControlledObjectsRecordset::AddNewObject(long ParentID, long Type, const char *pszName)
{
	if (!IsOpen())
		Open();
	AddNew();
	m_PARENT_ID = ParentID;
	m_TYPE = Type;
	m_NAME = pszName;
	m_bDeleted = 0;
	Update();

	m_strFilter = "PARENT_ID = ? AND TYPE = ?";
	m_strSort = "ID";
	m_Param1 = ParentID;
	m_Param2 = Type;
	Requery();
	if (!IsEOF())
		MoveLast();
	return m_ID;
}

BOOL CControlledObjectsRecordset::DestroyObject(long ID)
{
	// Delete children first
	m_strFilter = "PARENT_ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();
	CControlledObjectsRecordset ChildrenSet(m_pDatabase);
	while (!IsEOF())
	{
		ChildrenSet.DestroyObject(m_ID);
		MoveNext();
	}

	// Delete object itself
	m_strFilter = "ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();
	while (!IsEOF())
	{
		Delete();
		MoveNext();
	}

	return TRUE;
}

BOOL CControlledObjectsRecordset::DeleteObject(long ID)
{
	// Delete children first
	m_strFilter = "PARENT_ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();
	CControlledObjectsRecordset ChildrenSet(m_pDatabase);
	while (!IsEOF())
	{
		ChildrenSet.DeleteObject(m_ID);
		MoveNext();
	}

	// Delete object itself
	m_strFilter = "ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();
	while (!IsEOF())
	{
		Edit();
		m_bDeleted = TRUE;
		Update();
		MoveNext();
	}

	return TRUE;
}

BOOL CControlledObjectsRecordset::SetObjectName(long ID, CString &NewName)
{
	m_strFilter = "ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();
	if (!IsEOF())
	{
		Edit();
		m_NAME = NewName;
		Update();
	}
	return TRUE;
}

BOOL CControlledObjectsRecordset::SetObjectType(long ID, long type)
{
	m_strFilter = "ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();
	if (!IsEOF())
	{
		Edit();
		m_TYPE = type;
		Update();
	}
	return TRUE;
}

BOOL CControlledObjectsRecordset::SetObjectNameAndType(long ID, CString &NewName, long type)
{
	m_strFilter = "ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();
	if (!IsEOF())
	{
		Edit();
		m_NAME = NewName;
		m_TYPE = type;
		Update();
	}
	return TRUE;
}

BOOL CControlledObjectsRecordset::SetObjectParent(long ID, long ParentID)
{
	m_strFilter = "ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();
	if (!IsEOF())
	{
		Edit();
		m_PARENT_ID = ParentID;
		Update();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CControlledObjectsRecordset diagnostics

#ifdef _DEBUG
void CControlledObjectsRecordset::AssertValid() const
{
	CControlSystemRecordset::AssertValid();
}

void CControlledObjectsRecordset::Dump(CDumpContext &dc) const
{
	CControlSystemRecordset::Dump(dc);
}
#endif //_DEBUG
