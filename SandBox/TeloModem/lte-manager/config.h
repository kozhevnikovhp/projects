/* 
 * config.h
 *
 * Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 */

#pragma once

#include <string>
#include <vector>

typedef std::pair<std::string, std::string> ConfigurationParam;
typedef std::vector<ConfigurationParam> ConfigurationParams;


class Configuration
{
public:
    Configuration(const std::string &configFile);
    bool load();
    void free();

    std::string get(const char *pszKey, const char *pszDefaultValue) const;
    bool getBoolean(const char *pszKey, const char *pszDefaultValue) const;

protected:
    std::string configFile_;
    ConfigurationParams params_;
};

