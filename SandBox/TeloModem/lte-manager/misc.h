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

bool isInterfaceUp(const std::string &ifaceName, bool &bUp);
bool putInterfaceDown(const std::string &ifaceName);
bool setInterfaceUp(const std::string &ifaceName);

bool getInterfaceAddressAndMask(const std::string &ifaceName, IPADDRESS_TYPE &ifaceIP, IPADDRESS_TYPE &ifaceMask);
bool setInterfaceIpAddress(const std::string &ifaceName, IPADDRESS_TYPE ifaceIP);
bool setInterfaceMask(const std::string &ifaceName, IPADDRESS_TYPE mask);
bool getInterfaceMacAddress(const std::string &ifaceName, void *pAddress);
bool getGateway(IPADDRESS_TYPE &GW);

void tolower(std::string &s);
void toupper(std::string &s);
void trimBlanks(std::string &s);

bool makeDirRecursively(const std::string &path, mode_t mode);
