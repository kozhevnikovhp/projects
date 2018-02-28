#include "counter.h"

int main(int argc, char* argv[])
{
    std::string ifaceName = "eth0";//"enp0s3";
    if (argc > 1)
        ifaceName = argv[1];
    if (!isItInterfaceName(ifaceName))
    {
        printf("'%s' is not a valid interface name\n", ifaceName.c_str());
        return 1;
    }

    TrafficCounter counter(ifaceName);
    if (!counter.isCreated())
        return 1;
    if (!counter.listen())
        return 1;

    counter.reportStatistics(true);
    while (1)
    {
        if (counter.waitForPacket())
        {
        }
        counter.reportStatistics(false);
    }

    return 0;
}
