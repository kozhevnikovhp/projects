#pragma once

#include <string>
#include "serial-connection.h"

class ModemGCT
{
public:
    ModemGCT(const char *pszDevice);

    bool execute(const std::string &command, std::string &reply);
    bool getStatusRaw(std::string &status);
    bool getStatusJSON(std::string &status);

protected:
    static bool parseToJSON(const std::string &input, std::string &output);

    SerialConnection connection_;
};

bool getStatusGTC(const char *pszDevice, std::string &status);
