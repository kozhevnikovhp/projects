/* SNMP_stuff.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	17 Sep 2008 - initial creation

*/

#ifndef __SNMP_STUFF_H_INCLUDED__
#define __SNMP_STUFF_H_INCLUDED__

#include "Udp.h"
#include <string>

const unsigned short SNMP_PORT = 161;
const unsigned short SNMP_TRAP_PORT = 162;

const unsigned char PDU_TYPE_GET_REQUEST = 0x00;
const unsigned char PDU_TYPE_GET_NEXT_REQUEST = 0x01;
const unsigned char PDU_TYPE_SET_REQUEST = 0x02;
const unsigned char PDU_TYPE_GET_RESPONSE = 0x03;
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

class cSnmpOID
{
public:
	cSnmpOID();
	cSnmpOID(const unsigned int *pOID, int nOidLength);

	void operator = (const cSnmpOID &OID);
	void set(const unsigned int *pOID, int nOidLength);
	void addDot0();
	void addDot1();

	bool isTheSame(const cSnmpOID &OID) const;
	bool isTheSameOID(const unsigned int *puiOID, int nOIDlen) const;
	bool isPartOfOID(const unsigned int *puiOID, int nOIDlen) const;
	std::string getSymbolicName() const;

	unsigned int m_uiOID[32];
	int m_nOIDlen;
};

class cSnmpVariable
{
public:
	cSnmpVariable();
	void setOID(const unsigned int *pOID, int nOidLength);
	void appendDot0ToOID();
	void appendDot1ToOID();
	void setInteger32Value(int iValue) { m_iIntegerValue = iValue; m_DataType = SNMP_FIELD_TYPE_INTEGER; }
	void setGaugeValue(int iValue) { m_GaugeValue = iValue; m_DataType = SNMP_FIELD_TYPE_GAUGE; }
	void setStringValue(const std::string &strValue) {	m_strValue = strValue;	m_DataType = SNMP_FIELD_TYPE_OCTET_STRING; }
	
	void operator = (const cSnmpVariable &var);

	cSnmpOID m_OID;
	unsigned char m_DataType;
	int m_iIntegerValue;
	unsigned int m_GaugeValue;
	std::string m_strValue;
};

class cSnmpDatagram
{
public:
	cSnmpDatagram();

	unsigned char getRequestType() const { return m_FieldType; }
	bool isRequest() const { return (isGetRequest() || isGetNextRequest() || isSetRequest()); }
	bool isGetRequest() const { return (m_FieldType == SNMP_FIELD_GET_REQUEST); }
	bool isGetNextRequest() const { return (m_FieldType == SNMP_FIELD_GET_NEXT_REQUEST); }
	void setGetResponse() { m_FieldType = SNMP_FIELD_GET_RESPONSE; }
	bool isSetRequest() const { return m_FieldType == SNMP_FIELD_SET_REQUEST; }
	bool isResponse() const { return isGetResponse(); }
	bool isGetResponse() const { return (m_FieldType == SNMP_FIELD_GET_RESPONSE); }
	IPADDRESS_TYPE getSourceAddress() { return srcIpAddress_; }


	void operator = (const cSnmpDatagram &dgm);

	IPADDRESS_TYPE srcIpAddress_;
	unsigned short m_FromIpPort;
	int m_version;
	unsigned char m_FieldType;
	std::string m_Community;
	int m_RequestID;
	unsigned char m_StatusCode, m_ErrorCode;
	cSnmpVariable m_Variable;	
};
 

class CSnmpSocket : public CUdpSocket
{
// Constructors/destructors
public:
	CSnmpSocket();
	virtual ~CSnmpSocket();

// Public methods
public:
	LOGICAL waitForRequest(cSnmpDatagram &dgm);
	LOGICAL SendGetResponse(IPADDRESS_TYPE uIP, unsigned short uPort, int iVersion, const std::string &strCommunity, int ReqID, const cSnmpVariable &var);
	LOGICAL SendSnmpGetRequest(IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID);
	LOGICAL SendSnmpGetNextRequest(IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID);
	LOGICAL SendSnmpSetIntegerRequest(IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID, int iValue);
	LOGICAL SendSnmpSetUnsignedIntegerRequest(IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID, unsigned int uiValue);
	LOGICAL GetSnmpReply(cSnmpDatagram &replyDgm);

protected:
	LOGICAL SendSnmpRequest(unsigned char reqCode, IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID);
};

#endif //__SNMP_STUFF_H_INCLUDED__