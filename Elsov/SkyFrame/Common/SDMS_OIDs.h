
#define ELEMENT_COUNT(x) (sizeof(x)/sizeof(x[0]))

#define ENTERPRISES 1,3,6,1,4,1

#define POZHARSKY_SYSTEMS ENTERPRISES,6840
#define		POZHARSKY_UP_DOWN_MIB POZHARSKY_SYSTEMS,1
#define			POZHARSKY_UP_CONVERTER		POZHARSKY_UP_DOWN_MIB,1
#define				POZHARSKY_UP_ONOFF			POZHARSKY_UP_CONVERTER,1
#define				POZHARSKY_UP_FREQUENCY		POZHARSKY_UP_CONVERTER,2
#define				POZHARSKY_UP_ATTENUATOR		POZHARSKY_UP_CONVERTER,3
#define			POZHARSKY_DOWN_CONVERTER	POZHARSKY_UP_DOWN_MIB,2
#define				POZHARSKY_DOWN_ONOFF		POZHARSKY_DOWN_CONVERTER,1
#define				POZHARSKY_DOWN_FREQUENCY	POZHARSKY_DOWN_CONVERTER,2
#define				POZHARSKY_DOWN_ATTENUATOR	POZHARSKY_DOWN_CONVERTER,3

#define DATUM_SYSTEMS ENTERPRISES,7840
#define		DATUM_REG		DATUM_SYSTEMS,1
#define		DATUM_GENERIC	DATUM_SYSTEMS,2
#define		DATUM_PRODUCTS	DATUM_SYSTEMS,3
#define			DATUM_MODEM_MIB DATUM_PRODUCTS,1
#define				DATUM_MODEM_CONF	DATUM_MODEM_MIB,1
#define					DATUM_MODEM_GROUPS	DATUM_MODEM_CONF,1
#define					DATUM_MODEM_COMPL	DATUM_MODEM_CONF,2
#define				DATUM_MODEM_OBJS	DATUM_MODEM_MIB,2
#define					DATUM_MODEM_UNIT_OBJS DATUM_MODEM_OBJS,1
#define						DATUM_MODEM_UNIT_STATUS_OBJS	DATUM_MODEM_UNIT_OBJS,1
#define							DATUM_MODEM_UNIT_NAME		DATUM_MODEM_UNIT_STATUS_OBJS,1
#define							DATUM_MODEM_UNIT_MODEL		DATUM_MODEM_UNIT_STATUS_OBJS,2
#define							DATUM_MODEM_UNIT_SERIAL_NO	DATUM_MODEM_UNIT_STATUS_OBJS,3
#define							DATUM_MODEM_UNIT_VERSION	DATUM_MODEM_UNIT_STATUS_OBJS,4
#define							DATUM_MODEM_UNIT_ADDRESS	DATUM_MODEM_UNIT_STATUS_OBJS,5
#define						UNIT_CONFIG_OBJS	DATUM_MODEM_UNIT_OBJS,2
#define						UNIT_KEYBRD_OBJS	DATUM_MODEM_UNIT_OBJS,3
#define						UNIT_IRDA_OBJS		DATUM_MODEM_UNIT_OBJS,4
#define						UNIT_REMOTE_OBJS	DATUM_MODEM_UNIT_OBJS,5
#define						UNIT_REF_OBJS		DATUM_MODEM_UNIT_OBJS,6
#define						UNIT_RDNDNCY_OBJS	DATUM_MODEM_UNIT_OBJS,7
#define						UNIT_MONITOR_OBJS	DATUM_MODEM_UNIT_OBJS,8
#define						DATUM_MODEM_UNIT_ALARM			DATUM_MODEM_UNIT_OBJS,9
#define							DATUM_MODEM_UNIT_REFERENCE_ALARM	DATUM_MODEM_UNIT_ALARM,1
#define							DATUM_MODEM_UNIT_TEST_ACTIVE_ALARM	DATUM_MODEM_UNIT_ALARM,2
#define							DATUM_MODEM_UNIT_HARDWARE_ALARM		DATUM_MODEM_UNIT_ALARM,3
#define							DATUM_MODEM_UNIT_BEEPER_ALARM		DATUM_MODEM_UNIT_ALARM,4
#define							DATUM_MODEM_UNIT_OCXO_OVEN_ALARM	DATUM_MODEM_UNIT_ALARM,5
#define					DATUM_MODEM_MOD_OBJS DATUM_MODEM_OBJS,2
#define						MOD_STATUS_OBJS	DATUM_MODEM_MOD_OBJS,1
#define							MOD_COUNT			MOD_STATUS_OBJS,1
#define							MOD_STATUS_TABLE	MOD_STATUS_OBJS,2
#define								MOD_STATUS_ENTRY	MOD_STATUS_TABLE,1
#define									MOD_INDEX		MOD_STATUS_ENTRY,1
#define									MOD_CXR_EN		MOD_STATUS_ENTRY,2
#define									MOD_CXR_ALARM	MOD_STATUS_ENTRY,3
#define									MOD_RTS_MUTE	MOD_STATUS_ENTRY,4
#define									MOD_OPT_ALARM	MOD_STATUS_ENTRY,5
#define									MOD_APC_ALARM	MOD_STATUS_ENTRY,6
#define									MOD_APC_LMT		MOD_STATUS_ENTRY,7
#define									MOD_BIT_ALARM	MOD_STATUS_ENTRY,8
#define									MOD_REF_ALARM	MOD_STATUS_ENTRY,9
#define									MOD_DLCK_ALARM	MOD_STATUS_ENTRY,10
#define									MOD_LVL_ALARM	MOD_STATUS_ENTRY,11
#define									MOD_LO_ALARM	MOD_STATUS_ENTRY,12
#define									MOD_STP_ALARM	MOD_STATUS_ENTRY,13
#define									MOD_SYS_ALARM	MOD_STATUS_ENTRY,14
#define									MOD_PURE_CXR	MOD_STATUS_ENTRY,15
#define									MOD_ALT0FOR1	MOD_STATUS_ENTRY,16
#define									MOD_SIDEBAND	MOD_STATUS_ENTRY,17
#define									MOD_ONLINE		MOD_STATUS_ENTRY,18
#define									MOD_OCXO_ALARM	MOD_STATUS_ENTRY,19
#define						MOD_CONFIG_OBJS	DATUM_MODEM_MOD_OBJS,2
#define							MOD_IF_TABLE	MOD_CONFIG_OBJS,1
#define								MOD_IF_ENTRY	MOD_IF_TABLE,1
#define									MOD_IF_FREQUENCY		MOD_IF_ENTRY,1
#define									MOD_IF_OFFSET			MOD_IF_ENTRY,2
#define									MOD_IF_LEVEL			MOD_IF_ENTRY,3
#define									MOD_IF_OUTPUT_EN		MOD_IF_ENTRY,4
#define									MOD_IF_MODULATION		MOD_IF_ENTRY,5
#define									MOD_IF_SPECTRUM			MOD_IF_ENTRY,6
#define									MOD_IF_MODE				MOD_IF_ENTRY,7
#define									MOD_IF_BURST_PRE_LEN	MOD_IF_ENTRY,8
#define									MOD_IF_AUPC_MODE		MOD_IF_ENTRY,9
#define									MOD_IF_CXR_MUTE_MODE	MOD_IF_ENTRY,10
#define									MOD_IF_OUTPUT_IMP		MOD_IF_ENTRY,11
#define							MOD_DATA_TABLE	MOD_CONFIG_OBJS,2
#define								MOD_DATA_ENTRY	MOD_DATA_TABLE,1
#define									MOD_DATA_BIT_RATE			MOD_DATA_ENTRY,1
#define									MOD_DATA_MODULATION			MOD_DATA_ENTRY,2
#define									MOD_DATA_FEC_TYPE			MOD_DATA_ENTRY,3
#define									MOD_DATA_CODE_RATE			MOD_DATA_ENTRY,4
#define									MOD_DATA_DIFF_ENC			MOD_DATA_ENTRY,5
#define									MOD_DATA_SCRAMBLER			MOD_DATA_ENTRY,6
#define									MOD_DATA_CLOCK_SRC			MOD_DATA_ENTRY,7
#define									MOD_DATA_FEC_MODE			MOD_DATA_ENTRY,8
#define									MOD_DATA_FEC_OPTION			MOD_DATA_ENTRY,9
#define									MOD_DATA_REED_SOLOMON_MODE	MOD_DATA_ENTRY,10
#define							MOD_ALARM_TABLE	MOD_CONFIG_OBJS,3
#define								MOD_ALARM_ENTRY	MOD_ALARM_TABLE,1
#define									MOD_ALARM_CXR_ALARM			MOD_ALARM_ENTRY,1
#define									MOD_ALARM_BITCLOCK_ALARM	MOD_ALARM_ENTRY,2
#define									MOD_ALARM_AUPC_ALARM		MOD_ALARM_ENTRY,3
#define									MOD_ALARM_TST_ACT_ALARM		MOD_ALARM_ENTRY,4
#define									MOD_ALARM_HDW_ALARM			MOD_ALARM_ENTRY,5
#define									MOD_ALARM_BUC_PWR_ALARM	MOD_ALARM_ENTRY,6
#define							MOD_TEST_TABLE	MOD_CONFIG_OBJS,4
#define								MOD_TEST_ENTRY	MOD_TEST_TABLE,1
#define									MOD_TEST_CXR_ALC		MOD_TEST_ENTRY,1
#define									MOD_TEST_LO_AFC			MOD_TEST_ENTRY,2
#define									MOD_TEST_STEP_AFC		MOD_TEST_ENTRY,3
#define									MOD_TEST_SYMBOL_RATE	MOD_TEST_ENTRY,4
#define									MOD_TEST_MODULATION		MOD_TEST_ENTRY,5
#define							MOD_BUC_TABLE	MOD_CONFIG_OBJS,5
#define								MOD_BUC_ENTRY	MOD_BUC_TABLE,1
#define									MOD_BUC_VOLTAGE_OUT		MOD_BUC_ENTRY,1
#define									MOD_BUC_VOLTAGE_MIN		MOD_BUC_ENTRY,2
#define									MOD_BUC_CURRENT_OUT		MOD_BUC_ENTRY,3
#define									MOD_BUC_CURRENT_MAX		MOD_BUC_ENTRY,4
#define									MOD_BUC_CURRENT_MIN		MOD_BUC_ENTRY,5
#define									MOD_BUC_LO_FREQUENCY	MOD_BUC_ENTRY,6
#define									MOD_BUC_POWER			MOD_BUC_ENTRY,7
#define									MOD_BUC_10MHZ			MOD_BUC_ENTRY,8
#define					DATUM_MODEM_DEMOD_OBJS DATUM_MODEM_OBJS,3
#define						DEMOD_STATUS_OBJS	DATUM_MODEM_DEMOD_OBJS,1
#define							DEMOD_COUNT			DEMOD_STATUS_OBJS,1
#define							DEMOD_STATUS_TABLE	DEMOD_STATUS_OBJS,2
#define								DEMOD_STATUS_ENTRY	DEMOD_STATUS_TABLE,1
#define									DEMOD_INDEX			DEMOD_STATUS_ENTRY,1
#define									DEMOD_RCV_OFFSET	DEMOD_STATUS_ENTRY,2
#define									DEMOD_RCV_LEVEL		DEMOD_STATUS_ENTRY,3
#define									DEMOD_RCV_EBNO		DEMOD_STATUS_ENTRY,4
#define									DEMOD_EST_BER		DEMOD_STATUS_ENTRY,5
#define									DEMOD_SER			DEMOD_STATUS_ENTRY,6
#define									DEMOD_BUFFER		DEMOD_STATUS_ENTRY,7
#define									DEMOD_RCV_LOCK		DEMOD_STATUS_ENTRY,8
#define						DEMOD_CONFIG_OBJS	DATUM_MODEM_DEMOD_OBJS,2
#define							DEMOD_IF_TABLE		DEMOD_CONFIG_OBJS,1
#define								DEMOD_IF_ENTRY	DEMOD_IF_TABLE,1
#define									DEMOD_IF_FREQUENCY		DEMOD_IF_ENTRY,1
#define									DEMOD_IF_SWEEP_RANGE	DEMOD_IF_ENTRY,2
#define									DEMOD_IF_SWEEP_TIME		DEMOD_IF_ENTRY,3
#define									DEMOD_IF_LOW_LEVEL		DEMOD_IF_ENTRY,4
#define									DEMOD_IF_LOW_EBNO		DEMOD_IF_ENTRY,5
#define									DEMOD_IF_SWEEP_MODE		DEMOD_IF_ENTRY,6
#define									DEMOD_IF_MODULATION		DEMOD_IF_ENTRY,7
#define									DEMOD_IF_SPECTRUM		DEMOD_IF_ENTRY,8
#define									DEMOD_IF_INPUT_IMP		DEMOD_IF_ENTRY,9
#define							DEMOD_DATA_TABLE	DEMOD_CONFIG_OBJS,2
#define								DEMOD_DATA_ENTRY	DEMOD_DATA_TABLE,1
#define									DEMOD_DATA_BIT_RATE				DEMOD_DATA_ENTRY,1
#define									DEMOD_DATA_BUFFER_DELAY			DEMOD_DATA_ENTRY,2
#define									DEMOD_DATA_BUFFER_SIZE			DEMOD_DATA_ENTRY,3
#define									DEMOD_DATA_FEC_HOLD_COUNT		DEMOD_DATA_ENTRY,4
#define									DEMOD_DATA_FEC_TYPE				DEMOD_DATA_ENTRY,5
#define									DEMOD_DATA_CODE_RATE			DEMOD_DATA_ENTRY,6
#define									DEMOD_DATA_DIFF_DECODER			DEMOD_DATA_ENTRY,7
#define									DEMOD_DATA_DESCRAMBLER			DEMOD_DATA_ENTRY,8
#define									DEMOD_DATA_CLOCK_SOURCE			DEMOD_DATA_ENTRY,9
#define									DEMOD_DATA_FEC_C0C1_MODE		DEMOD_DATA_ENTRY,10
#define									DEMOD_DATA_FEC_OPTION			DEMOD_DATA_ENTRY,11 //???
#define									DEMOD_DATA_REED_SOLOMON_MODE	DEMOD_DATA_ENTRY,12
#define							DEMOD_ALARM_TABLE	DEMOD_CONFIG_OBJS,3
#define								DEMOD_ALARM_ENTRY	DEMOD_ALARM_TABLE,1
#define									DEMOD_ALARM_CXR_ALARM		DEMOD_ALARM_ENTRY,1
#define									DEMOD_ALARM_LVL_ALARM		DEMOD_ALARM_ENTRY,2
#define									DEMOD_ALARM_EBNO_ALARM		DEMOD_ALARM_ENTRY,3
#define									DEMOD_ALARM_TSTACT_ALARM	DEMOD_ALARM_ENTRY,4
#define									DEMOD_ALARM_HDW_ALARM		DEMOD_ALARM_ENTRY,5
#define									DEMOD_ALARM_BCK_ALARM		DEMOD_ALARM_ENTRY,6
#define									DEMOD_ALARM_LNB_POWER_ALARM	DEMOD_ALARM_ENTRY,7
#define							DEMOD_TEST_TABLE	DEMOD_CONFIG_OBJS,4
#define								DEMOD_TEST_ENTRY	DEMOD_TEST_TABLE,1
#define									DEMOD_TEST_AGC		DEMOD_TEST_ENTRY,1
#define									DEMOD_TEST_LO_AFC	DEMOD_TEST_ENTRY,2
#define									DEMOD_TEST_STEP_AFC	DEMOD_TEST_ENTRY,3
#define									DEMOD_TEST_IDC_OFF	DEMOD_TEST_ENTRY,4
#define									DEMOD_TEST_QDC_OFF	DEMOD_TEST_ENTRY,5
#define							DEMOD_LNB_TABLE		DEMOD_CONFIG_OBJS,5
#define								DEMOD_LNB_ENTRY	DEMOD_LNB_TABLE,1
#define									DEMOD_LNB_CURRENT_OUT	DEMOD_LNB_ENTRY,1
#define									DEMOD_LNB_CURRENT_MAX	DEMOD_LNB_ENTRY,2
#define									DEMOD_LNB_CURRENT_MIN	DEMOD_LNB_ENTRY,3
#define									DEMOD_LNB_LO_FREQUENCY	DEMOD_LNB_ENTRY,4
#define									DEMOD_LNB_POWER			DEMOD_LNB_ENTRY,5
#define									DEMOD_LNB_10_MHZ		DEMOD_LNB_ENTRY,6
#define					DATUM_MODEM_INTERFACE_OBJS DATUM_MODEM_OBJS,4
#define						DATUM_MODEM_INTERFACE_IO_TABLE		DATUM_MODEM_INTERFACE_OBJS,2
#define						DATUM_MODEM_INTERFACE_ALARM_TABLE	DATUM_MODEM_INTERFACE_OBJS,3
#define							DATUM_MODEM_INTERFACE_TEST_ALARM	DATUM_MODEM_INTERFACE_ALARM_TABLE,1
#define							DATUM_MODEM_INTERFACE_SDMS_ALARM	DATUM_MODEM_INTERFACE_ALARM_TABLE,2
#define							DATUM_MODEM_INTERFACE_BER_ALARM		DATUM_MODEM_INTERFACE_ALARM_TABLE,3
#define						DATUM_MODEM_INTERFACE_TEST_TABLE	DATUM_MODEM_INTERFACE_OBJS,4
#define						DATUM_MODEM_INTERFACE_SDMS_TABLE	DATUM_MODEM_INTERFACE_OBJS,5
#define				DATUM_MODEM_EVENTS		DATUM_MODEM_MIB,3
#define					DATUM_MODEM_EVENTS_V2 DATUM_MODEM_EVENTS,0
#define		DATUM_CAPS DATUM_SYSTEMS,4
#define		DATUM_REQS DATUM_SYSTEMS,5
#define		DATUM_EXPR DATUM_SYSTEMS,6

// Pozharsky-systems
extern unsigned int OidPozharskyUpConverter[];
extern unsigned int OidPozharskyUpConverterLen;
extern unsigned int OidPozharskyUpOnOff[];
extern unsigned int OidPozharskyUpOnOffLen;
extern unsigned int OidPozharskyUpFrequency[];
extern unsigned int OidPozharskyUpFrequencyLen;
extern unsigned int OidPozharskyUpAttenuator[];
extern unsigned int OidPozharskyUpAttenuatorLen;

extern unsigned int OidPozharskyDownConverter[];
extern unsigned int OidPozharskyDownConverterLen;
extern unsigned int OidPozharskyDownOnOff[];
extern unsigned int OidPozharskyDownOnOffLen;
extern unsigned int OidPozharskyDownFrequency[];
extern unsigned int OidPozharskyDownFrequencyLen;
extern unsigned int OidPozharskyDownAttenuator[];
extern unsigned int OidPozharskyDownAttenuatorLen;


// Datum-systems
extern unsigned int OidDatumSystems[];
extern unsigned int OidDatumSystemsLen;

// Datum products
extern unsigned int OidDatumProducts[];
extern unsigned int OidDatumProductsLen;

// Datum modem MIB
extern unsigned int OidDatumModemMIB[];
extern unsigned int OidDatumModemMIBLen;

// Datum modem UNIT objs
extern unsigned int OidDatumModemUnitObjs[];
extern unsigned int OidDatumModemUnitObjsLen;

// Datum modem UNIT ALARMS
extern unsigned int OidDatumModemUnitAlarm[];
extern unsigned int OidDatumModemUnitAlarmLen;
extern unsigned int OidDatumModemUnitReferenceAlarm[];
extern unsigned int OidDatumModemUnitReferenceAlarmLen;
extern unsigned int OidDatumModemUnitTestActiveAlarm[];
extern unsigned int OidDatumModemUnitTestActiveAlarmLen;
extern unsigned int OidDatumModemUnitHardwareAlarm[];
extern unsigned int OidDatumModemUnitHardwareAlarmLen;
extern unsigned int OidDatumModemUnitBeeperAlarm[];
extern unsigned int OidDatumModemUnitBeeperAlarmLen;
extern unsigned int OidDatumModemUnitOcxoOvenAlarm[];
extern unsigned int OidDatumModemUnitOcxoOvenAlarmLen;

// Datum modem UNIT STATUS
extern unsigned int OidDatumModemUnitName[];
extern unsigned int OidDatumModemUnitNameLen;
extern unsigned int OidDatumModemUnitModel[];
extern unsigned int OidDatumModemUnitModelLen;
extern unsigned int OidDatumModemUnitSerialNo[];
extern unsigned int OidDatumModemUnitSerialNoLen;
extern unsigned int OidDatumModemUnitVersion[];
extern unsigned int OidDatumModemUnitVersionLen;
extern unsigned int OidDatumModemUnitAddress[];
extern unsigned int OidDatumModemUnitAddressLen;
extern unsigned int OidDatumModemUnit6[];
extern unsigned int OidDatumModemUnit6Len;
extern unsigned int OidDatumModemUnit7[];
extern unsigned int OidDatumModemUnit7Len;
extern unsigned int OidDatumModemUnit8[];
extern unsigned int OidDatumModemUnit8Len;
extern unsigned int OidDatumModemUnit9[];
extern unsigned int OidDatumModemUnit9Len;

// Datum modem MODULATOR objs
extern unsigned int OidDatumModemModulatorObjs[];
extern unsigned int OidDatumModemModulatorObjsLen;
extern unsigned int OidDatumModemModulatorCount[];
extern unsigned int OidDatumModemModulatorCountLen;
extern unsigned int OidDatumModemModulatorIndex[];
extern unsigned int OidDatumModemModulatorIndexLen;

// Datum modem DEMODULATOR objs
extern unsigned int OidDatumModemDemodulatorObjs[];
extern unsigned int OidDatumModemDemodulatorObjsLen;
extern unsigned int OidDatumModemDemodulatorCount[];
extern unsigned int OidDatumModemDemodulatorCountLen;
extern unsigned int OidDatumModemDemodulatorIndex[];
extern unsigned int OidDatumModemDemodulatorIndexLen;

// Datum modem INTERFACE objs
extern unsigned int OidDatumModemInterfaceObjs[];
extern unsigned int OidDatumModemInterfaceObjsLen;
extern unsigned int OidDatumModemInterfaceIOTable[];
extern unsigned int OidDatumModemInterfaceIOTableLen;
extern unsigned int OidDatumModemInterfaceAlarmTable[];
extern unsigned int OidDatumModemInterfaceAlarmTableLen;
extern unsigned int OidDatumModemInterfaceAlarmTest[];
extern unsigned int OidDatumModemInterfaceAlarmTestLen;
extern unsigned int OidDatumModemInterfaceAlarmSdms[];
extern unsigned int OidDatumModemInterfaceAlarmSdmsLen;
extern unsigned int OidDatumModemInterfaceAlarmBer[];
extern unsigned int OidDatumModemInterfaceAlarmBerLen;

// ModIfTable
extern unsigned int OidModulatorIf[];
extern unsigned int OidModulatorIfLen;
extern unsigned int OidModulatorIfFrequency[];
extern unsigned int OidModulatorIfFrequencyLen;
extern unsigned int OidModulatorIfOffset[];
extern unsigned int OidModulatorIfOffsetLen;
extern unsigned int OidModulatorIfOutputLevel[];
extern unsigned int OidModulatorIfOutputLevelLen;
extern unsigned int OidModulatorIfOutputEnabled[];
extern unsigned int OidModulatorIfOutputEnabledLen;
extern unsigned int OidModulatorIfModulation[];
extern unsigned int OidModulatorIfModulationLen;
extern unsigned int OidModulatorIfSpectrum[];
extern unsigned int OidModulatorIfSpectrumLen;
extern unsigned int OidModulatorIfMode[];
extern unsigned int OidModulatorIfModeLen;

// ModDataTable
extern unsigned int OidModulatorData[];
extern unsigned int OidModulatorDataLen;
extern unsigned int OidModulatorDataBitRate[];
extern unsigned int OidModulatorDataBitRateLen;
extern unsigned int OidModulatorDataFecType[];
extern unsigned int OidModulatorDataFecTypeLen;
extern unsigned int OidModulatorDataCodeRate[];
extern unsigned int OidModulatorDataCodeRateLen;
extern unsigned int OidModulatorDataDiffEncoder[];
extern unsigned int OidModulatorDataDiffEncoderLen;
extern unsigned int OidModulatorDataScrambler[];
extern unsigned int OidModulatorDataScramblerLen;
extern unsigned int OidModulatorDataClockSource[];
extern unsigned int OidModulatorDataClockSourceLen;
extern unsigned int OidModulatorDataC0C1Mode[];
extern unsigned int OidModulatorDataC0C1ModeLen;
extern unsigned int OidModulatorDataFecOption[];
extern unsigned int OidModulatorDataFecOptionLen;
extern unsigned int OidModulatorDataReedSolomonMode[];
extern unsigned int OidModulatorDataReedSolomonModeLen;

// ModBucTable
extern unsigned int OidModulatorBuc[];
extern unsigned int OidModulatorBucLen;
extern unsigned int OidModulatorBucVoltageOut[];
extern unsigned int OidModulatorBucVoltageOutLen;
extern unsigned int OidModulatorBucVoltageMin[];
extern unsigned int OidModulatorBucVoltageMinLen;
extern unsigned int OidModulatorBucCurrentOut[];
extern unsigned int OidModulatorBucCurrentOutLen;
extern unsigned int OidModulatorBucCurrentMin[];
extern unsigned int OidModulatorBucCurrentMinLen;
extern unsigned int OidModulatorBucCurrentMax[];
extern unsigned int OidModulatorBucCurrentMaxLen;
extern unsigned int OidModulatorBucLoFreq[];
extern unsigned int OidModulatorBucLoFreqLen;
extern unsigned int OidModulatorBucPower[];
extern unsigned int OidModulatorBucPowerLen;
extern unsigned int OidModulatorBuc10MHz[];
extern unsigned int OidModulatorBuc10MHzLen;

// DemodIfTable
extern unsigned int OidDemodulatorIf[];
extern unsigned int OidDemodulatorIfLen;
extern unsigned int OidDemodulatorIfFrequency[];
extern unsigned int OidDemodulatorIfFrequencyLen;
extern unsigned int OidDemodulatorIfSweepRange[];
extern unsigned int OidDemodulatorIfSweepRangeLen;
extern unsigned int OidDemodulatorIfSweepTime[];
extern unsigned int OidDemodulatorIfSweepTimeLen;
extern unsigned int OidDemodulatorIfSweepMode[];
extern unsigned int OidDemodulatorIfSweepModeLen;
extern unsigned int OidDemodulatorIfLowEbno[];
extern unsigned int OidDemodulatorIfLowEbnoLen;
extern unsigned int OidDemodulatorIfLowLevel[];
extern unsigned int OidDemodulatorIfLowLevelLen;
extern unsigned int OidDemodulatorIfModulation[];
extern unsigned int OidDemodulatorIfModulationLen;
extern unsigned int OidDemodulatorIfSpectrum[];
extern unsigned int OidDemodulatorIfSpectrumLen;

// DemodDataTable
extern unsigned int OidDemodulatorData[];
extern unsigned int OidDemodulatorDataLen;
extern unsigned int OidDemodulatorDataBitRate[];
extern unsigned int OidDemodulatorDataBitRateLen;
extern unsigned int OidDemodulatorDataBufferDelay[];
extern unsigned int OidDemodulatorDataBufferDelayLen;
extern unsigned int OidDemodulatorDataBufferSize[];
extern unsigned int OidDemodulatorDataBufferSizeLen;
extern unsigned int OidDemodulatorDataFecType[];
extern unsigned int OidDemodulatorDataFecTypeLen;
extern unsigned int OidDemodulatorDataCodeRate[];
extern unsigned int OidDemodulatorDataCodeRateLen;
extern unsigned int OidDemodulatorDataDiffDecoder[];
extern unsigned int OidDemodulatorDataDiffDecoderLen;
extern unsigned int OidDemodulatorDataDescrambler[];
extern unsigned int OidDemodulatorDataDescramblerLen;
extern unsigned int OidDemodulatorDataClockSource[];
extern unsigned int OidDemodulatorDataClockSourceLen;
extern unsigned int OidDemodulatorDataFecC0C1Mode[];
extern unsigned int OidDemodulatorDataFecC0C1ModeLen;
extern unsigned int OidDemodulatorDataFecOption[];
extern unsigned int OidDemodulatorDataFecOptionLen;
extern unsigned int OidDemodulatorDataReedSolomonMode[];
extern unsigned int OidDemodulatorDataReedSolomonModeLen;

// DemodAlarmTable
extern unsigned int OidDemodulatorAlarm[];
extern unsigned int OidDemodulatorAlarmLen;
extern unsigned int OidDemodulatorCxrAlarm[];
extern unsigned int OidDemodulatorCxrAlarmLen;
extern unsigned int OidDemodulatorLvlAlarm[];
extern unsigned int OidDemodulatorLvlAlarmLen;
extern unsigned int OidDemodulatorEbnoAlarm[];
extern unsigned int OidDemodulatorEbnoAlarmLen;
extern unsigned int OidDemodulatorTstActAlarm[];
extern unsigned int OidDemodulatorTstActAlarmLen;
extern unsigned int OidDemodulatorHdwAlarm[];
extern unsigned int OidDemodulatorHdwAlarmLen;
extern unsigned int OidDemodulatorBckAlarm[];
extern unsigned int OidDemodulatorBckAlarmLen;
extern unsigned int OidDemodulatorLnbPowerAlarm[];
extern unsigned int OidDemodulatorLnbPowerAlarmLen;

// DemodLnbTable
extern unsigned int OidDemodulatorLnb[];
extern unsigned int OidDemodulatorLnbLen;
extern unsigned int OidDemodulatorLnbCurrentOut[];
extern unsigned int OidDemodulatorLnbCurrentOutLen;
extern unsigned int OidDemodulatorLnbCurrentMin[];
extern unsigned int OidDemodulatorLnbCurrentMinLen;
extern unsigned int OidDemodulatorLnbCurrentMax[];
extern unsigned int OidDemodulatorLnbCurrentMaxLen;
extern unsigned int OidDemodulatorLnbLoFreq[];
extern unsigned int OidDemodulatorLnbLoFreqLen;
extern unsigned int OidDemodulatorLnbPower[];
extern unsigned int OidDemodulatorLnbPowerLen;
extern unsigned int OidDemodulatorLnb10MHz[];
extern unsigned int OidDemodulatorLnb10MHzLen;

// DemodStatus
extern unsigned int OidDemodulatorStatus[];
extern unsigned int OidDemodulatorStatusLen;
extern unsigned int OidDemodulatorStatusOffset[];
extern unsigned int OidDemodulatorStatusOffsetLen;
extern unsigned int OidDemodulatorStatusInputLevel[];
extern unsigned int OidDemodulatorStatusInputLevelLen;
extern unsigned int OidDemodulatorStatusEbNo[];
extern unsigned int OidDemodulatorStatusEbNoLen;
extern unsigned int OidDemodulatorStatusBER[];
extern unsigned int OidDemodulatorStatusBERLen;
extern unsigned int OidDemodulatorStatusSER[];
extern unsigned int OidDemodulatorStatusSERLen;
extern unsigned int OidDemodulatorStatusBuffer[];
extern unsigned int OidDemodulatorStatusBufferLen;
extern unsigned int OidDemodulatorStatusLock[];
extern unsigned int OidDemodulatorStatusLockLen;

extern unsigned int OidModulatorDataBitRate[];
extern unsigned int OidModulatorDataBitRateLen;
extern unsigned int OidDemodulatorDataBitRate[];
extern unsigned int OidDemodulatorDataBitRateLen;

#define OID_AND_LEN(x) x, ELEMENT_COUNT(x)


