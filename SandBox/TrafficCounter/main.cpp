#include "counter.h"

#include <pcap.h>
#include <string.h>

int main(int argc, char* argv[])
{
    TrafficCounter counter;

    bool bDuration = false;
    for (int i = 1; i < argc; ++i)
    {
        if (!strcasecmp(argv[i], "-d"))
            bDuration = true;
        else if (bDuration)
        {
            int duration = atoi(argv[i]);
            counter.setDuration(duration);
            bDuration = false;
        }
        else
            counter.addInterface(argv[i]);
    }
    if (!counter.hasJob())
    {
        counter.addInterface("eth0");
    }

    int iResult = counter.doJob();

    return iResult;
}
