/* SNMP_stuff.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	17 Sep 2008 - initial creation

*/

#include "stdafx.h"
#include "SNMP_stuff.h"


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

int EncodeGetRequest(unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_GET_REQUEST;
	++pucCurrentPos;
	// Length
	*pucCurrentPos = 0x00; // unknown yet
	++pucCurrentPos;

	return 2; // 2 octets required
}

int EncodeSetRequest(unsigned char *&pucCurrentPos)
{
	// Field type
	*pucCurrentPos = SNMP_FIELD_SET_REQUEST;
	++pucCurrentPos;
	// Length
	*pucCurrentPos = 0x00; // unknown yet
	++pucCurrentPos;

	return 2; // 2 octets required
}

LOGICAL DecodeResponse(unsigned char *&pucCurrentPos, int &nOctets)
{
	nOctets = 0;
	if (*pucCurrentPos != SNMP_FIELD_GET_RESPONSE)
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
		(*pucCurrentPos) = uiValue;
	}
	
	if (bMultyByteSign)
		(*pucCurrentPos) |= 0x80;
	++pucCurrentPos;
	++nOctets;
	(*pucOidLen)++;
}

int EncodeOID(unsigned int *puiOID, int nOidLen, unsigned char *&pucCurrentPos)
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
	(*pucCurrentPos) = puiOID[0]*40 + puiOID[1];
	++pucCurrentPos;
	++nOctets;
	(*pucOidLen)++;
	for (int i = 2; i < nOidLen; i++)
	{
		unsigned int iValue = puiOID[i];
		OidIntToByte(iValue, pucCurrentPos, nOctets, pucOidLen, LOGICAL_FALSE);
	}
	return nOctets;
}

LOGICAL DecodeOID(unsigned int *puiOID, int &nOidLen, const unsigned char *&pucCurrentPos)
{
	nOidLen = 0;
	if (*pucCurrentPos != SNMP_FIELD_TYPE_OID)
		return LOGICAL_FALSE;
	++pucCurrentPos;
	// Length
	int nOidOctets = *pucCurrentPos;
	++pucCurrentPos;
	// packed two first octets
	unsigned char subidentifier = *pucCurrentPos;
	puiOID[0] = subidentifier/40;
	puiOID[1] = subidentifier - puiOID[0]*40;
	pucCurrentPos += 1;
	nOidLen = 2;
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

		puiOID[nOidLen] = uiOid;
		++nOidLen;
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

int EncodeUnsignedInteger(const unsigned int uiValue, unsigned char *&pucCurrentPos)
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

LOGICAL DecodeUnsignedInteger(unsigned int &uiValue, const unsigned char *&pucCurrentPos)
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

LOGICAL DecodeValue(SSnmpReply *pReply, const unsigned char *&pucCurrentPos)
{
	pReply->m_type = *pucCurrentPos;
	// Value
	pReply->m_iIntegerValue = 0;
	pReply->m_OctetStringValue[0] = 0;
	switch (pReply->m_type)
	{
	case SNMP_FIELD_TYPE_IPADDRESS:
		break;
	case SNMP_FIELD_TYPE_GAUGE:
	case SNMP_FIELD_TYPE_COUNTER32:
		DecodeUnsignedInteger(pReply->m_uiIntegerValue, pucCurrentPos);
		break;
	case SNMP_FIELD_TYPE_INTEGER:
		DecodeInteger(pReply->m_iIntegerValue, pucCurrentPos);
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
		DecodeString(pReply->m_OctetStringValue, pucCurrentPos);
		break;
	default:
		ASSERT(0);
	}

	return LOGICAL_TRUE;
}

LOGICAL DecodeSnmpDgm(unsigned char *pDgm, SSnmpReply *pReply)
{
	unsigned char *pucCurrentPos = pDgm;
	int nOctetsRemaining = 0;
	// Seguence header
	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	// Version
	if (!DecodeInteger(pReply->m_version, pucCurrentPos))
		return LOGICAL_FALSE;
	// Community
	char szCommunity[128];
	if (!DecodeString(szCommunity, pucCurrentPos))
		return LOGICAL_FALSE;
	// Response header
	if (!DecodeResponse(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	// Request ID
	if (!DecodeInteger(pReply->m_RequestID, pucCurrentPos))
		return LOGICAL_FALSE;
	// Status code
	if (!DecodeOctet(pReply->m_StatusCode, pucCurrentPos))
		return LOGICAL_FALSE;
	// Error code
	if (!DecodeOctet(pReply->m_ErrorCode, pucCurrentPos))
		return LOGICAL_FALSE;
	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	if (!DecodeSequence(pucCurrentPos, nOctetsRemaining))
		return LOGICAL_FALSE;
	if (!DecodeOID(pReply->m_uiOID, pReply->m_nOIDlen, pucCurrentPos))
		return LOGICAL_FALSE;
	if (!DecodeValue(pReply, pucCurrentPos))
		return LOGICAL_FALSE;
	
	return LOGICAL_TRUE;
}

LOGICAL SSnmpReply::IsTheSameOID(const unsigned int *puiOID, int nOIDlen)
{
	if (nOIDlen != m_nOIDlen)
		return LOGICAL_FALSE;
	for (int i = 0; i < m_nOIDlen; i++)
	{
		if (m_uiOID[i] != puiOID[i])
			return LOGICAL_FALSE;
	}
	return LOGICAL_TRUE;
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

LOGICAL CSnmpSocket::SendSnmpGetRequest(IPADDRESS_TYPE uIP, const char *pszCommunity, unsigned int *puiOID, int nOidLen, int ReqID)
{
	unsigned char datagram[1024];
	unsigned char *pucCurrentPos = datagram;

	// Sequence
	unsigned char ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucTotalLen = pucCurrentPos-1;
	// version
	(*pucTotalLen) += EncodeOctet(0, pucCurrentPos);
	// Community
	(*pucTotalLen) += EncodeString(pszCommunity, pucCurrentPos);
	// Request header
	(*pucTotalLen) += EncodeGetRequest(pucCurrentPos);
	// Store pdu length
	unsigned char *pucPduLen = pucCurrentPos-1;
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
	ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucOidLen2 = pucCurrentPos-1;
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets;
	ucOctets = EncodeOID(puiOID, nOidLen, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets; (*pucOidLen2) += ucOctets;
	// null
	ucOctets = EncodeNull(pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets; (*pucOidLen2) += ucOctets;
	
	unsigned int nWrittenBytes = 0;
	return WriteTo(&datagram, *pucTotalLen + 2, nWrittenBytes, uIP, SNMP_PORT);
}

LOGICAL CSnmpSocket::SendSnmpSetIntegerRequest(IPADDRESS_TYPE uIP, const char *pszCommunity, unsigned int *puiOID, int nOidLen, int ReqID, int iValue)
{
	unsigned char datagram[1024];
	unsigned char *pucCurrentPos = datagram;

	// Sequence
	unsigned char ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucTotalLen = pucCurrentPos-1;
	// version
	(*pucTotalLen) += EncodeOctet(0, pucCurrentPos);
	// Community
	(*pucTotalLen) += EncodeString(pszCommunity, pucCurrentPos);
	// Request header
	(*pucTotalLen) += EncodeSetRequest(pucCurrentPos);
	// Store pdu length
	unsigned char *pucPduLen = pucCurrentPos-1;
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
	ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucOidLen2 = pucCurrentPos-1;
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets;
	ucOctets = EncodeOID(puiOID, nOidLen, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets; (*pucOidLen2) += ucOctets;
	// Value
	ucOctets = EncodeInteger(iValue, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets; (*pucOidLen2) += ucOctets;
	
	unsigned int nWrittenBytes = 0;
	return WriteTo(&datagram, *pucTotalLen + 2, nWrittenBytes, uIP, SNMP_PORT);
}

LOGICAL CSnmpSocket::SendSnmpSetUnsignedIntegerRequest(IPADDRESS_TYPE uIP, const char *pszCommunity, unsigned int *puiOID, int nOidLen, int ReqID, unsigned int uiValue)
{
	unsigned char datagram[1024];
	unsigned char *pucCurrentPos = datagram;

	// Sequence
	unsigned char ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucTotalLen = pucCurrentPos-1;
	// version
	(*pucTotalLen) += EncodeOctet(0, pucCurrentPos);
	// Community
	(*pucTotalLen) += EncodeString(pszCommunity, pucCurrentPos);
	// Request header
	(*pucTotalLen) += EncodeSetRequest(pucCurrentPos);
	// Store pdu length
	unsigned char *pucPduLen = pucCurrentPos-1;
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
	ucOctets = EncodeSequence(pucCurrentPos);
	unsigned char *pucOidLen2 = pucCurrentPos-1;
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets;
	ucOctets = EncodeOID(puiOID, nOidLen, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets; (*pucOidLen2) += ucOctets;
	// Value
	ucOctets = EncodeUnsignedInteger(uiValue, pucCurrentPos);
	(*pucTotalLen) += ucOctets; (*pucPduLen) += ucOctets; (*pucOidLen) += ucOctets; (*pucOidLen2) += ucOctets;
	
	unsigned int nWrittenBytes = 0;
	return WriteTo(&datagram, *pucTotalLen + 2, nWrittenBytes, uIP, SNMP_PORT);
}

LOGICAL CSnmpSocket::GetSnmpReply(SSnmpReply *pReply)
{
	unsigned char datagram[512];
	unsigned int nReadBytes;
	unsigned short port;
	IPADDRESS_TYPE IP;
	LOGICAL bSuccess = ReadFrom(datagram, sizeof(datagram), nReadBytes, IP, port);
	if (!bSuccess)
		return LOGICAL_FALSE;
	return DecodeSnmpDgm(datagram, pReply);
}
