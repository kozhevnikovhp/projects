// CuvsDoc.cpp : implementation of the CCuvsDoc class
//

#include "stdafx.h"

#include "CuvsDoc.h"
#include "StationsView.h"
#include "TmiToBeAskedSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCuvsDoc

IMPLEMENT_DYNCREATE(CCuvsDoc, CDocument)

BEGIN_MESSAGE_MAP(CCuvsDoc, CDocument)
	//{{AFX_MSG_MAP(CCuvsDoc)
	ON_COMMAND(ID_STATIONS_ADDNEW, OnStationsAddNew)
	ON_COMMAND(ID_STATIONS_DELETE, OnStationsDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCuvsDoc construction/destruction

CCuvsDoc::CCuvsDoc()
{
	m_pDatabase = NULL;
}

CCuvsDoc::~CCuvsDoc()
{
	CloseDatabase();
}

BOOL CCuvsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	OpenDatabase();
	RefreshStations();
	return TRUE;
}

BOOL CCuvsDoc::OpenDatabase()
{
	BOOL bSuccess = TRUE;
	if (m_pDatabase != NULL)
		return TRUE; // already opened
	TRY
	{
		m_StationsSet.m_strFilter = "ParentStationID = -1";
		bSuccess &= m_StationsSet.Open();
		m_pDatabase = m_StationsSet.m_pDatabase;

		m_StationDevicesSet.m_strFilter = "StationID = -1";
		m_StationDevicesSet.m_pDatabase = m_pDatabase;
		bSuccess &= m_StationDevicesSet.Open();

		m_ChannelsSet.m_strFilter = "N_CHANNEL = -1";
		m_ChannelsSet.m_pDatabase = m_pDatabase;
		bSuccess &= m_ChannelsSet.Open();

		m_AllDevicesAndChildDevicesSet.m_pDatabase = m_pDatabase;
		bSuccess &= m_AllDevicesAndChildDevicesSet.Open();

		m_TmiAvailableParamsSet.m_pDatabase = m_pDatabase;
		bSuccess &= m_TmiAvailableParamsSet.Open();		

		m_TmiParamSet.m_pDatabase = m_pDatabase;
		bSuccess &= m_TmiParamSet.Open();
		m_TmiParamSet.m_LastTmi = CTime::GetCurrentTime();

		m_DeviceTypesSet.m_pDatabase = m_pDatabase;
		bSuccess &= m_DeviceTypesSet.Open();
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при открытии БД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
	return bSuccess;
}

BOOL CCuvsDoc::CloseDatabase()
{
	if (m_pDatabase == NULL || !m_pDatabase->IsOpen())
		return TRUE; // already closed
	
	TRY
	{
		m_StationsSet.Close();
		m_StationsSet.m_pDatabase = NULL;

		m_StationDevicesSet.Close();
		m_StationDevicesSet.m_pDatabase = NULL;

		m_ChannelsSet.Close();
		m_ChannelsSet.m_pDatabase = NULL;

		m_AllDevicesAndChildDevicesSet.Close();
		m_AllDevicesAndChildDevicesSet.m_pDatabase = NULL;

		m_TmiAvailableParamsSet.Close();
		m_TmiAvailableParamsSet.m_pDatabase = NULL;

		m_TmiParamSet.Close();
		m_TmiParamSet.m_pDatabase = NULL;

		m_DeviceTypesSet.Close();
		m_DeviceTypesSet.m_pDatabase = NULL;

		m_pDatabase->Close();
		delete m_pDatabase;
		m_pDatabase = NULL;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при закрытии БД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCuvsDoc serialization

void CCuvsDoc::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCuvsDoc diagnostics

#ifdef _DEBUG
void CCuvsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCuvsDoc::Dump(CDumpContext &dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCuvsDoc commands

void CCuvsDoc::OnStationsAddNew() 
{
	CWaitCursor cursor;
	CString NewStationName = "Новая станция";
	TRY
	{
		m_StationsSet.AddNew();
		m_StationsSet.m_Name = NewStationName;
		m_StationsSet.m_ParentStationID = UNDEFINED_PARENT; // undefined yet
		m_StationsSet.Update();
		m_StationsSet.m_strFilter = "ParentStationID = ?";
		m_StationsSet.m_strSort = "StationID";
		m_StationsSet.m_Param1 = UNDEFINED_PARENT;
		m_StationsSet.Requery();
		m_StationsSet.MoveLast();
		m_pStationsView->InsertStation(m_StationsSet.m_Name, m_StationsSet.m_StationID, m_AllNetRootItem);
		// Add one modem Datum and monoblock to the station
		CreateStationDevice(m_StationsSet.m_StationID, 1);
		CreateStationDevice(m_StationsSet.m_StationID, 2);
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при добавлении записи в ГБД", MB_ICONEXCLAMATION);
	END_CATCH
}

void CCuvsDoc::AddNewForm(FI_FormIndex IndexInViewArray, CAbstractForm *pViewToAdd, CWnd *pParent)
{
	m_pForms[IndexInViewArray] = pViewToAdd;
	m_pForms[IndexInViewArray]->SetDocument(this);
	pViewToAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CFrameWnd::rectDefault, pParent,
		AFX_IDW_PANE_FIRST+IndexInViewArray+1, NULL);
	AddView(pViewToAdd);
	pViewToAdd->m_pStationsView = m_pStationsView;
}

void CCuvsDoc::AddStation(CTreeCtrl *pTreeCtrl, HTREEITEM Item, CStationsRecordSet *pStationsSet)
{
	HTREEITEM NewItem = m_pStationsView->InsertStation(pStationsSet->m_Name, pStationsSet->m_StationID, Item);
	CStationsRecordSet Set;
	Set.m_pDatabase = m_pDatabase;
	Set.Open();
	Set.m_strFilter = "ParentStationID = ?";
	Set.m_Param1 = pStationsSet->m_StationID;
	Set.m_strSort = "Name";
	Set.Requery();
	while (!Set.IsEOF())
	{
		AddStation(pTreeCtrl, NewItem, &Set);
		Set.MoveNext();
	}
	Set.Close();
}

void CCuvsDoc::RefreshStations()
{
	CTreeCtrl *pTreeCtrl = &m_pStationsView->GetTreeCtrl();
	pTreeCtrl->DeleteAllItems();
	m_AllNetRootItem = m_pStationsView->InsertAllNetItem();
	TRY
	{
		m_StationsSet.m_strFilter = "ParentStationID = ?";
		m_StationsSet.m_Param1 = UNDEFINED_PARENT;
		m_StationsSet.Requery();
		while (!m_StationsSet.IsEOF())
		{
			AddStation(pTreeCtrl, m_AllNetRootItem, &m_StationsSet);
			m_StationsSet.MoveNext();
		}
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
	END_CATCH
}

BOOL CCuvsDoc::GetStationName(long StationID, CString &Name)
{
	TRY
	{
		m_StationsSet.m_strFilter = "StationID = ?";
		m_StationsSet.m_Param1 = StationID;
		m_StationsSet.Requery();
		if (!m_StationsSet.IsEOF())
		{
			Name = m_StationsSet.m_Name;
			return TRUE;
		}
		else
		{
			Name = "Unknown";
			return FALSE;
		}
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
	END_CATCH
	return TRUE;
}

BOOL CCuvsDoc::SetStationName(long StationID, CString &Name)
{
	TRY
	{
		m_StationsSet.m_strFilter = "StationID = ?";
		m_StationsSet.m_Param1 = StationID;
		m_StationsSet.Requery();
		if (!m_StationsSet.IsEOF())
		{
			m_StationsSet.Edit();
			m_StationsSet.m_Name = Name;
			m_StationsSet.Update();
			return TRUE;
		}
		else
		{
			Name = "Unknown";
			return FALSE;
		}
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении/записи ГБД", MB_ICONEXCLAMATION);
	END_CATCH
	return TRUE;
}

BOOL CCuvsDoc::SetStationParentID(long StationID, long ParentStationID)
{
	TRY
	{
		m_StationsSet.m_strFilter = "StationID = ?";
		m_StationsSet.m_Param1 = StationID;
		m_StationsSet.Requery();
		if (!m_StationsSet.IsEOF())
		{
			m_StationsSet.Edit();
			m_StationsSet.m_ParentStationID = ParentStationID;
			m_StationsSet.Update();
			return TRUE;
		}
		else
			return FALSE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении/записи ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::QueryChildStations(long StationID)
{
	TRY
	{
		m_StationsSet.m_strFilter = "ParentStationID = ?";
		m_StationsSet.m_strSort = "Name";
		m_StationsSet.m_Param1 = StationID;
		m_StationsSet.Requery();
		return (!m_StationsSet.IsEOF());
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::GetNextChildStation(long &StationID, CString &StationName)
{
	TRY
		if (m_StationsSet.IsEOF())
		{
			StationID = 0;
			StationName = afxChNil;
			return FALSE;
		}
		StationID = m_StationsSet.m_StationID;
		StationName = m_StationsSet.m_Name;
		m_StationsSet.MoveNext();
		return TRUE;
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

// Device types (read from database)
BOOL CCuvsDoc::QueryDeviceTypes()
{
	TRY
	{
		m_DeviceTypesSet.Requery();
		return (!m_DeviceTypesSet.IsEOF());
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::GetNextDeviceType(long &DeviceType, CString &DeviceTypeDescription)
{
	TRY
		if (m_DeviceTypesSet.IsEOF())
		{
			DeviceType = 0;
			DeviceTypeDescription = afxChNil;
			return FALSE;
		}
		DeviceType = m_DeviceTypesSet.m_DeviceType;
		DeviceTypeDescription = m_DeviceTypesSet.m_Description;
		m_DeviceTypesSet.MoveNext();
		return TRUE;
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::CreateStationDevice(long StationID, long DeviceType)
{
	TRY
	{
		m_StationDevicesSet.AddNew();
		m_StationDevicesSet.m_Name = "Новое устройство";
		m_StationDevicesSet.m_StationID = StationID;
		m_StationDevicesSet.m_Type = DeviceType; // modem
		m_StationDevicesSet.Update();
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при добавлении записи в ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::DeleteStationDevice(long DeviceID)
{
	TRY
	{
		m_StationDevicesSet.m_strFilter = "DeviceID = ?";
		m_StationDevicesSet.m_Param1 = DeviceID;
		m_StationDevicesSet.Requery();
		if (m_StationDevicesSet.IsEOF())
		{
			ASSERT(FALSE);
			return FALSE;
		}
		m_StationDevicesSet.Delete();
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при удалении записи из ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::QueryStationDevices(long StationID)
{
	TRY
	{
		m_StationDevicesSet.m_strFilter = "StationID = ?";
		m_StationDevicesSet.m_strSort = "Name";
		m_StationDevicesSet.m_Param1 = StationID;
		m_StationDevicesSet.Requery();
		return (!m_StationDevicesSet.IsEOF());
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::GetNextStationDevice(long &DeviceID, CString &DeviceName)
{
	TRY
		if (m_StationDevicesSet.IsEOF())
		{
			DeviceID = 0;
			DeviceName = afxChNil;
			return FALSE;
		}
		DeviceID = m_StationDevicesSet.m_DeviceID;
		DeviceName = m_StationDevicesSet.m_Name;
		m_StationDevicesSet.MoveNext();
		return TRUE;
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::GetStationDevice(long DeviceID, CString &Name, long &Type, long &IPAddress)
{
	TRY
	{
		m_StationDevicesSet.m_strFilter = "DeviceID = ?";
		m_StationDevicesSet.m_Param1 = DeviceID;
		m_StationDevicesSet.Requery();
		Name = m_StationDevicesSet.m_Name;
		Type = m_StationDevicesSet.m_Type;
		IPAddress = m_StationDevicesSet.m_IP;
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::SetStationDevice(long DeviceID, CString &Name, long Type, long IPAddress)
{
	TRY
	{
		m_StationDevicesSet.m_strFilter = "DeviceID = ?";
		m_StationDevicesSet.m_Param1 = DeviceID;
		m_StationDevicesSet.Requery();
		m_StationDevicesSet.Edit();
		m_StationDevicesSet.m_Name = Name;
		m_StationDevicesSet.m_Type = Type;
		m_StationDevicesSet.m_IP = IPAddress;
		m_StationDevicesSet.Update();
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::GetStationDeviceType(long DeviceID, long &Type)
{
	CString DeviceName;
	long IpAddress;
	return GetStationDevice(DeviceID, DeviceName, Type, IpAddress); // TODO! rewrite with new recordset (to avoid IP-address and DeviceName);
}

BOOL CCuvsDoc::QueryStationDevicesAndChildDevices(long StationID)
{
	TRY
	{
		m_AllDevicesAndChildDevicesSet.m_strFilter = "(StationDevices.StationID = Stations.StationID) AND ((StationDevices.StationID = ?) OR (Stations.ParentStationID = ?))";
		m_AllDevicesAndChildDevicesSet.m_strSort = "Stations.Name";
		m_AllDevicesAndChildDevicesSet.m_Param1 = m_AllDevicesAndChildDevicesSet.m_Param2 = StationID;
		m_AllDevicesAndChildDevicesSet.Requery();
		return (!m_AllDevicesAndChildDevicesSet.IsEOF());
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::GetNextDeviceAndChildDevice(long &DeviceID, CString &DeviceName, long &DeviceType, CString &StationName)
{
	TRY
		if (m_AllDevicesAndChildDevicesSet.IsEOF())
		{
			DeviceID = 0;
			DeviceName = afxChNil;
			StationName = afxChNil;
			return FALSE;
		}
		DeviceID = m_AllDevicesAndChildDevicesSet.m_DeviceID;
		DeviceName = m_AllDevicesAndChildDevicesSet.m_DeviceName;
		DeviceType = m_AllDevicesAndChildDevicesSet.m_DeviceType;
		StationName = m_AllDevicesAndChildDevicesSet.m_StationName;
		m_AllDevicesAndChildDevicesSet.MoveNext();
		return TRUE;
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::AddNewChannel(long TransmitterID, long ReceiverID)
{
	TRY
	{
		m_ChannelsSet.AddNew();
		m_ChannelsSet.m_TRANSMITTER = TransmitterID;
		m_ChannelsSet.m_RECEIVER = ReceiverID;
		m_ChannelsSet.Update();
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при добавлении записи в ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::DeleteChannel(long TransmitterID, long ReceiverID)
{
	TRY
	{
		m_ChannelsSet.m_strFilter = "TRANSMITTER =? AND RECEIVER = ?";
		m_ChannelsSet.m_Param1 = TransmitterID;
		m_ChannelsSet.m_Param2 = ReceiverID;
		m_ChannelsSet.Requery();
		if (m_ChannelsSet.IsEOF())
		{
			ASSERT(FALSE);
			return FALSE;
		}
		m_ChannelsSet.Delete();
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при удалении записи из ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::QueryChannelByReceiverID(long &TransmitterID, long ReceiverID)
{
	TRY
	{
		m_ChannelsSet.m_strFilter = "RECEIVER = ?";
		m_ChannelsSet.m_Param1 = ReceiverID;
		m_ChannelsSet.Requery();
		if (m_ChannelsSet.IsEOF())
			return FALSE;
		TransmitterID = m_ChannelsSet.m_TRANSMITTER;
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::QueryChannelByReceiverID(long ReceiverID, BOOL &bAutoSN, double &MinSN, double &MaxSN)
{
	TRY
	{
		m_ChannelsSet.m_strFilter = "RECEIVER = ?";
		m_ChannelsSet.m_Param1 = ReceiverID;
		m_ChannelsSet.Requery();
		if (m_ChannelsSet.IsEOF())
			return FALSE;
		bAutoSN = m_ChannelsSet.m_bAutoSN;
		MinSN = m_ChannelsSet.m_MinSN;
		MaxSN = m_ChannelsSet.m_MaxSN;
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::EditChannelByReceiverID(long ReceiverID, BOOL bAutoSN, double MinSN, double MaxSN)
{
	TRY
	{
		m_ChannelsSet.m_strFilter = "RECEIVER = ?";
		m_ChannelsSet.m_Param1 = ReceiverID;
		m_ChannelsSet.Requery();
		if (m_ChannelsSet.IsEOF())
			return FALSE;
		m_ChannelsSet.Edit();
		m_ChannelsSet.m_bAutoSN = bAutoSN;
		m_ChannelsSet.m_MinSN = MinSN;
		m_ChannelsSet.m_MaxSN = MaxSN;
		m_ChannelsSet.Update();
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении/записи ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::QueryTmiAvailableParams(long DeviceID)
{
	TRY
	{
		m_TmiAvailableParamsSet.m_strFilter = "DeviceID = ? AND \
DeviceTypes.DeviceType = StationDevices.DeviceType AND \
DeviceTypes.DeviceType = TmiAvailability.DeviceType AND \
TmiAvailability.TmiType = TmiTypes.TmiType";
		m_TmiAvailableParamsSet.m_Param1 = DeviceID;
		m_TmiAvailableParamsSet.Requery();
		return (!m_TmiAvailableParamsSet.IsEOF());
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::GetNextTmiAvailableParam(CString &ParamDescription, long &TmiType)
{
	TRY
		if (m_TmiAvailableParamsSet.IsEOF())
		{
			ParamDescription = afxChNil;
			return FALSE;
		}
		ParamDescription = m_TmiAvailableParamsSet.m_TmiParamDescription;
		TmiType = m_TmiAvailableParamsSet.m_TmiType;
		m_TmiAvailableParamsSet.MoveNext();
		return TRUE;
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
	return TRUE;
}

BOOL CCuvsDoc::QueryTmiParam(long DeviceID, long TmiType, BOOL &bEnabled, long &Period)
{
	TRY
	{
		m_TmiParamSet.m_strFilter = "DeviceID = ? AND TmiType = ?";
		m_TmiParamSet.m_Param1 = DeviceID;
		m_TmiParamSet.m_Param2 = TmiType;
		m_TmiParamSet.Requery();
		bEnabled = FALSE;
		Period = 1;
		if (m_TmiParamSet.IsEOF())
			return FALSE;
		bEnabled = m_TmiParamSet.m_Enabled;
		Period = m_TmiParamSet.m_Period;
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CCuvsDoc::EditTmiParam(long DeviceID, long TmiType, BOOL bEnabled, long Period)
{
	TRY
	{
		m_TmiParamSet.m_strFilter = "DeviceID = ? AND TmiType = ?";
		m_TmiParamSet.m_Param1 = DeviceID;
		m_TmiParamSet.m_Param2 = TmiType;
		m_TmiParamSet.Requery();
		if (m_TmiParamSet.IsEOF())
		{ // no such record - add new one
			m_TmiParamSet.AddNew();
			m_TmiParamSet.m_DeviceID = DeviceID;
			m_TmiParamSet.m_TmiType = TmiType;
			m_TmiParamSet.m_LastTmi = CTime(2000, 1, 1, 0, 0, 0);
		}
		else
		{
			m_TmiParamSet.Edit();
		}
		m_TmiParamSet.m_Enabled = bEnabled;
		m_TmiParamSet.m_Period = Period;
		m_TmiParamSet.Update();
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении/записи ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

/*BOOL CCuvsDoc::QueryTmiToBeAsked(long StationID)
{
	TRY
	{
		m_TmiToBeAskedSet.m_strFilter = "StationDevices.DeviceID = TmiQueryParameters.DeviceID AND StationID = ? AND Enabled = TRUE";
		m_TmiToBeAskedSet.m_Param1 = StationID;
		m_TmiToBeAskedSet.Requery();
		if (m_TmiToBeAskedSet.IsEOF())
			return TRUE;
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}
*/
void CCuvsDoc::OnStationsDelete() 
{
/*	long TmiType, IpAddress, Period, DeviceID;
	CTime LastTime, CurrentTime = CTime::GetCurrentTime();
	QueryTmiToBeAsked(35);
	while (GetNextTmiParamToBeAsked(DeviceID, TmiType, IpAddress, LastTime, Period))
	{
		int days = Period/24/60;
		Period = Period - days*24*60;
		int hours = Period/60;
		Period = Period - hours *60;
		int mins = Period;
		CTimeSpan DeltaTimeMax = CTimeSpan(days, hours, mins, 0);
		if ((CurrentTime - LastTime) > DeltaTimeMax)
		{ // query
			TRACE("\tQuery: IP=%d TmiType=%d\n", IpAddress, TmiType);
			TmiReceived(DeviceID, TmiType, CurrentTime);
		}
	}*/
}
