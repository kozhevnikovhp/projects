#include <stdio.h>
#include <unistd.h>

#include <librdkafka/rdkafkacpp.h>
#include "lte-parameters.h"
#include "traffic-counter.h"

const char *PSZ_VERSION = "1";
int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("USAGE: %s <LteModemDeviceName> <LteNetworkInterfaceName> <myxInterfaceName> <KafkaClusterIP>\n", argv[0]);
        return 1;
    }

    // get myxID
    char *pszMyxInterface = argv[3];
    unsigned char ucMacAddress[8];
    if (!getInterfaceMacAddress(pszMyxInterface, ucMacAddress))
    {
        printf("Cannot get MYX_ID as MAC-address of interface %s\n", pszMyxInterface);
        return 1;
    }
    std::string myxID;
    char szTmp[8];
    sprintf(szTmp, "%X", ucMacAddress[3]);
    myxID += szTmp;
    sprintf(szTmp, "%X", ucMacAddress[4]);
    myxID += szTmp;
    sprintf(szTmp, "%X", ucMacAddress[5]-1);
    myxID += szTmp;

    char *pszDevice = argv[1];
    ModemGTC modem(pszDevice);

    char *pszInterface = argv[2];
    TrafficCounter trafficCounter;
    trafficCounter.addInterface(pszInterface);
    if (!trafficCounter.startListening())
    {
        printf("Cannot start listening traffic\n");
        return 1;
    }

    std::string brokers = argv[4];
    printf("Kafka brokers: %s\n", brokers.c_str());
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

    printf("Created producer %s\n", producer->name().c_str());

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
                    RdKafka::Producer::RK_MSG_COPY,
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
