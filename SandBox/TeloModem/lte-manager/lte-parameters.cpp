#include <sys/time.h>
#include "lte-parameters.h"

unsigned int getCurrentTimeSec()
{
    timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec;
}

//////////////////////////////////////////////////////////////////
/// \brief LteParameterGroup::LteParameterGroup
///
LteParameterGroup::LteParameterGroup()
    : bForceQuery_(true)
{
    actualTime_ = 0;
}

bool LteParameterGroup::get(JsonContent &allReport)
{
    ReportAction action = REPORT_NOTHING;
    if (bForceQuery_)
        action = REPORT_EVERYTHING;
    else
    {
        unsigned int elapsedTime = ::getCurrentTimeSec() - actualTime_;
        if (elapsedTime < getMinExpirationTime())
            action = REPORT_NOTHING; // never(!) query too frequently
        else if (elapsedTime > getMaxExpirationTime())
            action = REPORT_EVERYTHING; // data received is obsolete
    }
    if (action == REPORT_NOTHING)
        return true;

    thisQueryResult_.clear();
    bool bSuccess = doGet(thisQueryResult_);
    if (bSuccess)
    {
        bForceQuery_ = false;
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
                allReport.push_back(entry);
        }

        lastQueryResult_.swap(thisQueryResult_);
    }
    return bSuccess;
}


///////////////////////////////////////////////////////////////////
/// \brief ModemControlParameterGroup::ModemControlParameterGroup
///
ModemControlParameterGroup::ModemControlParameterGroup(ModemGCT &modem)
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
}


///////////////////////////////////////////////////////////////////
/// \brief ConstantModemParameterGroup::ConstantModemParameterGroup
///
ConstantModemParameterGroup::ConstantModemParameterGroup(ModemGCT &modem)
    : modem_(modem)
{
}

//virtual
bool ConstantModemParameterGroup::doGet(JsonContent &content)
{
    bool bSuccess = true;
    bSuccess *= modem_.getManufacturerInfo(content);
    return bSuccess;
}


///////////////////////////////////////////////////////////////////
/// \brief VariableModemParameterGroup::VariableModemParameterGroup
///
VariableModemParameterGroup::VariableModemParameterGroup(ModemGCT &modem)
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
/// \brief NetworkParameterGroup::NetworkParameterGroup
///
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
/// \brief VariableModemParameterGroup::VariableModemParameterGroup
///
TrafficParameterGroup::TrafficParameterGroup(TrafficCounter &counter)
    : counter_(counter)
{
}


//virtual
bool TrafficParameterGroup::doGet(JsonContent &content)
{
    const char *FORMAT_STRING = "%d";
    char szString[128];

    if (counter_.getTeloOutputBytes())
    {
        sprintf(szString, FORMAT_STRING, counter_.getTeloOutputBytes());
        content.emplace_back(KeyValue("sent_bytes_telo", szString));
    }
    if (counter_.getTeloInputBytes())
    {
        sprintf(szString, FORMAT_STRING, counter_.getTeloInputBytes());
        content.emplace_back(KeyValue("received_bytes_telo", szString));
    }

    if (counter_.getUserOutputBytes())
    {
        sprintf(szString, FORMAT_STRING, counter_.getUserOutputBytes());
        content.emplace_back(KeyValue("sent_bytes_user", szString));
    }
    if (counter_.getUserOutputBytes())
    {
        sprintf(szString, FORMAT_STRING, counter_.getUserInputBytes());
        content.emplace_back(KeyValue("received_bytes_user", szString));
    }

    counter_.clearStatistics();

    return true;
}

