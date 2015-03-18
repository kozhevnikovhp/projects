#ifndef __DEVICE_FROM_NAME_H_INCLUDED__
#define __DEVICE_FROM_NAME_H_INCLUDED__

// Devices
#include "Datum_PSM_4900.h"
#include "Datum_M500.h"
#include "Datum_M7.h"
#include "Radyne.h"
#include "StelaM.h"
#include "WaveSat.h"
#include "Advantech.h"
#include "Pozharsky.h"
#include "NsgFrad.h"
#include "AgilentPowerMeter.h"
#include "Vertex.h"

CDevice *DeviceFromName(CString &DevName);

#endif //__DEVICE_FROM_NAME_H_INCLUDED__