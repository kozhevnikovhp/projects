/*
 *  lte-manager.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <librdkafka/rdkafkacpp.h>

#include "lte-parameters.h"
#include "traffic-counter.h"
#include "config.h"
#include "const.h"
#include "log.h"

#include "kafka-rest-proxy.h"

static const char *PSZ_VERSION = "1";

int main(int argc, char *argv[])
{
    // to daemonize myself, or not to daemonize?
    // quick and dirty - just iterate through the list of cmd-line args looking for -d option, meaning daemonizing.
    // TODO: make something better with ehhanced and sophisticated cmd-line processing
    bool bDaemonize = false;
    bool bVerbose = false;
    for (int i = 1; i < argc; ++i)
    {
        const char *pszArg = argv[i];
        if (!strcasecmp(pszArg, "--daemon"))
            bDaemonize = true;
        if (!strcasecmp(pszArg, "--verbose"))
            bVerbose = true;
        //  many options together like -abcdefghstuvwxyz possible in the future
        if (pszArg[0] == '-')
        {
            if (strchr(pszArg, 'd') || strchr(pszArg, 'D'))
                bDaemonize = true;
            if (strchr(pszArg, 'v') || strchr(pszArg, 'V'))
                bVerbose = true;
        }
    }
    if (bDaemonize)
        bVerbose = false; // silent in daemon mode

    int logToStderr = 0;
    if (bVerbose)
        logToStderr = 1;

    log_init(logToStderr);
    log_level(1);

    Configuration cfg(PSZ_CFG_FILE_PATH);
    if (!cfg.load())
        cfg.createDefaultFile();

    std::string basicDelayString = cfg.get(PSZ_BASIC_QUERY_DELAY, "60");
    unsigned int basicDelay = (unsigned int)atoi(basicDelayString.c_str());
    if (basicDelay < 10)
        basicDelay = 10;
    if (basicDelay > 7200)
        basicDelay = 7200;
    log_info("Basic delay %d seconds", basicDelay);

    // get myxID
    std::string myxInterfaceName = cfg.get(PSZ_MYXID_INTERFACE, "eth0");
    unsigned char ucMacAddress[8];
    if (!getInterfaceMacAddress(myxInterfaceName, ucMacAddress))
    {
        log_error("Cannot get MYX_ID as MAC-address of interface %s", myxInterfaceName.c_str());
        return 1;
    }
    char szTmp[32];
    sprintf(szTmp, "%02X%02X%02X", ucMacAddress[3], ucMacAddress[4], ucMacAddress[5]-1);
    std::string myxID(szTmp);

    std::string deviceName = cfg.get(PSZ_DEVICE_NAME, "/dev/ttyACM0");
    ModemGTC modem(deviceName);

    std::string trafficInterfaceName = cfg.get(PSZ_TRAFFIC_INTERFACE, "usbnet0");
    TrafficCounter trafficCounter;
    trafficCounter.addInterface(trafficInterfaceName.c_str());
    if (!trafficCounter.startListening())
    {
        log_error("Cannot start listening traffic of %s", trafficInterfaceName.c_str());
        return 1;
    }

    KafkaRestProxy &kafkaRestProxy = KafkaRestProxy::instance();
    kafkaRestProxy.configure(cfg);

    bool bKafkaConventionalEnabled = cfg.getBoolean(PSZ_KAFKA_ENABLED, "false");
    std::string kafkaBrokers = cfg.get(PSZ_KAFKA_BROKERS, "52.53.80.222:9092");
    log_info("Kafka brokers: %s", kafkaBrokers.c_str());
    std::string errstr;
    std::string kafkaTopic = cfg.get(PSZ_KAFKA_TOPIC, "lte-service");
    log_info("Kafka topic: %s\n", kafkaTopic.c_str());

    RdKafka::Conf *pConf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *pTCconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    pConf->set("metadata.broker.list", kafkaBrokers, errstr);

    RdKafka::Producer *pProducer = RdKafka::Producer::create(pConf, errstr);
    if (!pProducer)
    {
        log_error("Failed to create producer: %s\n", errstr.c_str());
        return 1;
    }

    RdKafka::Topic *pTopic = RdKafka::Topic::create(pProducer, kafkaTopic, pTCconf, errstr);
    if (!pTopic)
    {
        log_error("Failed to create topic: %s", errstr.c_str());
        return 1;
    }

    if (bDaemonize)
    {
        // first '0' - change working dir to ~/, '1' - do not change, leave it "as is",
        // second '0' - nullify standard file descriptors, '1' - leave them "as is"
        int errorCode = daemon(1, 0);
        if (errorCode != 0)
        {
            log_error("Failed to daemonise itself");
            return 1;
        }
    }
    // else ordinary program


    std::vector<LteValuesGroup *> allGroups;
    allGroups.emplace_back(new ModemControlParameterGroup(modem));
    allGroups.emplace_back(new ConstantModemParameterGroup(modem));
    allGroups.emplace_back(new VariableModemParameterGroup(modem));
    allGroups.emplace_back(new NetworkParameterGroup(trafficInterfaceName));
    allGroups.emplace_back(new TrafficParameterGroup(trafficCounter));
    allGroups.emplace_back(new WanSwitchStateGroup());

    bool bNeedToContinue = true;
    JsonContent queryResult;

    while (bNeedToContinue)
    {
        trafficCounter.doJob();

        queryResult.clear();

        for (auto pGroup : allGroups)
            pGroup->get(basicDelay, queryResult);

        if (!queryResult.empty())
        {
            queryResult.emplace_back(KeyValue("version", PSZ_VERSION));
            queryResult.emplace_back(KeyValue("myx_id", myxID));
            std::string json;
            toJSON(queryResult, json);

            if (bVerbose)
                fprintf(stderr, "%s\n", json.c_str());

            if (kafkaRestProxy.isEnabled()) // kafka REST proxy
            {
                std::string kafkaProxyJson = "{\"records\":[{\"value\":";
                kafkaProxyJson += json;
                kafkaProxyJson += "}]}";
                //printf("%s\n", kafkaJson.c_str());
                kafkaRestProxy.post(kafkaProxyJson);
            }

            if (bKafkaConventionalEnabled) // conventional kafka
            {
                pProducer->produce(pTopic, RdKafka::Topic::PARTITION_UA,
                    RdKafka::Producer::RK_MSG_COPY,
                    (char *)json.c_str(),
                    json.size(),
                    nullptr,
                    nullptr);
            }
        }
        sleep(10);
        bNeedToContinue = true; // TODO: check for interruption
    }

    for (auto pGroup : allGroups)
        delete pGroup;

    allGroups.clear();

    return 0;
}

// TELO KILLER
/*
int main(int argc, char *argv[])
{
    printf("argc = %d\n", argc);
    if (argc < 2)
    {
        printf("USAGE: %s <UsbDeviceName> <timeout_in_seconds>\n", argv[0]);
        return 1;
    }

    char *pszDevice = argv[1];
    int timeout = 1;
    if (argc > 2)
        timeout = atoi(argv[2]);

    int nCount = 0;
    bool bSuccess = true;

    while (bSuccess)
    {
        printf("opening...");
        int fd = ::open(pszDevice, O_RDWR | O_NOCTTY);
        if (fd < 0)
        {
            fprintf(stderr, "error, counldn't open file %s\n", pszDevice);
            perror("open");
            return 1;
        }
        printf("done successfully\n");
        printf("sleeping %d seconds before closing\n", timeout);
        sleep(timeout);

        printf("closing...");
        ::close(fd);
        printf("closed\n");

        printf("\t***** %d times done *****\n", ++nCount);
        printf("sleeping %d seconds before re-opening\n", timeout);
        sleep(timeout);
    }

    return 0;
}
*/

