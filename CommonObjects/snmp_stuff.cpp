/* SNMP_stuff.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	17 Sep 2008 - initial creation

*/

#include "stdafx.h"
#include "snmp_stuff.h"

// The following methods return number of octets required for encoding, and modifies current pos in buffer

int EncodeSequence(unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_TYPE_SEQUENCE;
	++pucCurrentPos;
	// Length
	*pucCurrentPos = 0x00; // unknown yet
	++pucCurrentPos;

	return 2; // 2 octets required
}

LOGICAL DecodeSequence(unsigned char *&pucCurrentPos, int &nOctets)
{
	nOctets = 0;
	if (*pucCurrentPos != SNMP_FIELD_TYPE_SEQUENCE)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	nOctets = *pucCurrentPos;
	++pucCurrentPos;
	return LOGICAL_TRUE;
}

int EncodeByte(unsigned char cValue, unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = cValue;
	++pucCurrentPos;
	// Length
	*pucCurrentPos = 0x00; // just 0
	++pucCurrentPos;

	return 2; // 2 octets required
}

int EncodeNull(unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_TYPE_NULL;
	++pucCurrentPos;
	// Length
	*pucCurrentPos = 0x00; // just 0
	++pucCurrentPos;

	return 2; // 2 octets required
}

LOGICAL DecodeResponseField(unsigned char *&pucCurrentPos, unsigned char &fieldType, int &nOctets)
{
	fieldType = *pucCurrentPos;
	nOctets = 0;
	if (fieldType != SNMP_FIELD_GET_RESPONSE)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	// Length
	nOctets = *pucCurrentPos;
	++pucCurrentPos;
	return LOGICAL_TRUE;
}

LOGICAL DecodeRequestField(unsigned char *&pucCurrentPos, unsigned char &fieldType, int &nOctets)
{
	fieldType = *pucCurrentPos;
	nOctets = 0;
	if (fieldType != SNMP_FIELD_GET_REQUEST      &&
		fieldType != SNMP_FIELD_GET_NEXT_REQUEST &&
		fieldType != SNMP_FIELD_SET_REQUEST)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	// Length
	nOctets = *pucCurrentPos;
	++pucCurrentPos;
	return LOGICAL_TRUE;
}

void OidIntToByte(unsigned int &uiValue, unsigned char *&pucCurrentPos, int &nOctets, unsigned char *pucOidLen, LOGICAL bMultyByteSign)
{
	if (uiValue  >= 0x80)
	{
		unsigned char cLastOctet = uiValue % 0x80;
		uiValue /= 0x80;
		OidIntToByte(uiValue, pucCurrentPos, nOctets, pucOidLen, LOGICAL_TRUE);
		(*pucCurrentPos) = cLastOctet;
	}
	else
	{
		*pucCurrentPos = uiValue;
	}
	
	if (bMultyByteSign)
		(*pucCurrentPos) |= 0x80;
	++pucCurrentPos;
	++nOctets;
	(*pucOidLen)++;
}

int EncodeOID(const cSnmpOID &OID, unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_TYPE_OID;
	++pucCurrentPos;
	// Length
	unsigned char *pucOidLen = pucCurrentPos;
	*pucOidLen = 0x00; // unknown yet

	++pucCurrentPos;
	int nOctets = 2; // 2 octets occupied already
	// packed two first octets
	*pucCurrentPos = OID.m_uiOID[0]*40 + OID.m_uiOID[1];
	++pucCurrentPos;
	++nOctets;
	(*pucOidLen)++;
	for (int i = 2; i < OID.m_nOIDlen; i++)
	{
		unsigned int iValue = OID.m_uiOID[i];
		OidIntToByte(iValue, pucCurrentPos, nOctets, pucOidLen, LOGICAL_FALSE);
	}
	return nOctets;
}

LOGICAL DecodeOID(cSnmpOID &OID, const unsigned char *&pucCurrentPos)
{
	OID.m_nOIDlen = 0;
	if (*pucCurrentPos != SNMP_FIELD_TYPE_OID)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	// Length
	int nOidOctets = *pucCurrentPos;
	++pucCurrentPos;
	// packed two first octets
	unsigned char subidentifier = *pucCurrentPos;
	OID.m_uiOID[0] = subidentifier/40;
	OID.m_uiOID[1] = subidentifier - OID.m_uiOID[0]*40;
	pucCurrentPos += 1;
	OID.m_nOIDlen = 2;
	// The rest octets of OID
	for (int i = 2; i <= nOidOctets;)
	{
		unsigned int uiOid = 0;
		unsigned char ucOid;
		do
		{
			ucOid = *pucCurrentPos;
			uiOid <<= 7;
			uiOid |= (ucOid & 0x7F);
			++pucCurrentPos;
			++i;
		} while (ucOid & 0x80);

		OID.m_uiOID[OID.m_nOIDlen] = uiOid;
		++OID.m_nOIDlen;
	}

	return LOGICAL_TRUE;
}

int EncodeOctet(const unsigned char ucValue, unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_TYPE_INTEGER;
	++pucCurrentPos;
	// Length
	*pucCurrentPos = 0x01; // 1 octet
	++pucCurrentPos;
	// Value
	*pucCurrentPos = ucValue;
	++pucCurrentPos;

	return 3; // 3 octets required
}

LOGICAL DecodeOctet(unsigned char &ucValue, unsigned char *&pucCurrentPos)
{
	if (*pucCurrentPos != SNMP_FIELD_TYPE_INTEGER)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	// Length
	if (*pucCurrentPos != 1)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	ucValue = *pucCurrentPos;
	++pucCurrentPos;
	return LOGICAL_TRUE;
}

int EncodeInteger(const int iValue, unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_TYPE_INTEGER;
	++pucCurrentPos;

	// Store place for integer length
	unsigned char *pucIntLength = pucCurrentPos;
	++pucCurrentPos;
	
	int integer = iValue;
    int size = sizeof(int);
    
	// Truncate "unnecessary" bytes off of the most significant end of this
	// 2's complement integer.  There should be no sequence of 9
	// consecutive 1's or 0's at the most significant end of the
	// integer.

	// mask is 0xFF800000 on a big-endian machine 
	unsigned int mask = ((unsigned int) 0x1FF) << ((8 * (sizeof(int) - 1)) - 1);
	while ((((integer & mask) == 0) || ((integer & mask) == mask))
		   && size > 1)
	{
		size--;
		integer <<= 8;
	}

	// mask is 0xFF000000 on a big-endian machine
	mask = ((unsigned int) 0xFF) << (8 * (sizeof(int) - 1));
	
	*pucIntLength = 0;
	while (size--)
	{
		*pucCurrentPos = (unsigned char) ((integer & mask) >> (8 * (sizeof(long) - 1)));
		integer <<= 8;
		++(*pucIntLength);
		++pucCurrentPos;
	}

	return 1+1+(*pucIntLength);
}

int EncodeGauge(const unsigned int uiValue, unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_TYPE_GAUGE;
	++pucCurrentPos;

	// Store place for integer length
	unsigned char *pucIntLength = pucCurrentPos;
	++pucCurrentPos;
	
	unsigned int integer = uiValue;
    int size = sizeof(int);
	LOGICAL bAddNullByte= LOGICAL_FALSE;
    
    unsigned int mask = ((unsigned int)0xFF) << (8 * (sizeof(int) - 1));
    /*
     * mask is 0xFF000000 on a big-endian machine 
     */
    if ((unsigned char) ((integer & mask) >> (8 * (sizeof(int) - 1))) & 0x80)
	{
        /*
         * if MSB is set 
         */
        bAddNullByte = 1;
        size++;
    }
	else
	{
        /*
         * Truncate "unnecessary" bytes off of the most significant end of this 2's complement integer.
         * There should be no sequence of 9 consecutive 1's or 0's at the most significant end of the
         * integer.
         */
        mask = ((unsigned int) 0x1FF) << ((8 * (sizeof(int) - 1)) - 1);
        /*
         * mask is 0xFF800000 on a big-endian machine 
         */
        while ((((integer & mask) == 0) || ((integer & mask) == mask))
               && size > 1) {
            size--;
            integer <<= 8;
        }
    }
    
	if (bAddNullByte)
	{
        *pucCurrentPos++ = 0;
        size--;
    }
	
	// mask is 0xFF000000 on a big-endian machine
	mask = ((unsigned int) 0xFF) << (8 * (sizeof(int) - 1));
	
	*pucIntLength = 0;
	while (size--)
	{
		*pucCurrentPos = (unsigned char) ((integer & mask) >> (8 * (sizeof(long) - 1)));
		integer <<= 8;
		++(*pucIntLength);
		++pucCurrentPos;
	}

	return 1+1+(*pucIntLength);
}

LOGICAL DecodeInteger(int &iValue, const unsigned char *&pucCurrentPos)
{
	iValue = 0;
	if (*pucCurrentPos != SNMP_FIELD_TYPE_INTEGER &&
		*pucCurrentPos != SNMP_FIELD_TYPE_COUNTER32 &&
		*pucCurrentPos != SNMP_FIELD_TYPE_GAUGE)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	// Length
	int nLen = *pucCurrentPos;
	++pucCurrentPos;
	// Value
	if (*pucCurrentPos & 0x80)
		iValue = -1;
	while (nLen--)
	{
		iValue = (iValue << 8) | *pucCurrentPos;
		++pucCurrentPos;
	}

	return LOGICAL_TRUE;
}

LOGICAL DecodeGauge(unsigned int &uiValue, const unsigned char *&pucCurrentPos)
{
	uiValue = 0;
	if (*pucCurrentPos != SNMP_FIELD_TYPE_COUNTER32 &&
		*pucCurrentPos != SNMP_FIELD_TYPE_GAUGE)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	// Length
	int nLen = *pucCurrentPos;
	++pucCurrentPos;
	// Value
	if (*pucCurrentPos & 0x80)
        uiValue = ~uiValue;         /* integer is negative */
	while (nLen--)
	{
		uiValue = (uiValue << 8) | *pucCurrentPos;
		++pucCurrentPos;
	}

	return LOGICAL_TRUE;
}

int EncodeString(const std::string &str, unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_TYPE_OCTET_STRING;
	++pucCurrentPos;
	// Length
	int nLen = str.length();
	*pucCurrentPos = (unsigned char)nLen;
	++pucCurrentPos;
	// Value
	for (int i = 0; i < nLen; i++)
	{
		*pucCurrentPos = str[i];
		++pucCurrentPos;
	}
	return 1+1+nLen;
}

int EncodeString(const char *pszValue, unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_TYPE_OCTET_STRING;
	++pucCurrentPos;
	// Length
	int nLen = strlen(pszValue);
	*pucCurrentPos = (unsigned char)nLen;
	++pucCurrentPos;
	// Value
	memcpy(pucCurrentPos, pszValue, nLen);
	pucCurrentPos += nLen;

	return 1+1+nLen;
}

LOGICAL DecodeString(std::string &str, const unsigned char *&pucCurrentPos)
{
	str = "";

	if (*pucCurrentPos != SNMP_FIELD_TYPE_OCTET_STRING)
		return LOGICAL_FALSE;

	++pucCurrentPos;
	// Length
	int nLen = *pucCurrentPos;
	++pucCurrentPos;
	// Value
	for (int i = 0; i < nLen; i++)
	{
		str += *pucCurrentPos;
		++pucCurrentPos;
	}

	return LOGICAL_TRUE;
}

LOGICAL DecodeString(char *pszValue, const unsigned char *&pucCurrentPos)
{
	*pszValue = 0;
	if (*pucCurrentPos != SNMP_FIELD_TYPE_OCTET_STRING)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	// Length
	int nLen = *pucCurrentPos;
	++pucCurrentPos;
	// Value
	memcpy(pszValue, pucCurrentPos, nLen);
	pszValue[nLen] = 0;
	pucCurrentPos += nLen;

	return LOGICAL_TRUE;
}

LOGICAL DecodeValue(cSnmpVariable &var, const unsigned char *&pucCurrentPos)
{
	var.m_DataType = *pucCurrentPos;
	// Value
	var.m_iIntegerValue = 0;
	var.m_strValue = "";
	switch (var.m_DataType)
	{
	case SNMP_FIELD_TYPE_IPADDRESS:
		break;
	case SNMP_FIELD_TYPE_GAUGE:
	case SNMP_FIELD_TYPE_COUNTER32:
		DecodeGauge(var.m_GaugeValue, pucCurrentPos);
		break;
	case SNMP_FIELD_TYPE_INTEGER:
		DecodeInteger(var.m_iIntegerValue, pucCurrentPos);
		break;
	case SNMP_FIELD_TYPE_TIME_TICKS:
		break;
	case SNMP_FIELD_TYPE_OPAQUE:
		break;
	case SNMP_FIELD_TYPE_COUNTER64:
		break;
	case SNMP_FIELD_TYPE_NULL:
		break;
	case SNMP_FIELD_TYPE_OCTET_STRING:
		DecodeString(var.m_strValue, pucCurrentPos);
		break;
	default:
		//ASSERT(0);
		break;
	}

	return LOGICAL_TRUE;
}

LOGICAL DecodeSnmpResponse(unsigned char *pDgm, cSnmpDatagram &responseDgm)
{
	unsigned char *pucCurrentPos = pDgm;
	int nOctetsRemaining = 0;
	// Seguence header
	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	// Version
	if (!DecodeInteger(responseDgm.m_version, pucCurrentPos))
		return LOGICAL_FALSE;
	// Community
	char szCommunity[128];
	if (!DecodeString(szCommunity, pucCurrentPos))
		return LOGICAL_FALSE;
	responseDgm.m_Community = szCommunity;
	// Response header
	if (!DecodeResponseField(pucCurrentPos, responseDgm.m_FieldType, nOctetsRemaining))
		return LOGICAL_FALSE;
	// Request ID
	if (!DecodeInteger(responseDgm.m_RequestID, pucCurrentPos))
		return LOGICAL_FALSE;
	// Status code
	if (!DecodeOctet(responseDgm.m_StatusCode, pucCurrentPos))
		return LOGICAL_FALSE;
	// Error code
	if (!DecodeOctet(responseDgm.m_ErrorCode, pucCurrentPos))
		return LOGICAL_FALSE;
	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	if (!DecodeOID(responseDgm.m_Variable.m_OID, pucCurrentPos))
		return LOGICAL_FALSE;
	if (!DecodeValue(responseDgm.m_Variable, pucCurrentPos))
		return LOGICAL_FALSE;
	
	return LOGICAL_TRUE;
}

LOGICAL DecodeSnmpRequest(unsigned char *pDgm, cSnmpDatagram &requestDgm)
{
	unsigned char *pucCurrentPos = pDgm;
	int nOctetsRemaining = 0;
	// Seguence header
	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	// Version
	if (!DecodeInteger(requestDgm.m_version, pucCurrentPos))
		return LOGICAL_FALSE;
	// Community
	if (!DecodeString(requestDgm.m_Community, pucCurrentPos))
		return LOGICAL_FALSE;
	// Response header
	if (!DecodeRequestField(pucCurrentPos, requestDgm.m_FieldType, nOctetsRemaining))
		return LOGICAL_FALSE;
	// Request ID
	if (!DecodeInteger(requestDgm.m_RequestID, pucCurrentPos))
		return LOGICAL_FALSE;
	// Status code
	if (!DecodeOctet(requestDgm.m_StatusCode, pucCurrentPos))
		return LOGICAL_FALSE;
	// Error code
	if (!DecodeOctet(requestDgm.m_ErrorCode, pucCurrentPos))
		return LOGICAL_FALSE;

	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	if (!DecodeOID(requestDgm.m_Variable.m_OID, pucCurrentPos))
		return LOGICAL_FALSE;
	if (!DecodeValue(requestDgm.m_Variable, pucCurrentPos))
		return LOGICAL_FALSE;
	
	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////
// class cOID

cSnmpOID::cSnmpOID()
{
	memset(m_uiOID, 0, sizeof(m_uiOID));
	m_nOIDlen = 0;
}

cSnmpOID::cSnmpOID(const unsigned int *pOID, int nOidLength)
{
	set(pOID, nOidLength);
}

void cSnmpOID::operator = (const cSnmpOID &OID)
{
	memcpy(m_uiOID, OID.m_uiOID, sizeof(m_uiOID));
	m_nOIDlen = OID.m_nOIDlen;
}

void cSnmpOID::set(const unsigned int *pOID, int nOidLength)
{
	for (int i = 0; i < nOidLength; i++)
	{
		if (i >= sizeof(m_uiOID)/sizeof(m_uiOID[0]))
			break;
		m_uiOID[i] = *(pOID + i);
	}
	m_nOIDlen = nOidLength;
}

void cSnmpOID::addDot1()
{
	m_uiOID[m_nOIDlen] = 1;
	++m_nOIDlen;
}

bool cSnmpOID::isTheSameOID(const unsigned int *puiOID, int nOIDlen) const
{
	if (nOIDlen != m_nOIDlen)
		return false;
	return isPartOfOID(puiOID, nOIDlen);
}

bool cSnmpOID::isPartOfOID(const unsigned int *puiOID, int nOIDlen) const
{
	int nLen = m_nOIDlen;
	if (nOIDlen < nLen)
		nLen = nOIDlen;
	for (int i = 0; i < nLen; i++)
	{
		if (m_uiOID[i] != puiOID[i])
			return false;
	}
	return true;
}

std::string cSnmpOID::getSymbolicName() const
{
	std::string str;
	char szBuffer[32];
	bool bEmpty = true;
	for (int i = 0; i < m_nOIDlen; i++)
	{
		if (!bEmpty)
			str += '.';
		bEmpty = false;
		sprintf(szBuffer, "%d", m_uiOID[i]);
		str += szBuffer;
	}
	return str;
}

////////////////////////////////////////////////////////////////////
// class cSnmpVariable

cSnmpVariable::cSnmpVariable()
{
}

void cSnmpVariable::operator = (const cSnmpVariable &var)
{
	m_OID = var.m_OID;
	m_DataType = var.m_DataType;
	m_iIntegerValue = var.m_iIntegerValue;
	m_GaugeValue = var.m_GaugeValue;
	m_strValue = var.m_strValue;
}

void cSnmpVariable::setOID(const unsigned int *pOID, int nOidLength)
{
	m_OID.set(pOID, nOidLength);
}

void cSnmpVariable::appendDot1ToOID()
{
	m_OID.addDot1();
}

////////////////////////////////////////////////////////////////////
// class cSnmpDatagram

cSnmpDatagram::cSnmpDatagram()
{
}

void cSnmpDatagram::operator = (const cSnmpDatagram &dgm)
{
	srcIpAddress_ = dgm.srcIpAddress_;
	m_FromIpPort = dgm.m_FromIpPort;
	m_version = dgm.m_version;
	m_FieldType = dgm.m_FieldType;
	m_Community = dgm.m_Community;
	m_RequestID = dgm.m_RequestID;
	m_StatusCode = dgm.m_StatusCode;
	m_ErrorCode = dgm.m_ErrorCode;
	m_Variable = dgm.m_Variable;
}

/////////////////////////////////////////////////////////////////////
// class CSnmpSocket

CSnmpSocket::CSnmpSocket()
{
}

//virtual
CSnmpSocket::~CSnmpSocket()
{
}

LOGICAL CSnmpSocket::waitForRequest(cSnmpDatagram &dgm)
{
	unsigned char szBuffer[0xFFF];
	unsigned int nBytesRead = 0;
	LOGICAL bOK = ReadFrom(szBuffer, sizeof(szBuffer), nBytesRead, dgm.srcIpAddress_, dgm.m_FromIpPort);
	if (!bOK)
		return LOGICAL_FALSE;
	/*for (int i = 0; i < nBytesRead; i++)
	{
		printf("i = %d, value = %d\n", i, szBuffer[i]);
	}*/
	return DecodeSnmpRequest(szBuffer, dgm);
}

LOGICAL CSnmpSocket::SendGetResponse(IPADDRESS_TYPE uIP, unsigned short uPort, int iVersion, const std::string &strCommunity, int ReqID, const cSnmpVariable &var)
{
	unsigned char datagram[1024];
	unsigned char *pucCurrentPos = datagram;

	// Sequence
	unsigned char ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucTotalLen = pucCurrentPos-1;
	// version
	*pucTotalLen += EncodeOctet(iVersion, pucCurrentPos);
	// Community
	*pucTotalLen += EncodeString(strCommunity, pucCurrentPos);
	// Request header
	*pucTotalLen += EncodeByte(SNMP_FIELD_GET_RESPONSE, pucCurrentPos);
	// Store pdu length
	unsigned char *pucPduLen = pucCurrentPos-1;
	// RequestID
	ucOctets = EncodeInteger(ReqID, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// Status code
	ucOctets = EncodeOctet(0, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// Error code
	ucOctets = EncodeOctet(0, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// oid
	ucOctets = EncodeSequence(pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	unsigned char *pucOidLen = pucCurrentPos-1;
	ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucOidLen2 = pucCurrentPos-1;
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets;
	ucOctets = EncodeOID(var.m_OID, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets; *pucOidLen2 += ucOctets;
	// value
	if (var.m_DataType == SNMP_FIELD_TYPE_INTEGER)
		ucOctets = EncodeInteger(var.m_iIntegerValue, pucCurrentPos);
	if (var.m_DataType == SNMP_FIELD_TYPE_GAUGE)
		ucOctets = EncodeGauge(var.m_GaugeValue, pucCurrentPos);
	else if (var.m_DataType == SNMP_FIELD_TYPE_OCTET_STRING)
	{
		ucOctets = EncodeString(var.m_strValue, pucCurrentPos);
	}

	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets; *pucOidLen2 += ucOctets;
	
	unsigned int nWrittenBytes = 0;
	return WriteTo(&datagram, *pucTotalLen + 2, nWrittenBytes, uIP, uPort);
}

LOGICAL CSnmpSocket::SendSnmpRequest(unsigned char reqCode, IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID)
{
	unsigned char datagram[1024];
	unsigned char *pucCurrentPos = datagram;

	// Sequence
	unsigned char ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucTotalLen = pucCurrentPos-1;
	*pucTotalLen = 0;

	// version
	*pucTotalLen += EncodeOctet(0, pucCurrentPos);
	// Community
	*pucTotalLen += EncodeString(strCommunity, pucCurrentPos);
	// Request header
	*pucTotalLen += EncodeByte(reqCode, pucCurrentPos);
	// Store pdu length
	unsigned char *pucPduLen = pucCurrentPos-1;
	*pucPduLen = 0;

	// RequestID
	ucOctets = EncodeInteger(ReqID, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets;
	// Status code
	ucOctets = EncodeOctet(0, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets;
	// Error code
	ucOctets = EncodeOctet(0, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets;
	// oid
	ucOctets = EncodeSequence(pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets;
	unsigned char *pucOidLen = pucCurrentPos-1;
	(*pucOidLen) = 0;

	ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucOidLen2 = pucCurrentPos-1;
	(*pucOidLen2) = 0;

	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets;
	ucOctets = EncodeOID(OID, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets; (*pucOidLen2) += ucOctets;
	
	// value type (null)
	ucOctets = EncodeNull(pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets; (*pucOidLen2) += ucOctets;
	
	unsigned int nWrittenBytes = 0;
	return WriteTo(&datagram, *pucTotalLen + 2, nWrittenBytes, uIP, SNMP_PORT);
}

LOGICAL CSnmpSocket::SendSnmpGetRequest(IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID)
{
	return SendSnmpRequest(SNMP_FIELD_GET_REQUEST, uIP, strCommunity, OID, ReqID);
}

LOGICAL CSnmpSocket::SendSnmpGetNextRequest(IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID)
{
	return SendSnmpRequest(SNMP_FIELD_GET_NEXT_REQUEST, uIP, strCommunity, OID, ReqID);
}

LOGICAL CSnmpSocket::SendSnmpSetIntegerRequest(IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID, int iValue)
{
	unsigned char datagram[1024];
	unsigned char *pucCurrentPos = datagram;

	// Sequence
	unsigned char ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucTotalLen = pucCurrentPos-1;
	// version
	*pucTotalLen += EncodeOctet(0, pucCurrentPos);
	// Community
	*pucTotalLen += EncodeString(strCommunity, pucCurrentPos);
	// Request header
	*pucTotalLen += EncodeByte(SNMP_FIELD_SET_REQUEST, pucCurrentPos);
	// Store pdu length
	unsigned char *pucPduLen = pucCurrentPos-1;
	// RequestID
	ucOctets = EncodeInteger(ReqID, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// Status code
	ucOctets = EncodeOctet(0, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// Error code
	ucOctets = EncodeOctet(0, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// oid
	ucOctets = EncodeSequence(pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	unsigned char *pucOidLen = pucCurrentPos-1;
	ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucOidLen2 = pucCurrentPos-1;
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets;
	ucOctets = EncodeOID(OID, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets; *pucOidLen2 += ucOctets;
	// Value
	ucOctets = EncodeInteger(iValue, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets; *pucOidLen2 += ucOctets;
	
	unsigned int nWrittenBytes = 0;
	return WriteTo(&datagram, *pucTotalLen + 2, nWrittenBytes, uIP, SNMP_PORT);
}

LOGICAL CSnmpSocket::SendSnmpSetUnsignedIntegerRequest(IPADDRESS_TYPE uIP, const std::string &strCommunity, const cSnmpOID &OID, int ReqID, unsigned int uiValue)
{
	unsigned char datagram[1024];
	unsigned char *pucCurrentPos = datagram;

	// Sequence
	unsigned char ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucTotalLen = pucCurrentPos-1;
	// version
	*pucTotalLen += EncodeOctet(0, pucCurrentPos);
	// Community
	*pucTotalLen += EncodeString(strCommunity, pucCurrentPos);
	// Request header
	*pucTotalLen += EncodeByte(SNMP_FIELD_SET_REQUEST, pucCurrentPos);
	// Store pdu length
	unsigned char *pucPduLen = pucCurrentPos-1;
	// RequestID
	ucOctets = EncodeInteger(ReqID, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// Status code
	ucOctets = EncodeOctet(0, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// Error code
	ucOctets = EncodeOctet(0, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets;
	// oid
	ucOctets = EncodeSequence(pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets;
	unsigned char *pucOidLen = pucCurrentPos-1;
	ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucOidLen2 = pucCurrentPos-1;
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets;
	ucOctets = EncodeOID(OID, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets; *pucOidLen2 += ucOctets;
	// Value
	ucOctets = EncodeGauge(uiValue, pucCurrentPos);
	*pucTotalLen += ucOctets; *pucPduLen += ucOctets; *pucOidLen += ucOctets; *pucOidLen2 += ucOctets;
	
	unsigned int nWrittenBytes = 0;
	return WriteTo(&datagram, *pucTotalLen + 2, nWrittenBytes, uIP, SNMP_PORT);
}

LOGICAL CSnmpSocket::GetSnmpReply(cSnmpDatagram &replyDgm)
{
	unsigned char datagram[512];
	unsigned int nReadBytes;
	unsigned short port;
	IPADDRESS_TYPE IP;
	LOGICAL bSuccess = ReadFrom(datagram, sizeof(datagram), nReadBytes, IP, port);
	if (!bSuccess)
		return LOGICAL_FALSE;
	return DecodeSnmpResponse(datagram, replyDgm);
}
