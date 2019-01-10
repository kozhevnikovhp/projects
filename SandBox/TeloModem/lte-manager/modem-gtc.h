/*
 *  modem-gtc.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>
#ifndef PSEUDO_MODEM
#include "serial-connection.h"
#endif
#include "json.h"

void convertKeyValuePairTo(JsonContent &content, std::string &json);

class ModemGTC
{
public:
    ModemGTC(const std::string &deviceName);

    bool connect();
    bool disconnect();
    bool isConnected() const;

    bool isControllable();

    bool getManufacturerInfoRaw();
    bool getManufacturerInfo(JsonContent &content);

    bool getFirmwareVersionRaw();
    bool getFirmwareVersionInfo(JsonContent &content);

    bool getImeiRaw();
    bool getImei(JsonContent &content);

    bool getIccIdRaw();
    bool getIccId(JsonContent &content);

    bool getCarrierRaw();
    bool getCarrier(JsonContent &content);

    bool getStatusRaw();
    bool getStatus(JsonContent &content);

    bool getSPN(std::string &SPN);

    bool firmwareUpgrade(const std::string &fileName);

protected:

#ifndef PSEUDO_MODEM
    bool execute(const std::string &command, int timeout);

    SerialConnection connection_;
#endif

    std::string deviceName_;
    std::string raw_; // to avoid memory fragmentation
    JsonContent tmpContent_; // to avoid memory fragmentation
    int nCannotConnectReported_;
};

