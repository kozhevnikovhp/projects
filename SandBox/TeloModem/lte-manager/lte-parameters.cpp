/*
 *  lte-parameters.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <fstream>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>

#include "const.h"
#include "lte-parameters.h"
#include "log.h"
#include "misc.h"
#include "verbosity.h"
#include "version.h"

std::vector<LteValuesGroup *> allGroups;

//////////////////////////////////////////////////////////////////
/// LteValuesGroup::LteParameterGroup
///

//static
bool LteValuesGroup::bFirmwareUpdated_ = true; // query everything at start-up
bool LteValuesGroup::bConnectionTypeChanged_ = true; // query everything at start-up
bool LteValuesGroup::bHourlyFullUpdateRequired_ = true; // query everything at start-up
bool LteValuesGroup::bSendOK_ = false; // query everything at start-up

//static
std::map<std::string, std::string> LteValuesGroup::currentState_;
std::string LteValuesGroup::currentStateFileFullPath_;

LteValuesGroup::LteValuesGroup()
    : lastQueryTime_(0), lastFullReportTime_(0), bValuesSuccessfullyObtained_(false)
{
}

//virtual
LteValuesGroup::~LteValuesGroup()
{
}

bool LteValuesGroup::get(time_t basicDelay, JsonContent &allReport)
{
    ReportAction action = REPORT_CHANGED_ONLY;
    if (!bValuesSuccessfullyObtained_ || bFirmwareUpdated_ || bConnectionTypeChanged_ || bHourlyFullUpdateRequired_ || isFullUpdateRequired() || !bSendOK_)
        action = REPORT_EVERYTHING;
    else
    {
        if ((::getCurrentTimeSec() - lastQueryTime_) < getMinExpirationTime()*basicDelay)
            action = REPORT_NOTHING; // never(!) query too frequently
        else if ((::getCurrentTimeSec() - lastFullReportTime_) > getMaxExpirationTime()*basicDelay)
            action = REPORT_EVERYTHING; // data received is obsolete
    }
    clearFullUpdateRequired();
    if (action == REPORT_NOTHING)
        return true;

    thisQueryResult_.clear();

    if (g_VerbosityLevel >=2)
        printf("\tdoGet %s\n", getName());

    bValuesSuccessfullyObtained_ = doGet(thisQueryResult_);
    if (bValuesSuccessfullyObtained_)
    {
        lastQueryTime_ = ::getCurrentTimeSec();
        if (action == REPORT_EVERYTHING)
            lastFullReportTime_ = lastQueryTime_;

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
            // update current state
            LteValuesGroup::currentState_[entry.first] = entry.second;
        }

        lastQueryResult_.swap(thisQueryResult_);
    }
    return bValuesSuccessfullyObtained_;
}

//static
void LteValuesGroup::userSigUsr1Handler(int signalNumber)
{
    if (signalNumber == SIGUSR1)
    {
        //printf("Received SIGUSR1!\n");
        LteValuesGroup::writeCurrentState();
        for (auto group : allGroups)
            group->setFullUpdateRequired();
    }
}

//static
void LteValuesGroup::prepareCurrentState()
{
    currentStateFileFullPath_ = std::string(PSZ_CURRENT_STATE_PATH);
    currentStateFileFullPath_ += PSZ_CURRENT_STATE_FILE;
    ::makeDirRecursively(PSZ_CURRENT_STATE_PATH, S_IRWXU | S_IRWXG | S_IRWXO);

    signal(SIGUSR1, userSigUsr1Handler);
}

//static
bool LteValuesGroup::writeCurrentState()
{
    FILE *pFile = fopen(currentStateFileFullPath_.c_str(), "w");
    if (!pFile)
    {
        log_info("Cannot write LTE-status to file %s (%s)\n", currentStateFileFullPath_.c_str(), strerror(errno));
        return false;
    }
    //fprintf(pFile, "{\n");
    for (auto keyValue : currentState_)
    {
        fprintf(pFile, "%s : %s\n", keyValue.first.c_str(), keyValue.second.c_str());
        //printf("%s : %s\n", keyValue.first.c_str(), keyValue.second.c_str());
    }
    //fprintf(pFile, "}\n");
    fclose(pFile);
    return true;
}

////////////////////////////////////////////////////////////////////
/// CommonParameterGroup

CommonParameterGroup::CommonParameterGroup()
{
}

//virtual
bool CommonParameterGroup::doGet(JsonContent &content)
{
    // Version of TELO (content of /version file)
    std::ifstream f("/version");
    if (f.is_open())
    {
        std::string TeloBuildNumber, line;
        while (!f.eof() && !f.bad() && !f.fail())
        {
            line.clear();
            std::getline(f, line);
            TeloBuildNumber += line;
        }
        if (!TeloBuildNumber.empty())
            content.emplace_back(KeyValue("telo_build_number", TeloBuildNumber));
    }

    // version of LTE-Manager
    content.emplace_back(KeyValue("lte_manager_version", PSZ_VERSION));
    return true;
}


//////////////////////////////////////////////////////////////////
/// ModemControlParameterGroup

ModemControlParameterGroup::ModemControlParameterGroup(ModemGTC &modem)
    : modem_(modem)
{
}

//virtual
bool ModemControlParameterGroup::doGet(JsonContent &content)
{
    std::string status = "inactive";
    if (modem_.isConnected() && modem_.isControllable())
        status = "active";
    content.emplace_back(KeyValue("lte_dongle", status));
    return true;
}


///////////////////////////////////////////////////////////////////
///  ConstantModemParameterGroup::ConstantModemParameterGroup

ConstantModemParameterGroup::ConstantModemParameterGroup(ModemGTC &modem)
    : modem_(modem)
{
}

//virtual
bool ConstantModemParameterGroup::doGet(JsonContent &content)
{
    if (!modem_.isConnected())
        return false;

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
    if (!modem_.isConnected())
        return false;

    bool bSuccess = true;
    bSuccess *= modem_.getStatus(content);
    return bSuccess;
}


///////////////////////////////////////////////////////////////////
/// NetworkParameterGroup

NetworkParameterGroup::NetworkParameterGroup()
{
}

//virtual
bool NetworkParameterGroup::doGet(JsonContent &content)
{
    // get current connection
    std::string ifaceName;
    if (!getCurrentConnection(ifaceName))
        return false;

    if (ifaceName.find("eth") != std::string::npos)
        connectionType_ = std::string("ethernet");
    else if (ifaceName.find("usbnet") != std::string::npos)
        connectionType_ = std::string("lte");
    else if (ifaceName.find("ra") != std::string::npos)
        connectionType_ = std::string("wifi");
    else
        connectionType_ = std::string("unknown");
    content.emplace_back(KeyValue("connection_type", connectionType_));
    if (connectionType_.compare(previousConnectionType_))
    {
        LteValuesGroup::bConnectionTypeChanged_ = true;
        previousConnectionType_.swap(connectionType_);
        connectionType_.clear();
    }

    IPADDRESS_TYPE IP = 0, subnetMask = 0;
    if (!getInterfaceAddressAndMask(ifaceName, IP, subnetMask))
        return false;
    content.emplace_back(KeyValue("ip_address", addressToDotNotation(IP)));
    content.emplace_back(KeyValue("mask", addressToDotNotation(subnetMask)));

    IPADDRESS_TYPE GW = 0;
    if (!getGateway(GW))
        return false;
    content.emplace_back(KeyValue("gateway", addressToDotNotation(GW)));

    return true;
}

// to avoid including <linux/route.h>
#ifndef RTF_GATEWAY
#define	RTF_GATEWAY	0x0002		// destination is a gateway
#endif
bool NetworkParameterGroup::getCurrentConnection(std::string &ifaceName)
{
    std::ifstream f("/proc/net/route");
    if (!f.is_open())
        return false;

    // Iface	Destination	Gateway 	Flags	RefCnt	Use	Metric	Mask	MTU	Window	IRTT
    bool bSuccess = false;
    std::string line;
    char szName[IFNAMSIZ+1];
    unsigned int uiDest, uiGW, uiFlags;
    while (!f.eof() && !f.bad() && !f.fail())
    {
        std::getline(f, line);
        trimBlanks(line);
        if (line.empty())
            continue;
        int nScanfed = sscanf(line.c_str(), "%s %X %X %X", szName, &uiDest, &uiGW, &uiFlags);
        if (nScanfed == 4)
        {
            if (uiFlags & RTF_GATEWAY)
            {
                ifaceName = std::string(szName);
                bSuccess = true;
                break;
            }
        }
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
    if (counter_.getTeloOutputBytes())
        content.emplace_back(KeyValue("telo_ooma_apps_lte_sent_bytes", std::to_string(counter_.getTeloOutputBytes())));
    if (counter_.getTeloInputBytes())
        content.emplace_back(KeyValue("telo_ooma_apps_lte_received_bytes", std::to_string(counter_.getTeloInputBytes())));

    /*if (counter_.getUserOutputBytes())
        content.emplace_back(KeyValue("sent_bytes_user", std::to_string(counter_.getUserOutputBytes())));
    if (counter_.getUserOutputBytes())
        content.emplace_back(KeyValue("received_bytes_user", std::to_string(counter_.getUserInputBytes())));
        */

    counter_.clearStatistics();

    return true;
}


///////////////////////////////////////////////////////////////////
/// WanSwitchStateGroup

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


///////////////////////////////////////////////////////////////////
/// OomaServiceStatusGroup

OomaServiceStatusGroup::OomaServiceStatusGroup()
{
}

//virtual
OomaServiceStatusGroup::~OomaServiceStatusGroup()
{
}

//virtual
bool OomaServiceStatusGroup::doGet(JsonContent &content)
{
    std::string status;
    std::ifstream f("/var/status/freeswitch");
    if (f.is_open() && !f.bad() && !f.eof() && !f.fail())
    {
        std::string freeswitchStatus;
        std::getline(f, freeswitchStatus);
        tolower(freeswitchStatus);
        if (freeswitchStatus.compare("ready") == 0)
            status = std::string("live");
        else
            status = std::string("offline");
    }
    else
        status = std::string("unknown");

    content.emplace_back(KeyValue("ooma_service_status", status));
    return true;
}

