#pragma once

#include "telnet.h"

class TelnetTeloLE : public TelnetSocket
{
    typedef TelnetSocket inherited;
public:
    TelnetTeloLE(int af);
    virtual ~TelnetTeloLE();

public:

protected:
};

