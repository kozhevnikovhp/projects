#ifndef LTEPARAMETERS_H
#define LTEPARAMETERS_H

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
    bool get(JsonContent &allReport);

protected:
    virtual unsigned int getMinExpirationTime() const = 0;
    virtual unsigned int getMaxExpirationTime() const = 0;
    virtual bool doGet(JsonContent &content) = 0;
    virtual bool isValueChanged(const KeyValue &lastEntry, const KeyValue entry) const
    {
        return (lastEntry.second.compare(entry.second) != 0);
    }

    unsigned int actualTime_;
    bool bForceQuery_;
    JsonContent thisQueryResult_, lastQueryResult_;
};


class ModemControlParameterGroup : public LteParameterGroup
{
public:
    ModemControlParameterGroup(ModemGTC &modem);

protected:
    virtual unsigned int getMinExpirationTime() const { return 5*60; }
    virtual unsigned int getMaxExpirationTime() const { return 10*60; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};


// ManufacturesInfo, SPN
class ConstantModemParameterGroup : public LteParameterGroup
{
public:
    ConstantModemParameterGroup(ModemGTC &modem);

protected:
    virtual unsigned int getMinExpirationTime() const { return 5*60; }
    virtual unsigned int getMaxExpirationTime() const { return 10*60; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};


class VariableModemParameterGroup : public LteParameterGroup
{
public:
    VariableModemParameterGroup(ModemGTC &modem);

protected:
    virtual unsigned int getMinExpirationTime() const { return 1*60; }
    virtual unsigned int getMaxExpirationTime() const { return 2*60; }
    virtual bool doGet(JsonContent &content);

    ModemGTC &modem_;
};

class NetworkParameterGroup : public LteParameterGroup
{
public:
    NetworkParameterGroup(const std::string &ifaceName);

protected:
    virtual unsigned int getMinExpirationTime() const { return 5*60; }
    virtual unsigned int getMaxExpirationTime() const { return 10*60; }
    virtual bool doGet(JsonContent &content);

    std::string ifaceName_;
};


class TrafficParameterGroup : public LteParameterGroup
{
public:
    TrafficParameterGroup(TrafficCounter &counter);

protected:
    virtual unsigned int getMinExpirationTime() const { return 1*60; }
    virtual unsigned int getMaxExpirationTime() const { return 5*60; }
    virtual bool doGet(JsonContent &content);
    virtual bool isValueChanged(const KeyValue &lastEntry, const KeyValue &entry) const { return true; }

    TrafficCounter &counter_;
};


#endif // LTEPARAMETERS_H
