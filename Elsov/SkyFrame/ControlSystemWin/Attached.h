#ifndef __ATTACHED_H_INCLUDED__
#define __ATTACHED_H_INCLUDED__

// Forward declarations
class IControlTargetAPI;
class CControlObject;
class CTargetInstance;
class CAttachedBlockForObject;

class CAttachedBlock
{
// Constructors-destructors
public:
	CAttachedBlock(int ViewIndex, CTargetInstance *pTargetInstance);
	virtual ~CAttachedBlock();
// Public methods
	CTargetInstance *GetTargetInstance() { return m_pTargetInstance; }
	IControlTargetAPI *GetTarget() { return m_pTarget; }
	BOOL EqualInstances(CAttachedBlock *pAnother) { return m_pTargetInstance == pAnother->m_pTargetInstance; }
	// Drag-drop
	virtual BOOL CanDragItem() { return FALSE; }
	virtual BOOL CanDropItem(CAttachedBlock *pDragged) { return FALSE; }
	// Double dispathing here....
	virtual void OnAfterDrop(CAttachedBlock *pDragged) {}
	virtual void DropOn(CAttachedBlockForObject &DropOn) {}

// Public members
public:
	int m_ViewIndex;

// Protected methods
protected:
	CTargetInstance *m_pTargetInstance;
	IControlTargetAPI *m_pTarget;
};


class CAttachedBlockForObject : public CAttachedBlock
{
// Constructors-destructors
public:
	CAttachedBlockForObject(int ViewIndex, CTargetInstance *pTargetInstance, CControlObject *pControlObject);
	virtual ~CAttachedBlockForObject();

// Public methods
public:
	CControlObject *GetObject() { return m_pObject; }
	// Drag-drop
	virtual BOOL CanDragItem() { return TRUE; }
	virtual BOOL CanDropItem(CAttachedBlock *pDropped);
	virtual void OnAfterDrop(CAttachedBlock *pDragged);
	virtual void DropOn(CAttachedBlockForObject &DropOn);

// Public members
public:

// Protected methods
protected:
	CControlObject *m_pObject;
};

#endif // __ATTACHED_H_INCLUDED__