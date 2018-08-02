/* 
 * config.h
 *
 * Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 */

#pragma once

#include <string>
#include <vector>
#include <tuple>

typedef std::tuple<std::string, std::string, std::string> ConfigurationParam;

class Configuration
{
public:
    Configuration(const std::string configFile);
    bool createDefaultFile();
    bool save();
    bool load();
    std::string get(const char *pszKey, const char *pszDefaultValue);

protected:
    std::string configFile_;
    std::vector<ConfigurationParam> params_;
};

