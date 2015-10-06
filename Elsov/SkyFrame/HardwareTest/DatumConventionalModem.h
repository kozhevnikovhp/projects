#ifndef __DATUM_CONVENTIONAL_MODEM_H_INCLUDED__
#define __DATUM_CONVENTIONAL_MODEM_H_INCLUDED__

#include "Datum.h"

// Conventional means "with fixed structure", "without slots with various devices" etc
// 4900, M500 etc

const unsigned char modeRead = 0x00;
const unsigned char modeExecute = 0x0F;
const unsigned char modeExecuteAndWrite = 0xFF;

const unsigned char PAD = 0xFF; // pad byte
const unsigned char COMMAND_OPEN_FLAG = 0xA5; // open flag for command
const unsigned char REPLY_OPEN_FLAG = 0x5A; // open flag for response
const unsigned char CLOSE_FLAG = 0x96; // close flag for both command and response

class CDatumConventionalModem : public CDatumModem
{
public:
	CDatumConventionalModem();

	virtual int GetModulatorsNumber() { return 1; }
	virtual int GetDemodulatorsNumber() { return 1; }

	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char GetModeField(unsigned char Mode) = 0; // pure virtual - must be overriden

protected:
	int FillCommandBuffer(unsigned char CommandCode, unsigned char Mode, unsigned char *pcDataBytes, unsigned char nDataBytes);
	unsigned char CheckSum(int Start, int Stop);
	unsigned char CheckSum(unsigned char *pcData, int Start, int Stop);

	unsigned int RawDataToInt(unsigned char *pszRawData);
	short RawDataToSignedShort(unsigned char *pszRawData);
	void IntToRawData(unsigned int IntValue, unsigned char *pszRawData);
	void SignedShortToRawData(short ShortValue, unsigned char *pszRawData);

	unsigned char m_cStatusByte;
	unsigned char m_cErrorByte;
	unsigned char m_cDataBytes;
};



#endif //__DATUM_CONVENTIONAL_MODEM_H_INCLUDED__