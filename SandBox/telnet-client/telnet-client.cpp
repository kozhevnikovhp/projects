#include <stdio.h>
#include "telnet-gtc.h"

int main(int argc, char* argv[])
{
    if (argc < 1)
    {
        printf("Not enough arguments\n");
        return 0;
    }

    TelnetGTC telnetGTC(AF_INET);

    return 0;
}
