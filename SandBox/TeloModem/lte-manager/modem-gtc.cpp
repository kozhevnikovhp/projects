/*
 *  modem-gtc.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <regex>
#include <iostream>
#include <fstream>
#include "modem-gtc.h"
#include "log.h"
#include "misc.h"

ModemGTC::ModemGTC(const std::string &deviceName)
    : deviceName_(deviceName)
{
    nCannotConnectReported_ = 0;
}

bool ModemGTC::connect()
{
#ifndef PSEUDO_MODEM
    if (!connection_.open(deviceName_.c_str(), true))
    {
        if (nCannotConnectReported_ < 5)
            log_error("Could not connect to device %s\n", deviceName_.c_str());
        ++nCannotConnectReported_;
        return false;
    }
    log_info("Connected to the device %s", deviceName_.c_str());
    connection_.setSpeed(B115200);
    nCannotConnectReported_ = 0; // clear counter, can complain again
    return true;
#else
    return true;
#endif
}

bool ModemGTC::disconnect()
{
#ifndef PSEUDO_MODEM
    connection_.close();
    return true;
#else
    return true;
#endif
}

bool ModemGTC::isConnected() const
{
#ifndef PSEUDO_MODEM
    return connection_.isOpen();
#else
    return true;
#endif
}

#ifndef PSEUDO_MODEM
bool ModemGTC::execute(const std::string &command, int timeout)
{
    raw_.clear();

    if (!connection_.isOpen())
        return false;
    if (!connection_.write(command.c_str(), command.size()))
        return false;
    const char *pszCRLF = "\r\n";
    if (!connection_.write(pszCRLF, strlen(pszCRLF)))
        return false;

    size_t nRead = 0;
    char szReply[1024];
    if (!connection_.read(szReply, sizeof(szReply), timeout, nRead))
        return false;
    szReply[nRead] = 0;
    for (size_t i = 0; i < nRead; ++i)
    {
        char c = szReply[i];
        if (c == '"')
            continue; // skip all quotation marks
        if (isalnum(c) || isspace(c) || ispunct(c) || c == 0x0A || c == 0x0D)
            raw_ += c;
    }
    //printf("%s\n", raw_.c_str());
    bool bSuccess = (raw_.find("ERROR") == std::string::npos);
    return bSuccess;
}

#endif

bool ModemGTC::isControllable()
{
#ifndef PSEUDO_MODEM
    return execute("AT", 3000);
#else
    return true;
#endif
}


///////////////////////////////////////////////////////////////////////////////////
/// ManufacturerInfo version

bool ModemGTC::getManufacturerInfoRaw()
{
#ifndef PSEUDO_MODEM
    return execute("AT%SYSCMD=\"device --s\"", 5000);
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
    Dictionary dictionary = { DictionaryEntry("Board version", "board_version"),
                              DictionaryEntry("Board serial", "board_sn"),
                              DictionaryEntry("Board name", "board_name"),
                              DictionaryEntry("Device type", "device_type"),
                              DictionaryEntry("Soft version", "sw_version")
                            };
    if (!getManufacturerInfoRaw())
        return false;

    // remove garbage (sub-string %SYSCMD: appears in every line)
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
#ifndef PSEUDO_MODEM
    return execute("AT+CGMR", 3000);
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
#ifndef PSEUDO_MODEM
    return execute("AT%GIMEISV?", 3000);
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
#ifndef PSEUDO_MODEM
    return execute("AT%GICCID", 3000);
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
#ifndef PSEUDO_MODEM
    return execute("AT%GSERNETWORK", 3000);
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
    for (auto &entry : tmpContent_)
    {
        if (!entry.first.compare(PSZ_CARRIER))
        {
            std::string strValue = entry.second;
            std::string key_value, key, value;
            std::stringstream ss(strValue);
            while (!ss.eof() && !ss.bad() && !ss.fail())
            {
                std::getline(ss, key_value, ',');
                if (key_value.empty())
                    continue;
                trimBlanks(key_value);
                std::stringstream sss(key_value);
                std::getline(sss, key, ' ');
                trimBlanks(key);
                tolower(key);
                if (key.compare("name") != 0)
                    continue;
                std::getline(sss, value, ' ');
                trimBlanks(value);
                content.emplace_back(KeyValue(PSZ_CARRIER, value));
            }
        }
    }
    return true;
}

bool ModemGTC::getStatusRaw()
{
#ifndef PSEUDO_MODEM
    return execute("AT!GSTATUS", 3000);
#else
    raw_ = "                                                                         \n\
    Current Time : 0        Mode : ONLINE                                            \n\
    System mode : LTE       PS state : Attached                                      \n\
    LTE band : B25  LTE bw : 5MHz                                                    \n\
    LTE Rx chan : 8115      LTE Tx chan : 26115                                      \n\
    EMM state : 0   RRC state : RRC IDLE                                             \n\
    IMS reg state : No Srv                                                           \n\
    RSSI (dBm) : -78        Tx Power : -550                                          \n\
    RSRP (dBm) : -99        TAC : 84(3)                                              \n\
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
    const char *pszRSRP = "rsrp";
    const char *pszRSSI = "rssi";
    const char *pszSINR = "sinr";

    Dictionary dictionary = { DictionaryEntry("Mode", pszMode),
                              DictionaryEntry("PS state", pszPSState),
                              DictionaryEntry("RSRP (dBm)", pszRSRP),
                              DictionaryEntry("RSSI (dBm)", pszRSSI),
                              DictionaryEntry("SINR (dB)", pszSINR)
                            };
    if (!getStatusRaw())
        return false;
    //printf("Raw = %s\n", raw_.c_str());
    JsonContent modemValues;    
    if (!parseToContent(raw_, modemValues, dictionary))
        return false;

    std::string strMode, strPSState, strSINR, strRSRP;
    for (auto &entry : modemValues)
    {
        //printf("%s:%s\n", entry.first.c_str(), entry.second.c_str());
        if (!entry.first.compare(pszMode))
        {
            strMode = entry.second;
            tolower(strMode);
            content.emplace_back(KeyValue(pszMode, strMode));
        }
        else if (!entry.first.compare(pszPSState))
        {
            strPSState = entry.second;
            tolower(strPSState);
            content.emplace_back(KeyValue(pszPSState, strPSState));
        }
        else if (!entry.first.compare(pszRSRP))
        {
            strRSRP = entry.second;
            content.emplace_back(KeyValue(pszRSRP, entry.second));
        }
        else if (!entry.first.compare(pszRSSI))
            content.emplace_back(KeyValue(pszRSSI, entry.second));
        else if (!entry.first.compare(pszSINR))
        {
            strSINR = entry.second;
            content.emplace_back(KeyValue(pszSINR, strSINR));
        }
    }

    // LTEstate
    // if "PS state" is not "attached", and "mode" is not "online", it does not matter how strong the signal/noise ratio is, signal quality is "BAD" (==0)
    std::string LTEstate;
    if (strMode.compare("online") != 0 || strPSState.compare("attached") != 0)
        LTEstate = "down";
    else
        LTEstate = "up";

    // signal quality
    std::string signalQuality;
#if 1
    // Calculations based on RSRP value according to RamaK (http://devcon.corp.ooma.com/display/~rama.kamarajugadda/Signal+Strength+Vs+Bars)
    // -50 to -79 dBm, then it's generally considered great signal  (4 to 5 bars)
    // -80 to -89 dBm, then it's generally considered good signal (3 to 4 bars)
    // -90 to -99 dBm, then it's generally considered average signal (2 to 3 bars)
    // -100 to -109 dBm, then it's generally considered poor signal (1 to 2 bars)
    // -110 to -120 dBm, then it's generally considered very poor signal (0 to 1 bar)
    if (strRSRP.empty())
        signalQuality = "0";
    else
    {
        int RSRP = atoi(strRSRP.c_str());
        if (RSRP == 0)
        {
            signalQuality = "0";
            log_info("Invalid value for RSRP (%s)", strRSRP.c_str());
        }
        else if (RSRP <= -110)
            signalQuality = "0";
        else if (RSRP <= -100)
            signalQuality = "1";
        else if (RSRP <= -90)
            signalQuality = "2";
        else if (RSRP <= -80)
            signalQuality = "3";
        else
            signalQuality = "4";
    }
#else
    // calculations based on signal/noise ratio
    if (strSINR.empty())
        signalQuality = "0";
    else
    {
        int SN = atoi(strSINR.c_str());
        if (SN < 5)
            signalQuality = "0";
        else if (SN < 10)
            signalQuality = "1";
        else if (SN < 15)
            signalQuality = "2";
        else if (SN < 20)
            signalQuality = "3";
        else
            signalQuality = "4";
    }
#endif

    content.emplace_back(KeyValue("signal_quality", signalQuality));
    content.emplace_back(KeyValue("lte_state", LTEstate));

    return true;
}

bool ModemGTC::getSPN(std::string &SPN)
{
    SPN.clear();
    std::string line;
    const char *PSZ_MDM_CFG_FILE = "/etc/ooma/mdm.cfg";
    std::ifstream file(PSZ_MDM_CFG_FILE);
    if (file.is_open())
    {
        while (true)
        {
            std::getline(file, line);
            if (file.eof() || file.bad() || file.fail())
                break;
            const char *pszKey = strtok((char *)line.c_str(), "=");
            if (pszKey && !strcmp(pszKey, "MY_SPN"))
            {
                char *pszValue = strtok(nullptr, "=");
                if (pszValue)
                {
                    //SPN = std::string(pszValue);
                    // \r\n issue with local mdm.cfg file (Windows?), anyway, copy only digits and letters one by one
                    while (*pszValue != 0 && isalnum(*pszValue))
                    {
                        SPN += *pszValue;
                        ++pszValue;
                    }
                }
                break;
            }
        }
        file.close();
    }
    else
        log_info("Cannot open file %s", PSZ_MDM_CFG_FILE);

    return true;
}

// Described here: http://devcon.corp.ooma.com/pages/viewpage.action?pageId=21728633
bool ModemGTC::firmwareUpgrade(const std::string &fileName)
{
    log_info("Upgrade dongle's firmware with file %s", fileName.c_str());
    // file is assumed to be uploaded to modem's internal memory (via TFTProtocol)
#ifndef PSEUDO_MODEM
    log_info("Sleeping 5 secs");
    sleep(5);

    bool bOK = true;
    std::string cmd = "at%syscmd=";
    cmd += '"';
    cmd += "superload -l /var/update/";
    cmd += fileName;
    cmd += '"';
    log_info("AT-command: %s\n", cmd.c_str());
    bOK = execute(cmd, 30*1000);    // Takes around 25 seconds to upgrade according to RamaK.
    if (!bOK)
    {
        log_error("Could not upgrade dongle's firmware");
        return false;
    }
    log_info("Dongle's firmware has been successfully upgraded");

    log_info("Sleeping 5 secs");
    sleep(5);

    // reboot dongle
    log_info("Rebooting dongle");

    // set "reset factory defaults" flag
    // execute AT-command at%syscmd="ucfg add config ui restore_default 1"
    cmd = "at%syscmd=";
    cmd += '"';
    cmd += "ucfg add config ui restore_default 1";
    cmd += '"';
    log_info("AT-command: %s\n", cmd.c_str());
    bOK = execute(cmd, 3000);

    log_info("Sleeping 5 secs");
    sleep(5);

    // reboot now
    // execute AT-command at%syscmd="reboot -f"
    cmd = "at%syscmd=";
    cmd += '"';
    cmd += "reboot -f";
    cmd += '"';
    log_info("AT-command: %s\n", cmd.c_str());
    bOK = execute(cmd, 1000);
    return bOK;
#else
    return true;
#endif
 }
