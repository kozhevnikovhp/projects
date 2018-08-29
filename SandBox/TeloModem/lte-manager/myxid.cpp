/*
 *  myxid.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <fstream>
#include "myxid.h"

std::string getMyxID()
{
    std::string hostid;
    std::ifstream f("/proc/sys/kernel/hostname");
    std::getline(f, hostid);

    // Now get 6 last letters
    if (hostid.length() < 6)
        return hostid;

    return hostid.substr(hostid.length() - 6);
}
