/* HostTrafficRenderer.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	20 Jun 2007 - initial creation
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables

*/


#include "stdafx.h"
#include "HostTrafficRenderer.h"
#include "GuiAPI.h"

///////////////////////////////////////////////////////////////////////////
// CHostTrafficRenderer

CHostTrafficRenderer::CHostTrafficRenderer()
{
	m_pHost = NULL;
	time(&m_LatestDataTime);
	m_LatestDataTime += 30*60;
	m_EarliestDataTime = m_LatestDataTime-12*60*60;
	m_EarliestDataTime -= 30*60;
	m_bTcpOut = m_bUdpOut = m_bIcmpOut = m_bIgmpOut = m_bTotalSumOut = LOGICAL_TRUE;
	m_bTcpIn = m_bUdpIn = m_bIcmpIn = m_bTotalSumIn = LOGICAL_FALSE;
	m_bOctetsMode = LOGICAL_TRUE;
	m_MaxOctetsValue = m_MaxPacketsValue = 1;
	m_pEnds = NULL;
}

CHostTrafficRenderer::~CHostTrafficRenderer()
{
	if (m_pEnds)
		free(m_pEnds);
}

//virtual
void CHostTrafficRenderer::OnRenderScene()
{
	COpenGL_Chart2D::OnRenderScene();
}

//virtual
void CHostTrafficRenderer::OnInitialized()
{
	COpenGL_Chart2D::OnInitialized();
}

//virtual
void CHostTrafficRenderer::OnSetDefaultOptions()
{
	COpenGL_Chart2D::OnSetDefaultOptions();
}

//virtual
void CHostTrafficRenderer::RenderCurves()
{
	//glPushAttrib(GL_ENABLE_BIT);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// UDP
	unsigned char cGreenHigh[4] = { 0, 255,0,128 };
	if (m_bUdpOut)
	{
		glColor4ubv(cGreenHigh);
		RenderRecordList(m_UdpOutIntervals);
	}
	unsigned char cGreenLow[4] = { 0,128, 0,128 };
	if (m_bUdpIn)
	{
		glColor4ubv(cGreenLow);
		RenderRecordList(m_UdpInIntervals);
	}

	// TCP
	unsigned char cBlueHigh[4] = { 0, 0,255,128 };
	if (m_bTcpOut)
	{
		glColor4ubv(cBlueHigh);
		RenderRecordList(m_TcpOutIntervals);
	}
	unsigned char cBlueLow[4] = { 0,0,192, 128 };
	if (m_bTcpIn)
	{
		glColor4ubv(cBlueLow);
		RenderRecordList(m_TcpInIntervals);
	}

	// ICMP
	unsigned char cRedHigh[4] = { 255, 0,0,128 };
	if (m_bIcmpOut)
	{
		glColor4ubv(cRedHigh);
		RenderRecordList(m_IcmpOutIntervals);
	}
	unsigned char cRedLow[4] = { 128, 0,0,128 };
	if (m_bIcmpIn)
	{
		glColor4ubv(cRedLow);
		RenderRecordList(m_IcmpInIntervals);
	}

	// IGMP
	unsigned char cMagentaHigh[4] = { 255, 0,255,128 };
	if (m_bIgmpOut)
	{
		glColor4ubv(cMagentaHigh);
		RenderRecordList(m_IgmpOutIntervals);
	}
	//glPopAttrib();
	time_t current_time; time(&current_time);
	double cur_time_value = TimeToChart(current_time);
	if (cur_time_value > GetFrustrumLeft() && cur_time_value < GetFrustrumRight())
	{
		glColor4ubv(GetGridMarksColor());
		glLineStipple(1, 0xAAAA);
		glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINES);
			glVertex2d(cur_time_value, GetFrustrumBottom());
			glVertex2d(cur_time_value, GetFrustrumTop());
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
}

//virtual
double CHostTrafficRenderer::GetMaxYValue()
{
	if (IsInOctetsMode())
		return 1.1*m_MaxOctetsValue;
	return 1.1*m_MaxPacketsValue;
}

//virtual
void CHostTrafficRenderer::FormatXMark(char *pszMark, int MarkMaxLen, double value)
{
	time_t TimeValue = ChartToTime(value);
    struct tm *pTM = localtime(&TimeValue);

    strftime(pszMark, MarkMaxLen, "%d.%m.%Y %H:%M", pTM);
}

void CHostTrafficRenderer::Init(CHost *pHost)
{
	m_pHost = pHost;
	SetupCamera();
}

void CHostTrafficRenderer::Refresh()
{
	if (!m_pHost)
		return; // not initialized yet

	m_EarliestDataTime = ChartToTime(GetFrustrumLeft());
	m_LatestDataTime = ChartToTime(GetFrustrumRight());
	LoadDataFromDB();
	SetDefaultView();
}

void CHostTrafficRenderer::LoadDataFromDB()
{
	m_MaxOctetsValue = m_MaxPacketsValue = 1;
	
	// UDP
	if (m_bUdpOut)
		QueryTrafficOut(IPPROTO_UDP, m_UdpOutIntervals);
	if (m_bUdpIn)
		QueryTrafficIn(IPPROTO_UDP, m_UdpInIntervals);
		
	// TCP
	if (m_bTcpOut)
		QueryTrafficOut(IPPROTO_TCP, m_TcpOutIntervals);
	if (m_bTcpIn)
		QueryTrafficIn(IPPROTO_TCP, m_TcpInIntervals);

	// ICMP
	if (m_bIcmpOut)
		QueryTrafficOut(IPPROTO_ICMP, m_IcmpOutIntervals);
	if (m_bIcmpIn)
		QueryTrafficIn(IPPROTO_ICMP, m_IcmpInIntervals);
	
	// IGMP
	if (m_bIgmpOut)
		QueryTrafficOut(IPPROTO_IGMP, m_IgmpOutIntervals);

	RecalcTotalSumOut();
	RecalcTotalSumIn();
}

void CHostTrafficRenderer::QueryTrafficOut(unsigned short proto, CListOfTrafficRecords &ListOfIntervals)
{
	STrafficRecord *pRecords = NULL;
	int nRecords = 0;
	g_pDB->QueryTrafficFromHost(m_pHost, proto, m_EarliestDataTime, m_LatestDataTime, pRecords, nRecords);
	MakeIntervalList(pRecords, nRecords, ListOfIntervals);
}

void CHostTrafficRenderer::QueryTrafficIn(unsigned short proto, CListOfTrafficRecords &ListOfIntervals)
{
	STrafficRecord *pRecords = NULL;
	int nRecords = 0;
	g_pDB->QueryTrafficToHost(m_pHost, proto, m_EarliestDataTime, m_LatestDataTime, pRecords, nRecords);
	MakeIntervalList(pRecords, nRecords, ListOfIntervals);
}

int CompareEndsProc(const void *elem1, const void *elem2)
{
	time_t *pTime1 = (time_t *)elem1;
	time_t *pTime2 = (time_t *)elem2;
	return (*pTime1 - *pTime2);
}

void CHostTrafficRenderer::MakeIntervalList(STrafficRecord *pRecords, int nRecords, CListOfTrafficRecords &ListOfIntervals)
{
	time_t current_time;
	time(&current_time);
	m_pEnds = (time_t *)realloc(m_pEnds, (2+2*nRecords)*sizeof(time_t)); // each record has 2 ends, end possibly 2 ends of whole view range
	int nEnds = 0;
	time_t *pEnd = m_pEnds;

	// First end
	*pEnd = m_EarliestDataTime;
	++pEnd; ++nEnds;
	// Records
	time_t latest_time = 0;
	for (int i = 0; i < nRecords; i++)
	{
		STrafficRecord *pThisRecord = pRecords+i;
		if (pThisRecord->FromTime >= m_EarliestDataTime && pThisRecord->FromTime <= m_LatestDataTime)
		{
			*pEnd = pThisRecord->FromTime;
			++pEnd; ++nEnds;
		}
		if (pThisRecord->ToTime >= m_EarliestDataTime && pThisRecord->ToTime <= m_LatestDataTime)
		{
			*pEnd = pThisRecord->ToTime;
			++pEnd; ++nEnds;
		}
		if (pThisRecord->ToTime > latest_time)
			latest_time = pThisRecord->ToTime;
	}
	// Last end (if required)
	if ((m_LatestDataTime - latest_time) >= g_pGui->GetTrafficStoreInterval())
	{
		if (m_LatestDataTime < current_time)
			*pEnd = m_LatestDataTime;
		else
			*pEnd = current_time;
		++pEnd; ++nEnds;
	}
	
	::qsort(m_pEnds, nEnds, sizeof(time_t), CompareEndsProc);

	ListOfIntervals.resize(0);
	for (i = 0; i < nEnds; i++)
	{
		time_t from = m_pEnds[i];
		if (from >= current_time)
			continue;

		for (int j = i+1; j < nEnds; j++)
		{
			time_t to = m_pEnds[j];
			if (abs(to - from) > 10)
			{
				STrafficRecord interval;
				memset(&interval, 0, sizeof(interval));
				interval.FromTime = from;
				interval.ToTime = to;
				ListOfIntervals.push_back(interval);
				i = j-1;
				break;
			}
		}
	}

	for (CListOfTrafficRecords::iterator it = ListOfIntervals.begin(); it != ListOfIntervals.end(); it++)
	{
		STrafficRecord *pInterval = it;
		for (i = 0; i < nRecords; i++)
		{
			STrafficRecord *pThisRecord = pRecords+i;
			if (pInterval->ToTime <= pThisRecord->FromTime)
				continue;
			if (pInterval->FromTime >= pThisRecord->ToTime)
				continue;
			if (pThisRecord->ToTime <= pInterval->FromTime)
				continue;
			if (pThisRecord->FromTime >= pInterval->ToTime)
				continue;
			// calc overlapping
			time_t left = max(pInterval->FromTime, pThisRecord->FromTime);
			time_t right = min(pInterval->ToTime, pThisRecord->ToTime);
			if (left == right)
				continue;
			ASSERT(right > left);

			pInterval->octets_per_second += pThisRecord->octets_per_second;
			if (pInterval->octets_per_second > m_MaxOctetsValue)
				m_MaxOctetsValue = pInterval->octets_per_second;

			pInterval->packets_per_second += pThisRecord->packets_per_second;
			if (pInterval->packets_per_second > m_MaxPacketsValue)
				m_MaxPacketsValue = pInterval->packets_per_second;
		}
	}
}

void CHostTrafficRenderer::RenderRecordList(CListOfTrafficRecords &ListOfRecords)
{
	double y;
	glBegin(GL_LINE_STRIP);
		for (CListOfTrafficRecords::iterator it = ListOfRecords.begin(); it != ListOfRecords.end(); it++)
		{
			STrafficRecord *pInterval = it;
			if (IsInOctetsMode())
				y = pInterval->octets_per_second;
			else
				y = pInterval->packets_per_second;
			glVertex2d(TimeToChart(pInterval->FromTime), y);
			glVertex2d(TimeToChart(pInterval->ToTime), y);
		}
	glEnd();
}

void CHostTrafficRenderer::SetOctetsMode(LOGICAL bOctetsMode)
{
	m_bOctetsMode = bOctetsMode;
	SetDefaultView();
}

time_t CHostTrafficRenderer::GetDateTimeAt(int ScreenX)
{
	double value = ScreenToWorldX(ScreenX);
	return ChartToTime(value);
}

double CHostTrafficRenderer::GetTrafficAt(int ScreenY)
{
	return ScreenToWorldY(ScreenY);
}

void CHostTrafficRenderer::EnableTcpOut(LOGICAL bEnable)
{
	m_bTcpOut = bEnable;
	if (m_bTcpOut)
		QueryTrafficOut(IPPROTO_TCP, m_TcpOutIntervals);
	RecalcTotalSumOut();
	Redraw();
}

void CHostTrafficRenderer::EnableTcpIn(LOGICAL bEnable)
{
	m_bTcpIn = bEnable;
	if (m_bTcpIn)
		QueryTrafficIn(IPPROTO_TCP, m_TcpInIntervals);
	RecalcTotalSumIn();
	Redraw();
}

void CHostTrafficRenderer::EnableUdpOut(LOGICAL bEnable)
{
	m_bUdpOut = bEnable;
	if (m_bUdpOut)
		QueryTrafficOut(IPPROTO_UDP, m_UdpOutIntervals);
	RecalcTotalSumOut();
	Redraw();
}

void CHostTrafficRenderer::EnableUdpIn(LOGICAL bEnable)
{
	m_bUdpIn = bEnable;
	if (m_bUdpIn)
		QueryTrafficIn(IPPROTO_UDP, m_UdpInIntervals);
	RecalcTotalSumIn();
	Redraw();
}

void CHostTrafficRenderer::EnableIcmpOut(LOGICAL bEnable)
{
	m_bIcmpOut = bEnable;
	if (m_bIcmpOut)
		QueryTrafficOut(IPPROTO_ICMP, m_IcmpOutIntervals);
	RecalcTotalSumOut();
	Redraw();
}

void CHostTrafficRenderer::EnableIcmpIn(LOGICAL bEnable)
{
	m_bIcmpIn = bEnable;
	if (m_bIcmpIn)
		QueryTrafficIn(IPPROTO_ICMP, m_IcmpInIntervals);
	RecalcTotalSumIn();
	Redraw();
}

void CHostTrafficRenderer::EnableIgmpOut(LOGICAL bEnable)
{
	m_bIgmpOut = bEnable;
	if (m_bIgmpOut)
		QueryTrafficOut(IPPROTO_IGMP, m_IgmpOutIntervals);
	RecalcTotalSumOut();
	Redraw();
}

void CHostTrafficRenderer::EnableTotalSumOut(LOGICAL bEnable)
{
	m_bTotalSumOut = bEnable;
	RecalcTotalSumOut();
	Redraw();
}

void CHostTrafficRenderer::EnableTotalSumIn(LOGICAL bEnable)
{
	m_bTotalSumIn = bEnable;
	RecalcTotalSumIn();
	Redraw();
}

void CHostTrafficRenderer::RecalcTotalSumOut()
{
	if (!m_bTotalSumOut)
		return; // do nothing
}

void CHostTrafficRenderer::RecalcTotalSumIn()
{
	if (!m_bTotalSumIn)
		return; // do nothing
}

static zero_time = 1184000000;
double CHostTrafficRenderer::TimeToChart(time_t t)
{
	return t - zero_time;
}

time_t CHostTrafficRenderer::ChartToTime(double c)
{
	return zero_time + (time_t)(c+0.49999);
}
