#ifndef __DEVICE_TELNET_SOCKET_H_INCLUDED__
#define __DEVICE_TELNET_SOCKET_H_INCLUDED__

#include "TelnetSockets.h"

// Forward declarations

class CDevice;

class CDeviceTelnetSocket : public CTelnetSocket
{
// Construction-destruction
public:
	CDeviceTelnetSocket() { m_pDevice = NULL; }
	virtual ~CDeviceTelnetSocket() {}
	
// Public methods
	inline void SetDevice(CDevice *pDevice) { m_pDevice = pDevice; }
// Protected overridables
protected:
	virtual void OnSymbolToUser(char symbol);
// Protected members
	CDevice *m_pDevice;
};

#endif // __DEVICE_TELNET_SOCKET_H_INCLUDED__