/*
 *  lte-parameters.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include "modem-gtc.h"
#include "traffic-counter.h"
#include "json.h"


class LteParameterGroup
{
public:
    typedef enum {
        REPORT_NOTHING,
        REPORT_CHANGED_ONLY,
        REPORT_EVERYTHING
    } ReportAction;

public:
    LteParameterGroup();
    virtual ~LteParameterGroup()
    {
    }

public:
    bool get(unsigned int basicDelay, JsonContent &allReport);

protected:
    virtual const char *getName() = 0;
    virtual unsigned int getMinExpirationTime() const = 0; // factor, multiplying basic query delay
    virtual unsigned int getMaxExpirationTime() const = 0; // factor, multiplying basic query delay
    virtual bool doGet(JsonContent &content) = 0;
    virtual bool isValueChanged(const KeyValue &lastEntry, const KeyValue entry) const
    {
        return (lastEntry.second.compare(entry.second) != 0);
    }

    unsigned int actualTime_;
    bool bForceQuery_;
    JsonContent thisQueryResult_, lastQueryResult_;
};


//
class ModemControlParameterGroup : public LteParameterGroup
{
public:
    ModemControlParameterGroup(ModemGTC &modem);

protected:
    virtual const char *getName() { return "Modem control parameters"; }
    virtual unsigned int getMinExpirationTime() const { return 2; }
    virtual unsigned int getMaxExpirationTime() const { return 60; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};


// ManufacturesInfo, FirmwareVersion, IMEI, ICCD, Carrier, SPN
class ConstantModemParameterGroup : public LteParameterGroup
{
public:
    ConstantModemParameterGroup(ModemGTC &modem);

protected:
    virtual const char *getName() { return "Constant parameters"; }
    virtual unsigned int getMinExpirationTime() const { return 3; }
    virtual unsigned int getMaxExpirationTime() const { return 60; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};

// Status (RSSI, S/N, signal quality)
class VariableModemParameterGroup : public LteParameterGroup
{
public:
    VariableModemParameterGroup(ModemGTC &modem);

protected:
    virtual const char *getName() { return "Variable parameters"; }
    virtual unsigned int getMinExpirationTime() const { return 1; }
    virtual unsigned int getMaxExpirationTime() const { return 10; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};

// IP-address, subnet mask, gateway
class NetworkParameterGroup : public LteParameterGroup
{
public:
    NetworkParameterGroup(const std::string &ifaceName);

protected:
    virtual const char *getName() { return "Network parameters"; }
    virtual unsigned int getMinExpirationTime() const { return 1; }
    virtual unsigned int getMaxExpirationTime() const { return 10; }
    virtual bool doGet(JsonContent &content);

    std::string ifaceName_;
};


class TrafficParameterGroup : public LteParameterGroup
{
public:
    TrafficParameterGroup(TrafficCounter &counter);

protected:
    virtual const char *getName() { return "Traffic parameters"; }
    virtual unsigned int getMinExpirationTime() const { return 1; }
    virtual unsigned int getMaxExpirationTime() const { return 1; }
    virtual bool doGet(JsonContent &content);
    virtual bool isValueChanged(const KeyValue &lastEntry, const KeyValue &entry) const { return true; }

    TrafficCounter &counter_;
};

