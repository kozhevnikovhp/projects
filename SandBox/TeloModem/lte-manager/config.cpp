/*
 *  config.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <regex>
#include <fstream>

#include "config.h"
#include "log.h"
#include "misc.h"

Configuration::Configuration(const std::string &configFile)
    : configFile_(configFile)
{
}

bool Configuration::load()
{
    const char delimeter = '=';

    std::ifstream f(configFile_);
    if (!f.is_open())
        return false;

    if (f.eof() || f.bad() || f.fail())
    {
        log_error("Invalid config file %s\n", configFile_.c_str());
        return false;
    }

    std::string line;
    while (1)
    {
        std::getline(f, line);
        if (f.eof() || f.bad() || f.fail())
            break;
        if (line.find(delimeter) == std::string::npos)
            continue; // does not match format key = value

        // remove leading and heading blanks
        trimBlanks(line);
        if (line.empty())
            continue;
        if (line[0] == '#')
            continue; // comment

        std::stringstream ss(line);
        std::string key, value;
        std::getline(ss, key, '=');
        trimBlanks(key);
        std::getline(ss, value, '=');
        trimBlanks(value);

        params_.emplace_back(ConfigurationParam(key, value));
    }
    return true;
}

void Configuration::free()
{
    ConfigurationParams empty;
    params_.swap(empty);
}


std::string Configuration::get(const char *pszKey, const char *pszDefaultValue) const
{
    // linear search because no millions of enties assumed here
    for (auto param : params_)
    {
        const std::string &key = param.first;
        if (!key.compare(pszKey))
            return param.second;
    }
    return std::string(pszDefaultValue);
}

bool Configuration::getBoolean(const char *pszKey, bool bDefaultValue) const
{
    std::string value = get(pszKey, "");
    if (value.empty())
        return bDefaultValue;
    tolower(value);
    return (!value.compare("enable") || !value.compare("true"));
}

int Configuration::getInteger(const char *pszKey, int defaultValue) const
{
    std::string value = get(pszKey, std::to_string(defaultValue).c_str());
    tolower(value);
    return atoi(value.c_str());
}

long Configuration::getLong(const char *pszKey, long defaultValue) const
{
    std::string value = get(pszKey, std::to_string(defaultValue).c_str());
    tolower(value);
    return atol(value.c_str());
}
