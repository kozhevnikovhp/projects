#include "stdafx.h"
#include <stdio.h>
#include "../Common/SDMS_OIDs.h"
#include "DatumModem.h"

MC_ErrorCode processDatumUnitAlarmRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	unsigned int *pAlarms = NULL;
	int index = -1;

	if (var.m_OID.isPartOfOID(OidDatumModemUnitReferenceAlarm, OidDatumModemUnitReferenceAlarmLen))
	{
		EC = pModem->GetUnitAlarms(pAlarms);
		index = DATUM_UNIT_REFERENCE_ALARM;
		printf("\t\t reference alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemUnitTestActiveAlarm, OidDatumModemUnitTestActiveAlarmLen))
	{
		EC = pModem->GetUnitAlarms(pAlarms);
		index = DATUM_UNIT_TEST_ACTIVE_ALARM;
		printf("\t\t test active alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemUnitHardwareAlarm, OidDatumModemUnitHardwareAlarmLen))
	{
		EC = pModem->GetUnitAlarms(pAlarms);
		index = DATUM_UNIT_HARDWARE_ALARM;
		printf("\t\t hardware alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemUnitOcxoOvenAlarm, OidDatumModemUnitOcxoOvenAlarmLen))
	{
		EC = pModem->GetUnitAlarms(pAlarms);
		index = DATUM_UNIT_OCXO_OVEN_ALARM;
		printf("\t\t ocxo oven alarm");
	}

	if (EC == MC_OK && pAlarms != NULL && index < pModem->GetDemodulatorAlarmCount())
	{
		printf(" = %d", pAlarms[index]);
		var.setInteger32Value(pAlarms[index]);
	}
	else
		var.setInteger32Value(-1);

	return EC;
}

MC_ErrorCode processDatumUnitRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\tunit\n");
	if (var.m_OID.isPartOfOID(OidDatumModemUnitName, OidDatumModemUnitNameLen))
	{
		printf("\t\tname ");
		char *pszName = NULL;
		EC = pModem->GetUnitName(pszName);
		if (EC == MC_OK && pszName)
		{
			var.setStringValue(pszName);
			printf("%s", pszName);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemUnitModel, OidDatumModemUnitModelLen))
	{
		printf("\t\tmodel ");
		std::string strModel;
		EC = pModem->GetModelName(strModel);
		if (EC == MC_OK)
		{
			var.setStringValue(strModel);
			printf("%s", strModel.c_str());
		}
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemUnitSerialNo, OidDatumModemUnitSerialNoLen))
	{
		printf("\t\tserial number ");
		unsigned int serialNo = 0;
		EC = pModem->GetSerialNo(serialNo);
		if (EC == MC_OK)
		{
			var.setInteger32Value(serialNo);
			printf("%d", serialNo);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemUnitVersion, OidDatumModemUnitVersionLen))
	{
		printf("\t\tversion ");
		std::string strVersion = "";
		EC = pModem->GetSoftwareVersion(strVersion);
		if (EC == MC_OK)
		{
			var.setStringValue(strVersion);
			printf("%s", strVersion.c_str());
		}
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemUnitAlarm, OidDatumModemUnitAlarmLen))
	{
		EC = processDatumUnitAlarmRequest(pModem, var, bGet);
	}

	return EC;
}

MC_ErrorCode processDatumModulatorIfRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen))
	{
		printf("\t\tfrequency ");
		unsigned int frequency;
		if (bGet)
		{ // get
			EC = pModem->GetTFrequency(frequency, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(frequency);
				printf("%d KHz", frequency);
			}
		}
		else
		{ // set
			frequency = var.m_iIntegerValue/1000;
			printf("%d KHz", frequency);
			EC = pModem->SetTFrequency(frequency, 1);
			if (EC == MC_OK)
				printf(", result = %d KHz", frequency);
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfOffset, OidModulatorIfOffsetLen))
	{
		printf("\t\tfrequency offset ");
		int offset;
		if (bGet)
		{ // get
			EC = pModem->GetModulatorShift(offset, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(offset);
				printf("%d Hz", offset);
			}
		}
		else
		{ // set
			offset = var.m_iIntegerValue;
			printf("%d Hz", offset);
			EC = pModem->SetModulatorShift(offset, 1);
			if (EC == MC_OK)
				printf(", result = %d Hz", offset);
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen))
	{
		printf("\t\tOutput level ");
		double level;
		if (bGet)
		{ // get
			EC = pModem->GetOutputLevel(level, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value((int)(level*10));
				printf("%.2f dB", level);
			}
		}
		else
		{ // set
			level = var.m_iIntegerValue/10.;
			printf("%.2f dB", level);
			EC = pModem->SetOutputLevel(level, 1);
			if (EC == MC_OK)
				printf(", result = %.2f dB", level);
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen))
	{
		printf("\t\tOutput enabled ");
		BOOL bOn;
		if (bGet)
		{ // get
			EC = pModem->IsOutputOn(bOn, 1);
			if (EC == MC_OK)
			{
				if (bOn)
				{
					var.setInteger32Value(2);
					printf("ON");
				}
				else
				{
					var.setInteger32Value(1);
					printf("OFF");
				}
			}
		}
		else
		{ // set
			bOn = (var.m_iIntegerValue == 2);
			if (bOn)
				printf("ON");
			else
				printf("OFF");
			EC = pModem->TurnOutputOn(bOn, 1);
			if (EC == MC_OK)
			{
				printf(", result = ");
				if (bOn)
					printf("ON");
				else
					printf("OFF");
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfModulation, OidModulatorIfModulationLen))
	{
		printf("\t\tModulation ");
		int modType;
		if (bGet)
		{ //  get
			EC = pModem->GetTModulationType(modType, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(modType+1);
				printf("%s", pModem->GetTModulationTypeName(modType));
			}
		}
		else
		{
			modType = var.m_iIntegerValue - 1;
			printf("%s", pModem->GetTModulationTypeName(modType));
			EC = pModem->SetTModulationType(modType, 1);
			if (EC == MC_OK)
			{
				printf(", result = %s", pModem->GetTModulationTypeName(modType));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen))
	{
		printf("\t\tSpectrum ");
		BOOL bSpectrumInverted;
		if (bGet)
		{ // get
			EC = pModem->IsTSpectralInvEnabled(bSpectrumInverted, 1);
			if (EC == MC_OK)
			{
				if (bSpectrumInverted)
				{
					var.setInteger32Value(2);
					printf("INVERTED");
				}
				else
				{
					var.setInteger32Value(1);
					printf("NORMAL");
				}
			}
		}
		else
		{ // set
			bSpectrumInverted = (var.m_iIntegerValue == 2);
			if (bSpectrumInverted)
			{
				var.setInteger32Value(2);
				printf("INVERTED");
			}
			else
			{
				var.setInteger32Value(1);
				printf("NORMAL");
			}
			EC = pModem->IsTSpectralInvEnabled(bSpectrumInverted, 1);
			if (EC == MC_OK)
			{
				printf(", result = ");
				if (bSpectrumInverted)
				{
					var.setInteger32Value(2);
					printf("INVERTED");
				}
				else
				{
					var.setInteger32Value(1);
					printf("NORMAL");
				}
			}
		}
	}

	return EC;
}

MC_ErrorCode processDatumModulatorDataRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidModulatorDataBitRate, OidModulatorDataBitRateLen))
	{
		printf("\t\tBit rate ");
		unsigned int DataRate;
		if (bGet)
		{ // get
			EC = pModem->GetTDataRate(DataRate, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(DataRate);
				printf("%d Baud", DataRate);
			}
		}
		else
		{ // set
			DataRate = var.m_iIntegerValue;
			printf("%d Baud", DataRate);
			EC = pModem->SetTDataRate(DataRate, 1);
			if (EC == MC_OK)
			{
				printf(", result = %d Baud", DataRate);
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataClockSource, OidModulatorDataClockSourceLen))
	{
		printf("\t\tClock source ");
		int source;
		EC = pModem->GetTDataClockSource(source, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(source);
			printf("%s", pModem->GetTDataClockSourceName(source));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataFecType, OidModulatorDataFecTypeLen))
	{
		printf("\t\tFEC type ");
		int mode;
		if (bGet)
		{
			EC = pModem->GetTFecMode(mode, 1);
			if (EC == MC_OK)
			{
				if (mode == 0)
					var.setInteger32Value(0); // None
				else if (1 <= mode && mode <= 4)
					var.setInteger32Value(1); // Viterbi
				else if (5 <= mode && mode <= 53)
					var.setInteger32Value(8); // LDPC
				else
					var.setInteger32Value(4); // unknown
 
				printf("%s", pModem->GetTFecModeName(mode));
			}
		}
		else
		{
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen))
	{
		printf("\t\tFEC code rate ");
		int mode;
		EC = pModem->GetTFecMode(mode, 1);
		if (EC == MC_OK)
		{
			if (mode == 0)
				var.setInteger32Value(0); // None
			else if (mode == 1 || mode == 5 || mode == 12 || mode == 19 || mode == 26 || mode == 33 || mode == 40 || mode == 47)
				var.setInteger32Value(1); // 1/2
			else if (mode == 2 || mode == 7 || mode == 14 || mode == 21 || mode == 28 || mode == 35 || mode == 42 || mode == 49)
				var.setInteger32Value(2); // 3/4
			else if (mode == 4 || mode == 9 || mode == 16 || mode == 23 || mode == 30 || mode == 37 || mode == 44 || mode == 51)
				var.setInteger32Value(3); // 7/8
			else if (mode == 8 || mode == 15 || mode == 22 || mode == 29 || mode == 36 || mode == 43 || mode == 50)
				var.setInteger32Value(4); // 14/17
			else if (mode == 10 || mode == 17 || mode == 24 || mode == 31 || mode == 38 || mode == 45 || mode == 52)
				var.setInteger32Value(5); // 10/11
			else if (mode == 11 || mode == 18 || mode == 25 || mode == 32 || mode == 39 || mode == 46 || mode == 53)
				var.setInteger32Value(6); // 16/17
			else
				var.setInteger32Value(0); // unknown

			printf("%s", pModem->GetTFecModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataFecOption, OidModulatorDataFecOptionLen))
	{
		printf("\t\tFEC code option ");
		int mode;
		EC = pModem->GetTFecMode(mode, 1);
		if (EC == MC_OK)
		{
			if (mode == 0)
				var.setInteger32Value(0); // None
			else if (mode <= 5)
				var.setInteger32Value(0); // Viterbi, no option
			else if (mode <= 11)
				var.setInteger32Value(1); // LDPC 256 block
			else if (mode <= 18)
				var.setInteger32Value(2); // LDPC 512 block
			else if (mode <= 25)
				var.setInteger32Value(3); // LDPC 1K block
			else if (mode <= 32)
				var.setInteger32Value(4); // LDPC 2K block
			else if (mode <= 39)
				var.setInteger32Value(5); // LDPC 4K block
			else if (mode <= 46)
				var.setInteger32Value(6); // LDPC 8K block
			else if (mode <= 53)
				var.setInteger32Value(7); // LDPC 16K block
			else
				var.setInteger32Value(0); // error

			printf("%s", pModem->GetTFecModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen))
	{
		printf("\t\tdifferential encoder ");
		int Mode;
		EC = pModem->GetDiffEncoderMode(Mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(Mode);
			printf("%s", pModem->GetDiffEncoderModeName(Mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataScrambler, OidModulatorDataScramblerLen))
	{
		printf("\t\tscrambler ");
		int mode;
		EC = pModem->GetScramblerMode(mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(mode);
			printf("%s", pModem->GetScramblerModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataReedSolomonMode, OidModulatorDataReedSolomonModeLen))
	{
		printf("\t\tReed-Solomon mode ");
		int Mode;
		EC = pModem->GetTReedSolomonMode(Mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(Mode);
			printf("%s", pModem->GetReedSolomonModeName(Mode));
		}
	}

	return EC;
}

MC_ErrorCode processDatumModulatorBucRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidModulatorBucCurrentMin, OidModulatorBucCurrentMinLen))
	{
		printf("\t\tCurrent min ");
		CBucStatus status;
		EC = pModem->GetBucStatus(status, 1);
		if (EC == MC_OK)
		{
			double value = status.m_CurrentMin;
			printf("%f", value);
			var.setInteger32Value((int)(value*1000));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorBucCurrentMax, OidModulatorBucCurrentMaxLen))
	{
		printf("\t\tCurrent max ");
		CBucStatus status;
		EC = pModem->GetBucStatus(status, 1);
		if (EC == MC_OK)
		{
			double value = status.m_CurrentMax;
			printf("%f", value);
			var.setInteger32Value((int)(value*1000));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorBucCurrentOut, OidModulatorBucCurrentOutLen))
	{
		printf("\t\tCurrent out ");
		CBucStatus status;
		EC = pModem->GetBucStatus(status, 1);
		if (EC == MC_OK)
		{
			double value = status.m_Current;
			printf("%f", value);
			var.setInteger32Value((int)(value/10.));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorBucLoFreq, OidModulatorBucLoFreqLen))
	{
		printf("\t\tLow frequency ");

	}
	else if (var.m_OID.isPartOfOID(OidModulatorBucPower, OidModulatorBucPowerLen))
	{
		printf("\t\tpower ");
		int mode = 0;
		EC = pModem->GetTPowerSupplyMode(mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(mode+1);
			printf("%s", pModem->GetTPowerSupplyModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorBuc10MHz, OidModulatorBuc10MHzLen))
	{
		printf("\t\t10 MHz reference ");
		BOOL b = FALSE;
		EC = pModem->IsT10MHzSupplierEnabled(b, 1);
		if (EC == MC_OK)
		{
			int value = 0;
			if (b)
			{
				printf("enabled");
				value = 1;
			}
			else
			{
				printf("disabled");
				value = 0;
			}
			var.setInteger32Value(value);
		}
	}

	return EC;
}

MC_ErrorCode processDatumModulatorRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\tmodulator\n");

	if (var.m_OID.isPartOfOID(OidDatumModemModulatorCount, OidDatumModemModulatorCountLen))
	{
		EC = MC_OK;
		var.setInteger32Value(1);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemModulatorIndex, OidDatumModemModulatorIndexLen))
	{
		EC = MC_OK;
		var.setInteger32Value(1);
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIf, OidModulatorIfLen))
	{
		EC = processDatumModulatorIfRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidModulatorData, OidModulatorDataLen))
	{
		EC = processDatumModulatorDataRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidModulatorBuc, OidModulatorBucLen))
	{
		EC = processDatumModulatorBucRequest(pModem, var, bGet);
	}

	return EC;
}

MC_ErrorCode processDatumDemodulatorStatusRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidDemodulatorStatusOffset, OidDemodulatorStatusOffsetLen))
	{
		printf("\t\toffset ");
		int offset;
		EC = pModem->GetDemodulatorOffset(offset, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(offset);
			printf("%d Hz", offset);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusInputLevel, OidDemodulatorStatusInputLevelLen))
	{
		printf("\t\tinput level ");
		double level;
		EC = pModem->GetInputLevel(level, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(level*10);
			printf("%.2f dBm", level);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusEbNo, OidDemodulatorStatusEbNoLen))
	{
		printf("\t\tS/N = ");
		double SN;
		EC = pModem->GetEbNo(SN, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(SN*10);
			printf("%.2f dB", SN);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusLock, OidDemodulatorStatusLockLen))
	{
		printf("\t\tLock = ");
		double SN;
		EC = pModem->GetEbNo(SN, 1);
		if (EC == MC_OK)
		{
			int lock = 0;
			if (SN > 0)
				lock = 1;
			var.setInteger32Value(lock);
			printf("%d", lock);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusBuffer, OidDemodulatorStatusBufferLen))
	{
		printf("\t\tBuffer = ");
		CDemIfStatus status;
		EC = pModem->GetRIfStatus(status, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(status.m_Buffer);
			printf("%d", status.m_Buffer);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusBER, OidDemodulatorStatusBERLen))
	{
		printf("\t\tBER = ");
		double BER = -1;
		EC = pModem->GetBER(BER, 1);
		if (EC == MC_OK)
		{
			char szBer[128];
			sprintf(szBer, "%g", BER);
			var.setStringValue(szBer);
			printf("%f", BER);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusSER, OidDemodulatorStatusSERLen))
	{
		printf("\t\tSER = ");
		double SER = -1;
		EC = pModem->GetSER(SER, 1);
		if (EC == MC_OK)
		{
			char szSer[128];
			sprintf(szSer, "%g", SER);
			var.setStringValue(szSer);
			printf("%f", SER);
		}
	}
	return EC;
}

MC_ErrorCode processDatumDemodulatorIfRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	if (var.m_OID.isPartOfOID(OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen))
	{
		printf("\t\tfrequency ");
		unsigned int frequency;
		if (bGet)
		{ // get
			EC = pModem->GetRFrequency(frequency, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(frequency);
				printf("%d KHz", frequency);
			}
		}
		else
		{ // set
			frequency = var.m_uiIntegerValue/1000;
			printf("%d KHz", frequency);
			EC = pModem->SetRFrequency(frequency, 1);
			if (EC == MC_OK)
				printf(", result = %d KHz", frequency);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfLowEbno, OidDemodulatorIfLowEbnoLen))
	{
		printf("\t\tlow ebno ");
		CDemIfParams params;
		EC = pModem->GetRIfParams(params, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value((int)(params.m_LowEbno*10));
			printf("%f dB", params.m_LowEbno);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfLowLevel, OidDemodulatorIfLowLevelLen))
	{
		printf("\t\tlow level ");
		CDemIfParams params;
		EC = pModem->GetRIfParams(params, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value((int)(params.m_LowLevel*10));
			printf("%f dBm", params.m_LowLevel);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen))
	{
		printf("\t\tsweep range ");
		unsigned int range;
		EC = pModem->GetSearchRange(range, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(range);
			printf("%d Hz", range);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfSweepTime, OidDemodulatorIfSweepTimeLen))
	{
		printf("\t\tsweep range ");
		CDemIfParams params;
		EC = pModem->GetRIfParams(params, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(params.m_SweepTime);
			printf("%d msec", params.m_SweepTime);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfSweepMode, OidDemodulatorIfSweepModeLen))
	{
		printf("\t\tsweep mode ");
		int mode;
		EC = pModem->GetRSweepMode(mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(mode);
			printf("%s", pModem->GetRSweepModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen))
	{
		printf("\t\tModulation ");
		int modType;
		EC = pModem->GetRModulationType(modType, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(modType+1);
			printf("%s", pModem->GetRModulationTypeName(modType));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen))
	{
		printf("\t\tSpectrum ");
		BOOL bSpectrumInverted;
		EC = pModem->IsRSpectralInvEnabled(bSpectrumInverted, 1);
		if (EC == MC_OK)
		{
			if (bSpectrumInverted)
			{
				var.setInteger32Value(2);
				printf("INVERTED");
			}
			else
			{
				var.setInteger32Value(1);
				printf("NORMAL");
			}
		}
	}
	
	return EC;
}

MC_ErrorCode processDatumDemodulatorDataRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidDemodulatorDataBitRate, OidDemodulatorDataBitRateLen))
	{
		printf("\t\tBit rate ");
		unsigned int DataRate;
		EC = pModem->GetRDataRate(DataRate, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(DataRate);
			printf("%d baud", DataRate);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataClockSource, OidDemodulatorDataClockSourceLen))
	{
		printf("\t\tClock source ");
		int source;
		EC = pModem->GetRDataClockSource(source, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(source);
			printf("%s", pModem->GetRDataClockSourceName(source));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataFecType, OidDemodulatorDataFecTypeLen))
	{
		printf("\t\tFEC type ");
		int mode;
		EC = pModem->GetRFecMode(mode, 1);
		if (EC == MC_OK)
		{
			if (mode == 0)
				var.setInteger32Value(0); // None
			else if (1 <= mode && mode <= 4)
				var.setInteger32Value(1); // Viterbi
			else if (5 <= mode && mode <= 53)
				var.setInteger32Value(8); // LDPC
			else
				var.setInteger32Value(4); // unknown
 
			printf("%s", pModem->GetRFecModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen))
	{
		printf("\t\tFEC code rate ");
		int mode;
		EC = pModem->GetRFecMode(mode, 1);
		if (EC == MC_OK)
		{
			if (mode == 0)
				var.setInteger32Value(0); // None
			else if (mode == 1 || mode == 5 || mode == 12 || mode == 19 || mode == 26 || mode == 33 || mode == 40 || mode == 47)
				var.setInteger32Value(1); // 1/2
			else if (mode == 2 || mode == 7 || mode == 14 || mode == 21 || mode == 28 || mode == 35 || mode == 42 || mode == 49)
				var.setInteger32Value(2); // 3/4
			else if (mode == 4 || mode == 9 || mode == 16 || mode == 23 || mode == 30 || mode == 37 || mode == 44 || mode == 51)
				var.setInteger32Value(3); // 7/8
			else if (mode == 8 || mode == 15 || mode == 22 || mode == 29 || mode == 36 || mode == 43 || mode == 50)
				var.setInteger32Value(4); // 14/17
			else if (mode == 10 || mode == 17 || mode == 24 || mode == 31 || mode == 38 || mode == 45 || mode == 52)
				var.setInteger32Value(5); // 10/11
			else if (mode == 11 || mode == 18 || mode == 25 || mode == 32 || mode == 39 || mode == 46 || mode == 53)
				var.setInteger32Value(6); // 16/17
			else
				var.setInteger32Value(0); // unknown

			printf("%s", pModem->GetRFecModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataFecOption, OidDemodulatorDataFecOptionLen))
	{
		printf("\t\tFEC code option ");
		int mode;
		EC = pModem->GetRFecMode(mode, 1);
		if (EC == MC_OK)
		{
			if (mode == 0)
				var.setInteger32Value(0); // None
			else if (mode <= 5)
				var.setInteger32Value(0); // Viterbi, no option
			else if (mode <= 11)
				var.setInteger32Value(1); // LDPC 256 block
			else if (mode <= 18)
				var.setInteger32Value(2); // LDPC 512 block
			else if (mode <= 25)
				var.setInteger32Value(3); // LDPC 1K block
			else if (mode <= 32)
				var.setInteger32Value(4); // LDPC 2K block
			else if (mode <= 39)
				var.setInteger32Value(5); // LDPC 4K block
			else if (mode <= 46)
				var.setInteger32Value(6); // LDPC 8K block
			else if (mode <= 53)
				var.setInteger32Value(7); // LDPC 16K block
			else
				var.setInteger32Value(0); // error

			printf("%s", pModem->GetRFecModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataFecC0C1Mode, OidDemodulatorDataFecC0C1ModeLen))
	{
		printf("\t\tC0C1 mode ");
		printf("ATTENTION!!! TODO: IMPLEMENT THAT\n");
		var.setInteger32Value(0); // error
		EC = MC_OK;
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataReedSolomonMode, OidDemodulatorDataReedSolomonModeLen))
	{
		printf("\t\tReed-Solomon mode ");
		int Mode;
		EC = pModem->GetRReedSolomonMode(Mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(Mode);
			printf("%s", pModem->GetReedSolomonModeName(Mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen))
	{
		printf("\t\tdifferential decoder ");
		int Mode;
		EC = pModem->GetDiffDecoderMode(Mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(Mode);
			printf("%s", pModem->GetDiffDecoderModeName(Mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen))
	{
		printf("\t\tdescrambler ");
		int mode;
		EC = pModem->GetDescramblerMode(mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(mode);
			printf("%s", pModem->GetDescramblerModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataBufferDelay, OidDemodulatorDataBufferDelayLen))
	{
		printf("\t\tbuffer delay ");
		CModemDataParams params;
		EC = pModem->GetRDataParams(params, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(params.m_DopplerBufferDelay);
			printf("%d", params.m_DopplerBufferDelay);
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataBufferSize, OidDemodulatorDataBufferSizeLen))
	{
		printf("\t\tbuffer size ");
		CModemDataParams params;
		EC = pModem->GetRDataParams(params, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(params.m_DopplerBufferSize);
			printf("%d", params.m_DopplerBufferSize);
		}
	}

	return EC;
}

MC_ErrorCode processDatumDemodulatorLnbRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidDemodulatorLnbCurrentMin, OidDemodulatorLnbCurrentMinLen))
	{
		printf("\t\tCurrent min ");
		CLnbStatus status;
		EC = pModem->GetLnbStatus(status, 1);
		if (EC == MC_OK)
		{
			double value = status.m_CurrentMin;
			printf("%f", value);
			var.setInteger32Value((int)(value*1000));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnbCurrentMax, OidDemodulatorLnbCurrentMaxLen))
	{
		printf("\t\tCurrent max ");
		CLnbStatus status;
		EC = pModem->GetLnbStatus(status, 1);
		if (EC == MC_OK)
		{
			double value = status.m_CurrentMax;
			printf("%f", value);
			var.setInteger32Value((int)(value*1000));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnbCurrentOut, OidDemodulatorLnbCurrentOutLen))
	{
		printf("\t\tCurrent out ");
		CLnbStatus status;
		EC = pModem->GetLnbStatus(status, 1);
		if (EC == MC_OK)
		{
			double value = status.m_Current;
			printf("%f", value);
			var.setInteger32Value((int)(value/10.));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnbLoFreq, OidDemodulatorLnbLoFreqLen))
	{
		printf("\t\tLow frequency ");

	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnbPower, OidDemodulatorLnbPowerLen))
	{
		printf("\t\tpower ");
		int mode = 0;
		EC = pModem->GetRPowerSupplyMode(mode, 1);
		if (EC == MC_OK)
		{
			var.setInteger32Value(mode+1);
			printf("%s", pModem->GetRPowerSupplyModeName(mode));
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnb10MHz, OidDemodulatorLnb10MHzLen))
	{
		printf("\t\t10 MHz reference ");
		BOOL b = FALSE;
		EC = pModem->IsR10MHzSupplierEnabled(b, 1);
		if (EC == MC_OK)
		{
			int value = 0;
			if (b)
			{
				printf("enabled");
				value = 1;
			}
			else
			{
				printf("disabled");
				value = 0;
			}
			var.setInteger32Value(value);
		}
	}

	return EC;
}

MC_ErrorCode processDatumDemodulatorAlarmRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	unsigned int *pAlarms = NULL;
	int index = -1;

	if (var.m_OID.isPartOfOID(OidDemodulatorCxrAlarm, OidDemodulatorCxrAlarmLen))
	{
		EC = pModem->GetDemodulatorAlarms(pAlarms);
		index = DATUM_DEMODULATOR_CXR_ALARM;
		printf("\t\tCXR Alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLvlAlarm, OidDemodulatorLvlAlarmLen))
	{
		EC = pModem->GetDemodulatorAlarms(pAlarms);
		index = DATUM_DEMODULATOR_LVL_ALARM;
		printf("\t\tLVL Alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorEbnoAlarm, OidDemodulatorEbnoAlarmLen))
	{
		EC = pModem->GetDemodulatorAlarms(pAlarms);
		index = DATUM_DEMODULATOR_EBNO_ALARM;
		printf("\t\tEb/No alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorTstActAlarm, OidDemodulatorTstActAlarmLen))
	{
		EC = pModem->GetDemodulatorAlarms(pAlarms);
		index = DATUM_DEMODULATOR_TSTACT_ALARM;
		printf("\t\tTest act. alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorHdwAlarm, OidDemodulatorHdwAlarmLen))
	{
		EC = pModem->GetDemodulatorAlarms(pAlarms);
		index = DATUM_DEMODULATOR_HARD_ALARM;
		printf("\t\tHardware alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorBckAlarm, OidDemodulatorBckAlarmLen))
	{
		EC = pModem->GetDemodulatorAlarms(pAlarms);
		index = DATUM_DEMODULATOR_BCK_ALARM;
		printf("\t\tBCK alarm");
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnbPowerAlarm, OidDemodulatorLnbPowerAlarmLen))
	{
		EC = pModem->GetDemodulatorAlarms(pAlarms);
		index = DATUM_DEMODULATOR_LNB_ALARM;
		printf("\t\tLNB POWER alarm");
	}
	if (EC == MC_OK && pAlarms != NULL && index < pModem->GetDemodulatorAlarmCount())
	{
		printf(" = %d", pAlarms[index]);
		var.setInteger32Value(pAlarms[index]);
	}
	else
		var.setInteger32Value(-1);

	return EC;
}

MC_ErrorCode processDatumDemodulatorRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\tdemodulator\n");

	if (var.m_OID.isPartOfOID(OidDatumModemDemodulatorCount, OidDatumModemDemodulatorCountLen))
	{
		EC = MC_OK;
		var.setInteger32Value(1);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemDemodulatorIndex, OidDatumModemDemodulatorIndexLen))
	{
		EC = MC_OK;
		var.setInteger32Value(1);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIf, OidDemodulatorIfLen))
	{
		EC = processDatumDemodulatorIfRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatus, OidDemodulatorStatusLen))
	{
		EC = processDatumDemodulatorStatusRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorData, OidDemodulatorDataLen))
	{
		EC = processDatumDemodulatorDataRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnb, OidDemodulatorLnbLen))
	{
		EC = processDatumDemodulatorLnbRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorAlarm, OidDemodulatorAlarmLen))
	{
		EC = processDatumDemodulatorAlarmRequest(pModem, var, bGet);
	}

	return EC;
}

MC_ErrorCode processDatumInterfaceIORequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\t\tIO");

	return EC;
}

MC_ErrorCode processDatumInterfaceAlarmRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\t\tAlarm");

	if (var.m_OID.isPartOfOID(OidDatumModemInterfaceAlarmTest, OidDatumModemInterfaceAlarmTestLen))
	{
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemInterfaceAlarmSdms, OidDatumModemInterfaceAlarmSdmsLen))
	{
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemInterfaceAlarmBer, OidDatumModemInterfaceAlarmBerLen))
	{
	}
	return EC;
}

MC_ErrorCode processDatumInterfaceRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\tinterface\n");

	if (var.m_OID.isPartOfOID(OidDatumModemInterfaceIOTable, OidDatumModemInterfaceIOTableLen))
	{
		EC = processDatumInterfaceIORequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemInterfaceAlarmTable, OidDatumModemInterfaceAlarmTableLen))
	{
		EC = processDatumInterfaceAlarmRequest(pModem, var, bGet);
	}

	return EC;
}

MC_ErrorCode processDatumRequest(CDatumModem *pModem, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("Datum modem\n");

	if (var.m_OID.isPartOfOID(OidDatumModemUnitObjs, OidDatumModemUnitObjsLen))
	{
		EC = processDatumUnitRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemModulatorObjs, OidDatumModemModulatorObjsLen))
	{
		EC = processDatumModulatorRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemDemodulatorObjs, OidDatumModemDemodulatorObjsLen))
	{
		EC = processDatumDemodulatorRequest(pModem, var, bGet);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemInterfaceObjs, OidDatumModemInterfaceObjsLen))
	{
		EC = processDatumInterfaceRequest(pModem, var, bGet);
	}

	if (EC == MC_DEVICE_NOT_RESPONDING)
	{
		printf(" NO REPLY FROM MODEM");
	}
	else if (EC == MC_COMMAND_NOT_SUPPORTED)
	{
		printf(" COMMAND IS NOT SUPPORTED BY MODEM");
	}

	printf("\n");

	return EC;
}

