#pragma once

#include "telnet.h"

class TelnetGTC : public TelnetSocket
{
    typedef TelnetSocket inherited;
public:
    TelnetGTC(int af);
    virtual ~TelnetGTC();

public:

protected:
};

