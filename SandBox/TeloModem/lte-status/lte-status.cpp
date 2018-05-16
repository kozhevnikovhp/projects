#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <modem-gct.h>
#define GTC 1 // 1 means "using helper class", 0 means "using raw communication"

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

int main(int argc, char *argv[])
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
    //bool b = modem.getManufacturerInfo(sss);
    //printf("success = %d\n%s", b, sss.c_str());
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
    //return 0;
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

      /*  bSuccess = modem.getStatusJSON(reply);
        printf("%s", reply.c_str());
        modem.getCgdContJSON(reply);
        printf("%s", reply.c_str());
        modem.getBandJSON(reply);
        printf("%s", reply.c_str());
        modem.getAttStatusJSON(reply);
        printf("%s", reply.c_str());
        modem.getActStatusJSON(reply);
        printf("%s", reply.c_str());
        */

#else
        bSuccess = getStatusGTC(pszDevice, reply);
        printf("%s", reply.c_str());
#endif

        printf("\t***** %d times done *****\n", ++nCount);
        sleep(1);
    }

    return 0;
}


