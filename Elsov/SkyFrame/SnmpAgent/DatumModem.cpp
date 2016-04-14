#include "stdafx.h"
#include <stdio.h>
#include "../Common/SDMS_OIDs.h"
#include "DatumModem.h"

MC_ErrorCode processDatumUnitAlarmRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
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

MC_ErrorCode processDatumUnitRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
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
		EC = processDatumUnitAlarmRequest(pModem, var, reqType);
	}

	return EC;
}

static MC_ErrorCode getTIndex(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tindex 1");
	var.setGaugeValue(1);
	return MC_OK;
}

static MC_ErrorCode getTFrequency(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tfrequency ");
	unsigned int frequency;
	MC_ErrorCode EC = pModem->GetTFrequency(frequency, 1); // KHz
	if (EC == MC_OK)
	{
		var.setGaugeValue(frequency*1000); // Hz
		printf("%d KHz", frequency);
	}
	return EC;
}

static MC_ErrorCode getTOffset(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\toffset ");
	int offset;
	MC_ErrorCode EC = pModem->GetModulatorShift(offset, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(offset);
		printf("%d Hz", offset);
	}
	return EC;
}

static MC_ErrorCode getTOutputLevel(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tOutput level ");
	double level;
	MC_ErrorCode EC = pModem->GetOutputLevel(level, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value((int)(level*10));
		printf("%.2f dB", level);
	}
	return EC;
}

static MC_ErrorCode getTOutputEnabled(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tOutput enabled ");
	BOOL bOn;
	MC_ErrorCode EC = pModem->IsOutputOn(bOn, 1);
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
	return EC;
}

static MC_ErrorCode getTModulationType(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tModulation ");
	int modType;
	MC_ErrorCode EC = pModem->GetTModulationType(modType, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(modType+1);
		printf("%s", pModem->GetTModulationTypeName(modType));
	}
	return EC;
}

static MC_ErrorCode getTSpectrumType(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tSpectrum ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetTSpectrumMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetTSpectrumModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getTBurstMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tburst mode ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetTBurstMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetTBurstModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getTBurstPreambleLength(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tburst preamble length ");
	int length = -1;
	MC_ErrorCode EC = pModem->getTBurstPreambleLength(length, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(length);
		printf("%d", length);
	}
	return EC;
}

static MC_ErrorCode getTAupcMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tAUPC mode ");
	int mode = -1;
	MC_ErrorCode EC = pModem->getTAupcMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetTAupcModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getTCxrMuteMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tcxr mute mode ");
	int mode = -1;
	MC_ErrorCode EC = pModem->getTCxrMuteMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetTCxrMuteModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getTOutputImpedance(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\toutput impedance ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetTOutputImpedanceMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetTOutputImpedanceModeName(mode));
	}
	return EC;
}

MC_ErrorCode processDatumModulatorStatusRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidModulatorIndex, OidModulatorIndexLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTIndex(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIndex, OidModulatorIndexLen))
			{
				var.setOID(OidModulatorStatusCxrEn, OidModulatorStatusCxrEnLen);
				EC = processDatumModulatorStatusRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTIndex(pModem, var);
				var.setOID(OidModulatorIndex, OidModulatorIndexLen);
			}
			break;
		}
	}
	return EC;
}

MC_ErrorCode processDatumModulatorIfRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTFrequency(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen))
			{
				var.setOID(OidModulatorIfOffset, OidModulatorIfOffsetLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTFrequency(pModem, var);
				var.setOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			unsigned int frequency = var.m_iIntegerValue/1000; // Hz -> KHz
			printf("\t\tfrequency %d KHz", frequency);
			EC = pModem->SetTFrequency(frequency, 1);
			if (EC == MC_OK)
				printf(", result = %d KHz", frequency);
			break;
		};
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfOffset, OidModulatorIfOffsetLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTOffset(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfOffset, OidModulatorIfOffsetLen))
			{
				var.setOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTOffset(pModem, var);
				var.setOID(OidModulatorIfOffset, OidModulatorIfOffsetLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int offset = var.m_iIntegerValue;
			printf("\t\tOffset %d Hz", offset);
			EC = pModem->SetModulatorShift(offset, 1);
			if (EC == MC_OK)
				printf(", result = %d Hz", offset);
			break;
		};
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTOutputLevel(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen))
			{
				var.setOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTOutputLevel(pModem, var);
				var.setOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			double level = var.m_iIntegerValue/10.;
			printf("\t\tOutput level %.2f dB", level);
			EC = pModem->SetOutputLevel(level, 1);
			if (EC == MC_OK)
				printf(", result = %.2f dB", level);
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTOutputEnabled(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen))
			{
				var.setOID(OidModulatorIfModulation, OidModulatorIfModulationLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTOutputEnabled(pModem, var);
				var.setOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			printf("\t\tOutput");
			BOOL bOn = (var.m_iIntegerValue == 2);
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
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTModulationType(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfModulation, OidModulatorIfModulationLen))
			{
				var.setOID(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTModulationType(pModem, var);
				var.setOID(OidModulatorIfModulation, OidModulatorIfModulationLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int modType = var.m_iIntegerValue - 1;
			printf("Modulation %s", pModem->GetTModulationTypeName(modType));
			EC = pModem->SetTModulationType(modType, 1);
			if (EC == MC_OK)
			{
				printf(", result = %s", pModem->GetTModulationTypeName(modType));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTSpectrumType(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen))
			{
				var.setOID(OidModulatorIfBurstMode, OidModulatorIfBurstModeLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTSpectrumType(pModem, var);
				var.setOID(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("\t\tSpectrum %s", pModem->GetTSpectrumModeName(mode));
			EC = pModem->SetTSpectrumMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetTSpectrumModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfBurstMode, OidModulatorIfBurstModeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTBurstMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfBurstMode, OidModulatorIfBurstModeLen))
			{
				var.setOID(OidModulatorIfBurstPreLength, OidModulatorIfBurstPreLengthLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTBurstMode(pModem, var);
				var.setOID(OidModulatorIfBurstMode, OidModulatorIfBurstModeLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("\t\tburst mode %s", pModem->GetTBurstModeName(mode));
			EC = pModem->SetTBurstMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetTBurstModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfBurstPreLength, OidModulatorIfBurstPreLengthLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTBurstPreambleLength(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfBurstPreLength, OidModulatorIfBurstPreLengthLen))
			{
				var.setOID(OidModulatorIfAupcMode, OidModulatorIfAupcModeLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTBurstPreambleLength(pModem, var);
				var.setOID(OidModulatorIfBurstPreLength, OidModulatorIfBurstPreLengthLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int length = var.m_iIntegerValue;
			printf("\t\tburst preamble length %d", length);
			EC = pModem->setTBurstPreambleLength(length, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(length);
				printf("%d", length);
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfAupcMode, OidModulatorIfAupcModeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTAupcMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfAupcMode, OidModulatorIfAupcModeLen))
			{
				var.setOID(OidModulatorIfCxrMuteMode, OidModulatorIfCxrMuteModeLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTAupcMode(pModem, var);
				var.setOID(OidModulatorIfAupcMode, OidModulatorIfAupcModeLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("\t\tAUPC mode %s", pModem->GetTAupcModeName(mode));
			EC = pModem->setTAupcMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetTAupcModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfCxrMuteMode, OidModulatorIfCxrMuteModeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTCxrMuteMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorIfCxrMuteMode, OidModulatorIfCxrMuteModeLen))
			{
				var.setOID(OidModulatorIfOutputImpedance, OidModulatorIfOutputImpedanceLen);
				EC = processDatumModulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTCxrMuteMode(pModem, var);
				var.setOID(OidModulatorIfCxrMuteMode, OidModulatorIfCxrMuteModeLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("\t\tAUPC mode %s", pModem->GetTCxrMuteModeName(mode));
			EC = pModem->setTAupcMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetTCxrMuteModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIfOutputImpedance, OidModulatorIfOutputImpedanceLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTOutputImpedance(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			EC = getTOutputImpedance(pModem, var);
			if (var.m_OID.isTheSameOID(OidModulatorIfOutputImpedance, OidModulatorIfOutputImpedanceLen))
			{
				var.setOID(OidModulatorDataBitRate, OidModulatorDataBitRateLen);
			}
			else
			{
				var.appendDot1ToOID();
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("\t\toutput impedance %s", pModem->GetTOutputImpedanceModeName(mode));
			EC = pModem->SetTOutputImpedanceMode(mode, 1);
			if (EC == MC_OK)
			{
				printf("%s", pModem->GetTOutputImpedanceModeName(mode));
			}
		}
	}

	return EC;
}

static MC_ErrorCode getTBitRate(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tbit rste ");
	unsigned int bitRate = 0;
	MC_ErrorCode EC = pModem->GetTDataRate(bitRate, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(bitRate);
		printf("%d baud", bitRate);
	}
	return EC;
}

static MC_ErrorCode getTFecMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tFEC type ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetTFecMode(mode, 1);
	if (EC == MC_OK)
	{
		int snmpValue = pModem->getRFecSnmpValueByMode(mode);
		var.setInteger32Value(snmpValue);
		printf("%s", pModem->GetTFecModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getTFecCodeRateMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tFEC code rate ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetTFecCodeRate(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetTFecCodeRateName(mode));
	}
	return EC;
}

static MC_ErrorCode getTDiffEncoderMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tdifferential encoder ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetDiffEncoderMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetDiffEncoderModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getTScramblerMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tscrambler ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetScramblerMode(mode, 1);
	if (EC == MC_OK)
	{
		int snmpValue = pModem->getScramblerSnmpValueByMode(mode);
		var.setInteger32Value(snmpValue);
		printf("%s", pModem->GetScramblerModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getTDataClockSource(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tclock source ");
	int source = -1;
	MC_ErrorCode EC = pModem->GetTDataClockSource(source, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(source+1);
		printf("%s", pModem->GetTDataClockSourceName(source));
	}
	return EC;
}

MC_ErrorCode processDatumModulatorDataRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidModulatorDataBitRate, OidModulatorDataBitRateLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTBitRate(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			EC = getTBitRate(pModem, var);
			if (var.m_OID.isTheSameOID(OidModulatorDataBitRate, OidModulatorDataBitRateLen))
			{
				var.setOID(OidModulatorDataModulation, OidModulatorDataModulationLen);
				EC = processDatumModulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				var.appendDot1ToOID();
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			unsigned int bitRate = var.m_iIntegerValue;
			printf("%d baud", bitRate);
			EC = pModem->SetTDataRate(bitRate, 1);
			if (EC == MC_OK)
			{
				printf(", result = %d Baud", bitRate);
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataModulation, OidModulatorDataModulationLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTModulationType(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorDataModulation, OidModulatorDataModulationLen))
			{
				var.setOID(OidModulatorDataFecType, OidModulatorDataFecTypeLen);
				EC = processDatumModulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTModulationType(pModem, var);
				var.setOID(OidModulatorDataModulation, OidModulatorDataModulationLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetTModulationTypeName(mode));
			EC = pModem->SetTModulationType(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetTModulationTypeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataFecType, OidModulatorDataFecTypeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTFecMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorDataFecType, OidModulatorDataFecTypeLen))
			{
				var.setOID(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen);
				EC = processDatumModulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTFecMode(pModem, var);
				var.setOID(OidModulatorDataFecType, OidModulatorDataFecTypeLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = pModem->getTFecModeBySnmpValue(var.m_iIntegerValue);
			printf("%s", pModem->GetTFecModeName(mode));
			EC = pModem->SetTFecMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(pModem->getTFecSnmpValueByMode(mode));
				printf("%s", pModem->GetTFecModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTFecCodeRateMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen))
			{
				var.setOID(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen);
				EC = processDatumModulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTFecCodeRateMode(pModem, var);
				var.setOID(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetTFecCodeRateName(mode));
			EC = pModem->SetTFecCodeRate(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetTFecCodeRateName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTDiffEncoderMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen))
			{
				var.setOID(OidModulatorDataScrambler, OidModulatorDataScramblerLen);
				EC = processDatumModulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTDiffEncoderMode(pModem, var);
				var.setOID(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetDiffEncoderModeName(mode));
			EC = pModem->SetTFecCodeRate(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf(" result = %s", pModem->GetDiffEncoderModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataScrambler, OidModulatorDataScramblerLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTScramblerMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorDataScrambler, OidModulatorDataScramblerLen))
			{
				var.setOID(OidModulatorDataClockSource, OidModulatorDataClockSourceLen);
				EC = processDatumModulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTScramblerMode(pModem, var);
				var.setOID(OidModulatorDataScrambler, OidModulatorDataScramblerLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = pModem->getScramblerModeBySnmpValue(var.m_iIntegerValue);
			printf("scrambler %s", pModem->GetScramblerModeName(mode));
			EC = pModem->SetScramblerMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(pModem->getDescramblerSnmpValueByMode(mode));
				printf(" result = %s", pModem->GetScramblerModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataClockSource, OidModulatorDataClockSourceLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getTDataClockSource(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidModulatorDataClockSource, OidModulatorDataClockSourceLen))
			{
				var.setOID(OidModulatorDataFecOption, OidModulatorDataFecOptionLen);
				EC = processDatumModulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getTDataClockSource(pModem, var);
				var.setOID(OidModulatorDataClockSource, OidModulatorDataClockSourceLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue;
			printf("clock source %s", pModem->GetTDataClockSourceName(mode));
			EC = pModem->SetTDataClockSource(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf(" result = %s", pModem->GetTDataClockSourceName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataFecOption, OidModulatorDataFecOptionLen))
	{
		printf("\t\tFEC code option ");
		int option = -1;
		if (reqType == SNMP_FIELD_GET_REQUEST)
		{
			EC = pModem->GetTFecOption(option, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(option+1);
				printf("%s", pModem->GetTFecOptionName(option));
			}
		}
		else if (reqType == SNMP_FIELD_GET_NEXT_REQUEST)
		{ // get_next
		} 
		else if (reqType == SNMP_FIELD_SET_REQUEST)
		{ // set
			option = var.m_iIntegerValue-1;
			printf("%s", pModem->GetTFecOptionName(option));
			EC = pModem->SetTFecOption(option, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(option+1);
				printf("%s", pModem->GetTFecOptionName(option));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorDataReedSolomonMode, OidModulatorDataReedSolomonModeLen))
	{
		printf("\t\tReed-Solomon mode ");
		int mode = -1;
		if (reqType == SNMP_FIELD_GET_REQUEST)
		{
			EC = pModem->GetTReedSolomonMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetReedSolomonModeName(mode));
			}
		}
		else if (reqType == SNMP_FIELD_GET_NEXT_REQUEST)
		{ // get_next
		} 
		else if (reqType == SNMP_FIELD_SET_REQUEST)
		{ //set
			mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetReedSolomonModeName(mode));
			EC = pModem->SetTReedSolomonMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetReedSolomonModeName(mode));
			}
		}
	}

	return EC;
}

MC_ErrorCode processDatumModulatorBucRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
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
			printf("%.3f", value);
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
			printf("%.3f", value);
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
			printf("%.3f", value);
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
		int mode = -1;
		if (reqType == SNMP_FIELD_GET_REQUEST)
		{ // get
			EC = pModem->GetTPowerSupplyMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetTPowerSupplyModeName(mode));
			}
		}
		else if (reqType == SNMP_FIELD_GET_NEXT_REQUEST)
		{ // get_next
		} 
		else if (reqType == SNMP_FIELD_SET_REQUEST)
		{ //set
			mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetTPowerSupplyModeName(mode));
			EC = pModem->SetTPowerSupplyMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode + 1);
				printf("%s", pModem->GetTPowerSupplyModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidModulatorBuc10MHz, OidModulatorBuc10MHzLen))
	{
		printf("\t\t10 MHz reference ");
		int mode = -1;
		if (reqType == SNMP_FIELD_GET_REQUEST)
		{ // get
			EC = pModem->GetT10MHzMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetT10MHzModeName(mode));
			}
		}
		else if (reqType == SNMP_FIELD_GET_NEXT_REQUEST)
		{ // get_next
		} 
		else if (reqType == SNMP_FIELD_SET_REQUEST)
		{ //set
			mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetT10MHzModeName(mode));
			EC = pModem->SetT10MHzMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetT10MHzModeName(mode));
			}
		}
	}

	return EC;
}

MC_ErrorCode processDatumModulatorRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\tmodulator\n");

	if (var.m_OID.isPartOfOID(OidDatumModemModulatorCount, OidDatumModemModulatorCountLen))
	{
		EC = MC_OK;
		var.setInteger32Value(1);
	}
	else if (var.m_OID.isPartOfOID(OidModulatorStatus, OidModulatorStatusLen))
	{
		EC = processDatumModulatorStatusRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidModulatorIf, OidModulatorIfLen))
	{
		EC = processDatumModulatorIfRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidModulatorData, OidModulatorDataLen))
	{
		EC = processDatumModulatorDataRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidModulatorBuc, OidModulatorBucLen))
	{
		EC = processDatumModulatorBucRequest(pModem, var, reqType);
	}

	return EC;
}

static MC_ErrorCode getRIndex(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tindex 1");
	var.setGaugeValue(1);
	return MC_OK;
}

static MC_ErrorCode getROffset(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\toffset ");
	int offset;
	MC_ErrorCode EC = pModem->GetDemodulatorOffset(offset, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(offset);
		printf("%d Hz", offset);
	}
	return EC;
}

static MC_ErrorCode getRInputLevel(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tinput level ");
	double level;
	MC_ErrorCode EC = pModem->GetInputLevel(level, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(level*10);
		printf("%.2f dBm", level);
	}
	return EC;
}

static MC_ErrorCode getREbNo(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tS/N = ");
	double SN;
	MC_ErrorCode EC = pModem->GetEbNo(SN, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(SN*10);
		printf("%.2f dB", SN);
	}
	return EC;
}

static MC_ErrorCode getRBER(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tBER = ");
	double BER = -1;
	MC_ErrorCode EC = pModem->GetBER(BER, 1);
	if (EC == MC_OK)
	{
		char szBer[128];
		sprintf(szBer, "%g", BER);
		var.setStringValue(szBer);
		printf("%f", BER);
	}
	return EC;
}

static MC_ErrorCode getRSER(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tSER = ");
	double SER = -1;
	MC_ErrorCode EC = pModem->GetSER(SER, 1);
	if (EC == MC_OK)
	{
		char szSer[128];
		sprintf(szSer, "%g", SER);
		var.setStringValue(szSer);
		printf("%f", SER);
	}
	return EC;
}

static MC_ErrorCode getRBuffer(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tbuffer = ");
	CDemIfStatus status;
	MC_ErrorCode EC = pModem->GetRIfStatus(status, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(status.m_Buffer);
		printf("%d", status.m_Buffer);
	}
	return EC;
}

static MC_ErrorCode getRLock(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tlock = ");
	double SN;
	MC_ErrorCode EC = pModem->GetEbNo(SN, 1);
	if (EC == MC_OK)
	{
		int lock = 0;
		if (SN > 0)
			lock = 1;
		var.setInteger32Value(lock);
		printf("%d", lock);
	}
	return EC;
}

static MC_ErrorCode processDatumDemodulatorStatusRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidDatumModemDemodulatorIndex, OidDatumModemDemodulatorIndexLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRIndex(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDatumModemDemodulatorIndex, OidDatumModemDemodulatorIndexLen))
			{
				var.setOID(OidDemodulatorStatusOffset, OidDemodulatorStatusOffsetLen);
				EC = processDatumDemodulatorStatusRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRIndex(pModem, var);
				var.setOID(OidDatumModemDemodulatorIndex, OidDatumModemDemodulatorIndexLen);
			}
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusOffset, OidDemodulatorStatusOffsetLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getROffset(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorStatusOffset, OidDemodulatorStatusOffsetLen))
			{
				var.setOID(OidDemodulatorStatusInputLevel, OidDemodulatorStatusInputLevelLen);
				EC = processDatumDemodulatorStatusRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getROffset(pModem, var);
				var.setOID(OidDemodulatorStatusOffset, OidDemodulatorStatusOffsetLen);
			}
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusInputLevel, OidDemodulatorStatusInputLevelLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRInputLevel(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorStatusInputLevel, OidDemodulatorStatusInputLevelLen))
			{
				var.setOID(OidDemodulatorStatusEbNo, OidDemodulatorStatusEbNoLen);
				EC = processDatumDemodulatorStatusRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRInputLevel(pModem, var);
				var.setOID(OidDemodulatorStatusInputLevel, OidDemodulatorStatusInputLevelLen);
			}
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusEbNo, OidDemodulatorStatusEbNoLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getREbNo(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorStatusEbNo, OidDemodulatorStatusEbNoLen))
			{
				var.setOID(OidDemodulatorStatusBER, OidDemodulatorStatusBERLen);
				EC = processDatumDemodulatorStatusRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getREbNo(pModem, var);
				var.setOID(OidDemodulatorStatusEbNo, OidDemodulatorStatusEbNoLen);
			}
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusBER, OidDemodulatorStatusBERLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRBER(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorStatusBER, OidDemodulatorStatusBERLen))
			{
				var.setOID(OidDemodulatorStatusSER, OidDemodulatorStatusSERLen);
				EC = processDatumDemodulatorStatusRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRBER(pModem, var);
				var.setOID(OidDemodulatorStatusBER, OidDemodulatorStatusBERLen);
			}
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusSER, OidDemodulatorStatusSERLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRSER(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorStatusSER, OidDemodulatorStatusBufferLen))
			{
				var.setOID(OidDemodulatorStatusBuffer, OidDemodulatorStatusBufferLen);
				EC = processDatumDemodulatorStatusRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRSER(pModem, var);
				var.setOID(OidDemodulatorStatusSER, OidDemodulatorStatusSERLen);
			}
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusBuffer, OidDemodulatorStatusBufferLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRBuffer(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorStatusBuffer, OidDemodulatorStatusBufferLen))
			{
				var.setOID(OidDemodulatorStatusLock, OidDemodulatorStatusLockLen);
				EC = processDatumDemodulatorStatusRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRBuffer(pModem, var);
				var.setOID(OidDemodulatorStatusBuffer, OidDemodulatorStatusBufferLen);
			}
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatusLock, OidDemodulatorStatusLockLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRLock(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			EC = getRLock(pModem, var);
			if (var.m_OID.isTheSameOID(OidDemodulatorStatusLock, OidDemodulatorStatusLockLen))
			{
				unsigned int id[] = {1,3,6,1,4,1,7840,3,1,2,3,2,1,1,1};
				var.setOID(id, ELEMENT_COUNT(id));
			}
			else
			{
				var.appendDot1ToOID();
			}
			break;
		}
	}
	return EC;
}

static MC_ErrorCode getRFrequency(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tfrequency ");
	unsigned int frequency;
	MC_ErrorCode EC = pModem->GetRFrequency(frequency, 1); // KHz
	if (EC == MC_OK)
	{
		var.setGaugeValue(frequency*1000); // Hz
		printf("%d KHz", frequency);
	}
	return EC;
}

static MC_ErrorCode getRSweepRange(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tsweep range ");
	unsigned int range;
	MC_ErrorCode EC = pModem->GetSearchRange(range, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(range);
		printf("%d Hz", range);
	}
	return EC;
}

static MC_ErrorCode getRSweepTime(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tsweep time ");
	CDemIfParams params;
	MC_ErrorCode EC = pModem->GetRIfParams(params, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value((int)(params.m_SweepTime*10));
		printf("%.1f sec", params.m_SweepTime);
	}
	return EC;
}

static MC_ErrorCode getRLowEbno(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tlow ebno ");
	CDemIfParams params;
	MC_ErrorCode EC = pModem->GetRIfParams(params, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value((int)(params.m_LowEbno*10));
		printf("%.1f dB", params.m_LowEbno);
	}
	return EC;
}

static MC_ErrorCode getRLowLevel(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tlow level ");
	CDemIfParams params;
	MC_ErrorCode EC = pModem->GetRIfParams(params, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value((int)(params.m_LowLevel*10));
		printf("%.1f dBm", params.m_LowLevel);
	}
	return EC;
}

static MC_ErrorCode getRSweepMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tsweep mode ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetRSweepMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetRSweepModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getRModulationType(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tmodulation ");
	int modType;
	MC_ErrorCode EC = pModem->GetRModulationType(modType, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(modType+1);
		printf("%s", pModem->GetRModulationTypeName(modType));
	}
	return EC;
}

static MC_ErrorCode getRSpectrumType(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tspectrum ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetRSpectrumMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetRSpectrumModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getRInputImpedance(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tinput impedance ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetRInputImpedanceMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetRInputImpedanceModeName(mode));
	}
	return EC;
}

MC_ErrorCode processDatumDemodulatorIfRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	if (var.m_OID.isPartOfOID(OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRFrequency(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen))
			{
				var.setOID(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen);
				EC = processDatumDemodulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRFrequency(pModem, var);
				var.setOID(OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			unsigned int frequency = var.m_iIntegerValue/1000; // Hz -> KHz
			printf("\t\tfrequency %d KHz", frequency);
			EC = pModem->SetRFrequency(frequency, 1);
			if (EC == MC_OK)
				printf(", result = %d KHz", frequency);
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRSweepRange(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen))
			{
				var.setOID(OidDemodulatorIfSweepTime, OidDemodulatorIfSweepTimeLen);
				EC = processDatumDemodulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRSweepRange(pModem, var);
				var.setOID(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			unsigned int range = var.m_iIntegerValue;
			printf("%d Hz", range);
			EC = pModem->SetSearchRange(range, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(range);
				printf("%d Hz", range);
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfSweepTime, OidDemodulatorIfSweepTimeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRSweepTime(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorIfSweepTime, OidDemodulatorIfSweepTimeLen))
			{
				var.setOID(OidDemodulatorIfLowEbno, OidDemodulatorIfLowEbnoLen);
				EC = processDatumDemodulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRSweepTime(pModem, var);
				var.setOID(OidDemodulatorIfSweepTime, OidDemodulatorIfSweepTimeLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfLowLevel, OidDemodulatorIfLowLevelLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRLowLevel(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorIfLowLevel, OidDemodulatorIfLowLevelLen))
			{
				var.setOID(OidDemodulatorIfLowEbno, OidDemodulatorIfLowEbnoLen);
				EC = processDatumDemodulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRLowLevel(pModem, var);
				var.setOID(OidDemodulatorIfLowLevel, OidDemodulatorIfLowLevelLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfLowEbno, OidDemodulatorIfLowEbnoLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRLowEbno(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorIfLowEbno, OidDemodulatorIfLowEbnoLen))
			{
				var.setOID(OidDemodulatorIfSweepMode, OidDemodulatorIfSweepModeLen);
				EC = processDatumDemodulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRLowEbno(pModem, var);
				var.setOID(OidDemodulatorIfLowEbno, OidDemodulatorIfLowEbnoLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			break;
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfSweepMode, OidDemodulatorIfSweepModeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRSweepMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorIfSweepMode, OidDemodulatorIfSweepModeLen))
			{
				var.setOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen);
				EC = processDatumDemodulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRSweepMode(pModem, var);
				var.setOID(OidDemodulatorIfSweepMode, OidDemodulatorIfSweepModeLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetRSweepModeName(mode));
			EC = pModem->SetRSweepMode(mode, 1);
			if (EC == MC_OK)
			{
				printf("%s", pModem->GetRSweepModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRModulationType(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen))
			{
				var.setOID(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen);
				EC = processDatumDemodulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRModulationType(pModem, var);
				var.setOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int modType = var.m_iIntegerValue - 1;
			printf("Modulation %s", pModem->GetRModulationTypeName(modType));
			EC = pModem->SetRModulationType(modType, 1);
			if (EC == MC_OK)
			{
				printf(", result = %s", pModem->GetRModulationTypeName(modType));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRSpectrumType(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen))
			{
				var.setOID(OidDemodulatorIfInputImpedance, OidDemodulatorIfInputImpedanceLen);
				EC = processDatumDemodulatorIfRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRSpectrumType(pModem, var);
				var.setOID(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("\t\tSpectrum %s", pModem->GetRSpectrumModeName(mode));
			EC = pModem->SetRSpectrumMode(mode, 1);
			if (EC == MC_OK)
			{
				printf("%s", pModem->GetRSpectrumModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIfInputImpedance, OidDemodulatorIfInputImpedanceLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRInputImpedance(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			EC = getRInputImpedance(pModem, var);
			if (var.m_OID.isTheSameOID(OidDemodulatorIfInputImpedance, OidDemodulatorIfInputImpedanceLen))
			{
				unsigned int id[] = {1,3,6,1,4,1,7840,3,1,2,3,2,2,1,1};
				var.setOID(id, ELEMENT_COUNT(id));
			}
			else
			{
				var.appendDot1ToOID();
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("\t\tinput impedance %s", pModem->GetRInputImpedanceModeName(mode));
			EC = pModem->SetRInputImpedanceMode(mode, 1);
			if (EC == MC_OK)
			{
				printf("%s", pModem->GetRInputImpedanceModeName(mode));
			}
		}
	}
	
	return EC;
}

static MC_ErrorCode getRBitRate(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tbit rste ");
	unsigned int bitRate = 0;
	MC_ErrorCode EC = pModem->GetRDataRate(bitRate, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(bitRate);
		printf("%d baud", bitRate);
	}
	return EC;
}

static MC_ErrorCode getRBufferDelay(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tbuffer delay ");
	CModemDataParams params;
	MC_ErrorCode EC = pModem->GetRDataParams(params, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(params.m_DopplerBufferDelay);
		printf("%d", params.m_DopplerBufferDelay);
	}
	return EC;
}

static MC_ErrorCode getRBufferSize(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tbuffer size ");
	CModemDataParams params;
	MC_ErrorCode EC = pModem->GetRDataParams(params, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(params.m_DopplerBufferSize);
		printf("%d", params.m_DopplerBufferSize);
	}
	return EC;
}

static MC_ErrorCode getRFecMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tFEC type ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetRFecMode(mode, 1);
	if (EC == MC_OK)
	{
		int snmpValue = pModem->getRFecSnmpValueByMode(mode);
		var.setInteger32Value(snmpValue);
		printf("%s", pModem->GetRFecModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getRFecCodeRateMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tFEC code rate ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetRFecCodeRate(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetRFecCodeRateName(mode));
	}
	return EC;
}

static MC_ErrorCode getRDiffDecoderMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tdifferential deccoder ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetDiffDecoderMode(mode, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(mode+1);
		printf("%s", pModem->GetDiffDecoderModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getRDescramblerMode(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tdescrambler ");
	int mode = -1;
	MC_ErrorCode EC = pModem->GetDescramblerMode(mode, 1);
	if (EC == MC_OK)
	{
		int snmpValue = pModem->getDescramblerSnmpValueByMode(mode);
		var.setInteger32Value(snmpValue);
		printf("%s", pModem->GetDescramblerModeName(mode));
	}
	return EC;
}

static MC_ErrorCode getRDataClockSource(CDatumModem *pModem, cSnmpVariable &var)
{
	printf("\t\tclock source ");
	int source = -1;
	MC_ErrorCode EC = pModem->GetRDataClockSource(source, 1);
	if (EC == MC_OK)
	{
		var.setInteger32Value(source+1);
		printf("%s", pModem->GetRDataClockSourceName(source));
	}
	return EC;
}

MC_ErrorCode processDatumDemodulatorDataRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;

	if (var.m_OID.isPartOfOID(OidDemodulatorDataBitRate, OidDemodulatorDataBitRateLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRBitRate(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			EC = getRBitRate(pModem, var);
			if (var.m_OID.isTheSameOID(OidDemodulatorDataBitRate, OidDemodulatorDataBitRateLen))
			{
				var.setOID(OidDemodulatorDataBufferDelay, OidDemodulatorDataBufferDelayLen);
				EC = processDatumDemodulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				var.appendDot1ToOID();
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			unsigned int bitRate = var.m_iIntegerValue;
			printf("%d baud", bitRate);
			EC = pModem->SetRDataRate(bitRate, 1);
			if (EC == MC_OK)
			{
				printf(", result = %d Baud", bitRate);
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataBufferDelay, OidDemodulatorDataBufferDelayLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRBufferDelay(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			EC = getRBufferDelay(pModem, var);
			if (var.m_OID.isTheSameOID(OidDemodulatorDataBufferDelay, OidDemodulatorDataBufferDelayLen))
			{
				var.setOID(OidDemodulatorDataBufferSize, OidDemodulatorDataBufferSizeLen);
				EC = processDatumDemodulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				var.appendDot1ToOID();
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			unsigned int bufferDelay = var.m_iIntegerValue;
			printf("%d msec", bufferDelay);
			/*EC = pModem->SetRDopplerBufferDelay(bufferDelay, 1);
			if (EC == MC_OK)
			{
				printf(", result = %d msec", bufferDelay);
			}*/
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataBufferSize, OidDemodulatorDataBufferSizeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRBufferSize(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			EC = getRBufferSize(pModem, var);
			if (var.m_OID.isTheSameOID(OidDemodulatorDataBufferSize, OidDemodulatorDataBufferSizeLen))
			{
				var.setOID(OidDemodulatorDataFecType, OidDemodulatorDataFecTypeLen);
				EC = processDatumDemodulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				var.appendDot1ToOID();
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			unsigned int bufferSize = var.m_iIntegerValue;
			printf("%d ", bufferSize);
			/*EC = pModem->SetRDopplerBufferSize(bufferDelay, 1);
			if (EC == MC_OK)
			{
				printf(", result = %d", bufferSize);
			}*/
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataFecType, OidDemodulatorDataFecTypeLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRFecMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorDataFecType, OidModulatorDataFecTypeLen))
			{
				var.setOID(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen);
				EC = processDatumDemodulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRFecMode(pModem, var);
				var.setOID(OidDemodulatorDataFecType, OidDemodulatorDataFecTypeLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = pModem->getRFecModeBySnmpValue(var.m_iIntegerValue);
			printf("%s", pModem->GetRFecModeName(mode));
			EC = pModem->SetRFecMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(pModem->getRFecSnmpValueByMode(mode));
				printf("%s", pModem->GetRFecModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRFecCodeRateMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen))
			{
				var.setOID(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen);
				EC = processDatumDemodulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRFecCodeRateMode(pModem, var);
				var.setOID(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetRFecCodeRateName(mode));
			EC = pModem->SetTFecCodeRate(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetRFecCodeRateName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRDiffDecoderMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen))
			{
				var.setOID(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen);
				EC = processDatumDemodulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRDiffDecoderMode(pModem, var);
				var.setOID(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetDiffDecoderModeName(mode));
			EC = pModem->SetRFecCodeRate(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf(" result = %s", pModem->GetDiffDecoderModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRDescramblerMode(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen))
			{
				var.setOID(OidDemodulatorDataClockSource, OidDemodulatorDataClockSourceLen);
				EC = processDatumDemodulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRDescramblerMode(pModem, var);
				var.setOID(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = pModem->getDescramblerModeBySnmpValue(var.m_iIntegerValue);
			printf("descrambler %s", pModem->GetDescramblerModeName(mode));
			EC = pModem->SetDescramblerMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(pModem->getDescramblerSnmpValueByMode(mode));
				printf(" result = %s", pModem->GetDescramblerModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataClockSource, OidDemodulatorDataClockSourceLen))
	{
		switch (reqType)
		{
		case SNMP_FIELD_GET_REQUEST:
			EC = getRDataClockSource(pModem, var);
			break;
		case SNMP_FIELD_GET_NEXT_REQUEST:
			if (var.m_OID.isTheSameOID(OidDemodulatorDataClockSource, OidDemodulatorDataClockSourceLen))
			{
				var.setOID(OidDemodulatorDataFecOption, OidDemodulatorDataFecOptionLen);
				EC = processDatumDemodulatorDataRequest(pModem, var, SNMP_FIELD_GET_REQUEST);
			}
			else
			{
				EC = getRDataClockSource(pModem, var);
				var.setOID(OidDemodulatorDataClockSource, OidDemodulatorDataClockSourceLen);
			}
			break;
		case SNMP_FIELD_SET_REQUEST:
			int mode = var.m_iIntegerValue;
			printf("clock source %s", pModem->GetRDataClockSourceName(mode));
			EC = pModem->SetRDataClockSource(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf(" result = %s", pModem->GetRDataClockSourceName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataFecOption, OidDemodulatorDataFecOptionLen))
	{
		printf("\t\tFEC code option ");
		int option = -1;
		if (reqType == SNMP_FIELD_GET_REQUEST)
		{ // get
			EC = pModem->GetRFecOption(option, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(option+1);
				printf("%s", pModem->GetRFecOptionName(option));
			}
		}
		else if (reqType == SNMP_FIELD_GET_NEXT_REQUEST)
		{ // get_next
		} 
		else if (reqType == SNMP_FIELD_SET_REQUEST)
		{ // set
			option = var.m_iIntegerValue-1;
			printf("%s", pModem->GetRFecOptionName(option));
			EC = pModem->SetRFecOption(option, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(option+1);
				printf("%s", pModem->GetRFecOptionName(option));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorDataReedSolomonMode, OidDemodulatorDataReedSolomonModeLen))
	{
		printf("\t\tReed-Solomon mode ");
		int mode = -1;
		if (reqType == SNMP_FIELD_GET_REQUEST)
		{ // get
			EC = pModem->GetRReedSolomonMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetReedSolomonModeName(mode));
			}
		}
		else if (reqType == SNMP_FIELD_GET_NEXT_REQUEST)
		{ // get_next
		} 
		else if (reqType == SNMP_FIELD_SET_REQUEST)
		{ //set
			mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetReedSolomonModeName(mode));
			EC = pModem->SetRReedSolomonMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetReedSolomonModeName(mode));
			}
		}
	}

	return EC;
}

MC_ErrorCode processDatumDemodulatorLnbRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
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
			printf("%.3f", value);
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
			printf("%.3f", value);
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
			printf("%.3f", value);
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
		int mode = -1;
		if (reqType == SNMP_FIELD_GET_REQUEST)
		{ // get
			EC = pModem->GetRPowerSupplyMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetRPowerSupplyModeName(mode));
			}
		}
		else if (reqType == SNMP_FIELD_GET_NEXT_REQUEST)
		{ // get_next
		} 
		else if (reqType == SNMP_FIELD_SET_REQUEST)
		{ //set
			mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetRPowerSupplyModeName(mode));
			EC = pModem->SetRPowerSupplyMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetRPowerSupplyModeName(mode));
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnb10MHz, OidDemodulatorLnb10MHzLen))
	{
		printf("\t\t10 MHz reference ");
		int mode = -1;
		if (reqType == SNMP_FIELD_GET_REQUEST)
		{ // get
			EC = pModem->GetR10MHzMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetR10MHzModeName(mode));
			}
		}
		else if (reqType == SNMP_FIELD_GET_NEXT_REQUEST)
		{ // get_next
		} 
		else if (reqType == SNMP_FIELD_SET_REQUEST)
		{ //set
			mode = var.m_iIntegerValue-1;
			printf("%s", pModem->GetR10MHzModeName(mode));
			EC = pModem->SetR10MHzMode(mode, 1);
			if (EC == MC_OK)
			{
				var.setInteger32Value(mode+1);
				printf("%s", pModem->GetR10MHzModeName(mode));
			}
		}
	}

	return EC;
}

MC_ErrorCode processDatumDemodulatorAlarmRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
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
	if ((EC == MC_OK) && (pAlarms != NULL) && (index < pModem->GetDemodulatorAlarmCount()))
	{
		printf(" = %d", pAlarms[index]+1);
		var.setInteger32Value(pAlarms[index]+1);
	}
	else
		var.setInteger32Value(0);

	return EC;
}

MC_ErrorCode processDatumDemodulatorRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\tdemodulator\n");

	if (var.m_OID.isPartOfOID(OidDatumModemDemodulatorCount, OidDatumModemDemodulatorCountLen))
	{
		EC = MC_OK;
		var.setInteger32Value(1);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorIf, OidDemodulatorIfLen))
	{
		EC = processDatumDemodulatorIfRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorStatus, OidDemodulatorStatusLen))
	{
		EC = processDatumDemodulatorStatusRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorData, OidDemodulatorDataLen))
	{
		EC = processDatumDemodulatorDataRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorLnb, OidDemodulatorLnbLen))
	{
		EC = processDatumDemodulatorLnbRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidDemodulatorAlarm, OidDemodulatorAlarmLen))
	{
		EC = processDatumDemodulatorAlarmRequest(pModem, var, reqType);
	}

	return EC;
}

MC_ErrorCode processDatumInterfaceIORequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\t\tIO");

	return EC;
}

MC_ErrorCode processDatumInterfaceAlarmRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
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

MC_ErrorCode processDatumInterfaceRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("\tinterface\n");

	if (var.m_OID.isPartOfOID(OidDatumModemInterfaceIOTable, OidDatumModemInterfaceIOTableLen))
	{
		EC = processDatumInterfaceIORequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemInterfaceAlarmTable, OidDatumModemInterfaceAlarmTableLen))
	{
		EC = processDatumInterfaceAlarmRequest(pModem, var, reqType);
	}

	return EC;
}

MC_ErrorCode processDatumRequest(CDatumModem *pModem, cSnmpVariable &var, unsigned char reqType)
{
	MC_ErrorCode EC  = MC_DEVICE_NOT_RESPONDING;
	printf("Datum modem\n");

	if (var.m_OID.isPartOfOID(OidDatumModemUnitObjs, OidDatumModemUnitObjsLen))
	{
		EC = processDatumUnitRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemModulatorObjs, OidDatumModemModulatorObjsLen))
	{
		EC = processDatumModulatorRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemDemodulatorObjs, OidDatumModemDemodulatorObjsLen))
	{
		EC = processDatumDemodulatorRequest(pModem, var, reqType);
	}
	else if (var.m_OID.isPartOfOID(OidDatumModemInterfaceObjs, OidDatumModemInterfaceObjsLen))
	{
		EC = processDatumInterfaceRequest(pModem, var, reqType);
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

