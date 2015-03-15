#ifndef __ATTACHED_H_INCLUDED__
#define __ATTACHED_H_INCLUDED__

// Forward declarations
class CAbstractForm;
class CStation;
class CCuvsDoc;

class CAttachedBlock
{
	// Constructors-destructors
public:
	CAttachedBlock(int ViewIndex, CAbstractForm *pForm);
	virtual ~CAttachedBlock();

// Public overridables
public:

// Public methods
public:
	long GetStationID();
	void SetStation(CStation *pStation) { m_pStation = pStation; }
	BOOL CanDragItem();
	BOOL CanDropItem();

// Public members
public:
	int m_ViewIndex;
	CStation *m_pStation;
	CAbstractForm *m_pAbstractForm;
	static CCuvsDoc *m_pDoc;
};

#endif // __ATTACHED_H_INCLUDED__