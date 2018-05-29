#include "modem-gct.h"
#include <string.h>

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
    if (!connection_.read(szReply, sizeof(szReply), 2000, nRead))
        return false;
    szReply[nRead] = 0;
    for (int i = 0; i < nRead; ++i)
    {
        char c = szReply[i];
        if (c == '"')
            continue; // skip all quotation marks
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

bool ModemGCT::getManufacturerInfo(JsonContent &content)
{
    std::vector<const char *> dictionary = { "Manufacturer", "Model", "Revision", "MEID", "IMEI", "IMEI", "FSN",  "+GCAP" };
    std::string raw;
    if (!getManufacturerInfoRaw(raw))
        return false;
    return parseToContent(raw, content, dictionary);
}

bool ModemGCT::getManufacturerInfo(std::string &json)
{
    JsonContent content;
    bool bSuccess = getManufacturerInfo(content);
    if (bSuccess)
        bSuccess = toJSON(content, json);
    if (!bSuccess)
        json = pszErrorJSON;
    return bSuccess;
}

bool ModemGCT::getBandRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at!band=?", raw);
#else
    return true;
#endif
}

bool ModemGCT::getBand(JsonContent &content)
{
    std::vector<const char *> dictionary = { "a" };
    std::string raw;
    if (!getBandRaw(raw))
        return false;
    return parseToContent(raw, content, dictionary);
}

bool ModemGCT::getBand(std::string &json)
{
    JsonContent content;
    bool bSuccess = getBand(content);
    if (bSuccess)
        bSuccess = toJSON(content, json);
    if (!bSuccess)
        json = pszErrorJSON;
    return bSuccess;
}

bool ModemGCT::getCopsRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cops?", raw);
#else    
    raw = "+COPS: 0,0,""Sprint"",7\n\
    \n\
    OK\n";
    return true;
#endif
}

bool ModemGCT::getCops(JsonContent &content)
{
    std::vector<const char *> dictionary = { "+COPS" };
    std::string raw;
    if (!getCopsRaw(raw))
        return false;
    return parseToContent(raw, content, dictionary);
}

bool ModemGCT::getCops(std::string &json)
{
    JsonContent content;
    bool bSuccess = getCops(content);
    if (bSuccess)
        bSuccess = toJSON(content, json);
    if (!bSuccess)
        json = pszErrorJSON;
    return bSuccess;
}

bool ModemGCT::getCgdContRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cgdcont?", raw);
#else
    raw ="\n\
    +CGDCONT: 1,""IPV4V6"",""r.ispsn"","""",0,0,0,0,0,0,0,0,0\n\
    +CGDCONT: 2,""IPV4V6"",""otasn"","""",0,0,0,0,0,0,0,0,0\n\
    +CGDCONT: 3,""IPV4V6"","","""",0,0,0,0,0,0,0,0,0\n\
    +CGDCONT: 4,""IP"","","",0,0,0,0,0,0,0,0,0\n\
    +CGDCONT: 5,""IP"","","",0,0,0,0,0,0,0,0,0\n\
    +CGDCONT: 6,""IP"","","",0,0,0,0,0,0,0,0,0\n\
    +CGDCONT: 7,""IP"","","",0,0,0,0,0,0,0,0,0\n\
    +CGDCONT: 8,""IP"","","",0,0,0,0,0,0,0,0,0\n\
    \n\
    OK\n";
    return true;
#endif
}

bool ModemGCT::getCgdCont(JsonContent &content)
{
    std::vector<const char *> dictionary = { "+CGDCONT" };
    std::string raw;
    if (!getCgdContRaw(raw))
        return false;
    return parseToContent(raw, content, dictionary);
}

bool ModemGCT::getCgdCont(std::string &json)
{
    JsonContent content;
    bool bSuccess = getCgdCont(content);
    if (bSuccess)
        bSuccess = toJSON(content, json);
    if (!bSuccess)
        json = pszErrorJSON;
    return bSuccess;
}

bool ModemGCT::getAttStatusRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cgatt?", raw);
#else
    raw = "\n\
           +CGATT:1\n\
            \n\
            OK\n\
            \n";
    return true;
#endif
}

bool ModemGCT::getAttStatus(JsonContent &content)
{
    std::vector<const char *> dictionary = { "+CGATT" };
    std::string raw;
    if (!getAttStatusRaw(raw))
        return false;
    return parseToContent(raw, content, dictionary);
}

bool ModemGCT::getAttStatus(std::string &json)
{
    JsonContent content;
    bool bSuccess = getAttStatus(content);
    if (bSuccess)
        bSuccess = toJSON(content, json);
    if (!bSuccess)
        json = pszErrorJSON;
    return bSuccess;
}

bool ModemGCT::getActStatusRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cgact?", raw);
#else
    raw = "\
at+cgact?\n\
\n\
+CGACT: 1,1\n\
+CGACT: 2,0\n\
+CGACT: 3,0\n\
+CGACT: 4,0\n\
+CGACT: 5,0\n\
+CGACT: 6,0\n\
+CGACT: 7,0\n\
+CGACT: 8,0\n\
\n\
OK\n";
    return true;
#endif
}

bool ModemGCT::getActStatus(JsonContent &content)
{
    std::vector<const char *> dictionary = { "+CGACT" };
    std::string raw;
    if (!getActStatusRaw(raw))
        return false;
    return parseToContent(raw, content, dictionary);
}

bool ModemGCT::getActStatus(std::string &json)
{
    JsonContent content;
    bool bSuccess = getActStatus(content);
    if (bSuccess)
        bSuccess = toJSON(content, json);
    if (!bSuccess)
        json = pszErrorJSON;
    return bSuccess;
}

bool ModemGCT::getCgpAddrRaw(std::string &raw)
{
#if REAL_MODEM
    return execute("at+cgpaddr", raw);
#else
    raw = "\n\
    +CGPADDR: 1,""173.139.108.55"",""38.0.0.1.154.112.2.25.0.0.0.68.248.43.239.1""\n\
    \n\
    OK\n";
    return true;
#endif
}

bool ModemGCT::getCgpAddr(JsonContent &content)
{
    std::vector<const char *> dictionary = { "+CGPADDR" };
    std::string raw;
    if (!getCgpAddrRaw(raw))
        return false;
    return parseToContent(raw, content, dictionary);
}

bool ModemGCT::getCgpAddr(std::string &json)
{
    JsonContent content;
    bool bSuccess = getCgpAddr(content);
    if (bSuccess)
        bSuccess = toJSON(content, json);
    if (!bSuccess)
        json = pszErrorJSON;
    return bSuccess;
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

bool ModemGCT::getStatus(JsonContent &content)
{
    std::vector<const char *> dictionary = { "Mode", "System mode", "PS state", "LTE band", "LTE bw", "LTE Rx chan",
                                             "LTE Tx chan", "EMM state", "RRC state", "IMS reg state", "RSSI (dBm)",
                                             "Tx Power", "RSRP (dBm)", "RSRP (dBm)", "TAC ", "RSRQ (dBm)", "RSRQ (dBm)", "SINR (dB)" };
    std::string raw;
    if (!getStatusRaw(raw))
        return false;
    return parseToContent(raw, content, dictionary);
}

bool ModemGCT::getStatus(std::string &json)
{
    JsonContent content;
    bool bSuccess = getCgpAddr(content);
    if (bSuccess)
        bSuccess = toJSON(content, json);
    if (!bSuccess)
        json = pszErrorJSON;
    return bSuccess;
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
