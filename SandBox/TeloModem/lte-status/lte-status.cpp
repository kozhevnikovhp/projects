#include <stdio.h>
#include <unistd.h>

#include "modem-gct.h"
#define GTC 1 // 1 means "using helper class", 0 means "using raw communication"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("USAGE: %s <UsbDeviceName>\n", argv[0]);
        return 1;
    }

    char *pszDevice = argv[1];

    int nCount = 0;
    bool bSuccess = true;
#if GTC
    ModemGCT modem(pszDevice);
#endif
    std::string status;

    while (bSuccess)
    {
#if GTC
        bSuccess = modem.getStatus(status);
#else
        bSuccess = getStatusGTC(pszDevice, status);
#endif
        printf("%s", status.c_str());

        printf("%d times done\n", ++nCount);
        usleep(3000);
    }

    return 0;
}


