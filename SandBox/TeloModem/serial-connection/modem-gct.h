#pragma once

#include <string>
#include <vector>
#include "serial-connection.h"
#include "json.h"

void convertKeyValuePairTo(JsonContent &content, std::string &json);

class ModemGCT
{
public:
    ModemGCT(const char *pszDevice);

    bool getManufacturerInfoRaw(std::string &raw);
    bool getManufacturerInfo(JsonContent &content);
    bool getManufacturerInfo(std::string &json);

    bool getBandRaw(std::string &raw);
    bool getBand(JsonContent &content);
    bool getBand(std::string &json);

    bool getCopsRaw(std::string &raw);
    bool getCops(JsonContent &content);
    bool getCops(std::string &json);

    bool getCgdContRaw(std::string &raw);
    bool getCgdCont(JsonContent &content);
    bool getCgdCont(std::string &json);

    bool getAttStatusRaw(std::string &raw);
    bool getAttStatus(JsonContent &content);
    bool getAttStatus(std::string &json);

    bool getActStatusRaw(std::string &raw);
    bool getActStatus(JsonContent &content);
    bool getActStatus(std::string &json);

    bool getCgpAddrRaw(std::string &raw);
    bool getCgpAddr(JsonContent &content);
    bool getCgpAddr(std::string &json);

    bool getStatusRaw(std::string &raw);
    bool getStatus(JsonContent &content);
    bool getStatus(std::string &json);

protected:
    bool execute(const std::string &command, std::string &reply);

    SerialConnection connection_;
};

bool getStatusGTC(const char *pszDevice, std::string &status);
