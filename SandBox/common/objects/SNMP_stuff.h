/* SNMP_stuff.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	17 Sep 2008 - initial creation

*/

#ifndef __SNMP_STUFF_H_INCLUDED__
#define __SNMP_STUFF_H_INCLUDED__

#include "Udp.h"

const unsigned short SNMP_PORT = 161;
const unsigned short SNMP_TRAP_PORT = 162;

const unsigned char PDU_TYPE_GET_REQUEST = 0x00;
const unsigned char PDU_TYPE_GET_NEXT_REQUEST = 0x01;
const unsigned char PDU_TYPE_SET_REQUEST = 0x02;
const unsigned char PDU_TYPE_GET_RESPONCE = 0x03;
const unsigned char PDU_TYPE_TRAP = 0x04;

const unsigned char SNMP_ERROR_CODE_OK = 0x00;
const unsigned char SNMP_ERROR_CODE_TOO_BIG = 0x01; //tooBig клиент не может поместить отклик в одно SNMP сообщение 
const unsigned char SNMP_ERROR_CODE_NO_SUCH_NAME = 0x02; // noSuchName оператор указывает на несуществующую переменную 
const unsigned char SNMP_ERROR_CODE_BAD_VALUE = 0x03; //badValue в операции установки использовано недопустимое значение или сделана ошибка в синтаксисе 
const unsigned char SNMP_ERROR_CODE_READ_ONLY = 0x04; //readOnly менеджер попытался изменить переменную, которая помечена как "только для чтения" 
const unsigned char SNMP_ERROR_CODE_GENERIC = 0x05; //genErr неопознанная ошибка 

// Field type (Universal Class)
const unsigned char SNMP_FIELD_TYPE_INTEGER = 0x02;
const unsigned char SNMP_FIELD_TYPE_OCTET_STRING = 0x04;
const unsigned char SNMP_FIELD_TYPE_NULL = 0x05;
const unsigned char SNMP_FIELD_TYPE_OID = 0x06;
const unsigned char SNMP_FIELD_TYPE_SEQUENCE = 0x30;

// Field type (Application Class)
const unsigned char SNMP_FIELD_TYPE_IPADDRESS = 0x40;
const unsigned char SNMP_FIELD_TYPE_COUNTER32 = 0x41;
const unsigned char SNMP_FIELD_TYPE_GAUGE = 0x42;
const unsigned char SNMP_FIELD_TYPE_TIME_TICKS = 0x43;
const unsigned char SNMP_FIELD_TYPE_OPAQUE = 0x44;
const unsigned char SNMP_FIELD_TYPE_COUNTER64 = 0x46;

// Field type (Context Specific)
const unsigned char SNMP_FIELD_GET_REQUEST = 0xA0;
const unsigned char SNMP_FIELD_GET_NEXT_REQUEST = 0xA1;
const unsigned char SNMP_FIELD_GET_RESPONSE = 0xA2;
const unsigned char SNMP_FIELD_SET_REQUEST = 0xA3;
const unsigned char SNMP_FIELD_TRAP = 0xA4;

typedef struct
{
	int m_version;
	int m_type;
	unsigned int m_uiOID[32];
	int m_nOIDlen;
	int m_RequestID;
	int m_iIntegerValue;
	unsigned int m_uiIntegerValue;
	unsigned char m_StatusCode, m_ErrorCode;
	char m_OctetStringValue[256];
	
	LOGICAL IsTheSameOID(const unsigned int *puiOID, int nOIDlen);
} SSnmpReply;
 
// The following methods return number of octets required for encoding, and modifies current pos in buffer
int EncodeSequence(unsigned char *&pucCurrentPos);
LOGICAL DecodeSequence(unsigned char *&pucCurrentPos, int &nOctets);
int EncodeNull(unsigned char *&pucCurrentPos);
int EncodeGetRequest(unsigned char *&pucCurrentPos);
int EncodeSetRequest(unsigned char *&pucCurrentPos);
LOGICAL DecodeResponse(unsigned char *&pucCurrentPos, int &nOctets);
int EncodeOID(unsigned int *puiOID, int nOidLen, unsigned char *&pucCurrentPos);
LOGICAL DecodeOID(unsigned int *puiOID, int &nOidLen, const unsigned char *&pucCurrentPos);
int EncodeOctet(const unsigned char ucValue, unsigned char *&pucCurrentPos);
LOGICAL DecodeOctet(unsigned char &ucValue, unsigned char *&pucCurrentPos);
int EncodeInteger(const int iValue, unsigned char *&pucCurrentPos);
int EncodeUnsignedInteger(const int iValue, unsigned char *&pucCurrentPos);
LOGICAL DecodeInteger(int &iValue, const unsigned char *&pucCurrentPos);
LOGICAL DecodeUnsignedInteger(unsigned int &uiValue, const unsigned char *&pucCurrentPos);
int EncodeString(const char *pszValue, unsigned char *&pucCurrentPos);
LOGICAL DecodeString(char *pszValue, const unsigned char *&pucCurrentPos);

LOGICAL DecodeValue(SSnmpReply *pReply, const unsigned char *&pucCurrentPos);
LOGICAL DecodeSnmpDgm(unsigned char *pDgm, SSnmpReply *pReply);

class CSnmpSocket : public CUdpSocket
{
// Constructors/destructors
public:
	CSnmpSocket();
	virtual ~CSnmpSocket();

// Public methods
public:
	LOGICAL SendSnmpGetRequest(IPADDRESS_TYPE uIP, const char *pszCommunity, unsigned int *puiOID, int nOidLen, int ReqID);
	LOGICAL SendSnmpSetIntegerRequest(IPADDRESS_TYPE uIP, const char *pszCommunity, unsigned int *puiOID, int nOidLen, int ReqID, int iValue);
	LOGICAL SendSnmpSetUnsignedIntegerRequest(IPADDRESS_TYPE uIP, const char *pszCommunity, unsigned int *puiOID, int nOidLen, int ReqID, unsigned int uiValue);
	LOGICAL GetSnmpReply(SSnmpReply *pReply);
};

#endif //__SNMP_STUFF_H_INCLUDED__