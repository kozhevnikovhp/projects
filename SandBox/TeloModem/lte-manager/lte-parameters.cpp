/*
 *  lte-parameters.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <sys/time.h>
#include "lte-parameters.h"
#include "log.h"

time_t getCurrentTimeSec()
{
    timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec;
}

//////////////////////////////////////////////////////////////////
/// LteValuesGroup::LteParameterGroup
///
LteValuesGroup::LteValuesGroup()
    : actualTime_(0), bForceQuery_(true)
{
}

//virtual
LteValuesGroup::~LteValuesGroup()
{
}

bool LteValuesGroup::get(time_t basicDelay, JsonContent &allReport)
{
    //printf("get %s\n", getName());
    ReportAction action = REPORT_CHANGED_ONLY;
    if (bForceQuery_)
        action = REPORT_EVERYTHING;
    else
    {
        time_t elapsedTime = ::getCurrentTimeSec() - actualTime_;
        if (elapsedTime < getMinExpirationTime()*basicDelay)
            action = REPORT_NOTHING; // never(!) query too frequently
        else if (elapsedTime > getMaxExpirationTime()*basicDelay)
            action = REPORT_EVERYTHING; // data received is obsolete
    }
    if (action == REPORT_NOTHING)
        return true;

    thisQueryResult_.clear();
    //printf("\tdoGet %s\n", getName());
    bool bSuccess = doGet(thisQueryResult_);
    bForceQuery_ = !bSuccess;
    if (bSuccess)
    {
        actualTime_ = ::getCurrentTimeSec();

        for (auto &entry : thisQueryResult_)
        {
            bool bNeedToAdd = (action == REPORT_EVERYTHING);
            if (!bNeedToAdd)
            { // probably, value is changed and needs to be added?
                // linear search, only a few entries assumed here
                bool bFound = false;
                bool bChanged = false;
                for (auto &lastEntry : lastQueryResult_)
                {
                    if (lastEntry.first.compare(entry.first) == 0)
                    { /// key found, compare values
                        bFound = true;
                        if (isValueChanged(lastEntry, entry))
                        {
                            bChanged = true;
                            break;
                        }
                    }
                }
                bNeedToAdd = !bFound || bChanged;
            }
            if (bNeedToAdd)
                allReport.emplace_back(entry);
        }

        lastQueryResult_.swap(thisQueryResult_);
    }
    return bSuccess;
}


///////////////////////////////////////////////////////////////////
/// ModemControlParameterGroup
///
ModemControlParameterGroup::ModemControlParameterGroup(ModemGTC &modem)
    : modem_(modem)
{
}

//virtual
bool ModemControlParameterGroup::doGet(JsonContent &content)
{
    std::string status = "inactive";
    if (modem_.isControllable())
        status = "active";
    content.emplace_back(KeyValue("lte_dongle", status));
    return true;
}


///////////////////////////////////////////////////////////////////
///  ConstantModemParameterGroup::ConstantModemParameterGroup
///
ConstantModemParameterGroup::ConstantModemParameterGroup(ModemGTC &modem)
    : modem_(modem)
{
}

//virtual
bool ConstantModemParameterGroup::doGet(JsonContent &content)
{
    bool bSuccess = true;
    bSuccess *= modem_.getManufacturerInfo(content);
    bSuccess *= modem_.getFirmwareVersionInfo(content);
    bSuccess *= modem_.getImei(content);
    bSuccess *= modem_.getIccId(content);
    bSuccess *= modem_.getCarrier(content);
    std::string SPN;
    modem_.getSPN(SPN);
    if (!SPN.empty())
        content.emplace_back(KeyValue("spn", SPN));
    return bSuccess;
}


///////////////////////////////////////////////////////////////////
/// VariableModemParameterGroup

VariableModemParameterGroup::VariableModemParameterGroup(ModemGTC &modem)
    : modem_(modem)
{
}

//virtual
bool VariableModemParameterGroup::doGet(JsonContent &content)
{
    bool bSuccess = true;
    bSuccess *= modem_.getStatus(content);
    return bSuccess;
}


///////////////////////////////////////////////////////////////////
/// NetworkParameterGroup

NetworkParameterGroup::NetworkParameterGroup(const std::string &iFaceName)
    : ifaceName_(iFaceName)
{
}

//virtual
bool NetworkParameterGroup::doGet(JsonContent &content)
{
    IPADDRESS_TYPE IP, subnetMask;
    bool bSuccess = getInterfaceAddressAndMask(ifaceName_, IP, subnetMask);
    if (bSuccess)
    {
        content.emplace_back(KeyValue("ip_address", addressToDotNotation(IP)));
        content.emplace_back(KeyValue("mask", addressToDotNotation(subnetMask)));
    }
    IPADDRESS_TYPE GW;
    bSuccess |= getInterfaceGateway(ifaceName_, GW);
    if (bSuccess)
    {
        content.emplace_back(KeyValue("gateway", addressToDotNotation(GW)));
    }
    return bSuccess;
}


///////////////////////////////////////////////////////////////////
/// TrafficParameterGroup

TrafficParameterGroup::TrafficParameterGroup(TrafficCounter &counter)
    : counter_(counter)
{
}

//virtual
bool TrafficParameterGroup::doGet(JsonContent &content)
{
    const char *const FORMAT_STRING = "%d";
    char szString[128];

    if (counter_.getTeloOutputBytes())
    {
        sprintf(szString, FORMAT_STRING, counter_.getTeloOutputBytes());
        content.emplace_back(KeyValue("telo_ooma_apps_lte_sent_bytes", szString));
    }
    if (counter_.getTeloInputBytes())
    {
        sprintf(szString, FORMAT_STRING, counter_.getTeloInputBytes());
        content.emplace_back(KeyValue("telo_ooma_apps_lte_received_bytes", szString));
    }

    /*if (counter_.getUserOutputBytes())
    {
        sprintf(szString, FORMAT_STRING, counter_.getUserOutputBytes());
        content.emplace_back(KeyValue("sent_bytes_user", szString));
    }
    if (counter_.getUserOutputBytes())
    {
        sprintf(szString, FORMAT_STRING, counter_.getUserInputBytes());
        content.emplace_back(KeyValue("received_bytes_user", szString));
    }*/

    counter_.clearStatistics();

    return true;
}


///////////////////////////////////////////////////////////////////
/// WanSwitchStateGroup
///
const char *const WANSWITCH_DBUS_OBJECT_PATH_NAME = "/org/ooma/wanswitch";
const char *const WANSWITCH_DBUS_INTERFACE_NAME = "org.ooma.wanswitch";
const char *const WANSWITCH_DBUS_GET_SERVICE_METHOD_NAME = "GetService";
const char *const WANSWITCH_DBUS_ISWANCONNECTED_METHOD_NAME = "isWANConnected";

WanSwitchStateGroup::WanSwitchStateGroup()
{
    dbus_error_init(&DbusError_);
    pDbusConnection_ = dbus_bus_get(DBUS_BUS_SYSTEM, &DbusError_);
    if (!pDbusConnection_)
    {
        reportDbusError();
    }
}

//virtual
WanSwitchStateGroup::~WanSwitchStateGroup()
{
    if (pDbusConnection_)
    {
        dbus_connection_close(pDbusConnection_);
        dbus_connection_unref(pDbusConnection_);
    }
}


//virtual
bool WanSwitchStateGroup::doGet(JsonContent &content)
{
    if (!pDbusConnection_)
        return true;

    content.emplace_back(KeyValue("wan_connected", getDbusValue(WANSWITCH_DBUS_ISWANCONNECTED_METHOD_NAME)));
    content.emplace_back(KeyValue("connection_type", getDbusValue(WANSWITCH_DBUS_GET_SERVICE_METHOD_NAME)));

    return true;
}

std::string WanSwitchStateGroup::getDbusValue(const char *pszMethodName)
{
    std::string retValue;
    DBusMessage *pRequest = dbus_message_new_method_call(nullptr, WANSWITCH_DBUS_OBJECT_PATH_NAME,
                                                     WANSWITCH_DBUS_INTERFACE_NAME, pszMethodName);
    if (!pRequest)
        return retValue;

    dbus_message_set_auto_start(pRequest, TRUE);
    dbus_message_set_destination(pRequest, WANSWITCH_DBUS_INTERFACE_NAME);

    DBusMessageIter iter;
    dbus_message_iter_init_append(pRequest, &iter);

    dbus_error_init(&DbusError_);
    DBusMessage *pReply = dbus_connection_send_with_reply_and_block(pDbusConnection_,
                                                                    pRequest,
                                                                    2000, // timeout in msecs
                                                                    &DbusError_);

    if (pReply)
    {
        //printf("sent with reply %s = OK\n", pszMethod);
        int message_type = dbus_message_get_type(pReply);
        if (message_type == DBUS_MESSAGE_TYPE_METHOD_RETURN)
        {
            dbus_message_iter_init(pReply, &iter);
            int type = dbus_message_iter_get_arg_type(&iter);
            switch (type)
            {
                case DBUS_TYPE_STRING:
                {
                    char *pszValue;
                    dbus_message_iter_get_basic(&iter, &pszValue);
                    retValue = std::string(pszValue);
                    break;
                }
                case DBUS_TYPE_BOOLEAN:
                {
                    dbus_bool_t bValue;
                    dbus_message_iter_get_basic(&iter, &bValue);
                    retValue = bValue ? "true" : "false";
                    break;
                }
            }
        }
    }
    else
        reportDbusError();

    if (pReply)
        dbus_message_unref(pReply);
    if (pRequest)
        dbus_message_unref(pRequest);

    return retValue;
}

void WanSwitchStateGroup::reportDbusError()
{
    if (dbus_error_is_set(&DbusError_))
    {
        log_error(DbusError_.message);
        dbus_error_free(&DbusError_);
    }
}


