// TmiDatabaseDoc.cpp : implementation of the CTmiDatabaseDoc class
//

#include "stdafx.h"
#include "TmiDatabase.h"

#include "TmiStationsSet.h"
#include "TmiDatabaseDoc.h"
#include "TmiSet.h"
#include "TmiQuerySet.h"
#include "WorkingHoursSet.h"
#include "TmiQueryParamsDialog.h"
#include "RequlationsQueryParamsDialog.h"
#include "WorkingHoursDialog.h"
#include "DeleteTMIDialog.h"
#include "ProgressDlg.h"
#include "ActionsSet.h"
#include "DeleteTMISet.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiDatabaseDoc

IMPLEMENT_DYNCREATE(CTmiDatabaseDoc, CDocument)

BEGIN_MESSAGE_MAP(CTmiDatabaseDoc, CDocument)
	//{{AFX_MSG_MAP(CTmiDatabaseDoc)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_QUERY_FROMTIMETOTIME, OnQueryFromTimeToTime)
	ON_COMMAND(ID_QUERY_REQULATIONSFROMTIMETOTIME, OnQueryRequlationsFromTimeToTime)
	ON_COMMAND(ID_QUERY_WORKINGHOURS, OnQueryWorkingHours)
	ON_COMMAND(ID_QUERY_DELETETMIFROMTIMETOTIME, OnQueryDeleteTmiFromTimeToTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiDatabaseDoc construction/destruction

CTmiDatabaseDoc::CTmiDatabaseDoc()
{
	// TODO: add one-time construction code here

}

CTmiDatabaseDoc::~CTmiDatabaseDoc()
{
}

BOOL CTmiDatabaseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTmiDatabaseDoc diagnostics

#ifdef _DEBUG
void CTmiDatabaseDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTmiDatabaseDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTmiDatabaseDoc commands

void CTmiDatabaseDoc::OnFileImport() 
{
	const int BufferSize = 0xFFFF;
	int count = 0;
	char buffer[BufferSize];
	ZeroMemory(buffer, sizeof(buffer));

	CFileDialog fd(TRUE, _T(".txt"), NULL,OFN_ALLOWMULTISELECT,
	_T("Текстовые файлы (*txt)|*txt|Все файлы (*.*)|*.*|"));
	fd.m_ofn.lpstrTitle = "Укажите импортируемые файлы";
	fd.m_ofn.lpstrFile = buffer;
	fd.m_ofn.nMaxFile = BufferSize;
	if (fd.DoModal() == IDCANCEL) return;

	CTmiSet set;
	set.Open();
	set.m_Time = CTime::GetCurrentTime();

	CActionsSet ActionsSet;
	ActionsSet.Open();
	ActionsSet.m_Time = CTime::GetCurrentTime();

	CProgressDlg dlg;
	dlg.Create();
	int TotalFiles = 0;
	int nRecords = 0;
	POSITION pos = fd.GetStartPosition();
	while (pos)
	{
		++TotalFiles;
		fd.GetNextPathName(pos);
	}
	dlg.SetRange(0, TotalFiles);

	pos = fd.GetStartPosition();
	CString Path, StatusString;
	CWaitCursor cursor;
	while (pos)
	{
		if (dlg.CheckCancelButton())
			break;
		Path = fd.GetNextPathName(pos);
		StatusString.Format("Importing  %d-th  file  of  %d  (%d records added)", ++count, TotalFiles, nRecords);
		dlg.SetStatus(StatusString);
		nRecords += Import(Path, &set, &ActionsSet);
		dlg.SetPos(count);
	}						
}

int CTmiDatabaseDoc::Import(CString &Path, CTmiSet *pSet, CActionsSet *pActionsSet)
{
	char pszBuffer[1024];
	CStdioFile FileIn;
	int nRecords = 0;

	// Check file name template
	int BackSlash = Path.ReverseFind('\\');
	if (BackSlash >= 0)
	{
		BackSlash++;
		if (Path[BackSlash+2] != '_')
		{
			MessageBox(NULL, Path + "\n\nInvalid file name", "Error", MB_ICONEXCLAMATION);
			return 0;
		}
	}
	else
		return 0;
	if (!FileIn.Open(Path, CFile::modeRead))
	{
		MessageBox(NULL, Path + "\n\nCannot open file", "Error", MB_ICONEXCLAMATION);
		return 0;
	}
	while (1)
	{
		int day = 0, month = 0, year = 0;
		int hour = 0, min = 0, sec = 0;
		double sn1 = 0, sn2 = 0;
		double level1 = 0, level2 = 0;
		int offset1 = 0, offset2 = 0;
		double t1 = -273.15, t2 = -273.15;
		double p1 = 0, p2 = 0;
		double DeltaLevel = 0;
		if (!FileIn.ReadString(pszBuffer, sizeof(pszBuffer)))
			break;
		if (strlen(pszBuffer) == 0)
			continue;
		int scanfed = sscanf(pszBuffer, "%d.%d.%d %d:%d:%d\t\t%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%lf\t%d\t%lf\t%lf\t%lf",
			&day, &month, &year, &hour, &min, &sec,
			&sn1, &level1, &offset1, &t1, &p1,
			&sn2, &level2, &offset2, &t2, &p2,
			&DeltaLevel);
		CTime Time(year, month, day, hour, min, sec);
		CString ErrorMessage;
		
		if ((sn1 > 0.5 && sn1 < 21) || (fabs(sn1)<0.1 && offset1 == 0))
		{
			BOOL bSuccess = TRUE;
			do
			{
				Sleep(0);
				TRY
				{
					pSet->AddNew();
					pSet->m_Azs = true;
					pSet->m_Time = Time;
					pSet->m_StationID = m_StationSet.m_StationID;
					pSet->m_SN = sn1;
					pSet->m_Level = level1;
					pSet->m_Offset = offset1;
					pSet->m_T = t1;
					pSet->m_Power = p1;
					pSet->Update();
				}
				CATCH(CDBException, e)
					ErrorMessage.Format("Error code %d:\n%s\n(%s)", e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin);
					MessageBox(NULL, ErrorMessage, "Проблемы при записи ТМИ в БД - переоткрытие таблицы", MB_ICONEXCLAMATION);
					pSet->Close();
					pSet->Open();
					bSuccess = FALSE;
				END_CATCH
			} while (!bSuccess);
			++nRecords;
		}
		
		if ((sn2 > 0.5 && sn2 < 21) || (fabs(sn2)<0.1 && offset2 == 0))
		{
			BOOL bSuccess = TRUE;
			do
			{
				Sleep(0);
				TRY
				{
					pSet->AddNew();
					pSet->m_Azs = false;
					pSet->m_Time = Time;
					pSet->m_StationID = m_StationSet.m_StationID;
					pSet->m_SN = sn2;
					pSet->m_Level = level2;
					pSet->m_Offset = offset2;
					pSet->m_T = t2;
					pSet->m_Power = p2;
					pSet->Update();
				}
				CATCH(CDBException, e)
					ErrorMessage.Format("Error code %d:\n%s\n(%s)", e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin);
					MessageBox(NULL, ErrorMessage, "Проблемы при записи ТМИ в БД - переоткрытие таблицы", MB_ICONEXCLAMATION);
					pSet->Close();
					bSuccess = pSet->Open();
					bSuccess = FALSE;
				END_CATCH
			} while (!bSuccess);
			++nRecords;
		}

		if (fabs(DeltaLevel) > 0.001)
		{
			BOOL bSuccess = TRUE;
			do
			{
				Sleep(0);
				TRY
				{
					pActionsSet->AddNew();
					pActionsSet->m_StationID = m_StationSet.m_StationID;
					pActionsSet->m_Time = Time;
					pActionsSet->m_DeltaLevel = DeltaLevel;
					pActionsSet->Update();
				}
				CATCH(CDBException, e)
					MessageBox(NULL, e->m_strError, "Проблемы при записи регулировок в БД", MB_ICONEXCLAMATION);
					pActionsSet->Close();
					bSuccess = pActionsSet->Open();
					bSuccess = FALSE;
				END_CATCH
			} while (!bSuccess);
		}
	}
	return nRecords;
}

void CTmiDatabaseDoc::OnQueryFromTimeToTime() 
{
	CTmiQueryParamsDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;
	CWaitCursor cursor;
	CTmiFromTimeToTimeQuerySet set;
	set.m_StationParam = m_StationSet.m_StationID;
	set.m_StartTime = dlg.m_BeginTime;
	set.m_EndTime = dlg.m_EndTime;
	set.m_bAzsParam = dlg.m_bAZS;
	set.Open();

	CFileDialog fdialog(FALSE, _T("txt"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Все файлы (*.*)|*.*||"));
	fdialog.m_ofn.lpstrTitle = "Укажите имя создаваемого файла";
	if (fdialog.DoModal()!=IDOK)
		return;

	int nTotalRecords = 0;
	while (!set.IsEOF())
	{
		++nTotalRecords;
		set.MoveNext();
	}
	set.MoveFirst();

	CProgressDlg ProgressDlg;
	ProgressDlg.Create();
	ProgressDlg.SetRange(0, nTotalRecords);
	CString time_str, StatusString;
	char *pszZSType = "UZS";
	if (dlg.m_bAZS)
		pszZSType = "AZS";
	int nRecord = 0;
	FILE *fd = fopen(fdialog.GetPathName(), "w");
	if (fd)
	{
		fprintf(fd, "TMI of %s_%s\n", pszZSType, m_StationSet.m_Name);
		while (!set.IsEOF())
		{
			if (abs(set.m_Offset) < 15000 && set.m_SN <21 && set.m_SN > 1)
			{
				time_str = set.m_Time.Format("%d.%m.%Y %H:%M:%S");
				fprintf(fd, "%s\t%5.2f\t%5.0f\t%+d\t%4.0f\t%6.2f\n", time_str, set.m_SN, set.m_Level, set.m_Offset, set.m_T, set.m_Power);
				if (nRecord/10*10 == nRecord)
				{
					StatusString.Format("Record %d of %d (time = %s)", nRecord, nTotalRecords, time_str);
					ProgressDlg.SetStatus(StatusString);
					ProgressDlg.SetPos(nRecord);
				}
				++nRecord;
			}
			set.MoveNext();
		}
	}
	else
		MessageBox(NULL, "Cannot open file!!!", fdialog.GetPathName(), MB_ICONEXCLAMATION);
	fclose(fd);
}

int CTmiDatabaseDoc::HowManyRegulations(long StationID, CTime FromTime, CTime ToTime, CActionsSet *pActionsSet)
{
	pActionsSet->m_StationIDParam = StationID;
	pActionsSet->m_StartTime = FromTime;
	pActionsSet->m_EndTime = ToTime;
	if (!pActionsSet->IsOpen())
		pActionsSet->Open();
	else
		pActionsSet->Requery();
	int nRegulations = 0;
	while (!pActionsSet->IsEOF())
	{
		++nRegulations;
		pActionsSet->MoveNext();
	}
	return nRegulations;
}

void CTmiDatabaseDoc::OnQueryRequlationsFromTimeToTime() 
{
	CRegulationsQueryParamsDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;
	CActionsSet ActionsSet;
	CStationSet StationsSet;
	StationsSet.Open();

	CFileDialog fdialog(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Все файлы (*.*)|*.*||"));
	fdialog.m_ofn.lpstrTitle = "Укажите имя создаваемого файла";
	if (fdialog.DoModal()!=IDOK)
		return;

	CProgressDlg ProgressDlg;
	ProgressDlg.Create();
	int nTotalStations = 0;
	while (!StationsSet.IsEOF())
	{
		++nTotalStations;
		StationsSet.MoveNext();
	}
	CTimeSpan DeltaTime = dlg.m_EndTime - dlg.m_BeginTime;
	int nDays = DeltaTime.GetDays()+1;
	ProgressDlg.SetRange(0, nTotalStations*(nDays+1));

	CString time_str, StatusString;
	int nRecord = 0;
	FILE *fd = fopen(fdialog.GetPathName(), "w");
	if (fd)
	{
		CString BeginTimeStr = dlg.m_BeginTime.Format("%d.%m.%Y");
		CString EndTimeStr = dlg.m_EndTime.Format("%d.%m.%Y");
		fprintf(fd, "Суточное количество управляющих воздействий на АЗС с %s по %s\n\n", BeginTimeStr, EndTimeStr);
		fprintf(fd, "Дата");
		StationsSet.MoveFirst();
		while (!StationsSet.IsEOF())
		{
			fprintf(fd, "\t%s", StationsSet.m_Name);
			StationsSet.MoveNext();
		}
		fprintf(fd, "\n");
		

		CTime BeginTime = dlg.m_BeginTime;
		CTime EndTime = BeginTime + CTimeSpan(1, 0, 0, 0);
			
		while (BeginTime < dlg.m_EndTime)
		{
			time_str = BeginTime.Format("%d.%m.%Y %a");
			fprintf(fd, "%s", time_str);
			StationsSet.MoveFirst();
			while (!StationsSet.IsEOF())
			{
				StatusString.Format("АЗС %s, %s", StationsSet.m_Name, time_str);
				ProgressDlg.SetStatus(StatusString);
				int nRegulations = HowManyRegulations(StationsSet.m_StationID, BeginTime, EndTime, &ActionsSet);
				if (nRegulations == 0)
					fprintf(fd, "\t");
				else
					fprintf(fd, "\t%d", nRegulations);
				StationsSet.MoveNext();
				ProgressDlg.StepIt();
				if (ProgressDlg.CheckCancelButton())
					break;
			}
			fprintf(fd, "\n");
			
			BeginTime = EndTime;
			EndTime += CTimeSpan(1, 0, 0, 0);
			if (ProgressDlg.CheckCancelButton())
				break;
		}
		// All interval
		fprintf(fd, "Итого:");
		StationsSet.MoveFirst();
		time_str.Format("с %s по %s", dlg.m_BeginTime.Format("%d.%m.%Y"), dlg.m_EndTime.Format("%d.%m.%Y"));
		while (!StationsSet.IsEOF())
		{
			StatusString.Format("АЗС %s, %s", StationsSet.m_Name, time_str);
			ProgressDlg.SetStatus(StatusString);
			int nRegulations = HowManyRegulations(StationsSet.m_StationID, dlg.m_BeginTime, dlg.m_EndTime, &ActionsSet);
			if (nRegulations == 0)
				fprintf(fd, "\t");
			else
				fprintf(fd, "\t%d", nRegulations);
			StationsSet.MoveNext();
		}
		fprintf(fd, "\n");
	}
	else
		MessageBox(NULL, "Cannot open file!!!", fdialog.GetPathName(), MB_ICONEXCLAMATION);
	fclose(fd);
}

CString CTmiDatabaseDoc::CalcWorkingTimeReport
(
	long StationID,
	CTime RequiredIntervalStart,
	CTime RequiredIntervalEnd,
	CWorkingHoursSet *pAzsUzsSet,
	CTmiFromTimeToTimeQuerySet *pUzsSet,
	BOOL bHoursNeeded
)
{
	const CTimeSpan DefinitelyKnownTimeInterval = CTimeSpan(0, 0, 20, 0); // 20 minutes
	CTime QueryStartTime = RequiredIntervalStart - DefinitelyKnownTimeInterval;
	CTime QueryEndTime = RequiredIntervalEnd + DefinitelyKnownTimeInterval;
	pAzsUzsSet->m_StationParam = StationID;
	pAzsUzsSet->m_StartTime = QueryStartTime;
	pAzsUzsSet->m_EndTime = QueryEndTime;
	pAzsUzsSet->Requery();

	int OnTime = 0;
	int OffTime = 0;
	CTime PrevTime = pAzsUzsSet->m_Time;
	BOOL bOnPrev;
	if (pAzsUzsSet->m_Azs == TRUE)
		bOnPrev = TRUE; // If TMI from AZS exists this AZS operates and it is ON of course
	else
		bOnPrev = (pAzsUzsSet->m_SN > 0.5); // TMI from UZS
		
	while (!pAzsUzsSet->IsEOF())
	{
		CTime ThisTime = pAzsUzsSet->m_Time;
		BOOL bOnThis;
		if (pAzsUzsSet->m_Azs == TRUE)
			bOnThis = TRUE; // If TMI from AZS exists this AZS operates and it is ON of course
		else
			bOnThis = (pAzsUzsSet->m_SN > 0.5); // TMI from UZS

		CTimeSpan DeltaTime = CTimeSpan(0, 0, 0, 0);
		if (ThisTime < RequiredIntervalEnd && ThisTime >= RequiredIntervalStart)
		{
			if (PrevTime < RequiredIntervalStart)
				DeltaTime = ThisTime - RequiredIntervalStart; // different days
			else
				DeltaTime = ThisTime - PrevTime; // the same day
		}
		else if (ThisTime > RequiredIntervalEnd)
		{
			if (PrevTime < RequiredIntervalEnd)
				DeltaTime = RequiredIntervalEnd - PrevTime;  // different days
		}
		int seconds = DeltaTime.GetTotalSeconds();
		if (seconds > 0)
		{
			if (DeltaTime < DefinitelyKnownTimeInterval)
			{
				if (bOnPrev && bOnThis)
					OnTime += seconds;
				else if (!bOnPrev && !bOnThis)
					OffTime += seconds;
				else
				{
					OnTime += seconds/2;
					OffTime += (seconds - seconds/2);
				}
			}
		}
		
		PrevTime = ThisTime;
		bOnPrev = bOnThis;
		pAzsUzsSet->MoveNext();
	}
	
	// New query for operating time
	pUzsSet->m_StationParam = StationID;
	pUzsSet->m_StartTime = QueryStartTime;
	pUzsSet->m_EndTime = QueryEndTime;
	pUzsSet->Requery();

	int OperateTime = 0;
	int NotOperateTime = 0;
	PrevTime = pUzsSet->m_Time;
		
	while (!pUzsSet->IsEOF())
	{
		CTime ThisTime = pUzsSet->m_Time;

		CTimeSpan DeltaTime = CTimeSpan(0, 0, 0, 0);
		if (ThisTime < RequiredIntervalEnd && ThisTime >= RequiredIntervalStart)
		{
			if (PrevTime < RequiredIntervalStart)
				DeltaTime = ThisTime - RequiredIntervalStart; // different days
			else
				DeltaTime = ThisTime - PrevTime; // the same day
		}
		else if (ThisTime > RequiredIntervalEnd)
		{
			if (PrevTime < RequiredIntervalEnd)
				DeltaTime = RequiredIntervalEnd - PrevTime;  // different days
		}
		int seconds = DeltaTime.GetTotalSeconds();
		if (seconds > 0)
		{
			if (DeltaTime < DefinitelyKnownTimeInterval)
				OperateTime += seconds;
			else
				NotOperateTime += seconds;
		}
		
		PrevTime = ThisTime;
		pUzsSet->MoveNext();
	}
	
	CTimeSpan AllIntervalDeltaTime = RequiredIntervalEnd - RequiredIntervalStart;
	int TotalSeconds = AllIntervalDeltaTime.GetTotalSeconds();
	int OnPercentage = (int)(OnTime*100./TotalSeconds+0.4999999);
	int OffPercentage = (int)(OffTime*100./TotalSeconds+0.4999999);
	int UnknownPercentage = 100 - OnPercentage - OffPercentage;
	if (OperateTime > OnTime)
		OnTime = OperateTime; // эта ситуация возможна, когда прямой о обратный канал слабые, АЗС то видит, то не видит прямой канал, и может прислать нули как свой SN
							// АЗС замерила SN, он равен нулю, а потом вдруг связь восстановилась, и мы все узнали, что АЗС не видит УЗС. Но ТМИ есть, что является признаком работы АЗС.
							// В это время УЗС тоже может не видеть АЗС (такое случается обычно при включении)
							// Корректируем время включенности, т.к. АЗС была, конечно, включена, раз присылала ТМИ, пусть даже не видя иногда УЗС
	int UnknownTime = TotalSeconds - OnTime - OffTime;
	CString str;
	if (UnknownPercentage == 100)
		str = "\t";
	else
	{
		int OperatePercentage = 0;
		if (OnTime != 0)
		{
			OperatePercentage = (int)(OperateTime*100./OnTime+0.4999999);
		}
		double SecondsInHour = 60*60;
		if (bHoursNeeded)
			str.Format("\t%4.1f/%4.1f/%4.1f/%4.1f", OnTime/SecondsInHour, OperateTime/SecondsInHour, OffTime/SecondsInHour, UnknownTime/SecondsInHour);
		else
			str.Format("\t%3d/%3d/%3d/%3d", OnPercentage, OperatePercentage, OffPercentage, UnknownPercentage);
	}
	return str;
}

void CTmiDatabaseDoc::OnQueryDeleteTmiFromTimeToTime() 
{
	CDeleteTMIDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	CWaitCursor cursor;
	int nDeletedTmiRecords = 0;
	int nDeletedActionRecords = 0;

	CProgressDlg ProgressDlg;
	ProgressDlg.Create();
	ProgressDlg.SetRange(0, 3);
	ProgressDlg.SetStatus("Requering of TMI data table...");

	CDeleteTMISet TmiSet;
	TmiSet.m_StartTime = dlg.m_BeginTime;
	TmiSet.m_EndTime = dlg.m_EndTime;
	TmiSet.m_StationParam = m_StationSet.m_StationID;
	if (!TmiSet.Open())
		return;
	ProgressDlg.StepIt();
	ProgressDlg.SetStatus("Deleting TMI records...");

	while (!TmiSet.IsEOF())
	{
		++nDeletedTmiRecords;
		TmiSet.Delete();
		TmiSet.MoveNext();
	}

	ProgressDlg.StepIt();
	ProgressDlg.SetStatus("Requering actions data table...");

	CActionsSet ActionsSet;
	ActionsSet.m_StartTime = dlg.m_BeginTime;
	ActionsSet.m_EndTime = dlg.m_EndTime;
	ActionsSet.m_StationIDParam = m_StationSet.m_StationID;
	if (!ActionsSet.Open())
		return;

	ProgressDlg.StepIt();
	ProgressDlg.SetStatus("Deleting action records...");
	while (!ActionsSet.IsEOF())
	{
		++nDeletedActionRecords;
		ActionsSet.Delete();
		ActionsSet.MoveNext();
	}
	CString report;
	report.Format("%d TMI records deleted\n%d action records deleted", nDeletedTmiRecords, nDeletedActionRecords);
	MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, report, "Information", MB_ICONINFORMATION);
}

void CTmiDatabaseDoc::OnQueryWorkingHours() 
{
	const CTimeSpan OneDayTimeInterval = CTimeSpan(1, 0, 0, 0);
	BOOL bCancel = FALSE;

	CWorkingHoursDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;
	
	CTimeSpan PeriodTime = dlg.m_EndTime-dlg.m_BeginTime;
	int nDays = PeriodTime.GetDays()+1;
	CWaitCursor cursor;

	CFileDialog fdialog(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Все файлы (*.*)|*.*||"));
	fdialog.m_ofn.lpstrTitle = "Укажите имя создаваемого файла";
	if (fdialog.DoModal()!=IDOK)
		return;

	CProgressDlg ProgressDlg;
	ProgressDlg.Create();
	ProgressDlg.SetStatus("Opening of data tables...");

	CWorkingHoursSet AzsUzsTmiSet;
	AzsUzsTmiSet.m_StartTime = AzsUzsTmiSet.m_EndTime = CTime::GetCurrentTime();
	AzsUzsTmiSet.Open();

	CTmiFromTimeToTimeQuerySet UzsTmiSet;
	UzsTmiSet.m_StartTime = UzsTmiSet.m_EndTime = CTime::GetCurrentTime();
	UzsTmiSet.m_Azs = TRUE;
	UzsTmiSet.Open();
	
	CStationSet StationsSet;
	StationsSet.Open();

	CString time_str, StatusString;
	int nRecord = 0;
	FILE *fd = fopen(fdialog.GetPathName(), "w");
	if (fd)
	{
		CString BeginTimeStr = dlg.m_BeginTime.Format("%d.%m.%Y");
		CString EndTimeStr = dlg.m_EndTime.Format("%d.%m.%Y");
		fprintf(fd, "Данные о включенности АЗС с %s по %s\n\n", BeginTimeStr, EndTimeStr);
		fprintf(fd, "Включено/Работает/Выключено/Неизвестно");
		if (dlg.HoursNeeded())
			fprintf(fd, " в часах");
		else
			fprintf(fd, " в процентах ('Работает' в процентах от 'Включено')");
		fprintf(fd, "\n\nДата");
		
		int nStations = 0;
		while (!StationsSet.IsEOF())
		{
			fprintf(fd, "\t%s", StationsSet.m_Name);
			++nStations;
			StationsSet.MoveNext();
		}
		fprintf(fd, "\n");
		ProgressDlg.SetRange(0, nStations*(nDays+1)); // +1 for total report

		CTime CurrentDayStart = dlg.m_BeginTime;
		while (CurrentDayStart < dlg.m_EndTime)
		{ // day after day
			CTime NextDayStart = CurrentDayStart + OneDayTimeInterval;

			time_str = CurrentDayStart.Format("%d.%m.%Y %a");
			fprintf(fd, "%s", time_str);

			// all the stations one after another
			StationsSet.MoveFirst();
			while (!StationsSet.IsEOF())
			{
				StatusString.Format("АЗС %s, %s", StationsSet.m_Name, time_str);
				ProgressDlg.SetStatus(StatusString);
				CString str = CalcWorkingTimeReport(StationsSet.m_StationID, CurrentDayStart, NextDayStart, &AzsUzsTmiSet, &UzsTmiSet, dlg.HoursNeeded());
				fprintf(fd, str);

				ProgressDlg.StepIt();
				if (ProgressDlg.CheckCancelButton())
				{
					bCancel = TRUE;
					break;
				}
				StationsSet.MoveNext();
				//Sleep(20);
			}
			fprintf(fd, "\n");
			CurrentDayStart = NextDayStart;
			if (bCancel)
				break;
		}
		// All interval
		// all the stations one after another
		time_str.Format("с %s по %s", dlg.m_BeginTime.Format("%d.%m.%Y"), dlg.m_EndTime.Format("%d.%m.%Y"));
		fprintf(fd, "Итого:");
		StationsSet.MoveFirst();
		while (!StationsSet.IsEOF())
		{
			StatusString.Format("АЗС %s, %s", StationsSet.m_Name, time_str);
			ProgressDlg.SetStatus(StatusString);
			CString str = CalcWorkingTimeReport(StationsSet.m_StationID, dlg.m_BeginTime, dlg.m_EndTime, &AzsUzsTmiSet, &UzsTmiSet, dlg.HoursNeeded());
			fprintf(fd, str);

			ProgressDlg.StepIt();
			if (ProgressDlg.CheckCancelButton())
			{
				bCancel = TRUE;
				break;
			}
			StationsSet.MoveNext();
			Sleep(20);
		}
		fprintf(fd, "\n");
	}
	else
		MessageBox(NULL, "Cannot open file!!!", fdialog.GetPathName(), MB_ICONEXCLAMATION);
	fclose(fd);
}

