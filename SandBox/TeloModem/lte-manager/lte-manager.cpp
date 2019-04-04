/*
 *  lte-manager.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>

#include <librdkafka/rdkafkacpp.h>

#include "lte-parameters.h"
#include "traffic-counter.h"
#include "config.h"
#include "const.h"
#include "log.h"
#include "myxid.h"
#include "misc.h"
#include "fw-upgrade.h"
#include "curl-lib.h"
#include "verbosity.h"
#include "version.h"

static bool needToContinue(int nCycles)
{
#ifdef VALGRIND
    return (nCycles < 1000);
#else
    return true;
#endif
}

int main(int argc, char *argv[])
{
    // to daemonize myself, or not to daemonize?
    // quick and dirty - just iterate through the list of cmd-line args looking for -d option, meaning daemonizing.
    // TODO: make something better with ehhanced and sophisticated cmd-line processing
    bool bDaemonize = false;
    g_VerbosityLevel = 0;
    for (int i = 1; i < argc; ++i)
    {
        const char *pszArg = argv[i];
        if (!strcasecmp(pszArg, "--daemon") || !strcasecmp(pszArg, "-d"))
            bDaemonize = true;
        if (!strcasecmp(pszArg, "--verbose") || !strcasecmp(pszArg, "-v"))
            g_VerbosityLevel = std::max(1, g_VerbosityLevel); // do not reduce level in case of ambiguosity
        if (!strcasecmp(pszArg, "--vv") || !strcasecmp(pszArg, "-vv"))
            g_VerbosityLevel = std::max(2, g_VerbosityLevel); // do not reduce level in case of ambiguosity
        if (!strcasecmp(pszArg, "--vvv") || !strcasecmp(pszArg, "-vvv"))
            g_VerbosityLevel = std::max(3, g_VerbosityLevel); // do not reduce level in case of ambiguosity
        //  many options together like -abcdefghstuvwxyz possible in the future
        if (pszArg[0] == '-')
        {
            if (strchr(pszArg, 'd') || strchr(pszArg, 'D'))
                bDaemonize = true;
            if (strchr(pszArg, 'v') || strchr(pszArg, 'V'))
                g_VerbosityLevel = std::max(1, g_VerbosityLevel); // do not reduce level in case of ambiguosity
        }
    }
    if (bDaemonize)
        g_VerbosityLevel = 0; // silent in daemon mode

    int bLogToStderr = false;
    if (g_VerbosityLevel > 0)
        bLogToStderr = true;

    log_init(bLogToStderr);

    Configuration cfg(PSZ_CFG_FILE_PATH);
    if (!cfg.load())
    {
        log_error("Cannot read cfg-file %s", PSZ_CFG_FILE_PATH);
        return 1;
    }

    std::string logLevelString = cfg.get(PSZ_SYSLOG_VERBOSE_LEVEL, "8"); // log everything by default
    int logLevel = atoi(logLevelString.c_str());
    if (logLevel == 0)
        logLevel = 8; // if invalid value - log everything
    log_level(logLevel);

    log_info("Version %s, compiled on %s at %s", PSZ_VERSION, __DATE__, __TIME__);

    // get myxID
    std::string myxID = getMyxID();
    if (myxID.empty())
    {
        log_error("Cannot get MYX_ID");
        return 1;
    }
    log_info("MyxID = %s", myxID.c_str());
    std::string keyForRestProxy = "\"key\" : \"" + myxID + "\",";

    time_t basicDelay = cfg.getLong(PSZ_BASIC_QUERY_DELAY, 60);
    if (basicDelay < 10)
        basicDelay = 10;
    if (basicDelay > 7200)
        basicDelay = 7200;
    log_info("Basic delay: %d seconds", basicDelay);

    FirmwareUpgrader &FWupgrader = FirmwareUpgrader::instance();
    FWupgrader.configure(cfg);

    std::string deviceNameTemplate = cfg.get(PSZ_DEVICE_NAME, "/dev/ttyACM0");
    // remove tailing digits for backward compatibility (to be able to work with existing cfg-files, device index 0...<N> will be added later)
    deviceNameTemplate.erase(std::find_if(deviceNameTemplate.rbegin(), deviceNameTemplate.rend(), [](int c) { return !std::isdigit(c); }).base(), deviceNameTemplate.end());
    ModemGTC modem(deviceNameTemplate);

#ifndef PSEUDO_MODEM
    std::string trafficInterfaceName(PSZ_LTE_NETWORK_INTERFACE);
#else
    std::string trafficInterfaceName("enp0s3");    // my CENTOS/Debian laptop. TODO: set as compiler option, for example, by means of -D option (define directive), another than PSEUDO_MODEM
#endif
    TrafficCounter trafficCounter;
    trafficCounter.addInterface(trafficInterfaceName.c_str());

    LteValuesGroup::prepareCurrentState();

    CurlLib &kafkaRestProxy = CurlLib::instance();
    kafkaRestProxy.configure(cfg, bDaemonize);

    bool bKafkaConventionalEnabled = cfg.getBoolean(PSZ_KAFKA_ENABLED, "false");
    std::string kafkaBrokers = cfg.get(PSZ_KAFKA_BROKERS, PSZ_KAFKA_BROKERS_DEFAULT);
    if (bKafkaConventionalEnabled)
        log_info("Kafka brokers: %s", kafkaBrokers.c_str());
    std::string errstr;
    std::string kafkaTopic = cfg.get(PSZ_KAFKA_TOPIC, PSZ_KAFKA_TOPIC_DEFAULT);
    log_info("Kafka topic: %s", kafkaTopic.c_str());

    RdKafka::Conf *pConf = nullptr;
    if (bKafkaConventionalEnabled)
        pConf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *pTCconf = nullptr;
    if (bKafkaConventionalEnabled)
        pTCconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    if (pConf)
        pConf->set("metadata.broker.list", kafkaBrokers, errstr);

    RdKafka::Producer *pProducer = nullptr;
    if (bKafkaConventionalEnabled)
    {
        pProducer = RdKafka::Producer::create(pConf, errstr);
        if (!pProducer)
        {
            log_error("Failed to create producer: %s\n", errstr.c_str());
            return 1;
        }
    }

    RdKafka::Topic *pTopic = nullptr;
    if (bKafkaConventionalEnabled)
    {
        pTopic = RdKafka::Topic::create(pProducer, kafkaTopic, pTCconf, errstr);
        if (!pTopic)
        {
            log_error("Failed to create topic: %s", errstr.c_str());
            return 1;
        }
    }

    cfg.free(); // as we read mdm.cfg file now, it is not a very good idea to keep all its long list of parameters permanently

    std::string startedAs("Started");
    if (bDaemonize)
    {
        // first param: '0' - change working dir to ~/, '1' - do not change, leave it "as is",
        // second param: '0' - nullify standard file descriptors, '1' - leave them "as is"
        int errorCode = daemon(1, 0);
        if (errorCode != 0)
        {
            log_error("Failed to daemonize itself");
            return 1;
        }
        startedAs += " as a daemon";
    }
    startedAs += ", PID = %d";
    // else ordinary program
    log_info(startedAs.c_str(), getpid());

    allGroups.emplace_back(new NetworkParameterGroup()); // must be first in the list for easier enforcement full re-quering in case of connection type changed - other groups are processed later
    allGroups.emplace_back(new CommonParameterGroup());
    allGroups.emplace_back(new ModemControlParameterGroup(modem));
    allGroups.emplace_back(new ConstantModemParameterGroup(modem));
    allGroups.emplace_back(new VariableModemParameterGroup(modem));
    allGroups.emplace_back(new TrafficParameterGroup(trafficCounter));
    //allGroups.emplace_back(new WanSwitchStateGroup());
    allGroups.emplace_back(new OomaServiceStatusGroup());

    JsonContent queryResult;
    int nCyclesDone = 0;
    std::string fwTgzFilePath; // tgz file with real firmware file

    time_t fullUpdateLastTime = ::getCurrentTimeSec();
    while (needToContinue(nCyclesDone))
    {
        if (!modem.isConnected())
            modem.connect();

        trafficCounter.doJob();

        queryResult.clear();

        time_t currentTime = ::getCurrentTimeSec();
        if ((currentTime - fullUpdateLastTime) >= 3600)
        {
            fullUpdateLastTime = currentTime;
            LteValuesGroup::bHourlyFullUpdateRequired_ = true;
            log_info("Scheduled full update");
        }
        for (auto pGroup : allGroups)
            pGroup->get(basicDelay, queryResult);
        LteValuesGroup::bHourlyFullUpdateRequired_ = false;

        if (!queryResult.empty())
        {
            queryResult.emplace_back(KeyValue("version", PSZ_JSON_VERSION));
            queryResult.emplace_back(KeyValue("myx_id", myxID));
            std::string json;
            toJSON(queryResult, json);

            if (g_VerbosityLevel > 0)
                fprintf(stderr, "%s\n", json.c_str());

            LteValuesGroup::bSendOK_ = true;
            if (kafkaRestProxy.isPostEnabled()) // kafka REST proxy
            {
                std::string kafkaProxyJson = "{\"records\":[{";
                kafkaProxyJson += keyForRestProxy;
                kafkaProxyJson += " \"value\" : ";
                kafkaProxyJson += json;
                kafkaProxyJson += "}]}";
                //fprintf(stderr, "%s\n", kafkaProxyJson.c_str());
                if (!kafkaRestProxy.post(kafkaProxyJson))
                    LteValuesGroup::bSendOK_ = false;
            }

            if (bKafkaConventionalEnabled) // conventional kafka
            {
                if (pProducer)
                    pProducer->produce(pTopic, RdKafka::Topic::PARTITION_UA,
                       RdKafka::Producer::RK_MSG_COPY,
                       (char *)json.c_str(),
                       json.size(),
                       nullptr,
                       nullptr);
            }
            LteValuesGroup::bConnectionTypeChanged_ = false; // clear this flag as everything is re-queried already
        }

        LteValuesGroup::bFirmwareUpdated_ = false; // clear this flag before checking for upgrade
        if (FWupgrader.isUpgradeAvailable(fwTgzFilePath))
        {
            if (FWupgrader.unpackPrgFile(fwTgzFilePath))
            {
                trafficCounter.stopListening();
                bool bUpdateSuccess = false;
                if (FWupgrader.upgrade(modem))
                {
                    log_info("Firmware upgrade is done");
                    log_info("Disconnecting dongle");
                    LteValuesGroup::bFirmwareUpdated_ = true; // to enforce querying of all parameters directly after FW update
                    modem.disconnect();
                    bUpdateSuccess = true;
                }
                // report status of upgrade to myxprov
                std::string fileName = PSZ_FW_UPGRADE_MARKER_PATH;
                fileName += PSZ_FW_UPGRADE_MARKER_FILE;
                FILE *pFile = fopen(fileName.c_str(), "a");
                if (pFile)
                {
                    fprintf(pFile, "\nstatus=%s\n", bUpdateSuccess ? "done" : "failed");
                    fclose(pFile);
                }
                else
                    log_error("Cannot open file %s%s for writing", PSZ_FW_UPGRADE_MARKER_PATH, PSZ_FW_UPGRADE_MARKER_FILE);

                if (bUpdateSuccess)
                    sleep(60); // wait while modem is rebooting
            }
        }

        for (int i = 0; i < 5; ++i)
        {
            bool bFullUpgradeRequired = false;
            for (auto pGroup : allGroups)
            {
                if (pGroup->isFullUpdateRequired())
                    bFullUpgradeRequired = true;
            }
            if (bFullUpgradeRequired)
                break;
            sleep(1);
        }
#ifdef VALGRIND
        ++nCyclesDone;
        printf("VALGRIND: %d cycles done\n", nCyclesDone);
#endif
    }

    for (auto pGroup : allGroups)
        delete pGroup;

    allGroups.clear();

    // clear all Kafka stuff
    if (pProducer)
        delete pProducer;
    if (pTCconf)
        delete pTCconf;
    if (pConf)
        delete pConf;
    if (pTopic)
        delete pTopic;

    log_info("Finished");

    return 0;
}

