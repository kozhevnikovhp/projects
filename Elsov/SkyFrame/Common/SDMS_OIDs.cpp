#include "stdafx.h"
#include "SDMS_OIDs.h"

// Pozharsky UP-converter
extern unsigned int OidPozharskyUpConverter[] = { POZHARSKY_UP_CONVERTER };
extern unsigned int OidPozharskyUpConverterLen = ELEMENT_COUNT(OidPozharskyUpConverter);
extern unsigned int OidPozharskyUpOnOff[] = { POZHARSKY_UP_ONOFF };
extern unsigned int OidPozharskyUpOnOffLen = ELEMENT_COUNT(OidPozharskyUpOnOff);
extern unsigned int OidPozharskyUpFrequency[] = { POZHARSKY_UP_FREQUENCY };
extern unsigned int OidPozharskyUpFrequencyLen = ELEMENT_COUNT(OidPozharskyUpFrequency);
extern unsigned int OidPozharskyUpAttenuator[] = {	POZHARSKY_UP_ATTENUATOR };
extern unsigned int OidPozharskyUpAttenuatorLen = ELEMENT_COUNT(OidPozharskyUpAttenuator);

// Pozharsky DOWN-converter
extern unsigned int OidPozharskyDownConverter[] = { POZHARSKY_DOWN_CONVERTER };
extern unsigned int OidPozharskyDownConverterLen = ELEMENT_COUNT(OidPozharskyDownConverter);
extern unsigned int OidPozharskyDownOnOff[] = { POZHARSKY_DOWN_ONOFF };
extern unsigned int OidPozharskyDownOnOffLen = ELEMENT_COUNT(OidPozharskyDownOnOff);
extern unsigned int OidPozharskyDownFrequency[] = { POZHARSKY_DOWN_FREQUENCY };
extern unsigned int OidPozharskyDownFrequencyLen = ELEMENT_COUNT(OidPozharskyDownFrequency);
extern unsigned int OidPozharskyDownAttenuator[] = { POZHARSKY_DOWN_ATTENUATOR };
extern unsigned int OidPozharskyDownAttenuatorLen = ELEMENT_COUNT(OidPozharskyDownAttenuator);

// Datum-systems
extern unsigned int OidDatumSystems[] = { DATUM_SYSTEMS };
extern unsigned int OidDatumSystemsLen = ELEMENT_COUNT(OidDatumSystems);

// Datum products
extern unsigned int OidDatumProducts[] = { DATUM_PRODUCTS };
extern unsigned int OidDatumProductsLen = ELEMENT_COUNT(OidDatumProducts);

// Datum modem
extern unsigned int OidDatumModemMIB[] = { DATUM_MODEM_MIB };
extern unsigned int OidDatumModemMIBLen = ELEMENT_COUNT(OidDatumModemMIB);

// Datum modem UNIT objs
extern unsigned int OidDatumModemUnitObjs[] = { DATUM_MODEM_UNIT_OBJS };
extern unsigned int OidDatumModemUnitObjsLen = ELEMENT_COUNT(OidDatumModemUnitObjs);

extern unsigned int OidDatumModemUnitAlarm[] = { DATUM_MODEM_UNIT_ALARM };
extern unsigned int OidDatumModemUnitAlarmLen = ELEMENT_COUNT(OidDatumModemUnitAlarm);
extern unsigned int OidDatumModemUnitReferenceAlarm[] = { DATUM_MODEM_UNIT_REFERENCE_ALARM };
extern unsigned int OidDatumModemUnitReferenceAlarmLen = ELEMENT_COUNT(OidDatumModemUnitReferenceAlarm);
extern unsigned int OidDatumModemUnitTestActiveAlarm[] = { DATUM_MODEM_UNIT_TEST_ACTIVE_ALARM };
extern unsigned int OidDatumModemUnitTestActiveAlarmLen = ELEMENT_COUNT(OidDatumModemUnitTestActiveAlarm);
extern unsigned int OidDatumModemUnitHardwareAlarm[] = { DATUM_MODEM_UNIT_HARDWARE_ALARM };
extern unsigned int OidDatumModemUnitHardwareAlarmLen = ELEMENT_COUNT(OidDatumModemUnitHardwareAlarm);
extern unsigned int OidDatumModemUnitBeeperAlarm[] = { DATUM_MODEM_UNIT_BEEPER_ALARM };
extern unsigned int OidDatumModemUnitBeeperAlarmLen = ELEMENT_COUNT(OidDatumModemUnitBeeperAlarm);
extern unsigned int OidDatumModemUnitOcxoOvenAlarm[] = { DATUM_MODEM_UNIT_OCXO_OVEN_ALARM };
extern unsigned int OidDatumModemUnitOcxoOvenAlarmLen = ELEMENT_COUNT(OidDatumModemUnitOcxoOvenAlarm);

// Datum modem UNIT STATUS
extern unsigned int OidDatumModemUnitName[] = { DATUM_MODEM_UNIT_NAME };
extern unsigned int OidDatumModemUnitNameLen = ELEMENT_COUNT(OidDatumModemUnitName);
extern unsigned int OidDatumModemUnitModel[] = { DATUM_MODEM_UNIT_MODEL };
extern unsigned int OidDatumModemUnitModelLen = ELEMENT_COUNT(OidDatumModemUnitModel);
extern unsigned int OidDatumModemUnitSerialNo[] = { DATUM_MODEM_UNIT_SERIAL_NO };
extern unsigned int OidDatumModemUnitSerialNoLen = ELEMENT_COUNT(OidDatumModemUnitSerialNo);
extern unsigned int OidDatumModemUnitVersion[] = { DATUM_MODEM_UNIT_VERSION };
extern unsigned int OidDatumModemUnitVersionLen = ELEMENT_COUNT(OidDatumModemUnitVersion);
extern unsigned int OidDatumModemUnitAddress[] = { DATUM_MODEM_UNIT_ADDRESS };
extern unsigned int OidDatumModemUnitAddressLen = ELEMENT_COUNT(OidDatumModemUnitAddress);

// Datum modem MODULATOR objs
extern unsigned int OidDatumModemModulatorObjs[] = { DATUM_MODEM_MOD_OBJS };
extern unsigned int OidDatumModemModulatorObjsLen = ELEMENT_COUNT(OidDatumModemModulatorObjs);

// Datum modem DMODULATOR objs
extern unsigned int OidDatumModemDemodulatorObjs[] = { DATUM_MODEM_DEMOD_OBJS };
extern unsigned int OidDatumModemDemodulatorObjsLen = ELEMENT_COUNT(OidDatumModemDemodulatorObjs);

// Datum modem INTERFACE objs
extern unsigned int OidDatumModemInterfaceObjs[] = { DATUM_MODEM_INTERFACE_OBJS };
extern unsigned int OidDatumModemInterfaceObjsLen = ELEMENT_COUNT(OidDatumModemInterfaceObjs);
extern unsigned int OidDatumModemInterfaceIOTable[] = { DATUM_MODEM_INTERFACE_IO_TABLE };
extern unsigned int OidDatumModemInterfaceIOTableLen = ELEMENT_COUNT(OidDatumModemInterfaceIOTable);
extern unsigned int OidDatumModemInterfaceAlarmTable[] = { DATUM_MODEM_INTERFACE_ALARM_TABLE };
extern unsigned int OidDatumModemInterfaceAlarmTableLen = ELEMENT_COUNT(OidDatumModemInterfaceAlarmTable);
extern unsigned int OidDatumModemInterfaceAlarmTest[] = { DATUM_MODEM_INTERFACE_TEST_ALARM };
extern unsigned int OidDatumModemInterfaceAlarmTestLen = ELEMENT_COUNT(OidDatumModemInterfaceAlarmTest);
extern unsigned int OidDatumModemInterfaceAlarmSdms[] = { DATUM_MODEM_INTERFACE_SDMS_ALARM };
extern unsigned int OidDatumModemInterfaceAlarmSdmsLen = ELEMENT_COUNT(OidDatumModemInterfaceAlarmSdms);
extern unsigned int OidDatumModemInterfaceAlarmBer[] = { DATUM_MODEM_INTERFACE_BER_ALARM };
extern unsigned int OidDatumModemInterfaceAlarmBerLen = ELEMENT_COUNT(OidDatumModemInterfaceAlarmBer);

// Modulator
extern unsigned int OidDatumModemModulatorCount[] = {MOD_COUNT,1};
extern unsigned int OidDatumModemModulatorCountLen = ELEMENT_COUNT(OidDatumModemModulatorCount);
extern unsigned int OidDatumModemModulatorIndex[] = {MOD_INDEX,1};
extern unsigned int OidDatumModemModulatorIndexLen = ELEMENT_COUNT(OidDatumModemModulatorIndex);

//  IF table
unsigned int OidModulatorIf[] = { MOD_IF_TABLE,1 };
unsigned int OidModulatorIfLen = ELEMENT_COUNT(OidModulatorIf);
unsigned int OidModulatorIfFrequency[] =   { MOD_IF_FREQUENCY,1 };
unsigned int OidModulatorIfFrequencyLen =   ELEMENT_COUNT(OidModulatorIfFrequency);
unsigned int OidModulatorIfOffset[] =   { MOD_IF_OFFSET,1 };
unsigned int OidModulatorIfOffsetLen =   ELEMENT_COUNT(OidModulatorIfFrequency);
unsigned int OidModulatorIfOutputLevel[] =   { MOD_IF_LEVEL,1 };
unsigned int OidModulatorIfOutputLevelLen =   ELEMENT_COUNT(OidModulatorIfOutputLevel);
unsigned int OidModulatorIfOutputEnabled[] =   { MOD_IF_OUTPUT_EN,1 };
unsigned int OidModulatorIfOutputEnabledLen =   ELEMENT_COUNT(OidModulatorIfOutputEnabled);
unsigned int OidModulatorIfModulation[] =   { MOD_IF_MODULATION,1 };
unsigned int OidModulatorIfModulationLen =   ELEMENT_COUNT(OidModulatorIfModulation);
unsigned int OidModulatorIfSpectrum[] =   { MOD_IF_SPECTRUM,1 };
unsigned int OidModulatorIfSpectrumLen =   ELEMENT_COUNT(OidModulatorIfSpectrum);
unsigned int OidModulatorIfMode[] =   { MOD_IF_MODE,1 };
unsigned int OidModulatorIfModeLen =   ELEMENT_COUNT(OidModulatorIfMode);

//  Data table
unsigned int OidModulatorData[] = { MOD_DATA_TABLE,1 };
unsigned int OidModulatorDataLen = ELEMENT_COUNT(OidModulatorData);
unsigned int OidModulatorDataBitRate[] =   { MOD_DATA_BIT_RATE,1 };
unsigned int OidModulatorDataBitRateLen =   ELEMENT_COUNT(OidModulatorDataBitRate);
unsigned int OidModulatorDataFecType[] =   { MOD_DATA_FEC_TYPE,1 };
unsigned int OidModulatorDataFecTypeLen =   ELEMENT_COUNT(OidModulatorDataFecType);
unsigned int OidModulatorDataCodeRate[] =   { MOD_DATA_CODE_RATE,1 };
unsigned int OidModulatorDataCodeRateLen =   ELEMENT_COUNT(OidModulatorDataCodeRate);
unsigned int OidModulatorDataDiffEncoder[] =   { MOD_DATA_DIFF_ENC,1 };
unsigned int OidModulatorDataDiffEncoderLen =   ELEMENT_COUNT(OidModulatorDataDiffEncoder);
unsigned int OidModulatorDataScrambler[] =   { MOD_DATA_SCRAMBLER,1 };
unsigned int OidModulatorDataScramblerLen =   ELEMENT_COUNT(OidModulatorDataScrambler);
unsigned int OidModulatorDataClockSource[] =   { MOD_DATA_CLOCK_SRC,1 };
unsigned int OidModulatorDataClockSourceLen =   ELEMENT_COUNT(OidModulatorDataClockSource);
//unsigned int OidModulatorDataC0C1Mode[] =   { MOD_DATA_FEC_C0C1_MODE,1 };
//unsigned int OidModulatorDataC0C1ModeLen =   ELEMENT_COUNT(OidModulatorDataC0C1Mode);
unsigned int OidModulatorDataFecOption[] =   { MOD_DATA_FEC_OPTION,1 };
unsigned int OidModulatorDataFecOptionLen =   ELEMENT_COUNT(OidModulatorDataFecOption);
unsigned int OidModulatorDataReedSolomonMode[] =   { MOD_DATA_REED_SOLOMON_MODE,1 };
unsigned int OidModulatorDataReedSolomonModeLen =   ELEMENT_COUNT(OidModulatorDataReedSolomonMode);

// ModBucTable
unsigned int OidModulatorBuc[] = { MOD_BUC_TABLE,1 };
unsigned int OidModulatorBucLen = ELEMENT_COUNT(OidModulatorBuc);
unsigned int OidModulatorBucVoltageOut[] = { MOD_BUC_VOLTAGE_OUT,1 };
unsigned int OidModulatorBucVoltageOutLen = ELEMENT_COUNT(OidModulatorBucVoltageOut);
unsigned int OidModulatorBucVoltageMin[] = { MOD_BUC_VOLTAGE_MIN,1 };
unsigned int OidModulatorBucVoltageMinLen = ELEMENT_COUNT(OidModulatorBucVoltageMin);
unsigned int OidModulatorBucCurrentOut[] = { MOD_BUC_CURRENT_OUT,1 };
unsigned int OidModulatorBucCurrentOutLen = ELEMENT_COUNT(OidModulatorBucCurrentOut);
unsigned int OidModulatorBucCurrentMin[] = { MOD_BUC_CURRENT_MIN,1 };
unsigned int OidModulatorBucCurrentMinLen = ELEMENT_COUNT(OidModulatorBucCurrentMin);
unsigned int OidModulatorBucCurrentMax[] = { MOD_BUC_CURRENT_MAX,1 };
unsigned int OidModulatorBucCurrentMaxLen = ELEMENT_COUNT(OidModulatorBucCurrentMax);
unsigned int OidModulatorBucLoFreq[] = { MOD_BUC_LO_FREQUENCY,1 };
unsigned int OidModulatorBucLoFreqLen = ELEMENT_COUNT(OidModulatorBucLoFreq);
unsigned int OidModulatorBucPower[] = { MOD_BUC_POWER,1 };
unsigned int OidModulatorBucPowerLen = ELEMENT_COUNT(OidModulatorBucPower);
unsigned int OidModulatorBuc10MHz[] = { MOD_BUC_10MHZ,1 };
unsigned int OidModulatorBuc10MHzLen = ELEMENT_COUNT(OidModulatorBuc10MHz);

// Demodulator
unsigned int OidDatumModemDemodulatorCount[] = {DEMOD_COUNT,1};
unsigned int OidDatumModemDemodulatorCountLen = ELEMENT_COUNT(OidDatumModemDemodulatorCount);
unsigned int OidDatumModemDemodulatorIndex[] = {DEMOD_INDEX,1};
unsigned int OidDatumModemDemodulatorIndexLen = ELEMENT_COUNT(OidDatumModemDemodulatorIndex);

//  Demod IF table
unsigned int OidDemodulatorIf[] = { DEMOD_IF_TABLE,1 };
unsigned int OidDemodulatorIfLen = ELEMENT_COUNT(OidDemodulatorIf);
unsigned int OidDemodulatorIfFrequency[] =   { DEMOD_IF_FREQUENCY,1 };
unsigned int OidDemodulatorIfFrequencyLen =   ELEMENT_COUNT(OidDemodulatorIfFrequency);
unsigned int OidDemodulatorIfSweepRange[] = { DEMOD_IF_SWEEP_RANGE,1 };
unsigned int OidDemodulatorIfSweepRangeLen =   ELEMENT_COUNT(OidDemodulatorIfSweepRange);
unsigned int OidDemodulatorIfSweepTime[] = { DEMOD_IF_SWEEP_TIME,1 };
unsigned int OidDemodulatorIfSweepTimeLen =   ELEMENT_COUNT(OidDemodulatorIfSweepTime);
unsigned int OidDemodulatorIfLowEbno[] = { DEMOD_IF_LOW_EBNO,1 };
unsigned int OidDemodulatorIfLowEbnoLen =   ELEMENT_COUNT(OidDemodulatorIfLowEbno);
unsigned int OidDemodulatorIfLowLevel[] = { DEMOD_IF_LOW_LEVEL,1 };
unsigned int OidDemodulatorIfLowLevelLen =   ELEMENT_COUNT(OidDemodulatorIfLowLevel);
unsigned int OidDemodulatorIfSweepMode[] = { DEMOD_IF_SWEEP_MODE,1 };
unsigned int OidDemodulatorIfSweepModeLen =   ELEMENT_COUNT(OidDemodulatorIfSweepMode);
unsigned int OidDemodulatorIfModulation[] =   { DEMOD_IF_MODULATION,1 };
unsigned int OidDemodulatorIfModulationLen =   ELEMENT_COUNT(OidDemodulatorIfModulation);
unsigned int OidDemodulatorIfSpectrum[] =   { DEMOD_IF_SPECTRUM,1 };
unsigned int OidDemodulatorIfSpectrumLen =   ELEMENT_COUNT(OidDemodulatorIfSpectrum);

//  Demod Data table
unsigned int OidDemodulatorData[] = { DEMOD_DATA_TABLE,1 };
unsigned int OidDemodulatorDataLen = ELEMENT_COUNT(OidDemodulatorData);
unsigned int OidDemodulatorDataBitRate[] =   { DEMOD_DATA_BIT_RATE,1 };
unsigned int OidDemodulatorDataBitRateLen =   ELEMENT_COUNT(OidDemodulatorDataBitRate);
unsigned int OidDemodulatorDataBufferDelay[] =   { DEMOD_DATA_BUFFER_DELAY,1 };
unsigned int OidDemodulatorDataBufferDelayLen =   ELEMENT_COUNT(OidDemodulatorDataBufferDelay);
unsigned int OidDemodulatorDataBufferSize[] =   { DEMOD_DATA_BUFFER_SIZE,1 };
unsigned int OidDemodulatorDataBufferSizeLen =   ELEMENT_COUNT(OidDemodulatorDataBufferSize);
unsigned int OidDemodulatorDataFecType[] =   { DEMOD_DATA_FEC_TYPE,1 };
unsigned int OidDemodulatorDataFecTypeLen =   ELEMENT_COUNT(OidDemodulatorDataFecType);
unsigned int OidDemodulatorDataCodeRate[] =   { DEMOD_DATA_CODE_RATE,1 };
unsigned int OidDemodulatorDataCodeRateLen =   ELEMENT_COUNT(OidDemodulatorDataCodeRate);
unsigned int OidDemodulatorDataDiffDecoder[] =   { DEMOD_DATA_DIFF_DECODER,1 };
unsigned int OidDemodulatorDataDiffDecoderLen =   ELEMENT_COUNT(OidDemodulatorDataDiffDecoder);
unsigned int OidDemodulatorDataDescrambler[] =   { DEMOD_DATA_DESCRAMBLER,1 };
unsigned int OidDemodulatorDataDescramblerLen =   ELEMENT_COUNT(OidDemodulatorDataDescrambler);
unsigned int OidDemodulatorDataClockSource[] =   { DEMOD_DATA_CLOCK_SOURCE,1 };
unsigned int OidDemodulatorDataClockSourceLen =   ELEMENT_COUNT(OidDemodulatorDataClockSource);
unsigned int OidDemodulatorDataFecC0C1Mode[] =   { DEMOD_DATA_FEC_C0C1_MODE,1 };
unsigned int OidDemodulatorDataFecC0C1ModeLen =   ELEMENT_COUNT(OidDemodulatorDataFecC0C1Mode);
unsigned int OidDemodulatorDataFecOption[] =   { DEMOD_DATA_FEC_OPTION,1 };
unsigned int OidDemodulatorDataFecOptionLen =   ELEMENT_COUNT(OidDemodulatorDataFecOption);
unsigned int OidDemodulatorDataReedSolomonMode[] =   { DEMOD_DATA_REED_SOLOMON_MODE,1 };
unsigned int OidDemodulatorDataReedSolomonModeLen =   ELEMENT_COUNT(OidDemodulatorDataReedSolomonMode);

//  Demod Alarm table
unsigned int OidDemodulatorAlarm[] = { DEMOD_ALARM_TABLE,1 };
unsigned int OidDemodulatorAlarmLen = ELEMENT_COUNT(OidDemodulatorAlarm);
unsigned int OidDemodulatorCxrAlarm[] = { DEMOD_ALARM_CXR_ALARM,1 };
unsigned int OidDemodulatorCxrAlarmLen = ELEMENT_COUNT(OidDemodulatorCxrAlarm);
unsigned int OidDemodulatorLvlAlarm[] = { DEMOD_ALARM_LVL_ALARM,1 };
unsigned int OidDemodulatorLvlAlarmLen = ELEMENT_COUNT(OidDemodulatorLvlAlarm);
unsigned int OidDemodulatorEbnoAlarm[] = { DEMOD_ALARM_EBNO_ALARM,1 };
unsigned int OidDemodulatorEbnoAlarmLen = ELEMENT_COUNT(OidDemodulatorEbnoAlarm);
unsigned int OidDemodulatorTstActAlarm[] = { DEMOD_ALARM_TSTACT_ALARM,1 };
unsigned int OidDemodulatorTstActAlarmLen = ELEMENT_COUNT(OidDemodulatorTstActAlarm);
unsigned int OidDemodulatorHdwAlarm[] = { DEMOD_ALARM_HDW_ALARM,1 };
unsigned int OidDemodulatorHdwAlarmLen = ELEMENT_COUNT(OidDemodulatorHdwAlarm);
unsigned int OidDemodulatorBckAlarm[] = { DEMOD_ALARM_BCK_ALARM,1 };
unsigned int OidDemodulatorBckAlarmLen = ELEMENT_COUNT(OidDemodulatorBckAlarm);
unsigned int OidDemodulatorLnbPowerAlarm[] = { DEMOD_ALARM_LNB_POWER_ALARM,1 };
unsigned int OidDemodulatorLnbPowerAlarmLen = ELEMENT_COUNT(OidDemodulatorBckAlarm);

// DemodLnbTable
unsigned int OidDemodulatorLnb[] = { DEMOD_LNB_TABLE,1 };
unsigned int OidDemodulatorLnbLen = ELEMENT_COUNT(OidDemodulatorLnb);
unsigned int OidDemodulatorLnbCurrentOut[] = { DEMOD_LNB_CURRENT_OUT,1 };
unsigned int OidDemodulatorLnbCurrentOutLen = ELEMENT_COUNT(OidDemodulatorLnbCurrentOut);
unsigned int OidDemodulatorLnbCurrentMin[] = { DEMOD_LNB_CURRENT_MIN,1 };
unsigned int OidDemodulatorLnbCurrentMinLen = ELEMENT_COUNT(OidDemodulatorLnbCurrentMin);
unsigned int OidDemodulatorLnbCurrentMax[] = { DEMOD_LNB_CURRENT_MAX,1 };
unsigned int OidDemodulatorLnbCurrentMaxLen = ELEMENT_COUNT(OidDemodulatorLnbCurrentMax);
unsigned int OidDemodulatorLnbLoFreq[] = { DEMOD_LNB_LO_FREQUENCY,1 };
unsigned int OidDemodulatorLnbLoFreqLen = ELEMENT_COUNT(OidDemodulatorLnbLoFreq);
unsigned int OidDemodulatorLnbPower[] = { DEMOD_LNB_POWER,1 };
unsigned int OidDemodulatorLnbPowerLen = ELEMENT_COUNT(OidDemodulatorLnbPower);
unsigned int OidDemodulatorLnb10MHz[] = { DEMOD_LNB_10_MHZ,1 };
unsigned int OidDemodulatorLnb10MHzLen = ELEMENT_COUNT(OidDemodulatorLnb10MHz);

// Status
unsigned int OidDemodulatorStatus[] =  { DEMOD_STATUS_TABLE,1 };
unsigned int OidDemodulatorStatusLen = ELEMENT_COUNT(OidDemodulatorStatus);
unsigned int OidDemodulatorStatusOffset[] = { DEMOD_RCV_OFFSET,1 };
unsigned int OidDemodulatorStatusOffsetLen = ELEMENT_COUNT(OidDemodulatorStatusOffset);
unsigned int OidDemodulatorStatusInputLevel[] = { DEMOD_RCV_LEVEL,1 };
unsigned int OidDemodulatorStatusInputLevelLen = ELEMENT_COUNT(OidDemodulatorStatusInputLevel);
unsigned int OidDemodulatorStatusEbNo[] = { DEMOD_RCV_EBNO,1 };
unsigned int OidDemodulatorStatusEbNoLen = ELEMENT_COUNT(OidDemodulatorStatusEbNo);
unsigned int OidDemodulatorStatusBER[] = { DEMOD_EST_BER,1 };
unsigned int OidDemodulatorStatusBERLen = ELEMENT_COUNT(OidDemodulatorStatusBER);
unsigned int OidDemodulatorStatusSER[] = { DEMOD_SER,1 };
unsigned int OidDemodulatorStatusSERLen = ELEMENT_COUNT(OidDemodulatorStatusSER);
unsigned int OidDemodulatorStatusBuffer[] = { DEMOD_BUFFER,1 };
unsigned int OidDemodulatorStatusBufferLen = ELEMENT_COUNT(OidDemodulatorStatusBuffer);
unsigned int OidDemodulatorStatusLock[] = { DEMOD_RCV_LOCK,1 };
unsigned int OidDemodulatorStatusLockLen = ELEMENT_COUNT(OidDemodulatorStatusLock);


