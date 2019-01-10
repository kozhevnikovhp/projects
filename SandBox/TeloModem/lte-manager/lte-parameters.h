/*
 *  lte-parameters.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <dbus/dbus.h>

#include "modem-gtc.h"
#include "traffic-counter.h"
#include "json.h"


class LteValuesGroup
{
public:
    typedef enum {
        REPORT_NOTHING,
        REPORT_CHANGED_ONLY,
        REPORT_EVERYTHING
    } ReportAction;

public:
    LteValuesGroup();
    virtual ~LteValuesGroup();

public:
    bool get(time_t basicDelay, JsonContent &allReport);

    static bool bFirmwareUpdated_; // if just updated - re-query everything
    static bool bConnectionTypeChanged_;// if just changed - re-query everything

protected:
    virtual const char *getName() = 0;
    virtual time_t getMinExpirationTime() const = 0; // factor, multiplying basic query delay
    virtual time_t getMaxExpirationTime() const = 0; // factor, multiplying basic query delay
    virtual bool doGet(JsonContent &content) = 0;
    virtual bool isValueChanged(const KeyValue &lastEntry, const KeyValue entry) const
    {
        return (lastEntry.second.compare(entry.second) != 0);
    }

    time_t actualTime_;
    bool bForceQuery_;
    JsonContent thisQueryResult_, lastQueryResult_;
};


// is modem connected and under control?
class ModemControlParameterGroup : public LteValuesGroup
{
public:
    ModemControlParameterGroup(ModemGTC &modem);

protected:
    virtual const char *getName() { return "Modem control parameters"; }
    virtual time_t getMinExpirationTime() const { return 1; }
    virtual time_t getMaxExpirationTime() const { return 10; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};


// ManufacturesInfo, FirmwareVersion, IMEI, ICCD, Carrier, SPN
class ConstantModemParameterGroup : public LteValuesGroup
{
public:
    ConstantModemParameterGroup(ModemGTC &modem);

protected:
    virtual const char *getName() { return "Constant parameters"; }
    virtual time_t getMinExpirationTime() const { return 3; }
    virtual time_t getMaxExpirationTime() const { return 60; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};

// Status (RSSI, S/N, signal quality)
class VariableModemParameterGroup : public LteValuesGroup
{
public:
    VariableModemParameterGroup(ModemGTC &modem);

protected:
    virtual const char *getName() { return "Variable parameters"; }
    virtual time_t getMinExpirationTime() const { return 1; }
    virtual time_t getMaxExpirationTime() const { return 10; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};

// IP-address, subnet mask, gateway
class NetworkParameterGroup : public LteValuesGroup
{
public:
    NetworkParameterGroup();

protected:
    virtual const char *getName() { return "Network parameters"; }
    virtual time_t getMinExpirationTime() const { return 1; }
    virtual time_t getMaxExpirationTime() const { return 10; }
    virtual bool doGet(JsonContent &content);

    bool getCurrentConnection(std::string &ifaceName);

    std::string previousConnectionType_; // introduced to detect connection type change
    std::string connectionType_; // made a member, not heap var, just to avoid memory fragmentation
};


class TrafficParameterGroup : public LteValuesGroup
{
public:
    TrafficParameterGroup(TrafficCounter &counter);

protected:
    virtual const char *getName() { return "Traffic parameters"; }
    virtual time_t getMinExpirationTime() const { return 1; }
    virtual time_t getMaxExpirationTime() const { return 1; }
    virtual bool doGet(JsonContent &content);
    virtual bool isValueChanged(const KeyValue &lastEntry, const KeyValue &entry) const { return true; }

    TrafficCounter &counter_;
};

class WanSwitchStateGroup : public LteValuesGroup
{
public:
    WanSwitchStateGroup();
    virtual ~WanSwitchStateGroup();

protected:
    virtual const char *getName() { return "WanSwitch state"; }
    virtual time_t getMinExpirationTime() const { return 1; }
    virtual time_t getMaxExpirationTime() const { return 10; }
    virtual bool doGet(JsonContent &content);

    void reportDbusError();
    std::string getDbusValue(const char *pszMethodName);

    DBusError DbusError_;
    DBusConnection *pDbusConnection_;
};

class OomaServiceStatusGroup : public LteValuesGroup
{
public:
    OomaServiceStatusGroup();
    virtual ~OomaServiceStatusGroup();

protected:
    virtual const char *getName() { return "OOMA service status"; }
    virtual time_t getMinExpirationTime() const { return 1; }
    virtual time_t getMaxExpirationTime() const { return 10; }
    virtual bool doGet(JsonContent &content);
};

