#ifndef __ATTACHED_H_INCLUDED__
#define __ATTACHED_H_INCLUDED__

class CDevice;

class CAttachedBlock
{
	// Constructors-destructors
public:
	CAttachedBlock(int ViewIndex, CDevice *pDevice);
	~CAttachedBlock();

	// Public members
public:
	int m_ViewIndex;
	CDevice *m_pDevice;
	int m_DeviceNumber;
};

#endif // __ATTACHED_H_INCLUDED__