#include "counter.h"

#include <pcap.h>

int main(int argc, char* argv[])
{

    TrafficCounter counter;

    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
            counter.addInterface(argv[i]);
    }
    else
    {
        counter.addInterface("eth0");
    }

    int iResult = counter.doJob();

    return iResult;
}
