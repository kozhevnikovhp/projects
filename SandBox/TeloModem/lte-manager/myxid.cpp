/*
 *  myxid.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <fstream>
#include "myxid.h"
#include "config.h"
#include "const.h"
#include "misc.h"
#include "log.h"

std::string getMyxID()
{
#if 1
    // get myxID from hostname
    std::string hostID;
    std::ifstream f("/proc/sys/kernel/hostname");
    std::getline(f, hostID);

    // Now get 6 last letters
    if (hostID.length() < 6)
        return hostID;

    return hostID.substr(hostID.length() - 6);
#else
    // get myxID from MAC-address of ethernet interface
    std::string myxID, myxInterfaceName("eth0");
    unsigned char ucMacAddress[8];
    if (getInterfaceMacAddress("eth0", ucMacAddress))
    {
        char szTmp[32];
        sprintf(szTmp, "%02X%02X%02X", ucMacAddress[3], ucMacAddress[4], ucMacAddress[5]-1);
        myxID = std::string(szTmp);
    }
    else
        log_error("Cannot get MYX_ID as MAC-address of interface %s", myxInterfaceName.c_str());

    return myxID;
#endif
}
