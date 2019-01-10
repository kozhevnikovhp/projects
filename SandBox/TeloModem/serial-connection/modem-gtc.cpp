#include <string.h>
#include <regex>
#include <iostream>
#include <fstream>
#include "modem-gtc.h"

#define REAL_MODEM 1 // 1 means work with real modem, 0 means pre-memorized raw output form modem

ModemGTC::ModemGTC(const std::string &deviceName)
{
    connection_.open(deviceName.c_str());
    connection_.setSpeed(B115200);
}

bool ModemGTC::execute(const std::string &command)
{
    raw_.clear();

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
            raw_ += c;
    }
    return true;
}

bool ModemGTC::isControllable()
{
#if REAL_MODEM
    return execute("AT");
#else
    return true;
#endif
}


///////////////////////////////////////////////////////////////////////////////////
/// ManufacturerInfo version

bool ModemGTC::getManufacturerInfoRaw()
{
#if REAL_MODEM
    return execute("AT%SYSCMD=\"device --s\"");
#else
    raw_ = "\
    %SYSCMD: The device hardware info:\n\
    \n\
    %SYSCMD:    Mac address        : 6C:AD:EF:FE:97:45\n\
    \n\
    %SYSCMD:    Board version      : V1.0\n\
    \n\
    %SYSCMD:    Board serial       : -\n\
    \n\
    %SYSCMD:    Min frequency      : NA\n\
    \n\
    %SYSCMD:    Max frequency      : NA\n\
    \n\
    %SYSCMD:    Flash size         : NA\n\
    \n\
    %SYSCMD:    Board manufacturer : Global\n\
    \n\
    %SYSCMD:    Board name         : GDM7243S-MOD-U-B25_26_41\n\
    \n\
    %SYSCMD: \n\
    \n\
    %SYSCMD: The device software info:\n\
    \n\
    %SYSCMD:    Long name          : Global Telecom Corp\n\
    \n\
    %SYSCMD:    Short name         : Global\n\
    \n\
    %SYSCMD:    Abbreviated name   : Global\n\
    \n\
    %SYSCMD:    Logo file          : \n\
    \n\
    %SYSCMD:    Device type        : Global-7243S\n\
    \n\
    %SYSCMD:    Image type         : 0001\n\
    \n\
    %SYSCMD:    Soft version       : V1.0.0 PACK 0 (Ver.1379) Build on: May 31 2018\n\
    \n\
    OK\n";


    return true;
#endif
}

bool ModemGTC::getManufacturerInfo(JsonContent &content)
{
    const std::string garbage = "%SYSCMD: ";
    //printf("GetManufacturerInfo\n");
    Dictionary dictionary = { DictionaryEntry("Board version", "board_version"),
                              DictionaryEntry("Board serial", "board_sn"),
                              DictionaryEntry("Board name", "board_name"),
                              DictionaryEntry("Device type", "device_type"),
                              DictionaryEntry("Soft version", "sw_version")
                            };
    if (!getManufacturerInfoRaw())
        return false;
    // remove garbage (sub-string %SYSCMD: iappears in every line)
    std::string::size_type i = raw_.find(garbage);
    while (i != std::string::npos)
    {
      raw_.erase(i, garbage.length());
      i = raw_.find(garbage, i);
    }
    return parseToContent(raw_, content, dictionary);
}

///////////////////////////////////////////////////////////////////////////////////
/// Firmware version

bool ModemGTC::getFirmwareVersionRaw()
{
#if REAL_MODEM
    return execute("AT+CGMR");
#else
    raw_ = "FW_VER: 0.3.2.4\nOK\n";
    return true;
#endif
}

bool ModemGTC::getFirmwareVersionInfo(JsonContent &content)
{
    Dictionary dictionary = { DictionaryEntry("FW_VER", "fw_version") };
    if (!getFirmwareVersionRaw())
        return false;
    return parseToContent(raw_, content, dictionary);
}

///////////////////////////////////////////////////////////////////////////////////
/// IMEI

bool ModemGTC::getImeiRaw()
{
#if REAL_MODEM
    return execute("AT%GIMEISV?");
#else
    raw_ = "%GIMEISV: 8623430390034200\nOK\n";
    return true;
#endif
}

bool ModemGTC::getImei(JsonContent &content)
{
    Dictionary dictionary = { DictionaryEntry("%GIMEISV", "imei") };
    if (!getImeiRaw())
        return false;
    return parseToContent(raw_, content, dictionary);
}

///////////////////////////////////////////////////////////////////////////////////
/// ICCID

bool ModemGTC::getIccIdRaw()
{
#if REAL_MODEM
    return execute("AT%GICCID");
#else
    raw_ = "%GICCID: 89011202000218997994\nOK\n";
    return true;
#endif
}

bool ModemGTC::getIccId(JsonContent &content)
{
    Dictionary dictionary = { DictionaryEntry("%GICCID", "iccid") };
    if (!getIccIdRaw())
        return false;
    return parseToContent(raw_, content, dictionary);
}

///////////////////////////////////////////////////////////////////////////////////
/// Carrier

bool ModemGTC::getCarrierRaw()
{
#if REAL_MODEM
    return execute("AT%GSERNETWORK");
#else
    raw_ = "%GSERNETWORK: RegistrationState 1, CSDomain 0, PSDomain 1, Roaming 0, MCC 310, MNC 120, namesize 6, name Sprint, VOLTE 1\nOK\n";
    return true;
#endif
}

bool ModemGTC::getCarrier(JsonContent &content)
{
    const char *PSZ_CARRIER = "carrier";
    Dictionary dictionary = { DictionaryEntry("%GSERNETWORK", PSZ_CARRIER) };
    if (!getCarrierRaw())
        return false;

    tmpContent_.clear();
    if (!parseToContent(raw_, tmpContent_, dictionary))
        return false;
    std::string strValue;
    char szKey[32], szValue[32];
    for (auto &entry : tmpContent_)
    {
        if (!entry.first.compare(PSZ_CARRIER))
        {
            strValue = entry.second;
            // look for substring kind of name Sprint
            char *pszToken = strtok((char *)strValue.c_str(), ",");
            while (pszToken)
            {
                if (sscanf(pszToken, "%s %s", szKey, szValue) == 2)
                {
                    if (strcasecmp(szKey, "name") == 0)
                    {
                        content.emplace_back(KeyValue(PSZ_CARRIER, szValue));
                    }
                }
                pszToken = strtok(NULL, ",");
            }
        }
    }
    return true;
}

bool ModemGTC::getStatusRaw()
{
#if REAL_MODEM
    return execute("AT!GSTATUS");
#else
    raw = "\                                                                         \n\
    Current Time : 0        Mode : ONLINE                                            \n\
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

bool ModemGTC::getStatus(JsonContent &content)
{
    const char *pszMode = "mode";
    const char *pszPSState = "ps_state";
    const char *pszRSSI = "rssi";
    const char *pszSINR = "sinr";

    Dictionary dictionary = { DictionaryEntry("Mode", pszMode),
                              DictionaryEntry("PS state", pszPSState),
                              DictionaryEntry("RSSI (dBm)", pszRSSI),
                              DictionaryEntry("SINR (dB)", pszSINR)
                            };
    if (!getStatusRaw())
        return false;
    JsonContent modemValues;
    if (!parseToContent(raw_, modemValues, dictionary))
        return false;
    std::string mode, PSState, SINR;
    for (auto &entry : modemValues)
    {
        //printf("%s:%s\n", entry.first.c_str(), entry.second.c_str());
        if (!entry.first.compare(pszMode))
        {
            mode = entry.second;
            content.emplace_back(KeyValue(pszMode, mode));
            std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
        }
        else if (!entry.first.compare(pszPSState))
        {
            PSState = entry.second;
            content.emplace_back(KeyValue(pszPSState, PSState));
            std::transform(PSState.begin(), PSState.end(), PSState.begin(), ::tolower);
        }
        else if (!entry.first.compare(pszRSSI))
            content.emplace_back(KeyValue(pszRSSI, entry.second));
        else if (!entry.first.compare(pszSINR))
        {
            SINR = entry.second;
            content.emplace_back(KeyValue(pszSINR, SINR));
        }
    }
    std::string signalQuality = "0";
    double SN = atof(SINR.c_str());
    if (SN >= 20)
        signalQuality = "4";
    else if (SN >= 15)
        signalQuality = "3";
    else if (SN >= 10)
        signalQuality = "2";
    else if (SN >= 5)
        signalQuality = "1";
    content.emplace_back(KeyValue("signal_quality", signalQuality));

    std::string status = "down";
    if (mode.compare("online") == 0 && PSState.compare("attached") == 0)
        status = "up";
    content.emplace_back(KeyValue("lte_status", status));
    return true;
}

bool ModemGTC::getSPN(std::string &SPN)
{
    SPN.clear();
    std::string line;
#if REAL_MODEM
    std::ifstream mdmFile("/etc/ooma/mdm.cfg");
#else
    std::ifstream mdmFile("mdm.cfg");
#endif
    if (mdmFile.is_open())
    {
        while (!mdmFile.eof())
        {
            std::getline(mdmFile, line);
            const char *pszKey = strtok((char *)line.c_str(), "=");
            if (pszKey && !strcmp(pszKey, "MY_SPN"))
            {
                char *pszValue = strtok(NULL, "=");
                if (pszValue)
                    SPN = std::string(pszValue);
                break;
            }
        }
        mdmFile.close();
    }
    else
        printf("Cannot open file\n");
    return true;
}


