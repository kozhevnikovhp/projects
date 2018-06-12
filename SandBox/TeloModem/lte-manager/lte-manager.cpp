#include <stdio.h>
//#include <fcntl.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <unistd.h>
//#include <arpa/inet.h>

#include <librdkafka/rdkafkacpp.h>
//#include <iostream>

#include "lte-parameters.h"
#include "traffic-counter.h"

//const char *PSZ_IDENTIFICATION = "2267EE";
const char *PSZ_VERSION = "1";
const char *PSZ_MYX_ID_INTERFACE = "enp0s3"; // eth0 at TELO

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("USAGE: %s <UsbDeviceName> <Interface>\n", argv[0]);
        return 1;
    }

    // get myxID
    unsigned char ucMacAddress[8];
    if (!getInterfaceMacAddress(PSZ_MYX_ID_INTERFACE, ucMacAddress))
    {
        printf("Cannot get MYX_ID as MAC-address of interface %s\n", PSZ_MYX_ID_INTERFACE);
        return 1;
    }
    std::string myxID;
    for (int i = 3; i < 6; i++)
    {
        char szTmp[8];
        sprintf(szTmp, "%X", ucMacAddress[i]);
        myxID += szTmp;
    }

    char *pszDevice = argv[1];
    ModemGCT modem(pszDevice);

    char *pszInterface = argv[2];
    TrafficCounter trafficCounter;
    trafficCounter.addInterface(pszInterface);
    if (!trafficCounter.startListening())
    {
        printf("cannot start listening traffic\n");
        return 1;
    }

    std::string brokers = "10.0.2.15";
    std::string errstr;
    std::string topic_str = "lte-service";
    int32_t partition = RdKafka::Topic::PARTITION_UA;

    /*
    * Create configuration objects
    */
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer)
    {
        printf("Failed to create producer: %s\n", errstr.c_str());
        return 1;
    }

    printf("% Created producer\n", producer->name().c_str());

    /*
    * Create topic handle.
    */
    RdKafka::Topic *topic = RdKafka::Topic::create(producer, topic_str, tconf, errstr);
    if (!topic)
    {
        printf("Failed to create topic: %s\n", errstr.c_str());
        return 1;
    }


    std::vector<LteParameterGroup *> allGroups;
    allGroups.emplace_back(new ModemControlParameterGroup(modem));
    allGroups.emplace_back(new ConstantModemParameterGroup(modem));
    allGroups.emplace_back(new VariableModemParameterGroup(modem));
    allGroups.emplace_back(new NetworkParameterGroup(pszInterface));
    allGroups.emplace_back(new TrafficParameterGroup(trafficCounter));

    bool bNeedToContinue = true;
    JsonContent queryResult;

    while (bNeedToContinue)
    {
        trafficCounter.doJob();

        queryResult.clear();
        for (auto pGroup : allGroups)
        {
            pGroup->get(queryResult);
        }
        if (!queryResult.empty())
        {
            queryResult.emplace_back(KeyValue("version", PSZ_VERSION));
            queryResult.emplace_back(KeyValue("myx_id", myxID));
            std::string json;
            toJSON(queryResult, json);
            printf("%s\n", json.c_str());
            producer->produce(topic, partition,
                    RdKafka::Producer::RK_MSG_COPY, // Copy payload
                    (char *)json.c_str(), json.size(),
                    NULL, NULL);
        }
        sleep(1);
    }

    for (auto pGroup : allGroups)
    {
        delete pGroup;
    }
    allGroups.clear();

    return 0;
}
