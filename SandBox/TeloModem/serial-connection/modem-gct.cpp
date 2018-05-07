#include "modem-gct.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>

#define PSEUDO_COMM 1

ModemGCT::ModemGCT(const char *pszDevice)
{
    connection_.open(pszDevice);
    connection_.setSpeed(B115200);
}

bool ModemGCT::execute(const std::string &command, std::string &reply)
{
    reply.clear();

    if (!connection_.isOpen())
        return false;
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

bool ModemGCT::getStatusRaw(std::string &status)
{
    status = "Current Time : 0	Mode : ONLINE \n\
              System mode : LTE	PS state : Attached \n\
              LTE band : B41	LTE bw : 20MHz \n\
              LTE Rx chan : 41176	LTE Tx chan : 41176 \n\
              EMM state : 0	RRC state : RRC IDLE \n\
              IMS reg state : No Srv \n\
              RSSI (dBm) : -44	Tx Power : -550 \n\
              RSRP (dBm) : -70	TAC : 223(3) \n\
              RSRQ (dBm) : -6.0	Cell ID : 262234244 \n\
              SINR (dB) : 34 \n\
              ";
    return true;
    //return execute("at!gstatus", status);
}

bool ModemGCT::getStatusJSON(std::string &status)
{
    std::string statusRaw;
    bool bSuccess = getStatusRaw(statusRaw);
    if (!bSuccess)
    {
        status = "{\nerror\n}\n";
        return false;
    }

    return parseToJSON(statusRaw, status);
}

// trim from start (in place) (leading spaces)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !isspace(c); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !isspace(c); }).base(), s.end());
}

//static
bool ModemGCT::parseToJSON(const std::string &input, std::string &output)
{
    std::string str;
    char *pszInput = strdup(input.c_str()); // TODO: do something better to avoid strdup/free
    const char *pszSeparator = ":\n\r\t";

    output = "{\n";
    char *pszToken = strtok(pszInput, pszSeparator);
    bool bKey = true;
    while (pszToken)
    {
        str = pszToken;
        ltrim(str);
        rtrim(str);
        if (!str.empty())
        {
            if (bKey)
            {
                output += "\t";
                output += "{ \"";
                output += str;
                output += "\" = ";
                bKey = false;
            }
            else
            {
                output += "\"";
                output += str;
                output += "\"";
                output += " }";
                output += "\n";
                bKey = true;
            }
        }
        pszToken = strtok(NULL, pszSeparator);
    }
    output += "}\n";

    free(pszInput);
    return true;
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
