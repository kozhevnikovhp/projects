#include "modem-gct.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>

#define REAL_MODEM 0 // 1 means work with real modem, 0 means pre-memorized raw output form modem

const char *pszErrorJSON = "{\n\tERROR\n}\n";

ModemGCT::ModemGCT(const char *pszDevice)
{
    connection_.open(pszDevice);
    connection_.setSpeed(B115200);
}

bool ModemGCT::execute(const std::string &command, std::string &reply)
{
    reply.clear();

    if (!connection_.isOpen())
        return false;
    if (!connection_.write(command.c_str(), command.size()))
        return false;
    const char *pszCRLF = "\r\n";
    if (!connection_.write(pszCRLF, strlen(pszCRLF)))
        return false;

    int nRead = 0;
    char szReply[1024];
    if (!connection_.read(szReply, sizeof(szReply), 3000, nRead))
        return false;
    szReply[nRead] = 0;
    for (int i = 0; i < nRead; ++i)
    {
        char c = szReply[i];
        if (isalnum(c) || isspace(c) || ispunct(c) || c == 0x0A || c == 0x0D)
            reply += c;
    }
    return true;
}

bool ModemGCT::getManufacturerInfoRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("ati", raw);
#else
    raw = "Manufacturer: Sierra Wireless, Incorporated \n\
           Model: MC7455 \n\
           Revision: SWI9X30C_02.24.03.00 r6978 CARMD-EV-FRMWR2 2017/03/02 13:36:45 \n\
           MEID: 35907206098876 \n\
           IMEI: 359072060988769 \n\
           IMEI SV: 9 \n\
           FSN: LQ721403510410 \n\
           +GCAP: +CGSM \n\
           ";

    return true;
#endif
}

bool ModemGCT::getManufacturerInfoJSON(std::string &json)
{
    std::string raw;
    printf("1\n");
    bool bSuccess = getManufacturerInfoRaw(raw);
    printf("2\n");
    if (!bSuccess)
    {
        json = pszErrorJSON;
        return false;
    }
    printf("3\n");

    return parseToJSON(raw, json);
}

bool ModemGCT::getBandRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at!band=?", raw);
#else
    return true;
#endif
}

bool ModemGCT::getBandJSON(std::string &json)
{
    std::string raw;
    bool bSuccess = getBandRaw(raw);
    if (!bSuccess)
    {
        json = pszErrorJSON;
        return false;
    }

    return parseToJSON(raw, json);
}

bool ModemGCT::getCopsRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cops?", raw);
#else
    return true;
#endif
}

bool ModemGCT::getCopsJSON(std::string &json)
{
    std::string raw;
    bool bSuccess = getCopsRaw(raw);
    if (!bSuccess)
    {
        json = pszErrorJSON;
        return false;
    }

    return parseToJSON(raw, json);
}

bool ModemGCT::getCgdContRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cgdcont?", raw);
#else
    return true;
#endif
}

bool ModemGCT::getCgdContJSON(std::string &json)
{
    std::string raw;
    bool bSuccess = getCgdContRaw(raw);
    if (!bSuccess)
    {
        json = pszErrorJSON;
        return false;
    }

    return parseToJSON(raw, json);
}

bool ModemGCT::getAttStatusRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cgatt?", raw);
#else
    raw = "+CGATT:1\n\
            \n\
            OK\n\
            \n\
    ";
    return true;
#endif
}

bool ModemGCT::getAttStatusJSON(std::string &json)
{
    std::string raw;
    bool bSuccess = getAttStatusRaw(raw);
    if (!bSuccess)
    {
        json = pszErrorJSON;
        return false;
    }

    return parseToJSON(raw, json);
}

bool ModemGCT::getActStatusRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cgact?", raw);
#else
    raw = "+CGATT:1\n\
            \n\
            OK\n\
            \n\
    ";
    return true;
#endif
}

bool ModemGCT::getActStatusJSON(std::string &json)
{
    std::string raw;
    bool bSuccess = getActStatusRaw(raw);
    if (!bSuccess)
    {
        json = pszErrorJSON;
        return false;
    }

    return parseToJSON(raw, json);
}

bool ModemGCT::getCgpAddrRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cgpaddr", raw);
#else
    return true;
#endif
}

bool ModemGCT::getCgpAddrJSON(std::string &json)
{
    std::string raw;
    bool bSuccess = getCgpAddrRaw(raw);
    if (!bSuccess)
    {
        json = pszErrorJSON;
        return false;
    }

    return parseToJSON(raw, json);
}

bool ModemGCT::getStatusRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at!gstatus", raw);
#else
    raw = "!GSTATUS:                                                                       \n\
                                                                                    \n\
    Current Time : 0        Mode : ONLINE                                          \n\
    System mode : LTE       PS state : Attached                                      \n\
    LTE band : B25  LTE bw : 5MHz                                                    \n\
    LTE Rx chan : 8115      LTE Tx chan : 26115                                      \n\
    EMM state : 0   RRC state : RRC IDLE                                             \n\
    IMS reg state : No Srv                                                           \n\
    RSSI (dBm) : -78        Tx Power : -550                                          \n\
    RSRP (dBm) : -100       TAC : 84(3)                                              \n\
    RSRQ (dBm) : -8.0       Cell ID : 176233235                                      \n\
    SINR (dB) : 15                                                                   \n\
                                                                                     \n\
    OK   \n\
            ";
    return true;
#endif
}

bool ModemGCT::getStatusJSON(std::string &json)
{
    std::string raw;
    bool bSuccess = getStatusRaw(raw);
    if (!bSuccess)
    {
        json = pszErrorJSON;
        return false;
    }

    return parseToJSON(raw, json);
}

// trim from start (in place) (leading spaces)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !isspace(c); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !isspace(c); }).base(), s.end());
}

//static
bool ModemGCT::parseToJSON(const std::string &input, std::string &output)
{
    //printf("input: %s\n", input.c_str());
    std::string key, value;
    char *pszInput = strdup(input.c_str()); // TODO: do something better to avoid strdup/free
    const char *pszSeparator = ":\n\r\t";
    // replace two subsequent blanks with tab - new separator
    for (int i = 1; i < strlen(pszInput); ++i)
        if (pszInput[i-1] == ' ' && pszInput[i] == ' ')
            pszInput[i-1] = pszInput[i] = '\t';

    output = "{\n";
    char *pszToken = strtok(pszInput, pszSeparator);
    bool bFirst = true;
    while (pszToken)
    {
        //printf("token %s, length = %d\n", pszToken, strlen(pszToken));
        key = pszToken;
        ltrim(key);
        rtrim(key);
        if (!key.empty())
        {
            //printf("key = %s\n", key.c_str());
            pszToken = strtok(NULL, "\n\r\t");
            if (pszToken)
            {
                //printf("\ttoken %s, length = %d\n", pszToken, strlen(pszToken));
                value = pszToken;
                ltrim(value);
                rtrim(value);
                if (!value.empty())
                {
                    //printf("\tvalue = %s\n", value.c_str());
                    if (!bFirst)
                    {
                        output += ",";
                    }
                    output += "\n\t";
                    output += "\"";
                    output += key;
                    output += "\" : \"";
                    output += value;
                    output += "\"";
                    bFirst = false;
                }
            }
        }
        pszToken = strtok(NULL, pszSeparator);
    }
    output += "\n}\n";

    free(pszInput);
    return true;
}

bool getStatusGTC(const char *pszDevice, std::string &status)
{
    status.clear();
    SerialConnection connection;
    if (!connection.open(pszDevice))
        return false;

    if (!connection.setSpeed(B115200))
        return false;

    const char *pszCommand = "at!gstatus";
    if (!connection.write(pszCommand, strlen(pszCommand)))
        return false;
    const char *pszCRLF = "\r\n";
    if (!connection.write(pszCRLF, strlen(pszCRLF)))
        return false;

    int nRead = 0;
    char szReply[256];
    if (!connection.read(szReply, sizeof(szReply), 3000, nRead))
        return false;
    szReply[nRead] = 0;
    for (int i = 0; i < nRead; ++i)
    {
        char c = szReply[i];
        if (isalnum(c) || isspace(c) || ispunct(c) || c == 0x0A || c == 0x0D)
            status += c;
    }

    return true;
}
