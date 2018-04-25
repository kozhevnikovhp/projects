#include <stdio.h>
#include <unistd.h>
#include <serial-connection.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("USAGE: %s <UsbDeviceName> <AT-command>\n", argv[0]);
        return 1;
    }

    char *pszDevice = argv[1];
    char *pszCommand = argv[2];

    int nCount = 0;
    int retCode = 0;
    while (retCode == 0)
    {
        retCode = runSerialDeviceCommand(pszDevice, pszCommand);
        printf("%d times done\n", ++nCount);
        usleep(3000);
    }

    return retCode;
}


