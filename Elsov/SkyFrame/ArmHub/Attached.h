#ifndef __ATTACHED_H_INCLUDED__
#define __ATTACHED_H_INCLUDED__

class CStation;

class CAttachedBlock
{
	// Constructors-destructors
public:
	CAttachedBlock(int ViewIndex);
	~CAttachedBlock();

	// Public members
public:
	int m_ViewIndex;
	CStation *m_pStation;
};

#endif // __ATTACHED_H_INCLUDED__