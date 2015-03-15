/* HostTrafficRenderer.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	20 Jun 2007 - initial creation
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables

*/

#ifndef __HOST_TRAFFIC_RENDERER_H_INCLUDED__
#define __HOST_TRAFFIC_RENDERER_H_INCLUDED__

#include "OpenGLRenderer.h"
#include "Network.h"

// Forward declarations
class IDatabaseAPI;

class CHostTrafficRenderer : public COpenGL_Chart2D
{	
// Construction/destruction
public:
	CHostTrafficRenderer();
	virtual ~CHostTrafficRenderer();

// Public overridable methods
public:
	virtual void OnRenderScene();
	virtual void OnInitialized();
	virtual void OnSetDefaultOptions();
	virtual void RenderCurves();

// Protected overridable methods
protected:
	virtual double GetMinXValue() { return TimeToChart(m_EarliestDataTime); }
	virtual double GetMaxXValue() { return TimeToChart(m_LatestDataTime); }
	virtual double GetMinYValue() { return -0.1*GetMaxYValue(); }
	virtual double GetMaxYValue();
	virtual void FormatXMark(char *pszMark, int MarkMaxLen, double value);
	// Event handlers

// Public methods
public:
	void Init(CHost *pHost);
	void Refresh();
	time_t GetEarliestTime() { return m_EarliestDataTime; }
	time_t GetLatestTime() { return m_LatestDataTime; }
	LOGICAL NeedTcpOut() { return m_bTcpOut; }
	void EnableTcpOut(LOGICAL bEnable);
	LOGICAL NeedTcpIn() { return m_bTcpIn; }
	void EnableTcpIn(LOGICAL bEnable);
	LOGICAL NeedUdpOut() { return m_bUdpOut; }
	void EnableUdpOut(LOGICAL bEnable);
	LOGICAL NeedUdpIn() { return m_bUdpIn; }
	void EnableUdpIn(LOGICAL bEnable);
	LOGICAL NeedIcmpOut() { return m_bIcmpOut; }
	void EnableIcmpOut(LOGICAL bEnable);
	LOGICAL NeedIcmpIn() { return m_bIcmpIn; }
	void EnableIcmpIn(LOGICAL bEnable);
	LOGICAL NeedIgmpOut() { return m_bIgmpOut; }
	void EnableIgmpOut(LOGICAL bEnable);
	LOGICAL NeedTotalSumOut() { return m_bTotalSumOut; }
	void EnableTotalSumOut(LOGICAL bEnable);
	LOGICAL NeedTotalSumIn() { return m_bTotalSumIn; }
	void EnableTotalSumIn(LOGICAL bEnable);

	LOGICAL IsInOctetsMode() { return m_bOctetsMode; }
	void SetOctetsMode(LOGICAL bOctetsMode);

	time_t GetDateTimeAt(int ScreenX);
	double GetTrafficAt(int ScreenY);

// Protected methods
protected:
	void LoadDataFromDB();
	void MakeIntervalList(STrafficRecord *pRecords, int nRecords, CListOfTrafficRecords &ListOfIntervals);
	void QueryTrafficOut(unsigned short proto, CListOfTrafficRecords &ListOfIntervals);
	void QueryTrafficIn(unsigned short proto, CListOfTrafficRecords &ListOfIntervals);
	void RenderRecordList(CListOfTrafficRecords &ListOfRecords);
	void RecalcTotalSumOut();
	void RecalcTotalSumIn();

// Private methods
private:
	double TimeToChart(time_t time);
	time_t ChartToTime(double chart);

// Public members
public:

// Protected members
protected:
	time_t m_EarliestDataTime, m_LatestDataTime;
	double m_MaxPacketsValue, m_MaxOctetsValue;
	CListOfTrafficRecords m_IcmpOutIntervals, m_IcmpInIntervals;
	CListOfTrafficRecords m_IgmpOutIntervals;
	CListOfTrafficRecords m_TcpOutIntervals, m_TcpInIntervals;
	CListOfTrafficRecords m_UdpOutIntervals, m_UdpInIntervals;
	CHost *m_pHost;
	LOGICAL m_bTcpOut, m_bTcpIn;
	LOGICAL m_bUdpOut, m_bUdpIn;
	LOGICAL m_bIcmpOut, m_bIcmpIn;
	LOGICAL m_bIgmpOut;
	LOGICAL m_bTotalSumOut, m_bTotalSumIn;
	LOGICAL m_bOctetsMode;
	time_t *m_pEnds;

// Private members
private:
};

#endif //__HOST_TRAFFIC_RENDERER_H_INCLUDED__

