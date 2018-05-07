#pragma once

#include <string>
#include "serial-connection.h"

class ModemGCT
{
public:
    ModemGCT(const char *pszDevice);

    bool execute(const std::string &command, std::string &reply);
    bool getStatus(std::string &status);

protected:
    SerialConnection connection_;
};

bool getStatusGTC(const char *pszDevice, std::string &status);
