// Titov.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Titov.h"

#include "TitovFrame.h"
#include "TitovDoc.h"
#include "TitovView.h"
#include "ParametersDialog.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct SParticle
{
	double x,y;		// position
	double vx, vy;	// velocity
	int color;		// 0 ������� �������� �����, 1 - ������
};

// ���������� ��������
// ��� ������� - � ������, �������� - � �/�, ��������� - � �/�**2
const int nMaxParticles = 65536;		// ������������ ����� ������
SParticle particle[nMaxParticles];		// ���� �������
double Xleft = -1.;			// ����� ������� (� ������)
double Xcenter = 0.;		// ����������� (� ������)
double YGapTop = 0.75;		// ������� ���� �����������
double YGapBottom = 0.6;	// ������ ���� �����������
double HoleSize, HolePosition;
double Xright = +1.;		// ������ ������� (� ������)
double Ymax = 1.;			// ������� ������� (� ������)
double Ymin = 0;			// ������ ������� (� ������)
double Vinit;			// ��������� �������� ���� ������ (� ������/���)
// Collisions
double DeltaVtop;		// ������� ��������, ������� �������� �������, �������� � ������� ������
double DeltaVbottom;	// ������� ��������, ������� �������� �������, �������� � ������ ������
double DeltaVside;		// ������� ��������, ������� �������� �������, �������� � ������� ������
double loss;			// ����������� ���������� ������������ �������

int nParticles;					// �������� ����� ������
double g;				// ��������� ���������� ������� (� �/�**2)
double Rparticle;		// ������ ����� �������  (� ������)

const double PI = 3.1415926;
const char *pszUnknownFileName = "<unknown>";
CString FileName;		// ��� ����� � ���������� �����������

// ����������� �������� (����������)
double			Time;				// ������� ����� (� ��������)
double			DeltaTime;			// ��� �� ������� (� ��������)
unsigned long	nTimeSteps;			// ���������� ���������� ����� �� �������
unsigned long	nCollisionsR;		// ���������� ������������ � ������ �����
unsigned long	nCollisionsL;		// ���������� ������������ � ����� �����
unsigned long	nRight;				// ���������� ������ � ������ �����
unsigned long	nLeft;				// ���������� ������ � ����� �����
double			VaverageR;			// ������� �������� ������ � ������ �����
double			VaverageL;			// ������� �������� ������ � ����� �����

double			LastTimeWritten;	// �����, ������� ���� �������� � ���� ��������� ���
int				nFluxFromLeft;
int				nFluxFromRight;

char szTime[128], szNLeft[128], szNRight[128];

// ���������� ��������������� ����� � ��������� from...to
double random(double from, double to)
{
	return (to-from)*rand()/RAND_MAX + from;
}

// ��������� ������� ���������� ����� ����� ��������� (����� ��������)
double Distance2(SParticle &p1, SParticle &p2)
{
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return (dx*dx+dy*dy);
}

// ��������� ���������� ����� ����� ��������� (����� ��������)
double Distance(SParticle &p1, SParticle &p2)
{
	return sqrt(Distance2(p1, p2));
}

// ����������, ��������� �� ��� ������� �� ����������, ���������� �������,
// ����� ����� ���� ������� �� ��������������
BOOL InContact(SParticle &p1, SParticle &p2)
{
	return (Distance(p1, p2) < 2*Rparticle);
}

// ����������, ���������� �� �������
BOOL Approaching(SParticle &p1, SParticle &p2)
{
	double dVx, dVy;	// �������� ��������� �� ����
	if (p1.x < p2.x)
		dVx = p1.vx - p2.vx;
	else
		dVx = p2.vx - p1.vx;

	if (p1.y < p2.y)
		dVy = p1.vy - p2.vy;
	else
		dVy = p2.vy - p1.vy;

	if (dVx > 0 && dVy > 0)
		return TRUE;	// ���������� �� ����� ����
	if (dVx < 0 && dVy < 0)
		return FALSE;	// ��������� �� ����� ����
	if (dVx > 0 && dVy < 0)
		return (dVx > dVy);
	return (dVy > dVx);
}

// �������� �������
double GetVelocity(SParticle &p)
{
	return sqrt(p.vx*p.vx + p.vy*p.vy);
}

// ������������ ������������ ������
void Collision(SParticle &p1, SParticle &p2)
{
	double factor = sqrt(1.-loss);
	double dummy = p2.vx;
	p2.vx = p1.vx*factor;
	p1.vx = dummy*factor;
	dummy = p2.vy;
	p2.vy = p1.vy*factor;
	p1.vy = dummy*factor;

	double angle = random(-PI/8, PI/8);
	double sine = sin(angle);
	double cosine = cos(angle);
	double vxnew = p1.vx*cosine + p1.vy*sine;
	double vynew = -p1.vx*sine + p1.vy*cosine;
	p1.vx = vxnew;
	p1.vy = vynew;
	vxnew = p2.vx*cosine + p2.vy*sine;
	vynew = -p2.vx*sine + p2.vy*cosine;
	p2.vx = vxnew;
	p2.vy = vynew;

	if (p1.x < Xcenter)
		++nCollisionsL;
	else
		++nCollisionsR;
}

// ������ ��������� ������ �������� (������ ���������� � ������ Vinit)
void RandomVelocity(SParticle &p)
{
	double angle = random(0, PI*2);			// ����������� �������� ������� - ��������� ���� 0...2PI
	p.vx = Vinit * cos(angle);
	p.vy = Vinit * sin(angle);
}

// ��������� ��������� ��� ������ �� ��������� ������ � ����� ��������, ����� ���� ����� ������� �������
// �� ���� ��� �� ��������� ����������, ������� �������� ������ ������� (��� ����� ����� ��������
// ������� ������ ������ ������� ����� �� ���� ���, � ����� ����� ������������ ������ ����� ����� �� ���� ���������).
void CalcTimeStep()
{
	double V, Vmax = 0;
	for (int i = 0; i < nParticles; i++)
	{
		V = GetVelocity(particle[i]);
		if (V > Vmax)
			Vmax = V;
	}
	if (Vmax == 0)
		Vmax = Vinit;
	if (Vmax == 0)
		Vmax = 0.1;
	DeltaTime =  (Rparticle/4)/Vmax;
}

// ������������ ����������
void RecalcStat()
{
	int i;
	nRight = nLeft = 0;
	VaverageR = VaverageL = 0;
	// ������� ������ ��������� ������ ���������, ������� ������ ������� � ������ ��������
	for (i = 0; i < nParticles; i++)
	{
		if (particle[i].x > Xcenter)
		{
			++nRight;
			VaverageR += GetVelocity(particle[i]);
		}
		else
		{
			++nLeft;
			VaverageL += GetVelocity(particle[i]);
		}
	}
	// ������ ����� �� ����� ������, ����� ����� ������� (�� ������� ��� ������, ����� ������ ���)
	if (nRight != 0)
		VaverageR = VaverageR/nRight;
	else
		VaverageR = 0;
	if (nLeft != 0)
		VaverageL = VaverageL/nLeft;
	else
		VaverageL = 0;
}

const char *pszSectionName = "Values";
int LoadInt(char *pszValueName, int nDefaultValue)
{
	return AfxGetApp()->GetProfileInt(pszSectionName, pszValueName, nDefaultValue);
}

void SaveInt(char *pszValueName, int nValue)
{
	AfxGetApp()->WriteProfileInt(pszSectionName, pszValueName, nValue);
}

double LoadDouble(char *pszValueName, double fDefaultValue)
{
	int iDefaultValue = (int)(fDefaultValue*1000);
	int iValue = LoadInt(pszValueName, iDefaultValue);
	return iValue/1000.;
}

void SaveDouble(char *pszValueName, double fValue)
{
	AfxGetApp()->WriteProfileInt(pszSectionName, pszValueName, (int)(fValue*1000));
}

//  �������������� ������������� ������
BOOL InitialDistribution()
{
	BOOL bTouching;		// ������������� �� �������
	int i,j;			// ���������� ������
	Time = 0;			// ������� �����
	nTimeSteps = 0;		// ���������� ���������� ����� �� �������
	nCollisionsR = 0;	// ���������� ������������ � ������ �����
	nCollisionsL = 0;	// ���������� ������������ � ����� �����
	
	CParametersDialog dlg;
	dlg.m_nParticles = LoadInt("nParticles", 1000);
	dlg.m_G = LoadDouble("g", 9.8);
	dlg.m_LossRatio = LoadDouble("CollisionLossRatio", 0.01);
	dlg.m_RParticle = LoadDouble("RParticle", 0.01);
	dlg.m_Xleft = LoadDouble("Xleft", -1);
	dlg.m_Xright = LoadDouble("Xright", +1);
	dlg.m_Ybottom = LoadDouble("Ybottom", 0);
	dlg.m_Ytop = LoadDouble("Ytop", +1);
	dlg.m_WallXPosition = LoadDouble("WallXPosition", 0);
	dlg.m_HolePosition = LoadDouble("HolePosition", 0.7);
	dlg.m_HoleSize = LoadDouble("HoleSize", 0.2);
	dlg.m_Vinit = LoadDouble("Vinit", 2);
	dlg.m_BounceTop = LoadDouble("DeltaVtop", 1);
	dlg.m_BounceBottom = LoadDouble("DeltaVbottom", 1);
	dlg.m_BounceSide = LoadDouble("DeltaVside", 1);
	dlg.m_FileName = pszUnknownFileName;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	nParticles = dlg.m_nParticles;
	SaveInt("nParticles", nParticles);
	g = dlg.m_G;
	SaveDouble("g", g);
	loss = dlg.m_LossRatio;
	SaveDouble("CollisionLossRatio", loss);
	Rparticle = dlg.m_RParticle;
	SaveDouble("RParticle", Rparticle);
	Xleft = dlg.m_Xleft;
	SaveDouble("Xleft", Xleft);
	Xright = dlg.m_Xright;
	SaveDouble("Xright", Xright);
	Ymax = dlg.m_Ytop;
	SaveDouble("Ytop", Ymax);
	Ymin = dlg.m_Ybottom;
	SaveDouble("Ybottom", Ymin);
	Xcenter = dlg.m_WallXPosition;
	SaveDouble("WallXPosition", Xcenter);
	HoleSize = dlg.m_HoleSize;
	HolePosition = dlg.m_HolePosition;
	YGapTop = HolePosition + HoleSize/2;
	YGapBottom = HolePosition - HoleSize/2;
	SaveDouble("HolePosition", dlg.m_HolePosition);
	SaveDouble("HoleSize", dlg.m_HoleSize);
	Vinit = dlg.m_Vinit;
	SaveDouble("Vinit", Vinit);
	DeltaVbottom = dlg.m_BounceBottom;
	SaveDouble("DeltaVbottom", DeltaVbottom);
	DeltaVtop = dlg.m_BounceTop;
	SaveDouble("DeltaVtop", DeltaVtop);
	DeltaVside = dlg.m_BounceSide;
	SaveDouble("DeltaVside", DeltaVside);
	FileName = dlg.m_FileName;

	srand(time(NULL));	// ������ ���������� ��������� �����
	// ������� � ����� ��������
	for (i = 0; i < nParticles/2; i++)
	{
		do
		{
			bTouching = FALSE;
			particle[i].x = random(Xleft+Rparticle, Xcenter-Rparticle);
			particle[i].y = random(Ymin+Rparticle, Ymax-Rparticle);
			for (j = 0; j<i; j++)
			{
				if (InContact(particle[i], particle[j]))
				{
					bTouching = TRUE;
					break;
				}
			}
		} while (bTouching);
		// ���������� ����������, ������ ��������
		RandomVelocity(particle[i]);
		particle[i].color = 0; // red
	}

	// ������ � ������ ��������
	for (i = nParticles/2; i < nParticles; i++)
	{
		do
		{
			bTouching = FALSE;
			particle[i].x = random(Xcenter+Rparticle, Xright-Rparticle);
			particle[i].y = random(Ymin+Rparticle, Ymax-Rparticle);
			for (j = nParticles/2; j<i; j++)
			{
				if (InContact(particle[i], particle[j]))
				{
					bTouching = TRUE;
					break;
				}
			}
		} while (bTouching);
		// ���������� ����������, ������ ��������
		RandomVelocity(particle[i]);
		particle[i].color = 1; // blue
	}
	CalcTimeStep();
	RecalcStat();
	return TRUE;
}

void MoveParticle(SParticle &p, double dt)
{
	BOOL IsLeftBefore = p.x < Xcenter;
	// �� ��� �
	p.x = p.x + p.vx*dt;
	BOOL IsLeftAfter = p.x < Xcenter;
	if (IsLeftAfter != IsLeftBefore)
	{
		if (IsLeftBefore)
			++nFluxFromLeft;
		else
			++nFluxFromRight;
	}
	// �� ��� Y
	p.y = p.y + p.vy*dt - g*dt*dt/2;
	p.vy = p.vy - g*dt;
}

// Bounce = ���������, ������
// ��������� �� ������������ ������ (�������� ���� �-���� ���������� ��������)
inline void BounceX(SParticle &p)
{
	p.vx = -p.vx;
}

// ��������� �� �������������� ������ (�������� ���� y-���� ���������� ��������)
inline void BounceY(SParticle &p)
{
	p.vy = -p.vy;
}

// ���������� ����������� ���������� � ����
void WriteStat()
{
	FILE *fd = fopen(FileName, "a");
	if (fd)
	{
		double FluxFromLeft, FluxFromRight;
		if (LastTimeWritten == Time)
		{
			FluxFromLeft = FluxFromRight = 0;
		}
		else
		{
			FluxFromLeft = nFluxFromLeft/(Time-LastTimeWritten);
			FluxFromRight = nFluxFromRight/(Time-LastTimeWritten);
		}

		fprintf(fd, "%7.2f\t%d\t%7.2f\t%7.2f\t\t%d\t%7.2f\t%7.2f\n", Time, nLeft, VaverageL, FluxFromLeft, nRight, VaverageR, FluxFromRight);
		fclose(fd);
		LastTimeWritten = Time;
		nFluxFromLeft = nFluxFromRight = 0;
	}
}

// ����� ���������� ������ �� �
static int CompareParticleProc(const void *elem1, const void *elem2)
{
	SParticle *p1 = (SParticle *)elem1;
	SParticle *p2 = (SParticle *)elem2;
    if (p1->x < p2->x)
		return -1;
	else
		return 1;
}

// ��������� ������������� ������ ���������� ����
// ���������� TRUE ���� � ���������� ����� ���� ��������� ���� �� ���� ������������ �������
// � ������ �������� ��� �� �������� (� ���������� ���� ���������� �������� ���� ����� �������)
// ��� ���� ���� ������� ���������� �� ����� �������� � ������
BOOL OneTimeStep()
{
	int i, j;
	double dt = DeltaTime/2;
	
	// ������� ������� ������� �� �������� ���������� ����
	for (i = 0; i < nParticles; i++)
	{
		MoveParticle(particle[i], dt);
	}
	
	// ��������� ������������ ������ ����� �����
	::qsort(particle, nParticles, sizeof(SParticle), CompareParticleProc);
	for (i = 0; i < nParticles; i++)
	{
		for (j = i+1; j < nParticles; j++)
		{
			if ((particle[j].x - particle[i].x) > 2*Rparticle)
				break;
			if (InContact(particle[i], particle[j]) && Approaching(particle[i], particle[j]))
			{
				Collision(particle[i], particle[j]);
			}
		}
	}
	// ������ ����� ������� ������� �� ������ �������� ���������� ���� (� ������������� ����������)
	for (i = 0; i < nParticles; i++)
	{
		MoveParticle(particle[i], dt);
	}

	// ������ ��������� ������������ ������ �� ��������
	for (i = 0; i < nParticles; i++)
	{
		double xx = particle[i].x;
		double yy = particle[i].y;
		if (yy > (Ymax-Rparticle))
		{ // ������� ������
			BounceY(particle[i]);
			particle[i].vy -= DeltaVtop;
		}
		else if (yy < (Ymin + Rparticle))
		{ // ������ ������
			BounceY(particle[i]);
			particle[i].vy += DeltaVbottom;
		}
		if (xx < (Xleft+Rparticle))
		{ // ����� ������
			BounceX(particle[i]);
			particle[i].vx += DeltaVside;
		}
		else if (xx > (Xright-Rparticle))
		{ // ������ ������
			BounceX(particle[i]);
			particle[i].vx -= DeltaVside;
		}
		else if ((yy > (YGapTop-Rparticle)) || (yy < (YGapBottom+Rparticle)))
		{
			if (xx < Xcenter && xx > (Xcenter-Rparticle) && particle[i].vx > 0)
			{ // ������� �� ������ � ����� � �����������, ��������� ����� �� ����� ����������� � ����� � ��� - ��������
				BounceX(particle[i]);
				// � ��������� �������� �����
				particle[i].vx -= DeltaVside;
			}
			else if (xx > Xcenter && xx < (Xcenter+Rparticle) && particle[i].vx < 0)
			{ // ������� �� ������ � ����� � �����������, ��������� ������ �� ����� ����������� � ����� � ��� - ��������
				BounceX(particle[i]);
				// � ��������� �������� ������
				particle[i].vx += DeltaVside;
			}
		}
	}

	Time = Time+DeltaTime;
	nTimeSteps = nTimeSteps+1;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTitovApp

BEGIN_MESSAGE_MAP(CTitovApp, CWinApp)
	//{{AFX_MSG_MAP(CTitovApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitovApp construction

CTitovApp::CTitovApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTitovApp object

CTitovApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTitovApp initialization

BOOL CTitovApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	if (!InitialDistribution())
		return FALSE;
	
	SetRegistryKey(_T("Kozhevnikov"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTitovDoc),
		RUNTIME_CLASS(CTitovFrame),       // main SDI frame window
		RUNTIME_CLASS(CTitovView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	FILE *fd = fopen(FileName, "w");
	if (fd)
	{
		fprintf(fd, "Particles:\n");
		fprintf(fd, "nParticles = %d\n", nParticles);
		fprintf(fd, "RParticle = %g m\n", Rparticle);

		fprintf(fd, "\nCollisions:\n");
		fprintf(fd, "CollisionLossRatio = %g\n", loss);
		fprintf(fd, "DeltaVbottom = %g m/sec\n", DeltaVbottom);
		fprintf(fd, "DeltaVtop = %g m/sec\n", DeltaVtop);
		fprintf(fd, "DeltaVside = %g m/sec\n", DeltaVside);

		fprintf(fd, "\nGeometry:\n");
		fprintf(fd, "Xleft = %g m\n", Xleft);
		fprintf(fd, "Xright = %g m\n", Xright);
		fprintf(fd, "Ytop = %g m\n", Ymax);
		fprintf(fd, "Ybottom = %g m\n", Ymin);
		fprintf(fd, "WallXPosition = %g m\n", Xcenter);
		fprintf(fd, "HolePosition = %g m\n", HolePosition);
		fprintf(fd, "HoleSize = %g m\n", HoleSize);

		fprintf(fd, "\nInitial distribution:\n");
		fprintf(fd, "Vinit = %g m/sec\n", Vinit);

		fprintf(fd, "\nOther:\n");
		fprintf(fd, "g = %g m/sec**2\n", g);

		fprintf(fd, "\nTime\tnLeft\t<V>left\t<flux>left\t\tnRight\t<V>right\t<flux>right\n");
		fclose(fd);
	}

	m_hLeftPen = CreatePen(PS_SOLID, 1, RGB(255,0,0));
	m_hRightPen = CreatePen(PS_SOLID, 1, RGB(0,0,255));


	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_EMAIL_HYPERLINK_STATIC, m_EMailHyperlink);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CTitovApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CTitovApp message handlers


BOOL CTitovApp::OnIdle(LONG lCount) 
{
	CWinApp::OnIdle(lCount);
	// ���������� ��� ���������� � ����
	if ((nTimeSteps % 1000) == 0)
		WriteStat();

	BOOL bChanged = OneTimeStep();
	if (bChanged)
	{
		// ����������� ����������
		RecalcStat();
		CalcTimeStep();
	}
	if ((nTimeSteps % 15) == 0)
	{
		m_pDoc->UpdateAllViews(NULL);
	}
	return 1;
}

// ��� ��������� - ��������� �������� ���������� � � ��������
int XScreen(double x, CRect &rect)
{
	double a = rect.Width()/(Xright-Xleft);
	double b = -a*Xleft;
	return (int)(a*x+b);
}

// ��� ��������� - ��������� �������� ���������� Y � ��������
int YScreen(double y, CRect &rect)
{
	double a = rect.Height()/(Ymin-Ymax);
	double b = rect.Height()*Ymax/(Ymax-Ymin);
	return (int)(a*y+b);
}

void CTitovApp::Draw(CDC *pDC, CRect DrawRect)
{
	// 1. ��������� �������������
	m_DrawRect = DrawRect;

	// 2. �����������
	pDC->MoveTo(XScreen(Xcenter, DrawRect), YScreen(Ymin, DrawRect));
	pDC->LineTo(XScreen(Xcenter, DrawRect), YScreen(YGapBottom, DrawRect));
	pDC->MoveTo(XScreen(Xcenter, DrawRect), YScreen(Ymax, DrawRect));
	pDC->LineTo(XScreen(Xcenter, DrawRect), YScreen(YGapTop, DrawRect));

	// 3. �������
	DrawParticles(pDC);
}

void CTitovApp::DrawParticles(CDC *pDC)
{
	int i;
	int x, y, Rx, Ry;
	Rx = XScreen(Rparticle, m_DrawRect)-XScreen(0, m_DrawRect);
	Ry = YScreen(Rparticle, m_DrawRect)-YScreen(0, m_DrawRect);
	// ������� �����
	HGDIOBJ hOldPen = SelectObject(pDC->m_hDC, m_hLeftPen);
	for (i = 0; i < nParticles; i++)
	{
		if (particle[i].color != 0)
			continue; // skip non-green particles
		x = XScreen(particle[i].x, m_DrawRect);
		y = YScreen(particle[i].y, m_DrawRect);
		pDC->Ellipse(x-Rx, y-Ry, x+Rx, y+Ry);
	}	
	// ������ ������
	SelectObject(pDC->m_hDC, m_hRightPen);
	for (i = 0; i < nParticles; i++)
	{
		if (particle[i].color != 1)
			continue; // skip non-blue particles
		x = XScreen(particle[i].x, m_DrawRect);
		y = YScreen(particle[i].y, m_DrawRect);
		pDC->Ellipse(x-Rx, y-Ry, x+Rx, y+Ry);
	}
	SelectObject(pDC->m_hDC, hOldPen);
}

int CTitovApp::ExitInstance() 
{
	DeleteObject(m_hLeftPen);
	DeleteObject(m_hRightPen);

	return CWinApp::ExitInstance();
}

char *CTitovApp::GetTimeString()
{
	sprintf(szTime, "Time = %7.2f, step = %.3g", Time, DeltaTime);
	return szTime;
}

char *CTitovApp::GetNLeftString()
{
	sprintf(szNLeft, "Left: %d (%4.1f%%), <V>=%7.3f", nLeft, (nLeft*100./nParticles), VaverageL);
	return szNLeft;
}

char *CTitovApp::GetNRightString()
{
	sprintf(szNRight, "Right: %d (%4.1f%%), <V>=%7.3f", nRight, (nRight*100./nParticles), VaverageR);
	return szNRight;
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_EMailHyperlink.SetURL("mailto:gaid@gf.miee.ru; evgeny@kozhevnikov.org");	
	m_EMailHyperlink.SetUnderline(TRUE);
	m_EMailHyperlink.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_HAND_FOR_HYPERLINK));
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
