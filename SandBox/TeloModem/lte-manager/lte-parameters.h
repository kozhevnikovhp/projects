#ifndef LTEPARAMETERS_H
#define LTEPARAMETERS_H

#include "modem-gct.h"
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

public:
    bool get(JsonContent &allReport);

protected:
    virtual int getMinExpirationTime() const = 0;
    virtual int getMaxExpirationTime() const = 0;
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
    ModemControlParameterGroup(ModemGCT &modem);

protected:
    virtual int getMinExpirationTime() const { return 1*60; }
    virtual int getMaxExpirationTime() const { return 5*60; }
    virtual bool doGet(JsonContent &content);

    ModemGCT &modem_;
};


class ConstantModemParameterGroup : public LteParameterGroup
{
public:
    ConstantModemParameterGroup(ModemGCT &modem);

protected:
    virtual int getMinExpirationTime() const { return 1*60; }
    virtual int getMaxExpirationTime() const { return 5*60; }
    virtual bool doGet(JsonContent &content);

    ModemGCT &modem_;
};


class VariableModemParameterGroup : public LteParameterGroup
{
public:
    VariableModemParameterGroup(ModemGCT &modem);

protected:
    virtual int getMinExpirationTime() const { return 1*60; }
    virtual int getMaxExpirationTime() const { return 2*60; }
    virtual bool doGet(JsonContent &content);

    ModemGCT &modem_;
};

class NetworkParameterGroup : public LteParameterGroup
{
public:
    NetworkParameterGroup(const std::string &ifaceName);

protected:
    virtual int getMinExpirationTime() const { return 5*60; }
    virtual int getMaxExpirationTime() const { return 10*60; }
    virtual bool doGet(JsonContent &content);

    std::string ifaceName_;
};


class TrafficParameterGroup : public LteParameterGroup
{
public:
    TrafficParameterGroup(TrafficCounter &counter);

protected:
    virtual int getMinExpirationTime() const { return 5; }
    virtual int getMaxExpirationTime() const { return 10; }
    virtual bool doGet(JsonContent &content);
    virtual bool isValueChanged(const KeyValue &lastEntry, const KeyValue &entry) const { return true; }

    TrafficCounter &counter_;
};


#endif // LTEPARAMETERS_H
