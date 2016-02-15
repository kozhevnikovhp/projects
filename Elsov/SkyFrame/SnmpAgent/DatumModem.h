#include "../HardwareTest/Datum.h"
#include "../HardwareTest/Datum_PSM_4900.h"
#include "../HardwareTest/Datum_M500.h"
#include "../HardwareTest/Datum_M7.h"
#include "snmp_stuff.h"

MC_ErrorCode processDatumRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType);
