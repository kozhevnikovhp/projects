/*
 *  misc.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <string>
#include "ip.h"

////////////////////////////////////////////////////////////////////////
// Misc

IPADDRESS_TYPE dotNotationToAddress(const std::string &str);
IPADDRESS_TYPE dotNotationToAddress(const char *pszStr);

std::string addressToDotNotation(IPADDRESS_TYPE IP);

bool isTheSameSubnet(IPADDRESS_TYPE a1, IPADDRESS_TYPE a2, IPADDRESS_TYPE subnetMask);

bool isItInterfaceName(const std::string &ifaceName);
bool isItInterfaceName(const std::string &ifaceName, int sock);

bool getInterfaceAddressAndMask(const std::string &ifaceName, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask);
bool getInterfaceMacAddress(const std::string &ifaceName, void *pAddress);
bool getInterfaceGateway(const std::string &ifaceName, IPADDRESS_TYPE &GW);

void tolower(std::string &s);
void toupper(std::string &s);
void trimBlanks(std::string &s);
