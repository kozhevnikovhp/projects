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
#include "const.h"
#include "log.h"
#include "misc.h"

Configuration::Configuration(const std::string configFile)
    : configFile_(configFile)
{
    params_.emplace_back(ConfigurationParam(PSZ_DEVICE_NAME, "/dev/ttyACM0", "Serial device name, something like /dev/ttyACM0"));

    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_ENABLED, "false", "Conventional Kafka enabled or not?"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_BROKERS, PSZ_KAFKA_BROKERS_DEFAULT, "Comma-separated list of Kafka-brokers"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_TOPIC, PSZ_KAFKA_TOPIC_DEFAULT, "Kafka topic"));

    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_URL, PSZ_KAFKA_REST_PROXY_URL_DEFAULT, "Kafka REST proxy URL"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_ENABLED, "true", "Kafka REST Proxy enabled or not?"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_CERT, "", "Certificate file for access to Kafka REST proxy"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_KEY, "", "User key file for access to Kafka REST proxy"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_PASSWORD, "", "User password for access to Kafka REST proxy"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_VERIFY_PEER, "false", "To verify Kafka REST proxy server or not?"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_CACERT, "", "CA certificate file for access to Kafka REST proxy"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_CAKEY, "", "CA key file for access to Kafka REST proxy"));
    params_.emplace_back(ConfigurationParam(PSZ_KAFKA_REST_PROXY_VERBOSE, "false", "For debug purposes"));

    params_.emplace_back(ConfigurationParam(PSZ_BASIC_QUERY_DELAY, "60", "Basic delay between subsequent queries (seconds)"));
}

bool Configuration::createDefaultFile()
{
    log_info("Writing default configuration file %s", configFile_.c_str());
    return save();
}

bool Configuration::save()
{
    FILE *fd = fopen(configFile_.c_str(), "w");
    if (!fd)
    {
        log_error("Cannot write config file %s\n", configFile_.c_str());
        return false;
    }

    for (auto param : params_)
    {
        fprintf(fd, "# %s\n", std::get<2>(param).c_str());
        fprintf(fd, "%s=%s\n\n", std::get<0>(param).c_str(), std::get<1>(param).c_str());
    }

    fclose(fd);
    return true;
}

bool Configuration::load()
{
    const char delimeter = '=';

    std::ifstream f(configFile_);
    if (!f.is_open())
    {
        log_error("Cannot read config file %s\n", configFile_.c_str());
        return false;
    }
    if (f.eof() || f.bad() || f.fail())
    {
        log_error("Invalid config file %s\n", configFile_.c_str());
        return false;
    }

    std::vector<ConfigurationParam> params;
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

        for (auto param : params_)
        {
            std::string &paramKey = std::get<0>(param);
            if (!paramKey.compare(key))
            {
                const std::string &description = std::get<2>(param);
                params.emplace_back(ConfigurationParam(key, value, description));
            }
        }
    }
    params.swap(params_);
    return true;
}

std::string Configuration::get(const char *pszKey, const char *pszDefaultValue) const
{
    // linear search because no millions of enties assumed here
    for (auto param : params_)
    {
        const std::string &key = std::get<0>(param);
        //std::string &value = std::get<1>(param);
        //printf("%s ===== %s\n", key.c_str(), value.c_str());
        if (!key.compare(pszKey))
            return std::get<1>(param);
    }
    return std::string(pszDefaultValue);
}

bool Configuration::getBoolean(const char *pszKey, const char *pszDefaultValue) const
{
    std::string value = get(pszKey, pszDefaultValue);
    tolower(value);
    return (!value.compare("enable") || !value.compare("true"));
}
