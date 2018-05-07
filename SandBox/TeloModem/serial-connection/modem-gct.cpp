#include "modem-gct.h"
#include "string.h"

ModemGCT::ModemGCT(const char *pszDevice)
{
    connection_.open(pszDevice);
    connection_.setSpeed(B115200);
}

bool ModemGCT::execute(const std::string &command, std::string &reply)
{
    reply.clear();

    if (!connection_.write(command.c_str(), command.size()))
        return false;
    const char *pszCRLF = "\r\n";
    if (!connection_.write(pszCRLF, strlen(pszCRLF)))
        return false;

    int nRead = 0;
    char szReply[256];
    if (!connection_.read(szReply, sizeof(szReply), 3000, nRead))
        return false;
    szReply[nRead] = 0;
    for (int i = 0; i < nRead; ++i)
    {
        char c = szReply[i];
        if (isalnum(c) || isspace(c) || ispunct(c) || c == 0x0A || c == 0x0D)
            reply += c;
    }
    return true;
}

bool ModemGCT::getStatus(std::string &status)
{
    return execute("at!gstatus", status);
}


bool getStatusGTC(const char *pszDevice, std::string &status)
{
    status.clear();
    SerialConnection connection;
    if (!connection.open(pszDevice))
        return false;

    if (!connection.setSpeed(B115200))
        return false;

    const char *pszCommand = "at!gstatus";
    if (!connection.write(pszCommand, strlen(pszCommand)))
        return false;
    const char *pszCRLF = "\r\n";
    if (!connection.write(pszCRLF, strlen(pszCRLF)))
        return false;

    int nRead = 0;
    char szReply[256];
    if (!connection.read(szReply, sizeof(szReply), 3000, nRead))
        return false;
    szReply[nRead] = 0;
    for (int i = 0; i < nRead; ++i)
    {
        char c = szReply[i];
        if (isalnum(c) || isspace(c) || ispunct(c) || c == 0x0A || c == 0x0D)
            status += c;
    }

    return true;
}
