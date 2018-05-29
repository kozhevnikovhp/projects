#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <librdkafka/rdkafkacpp.h>
#include <iostream>

#include <modem-gct.h>
#define GTC 1 // 1 means "using helper class", 0 means "using raw communication"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("USAGE: %s <UsbDeviceName>\n", argv[0]);
       // return 1;
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
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "% Created producer " << producer->name() << std::endl;

    /*
    * Create topic handle.
    */
    RdKafka::Topic *topic = NULL;
    if (!topic_str.empty())
    {
        topic = RdKafka::Topic::create(producer, topic_str, tconf, errstr);
        if (!topic)
        {
            std::cerr << "Failed to create topic: " << errstr << std::endl;
            exit(1);
        }
    }

    char szString[128];
    for (int i = 0; i < 3; ++i)
    {
        sprintf(szString, "%d: hello, Kafka, from C++!", i+1);
        RdKafka::ErrorCode resp = producer->produce(topic, partition,
           RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
           szString, strlen(szString),
           NULL, NULL);
        //printf("Error code %d\n", resp);
        sleep(5);
    }
    RdKafka::ErrorCode resp = producer->produce(topic, partition,
       RdKafka::Producer::RK_MSG_COPY,
       (char *)"\r\n", 2,
       NULL, NULL);

    char *pszDevice = argv[1];

    int nCount = 0;
    bool bSuccess = true;
    ModemGCT modem(pszDevice);

    std::string json;
    JsonContent content;

    while (bSuccess)
    {
#if GTC
        content.clear();
        bSuccess = modem.getStatus(content);
        bSuccess != modem.getCgdCont(content);
        bSuccess != modem.getBand(content);
        bSuccess != modem.getAttStatus(content);
        bSuccess != modem.getActStatus(content);
        bSuccess != modem.getCops(content);
        bSuccess != modem.getCgpAddr(content);

        bSuccess |= toJSON(content, json);
        printf("%s", json.c_str());

        RdKafka::ErrorCode resp = producer->produce(topic, partition,
           RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
           (char *)json.c_str(), strlen(json.c_str()),
           NULL, NULL);


#else
        bSuccess = getStatusGTC(pszDevice, reply);
        printf("%s", reply.c_str());
#endif

        sprintf(szString, "\t***** %d times done *****\n", ++nCount);
        producer->produce(topic, partition,
           RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
           (char *)szString, strlen(szString),
           NULL, NULL);

        printf(szString);
        sleep(5);
    }


    delete topic;
    delete producer;
    return 0;
}
// statistics
//https://stackoverflow.com/questions/4951257/using-c-code-to-get-same-info-as-ifconfig?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
//
/*int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("USAGE: %s <UsbDeviceName>\n", argv[0]);
        return 1;
    }

    char *pszDevice = argv[1];

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

        sleep(1);

        printf("closing...");
        ::close(fd);
        printf("closed\n");

        printf("\t***** %d times done *****\n", ++nCount);
        sleep(3);
    }

    return 0;
}
*/


/*int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("USAGE: %s <UsbDeviceName>\n", argv[0]);
       // return 1;
    }

    char *pszDevice = argv[1];

    int nCount = 0;
    bool bSuccess = true;
#if GTC
    ModemGCT modem(pszDevice);
    std::string sss;
    bool b = modem.getManufacturerInfo(sss);
    printf("success = %d\n%s", b, sss.c_str());
    //sleep(1);
    //b = modem.getCops(sss);
    //printf("success = %d\n%s", b, sss.c_str());
    //b = modem.getCgpAddr(sss);
    //printf("success = %d\n%s", b, sss.c_str());
    //b = modem.getActStatus(sss);
    //printf("success = %d\n%s", b, sss.c_str());
    //modem.getStatusJSON(sss);
    //printf("success = %d\n%s", b, sss.c_str());
    //modem.getAttStatusJSON(sss);
    //printf("success = %d\n%s", b, sss.c_str());
    //sleep(3);
    return 0;
#endif
    std::string reply, json;
    JsonContent content;


    while (bSuccess)
    {
#if GTC
        content.clear();
        bSuccess = modem.getStatus(content);
        bSuccess != modem.getCgdCont(content);
        bSuccess != modem.getBand(content);
        bSuccess != modem.getAttStatus(content);
        bSuccess != modem.getActStatus(content);
        bSuccess != modem.getCops(content);
        bSuccess != modem.getCgpAddr(content);

        bSuccess |= toJSON(content, json);
        printf("%s", json.c_str());

#else
        bSuccess = getStatusGTC(pszDevice, reply);
        printf("%s", reply.c_str());
#endif

        printf("\t***** %d times done *****\n", ++nCount);
        sleep(1);
    }

    return 0;
}
*/

