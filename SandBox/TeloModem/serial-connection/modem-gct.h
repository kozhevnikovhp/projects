#pragma once

#include <string>
#include "serial-connection.h"

class ModemGCT
{
public:
    ModemGCT(const char *pszDevice);

    bool getManufacturerInfoRaw(std::string &raw);
    bool getManufacturerInfoJSON(std::string &json);

    bool getBandRaw(std::string &raw);
    bool getBandJSON(std::string &json);

    bool getCopsRaw(std::string &raw);
    bool getCopsJSON(std::string &json);

    bool getCgdContRaw(std::string &raw);
    bool getCgdContJSON(std::string &json);

    bool getAttStatusRaw(std::string &raw);
    bool getAttStatusJSON(std::string &json);

    bool getActStatusRaw(std::string &raw);
    bool getActStatusJSON(std::string &json);

    bool getCgpAddrRaw(std::string &raw);
    bool getCgpAddrJSON(std::string &json);

    bool getStatusRaw(std::string &raw);
    bool getStatusJSON(std::string &json);

protected:
    bool execute(const std::string &command, std::string &reply);
    static bool parseToJSON(const std::string &input, std::string &output);

    SerialConnection connection_;
};

bool getStatusGTC(const char *pszDevice, std::string &status);
