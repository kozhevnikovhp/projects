#include <stdio.h>
#include "telnet-telo-le.h"

int main(int argc, char* argv[])
{
    if (argc < 1)
    {
        printf("Not enough arguments\n");
        return 0;
    }

    TelnetTeloLE teloLE(AF_INET);

    return 0;
}
