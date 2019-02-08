/*
 *  fw-upgrade.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/stat.h>
#include <sys/poll.h>

#include <algorithm>
#include <fstream>
#include <regex>

#include "const.h"
#include "fw-upgrade.h"
#include "config.h"
#include "curl-lib.h"
#include "modem-gtc.h"
#include "log.h"
#include "misc.h"

//static
FirmwareUpgrader &FirmwareUpgrader::instance()
{
    static FirmwareUpgrader s;
    return s;
}

FirmwareUpgrader::FirmwareUpgrader()
{
    state_ = FWU_IDLE;
    notifyDesc_ = inotify_init();
    if (notifyDesc_ < 0)
        log_error("Error initializing inotify (%s)", strerror(errno));

    // ensure that directories exist
    bool bExists = makeDirRecursively(PSZ_FW_UPGRADE_MARKER_PATH, S_IRWXU | S_IRWXG | S_IRWXO);
    if (!bExists)
        log_error("Cannot create directory %s (%s)", PSZ_FW_UPGRADE_MARKER_PATH, strerror(errno));
    bExists = makeDirRecursively(PSZ_FW_UPGRADE_PATH, S_IRWXU | S_IRWXG | S_IRWXO);
    if (!bExists)
        log_error("Cannot create directory %s (%s)", PSZ_FW_UPGRADE_PATH, strerror(errno));

    watchDesc_ = inotify_add_watch(notifyDesc_, PSZ_FW_UPGRADE_MARKER_PATH, IN_CLOSE_WRITE);
    if (watchDesc_ < 0)
    {
        log_error("Cannot monitor %s for firmware upgrade (%s)", PSZ_FW_UPGRADE_MARKER_PATH, strerror(errno));
        close(notifyDesc_);
        notifyDesc_ = -1;
    }
    else
        log_info("Waiting for LTE-dongle firmware upgrade at %s", PSZ_FW_UPGRADE_MARKER_PATH);
}

//virtual
FirmwareUpgrader::~FirmwareUpgrader()
{
    if (watchDesc_ > 0)
        inotify_rm_watch(notifyDesc_, watchDesc_);

    if (notifyDesc_ > 0)
        close(notifyDesc_);
}

void FirmwareUpgrader::configure(const Configuration &cfg)
{
}

// gets the path to tgz file
bool FirmwareUpgrader::isUpgradeAvailable(std::string &tgzFilePath)
{
    tgzFilePath.empty();
    if (notifyDesc_ < 0 || watchDesc_ < 0)
        return false;

    std::string markerFilePath(PSZ_FW_UPGRADE_MARKER_PATH);
    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = notifyDesc_;
    fds.events = POLLIN;

    bool bUpdateAvailable = false;
    int ec = poll(&fds, 1, 100); // 0 means "timeout expired" -> do nothing
    if (ec > 0)
    {
        if (fds.revents == POLLIN)
        {
            long length = ::read(notifyDesc_, events_, BUF_LEN);
            if (length < 0)
            {
                log_error("Error reading inotify file descriptor (%s)\n", strerror(errno));
                return false;
            }

            long i = 0;
            while (!bUpdateAvailable && (i < length)) //until first file, matching file name rules, found
            {
                struct inotify_event *pEvent = (struct inotify_event *)(events_+i);
                if (pEvent->len)
                {
                    if (pEvent->mask & IN_CLOSE_WRITE)
                    {
                        // pEvent->name is newly created file name
                        // check it, it can be not FW upgrade marker file but stranger
                        if (strcasecmp(pEvent->name, PSZ_FW_UPGRADE_MARKER_FILE) == 0)
                        {
                            markerFilePath += pEvent->name;
                            bUpdateAvailable = getTgzFileName(markerFilePath, tgzFilePath);
                        }
                    }
                }
                i += EVENT_SIZE + pEvent->len;
            }
            fds.revents = 0;
        }
    }

    if (bUpdateAvailable)
        log_info("File %s detected, archive is %s\n", markerFilePath.c_str(), tgzFilePath.c_str());

    return bUpdateAvailable;
}

// reads marker file and extracts TgzFileName from it. Returns false if substring 'status=' exists there - it indicates that marker file was edited by me
bool FirmwareUpgrader::getTgzFileName(const std::string &markerFilePath, std::string &tgzFilePath) const
{
    std::ifstream f(markerFilePath);
    if (!f.is_open())
    {
        log_error("Cannot open file %s\n", markerFilePath.c_str());
        return false;
    }

    // File format does not look stable, so iterate over tokens until it is a filename, probably followed by status
    std::string token;
    bool bStatusFound = false;
    char c;
    while (!f.eof() && !f.bad() && !f.fail() && f.get(c))
    {
        if (c != '\n' && c != ' ' && c != '\t' && c != '\r')
            token += c;
        else if (!token.empty())
        { // delimeter
            if (tgzFilePath.empty())
            { // not found yet
                if (access(token.c_str(), F_OK) != -1)
                    tgzFilePath = token; // file exists
            }
            else
            { // status
                if (token.find("status=") != std::string::npos)
                    bStatusFound = true;
            }
            token.clear();
        }
    }

    if (bStatusFound)
        return false; // status is created by me to inform myxprov, just wait for another file

    if (tgzFilePath.empty())
    {
        log_error("Cannot get path to tgz-file with upgrade");
        return false;
    }

    return true;
}

bool FirmwareUpgrader::checkPrgFileName(const char *pszFileName) const
{
    // dummy implementation, just get file extension and checks if it is exactly .prg (case insensitively)
    // TODO: invent something more sophisticated according to the file format, like version_number_date.prg, etc
    const char *pszExtension = strrchr(pszFileName, '.');
    if (!pszExtension)
        return false;
    return (strcasecmp(pszExtension, ".prg") == 0);
}

bool FirmwareUpgrader::unpackPrgFile(const std::string &tgzFilePath)
{
    log_info("Unpack file %s to %s", tgzFilePath.c_str(), PSZ_FW_UPGRADE_PATH);
    std::string commandLine = "tar -xzf ";
    commandLine += tgzFilePath;
    commandLine += " -C ";
    commandLine += PSZ_FW_UPGRADE_PATH; // where to unzip
    log_info(commandLine.c_str());

    // TODO: replace system() call with something more secure like popen() or fork()/dup2(). Ask Egor.
    int ec = system(commandLine.c_str());
    if (ec != 0)
    {
        log_error("Could not unpack file %s to folder %s, error code %d\n", tgzFilePath.c_str(), PSZ_FW_UPGRADE_PATH, ec);
        return false;
    }

    return true;
}

bool FirmwareUpgrader::upgrade(ModemGTC &modem)
{
    DIR *pDir = opendir(PSZ_FW_UPGRADE_PATH);
    if (!pDir)
    {
        log_error("Cannot read directory %s for firmware upgrade file", PSZ_FW_UPGRADE_PATH);
        return false;
    }
    struct dirent *pDirEnt;

    std::string fwFileName;
    while ((pDirEnt = readdir(pDir)))
    {
        if (pDirEnt->d_type != DT_REG)
            continue;
        if (checkPrgFileName(pDirEnt->d_name))
            fwFileName = pDirEnt->d_name;
    }
    closedir(pDir);
    if (fwFileName.empty())
    {
        log_error("Could not find .prg file for firmware upgrade in directory %s", PSZ_FW_UPGRADE_PATH);
        return false;
    }

    // File is discovered
    // TODO: check CRC and version, they are in corresponding files

    // 1.5 Rename file (modem does not like complicated and long file names now...)
    const char *PSZ_SIMPLE_FILE_NAME = "new.prg";
    std::string oldFilePath(PSZ_FW_UPGRADE_PATH);
    oldFilePath += fwFileName;
    std::string fwFileFullPath(PSZ_FW_UPGRADE_PATH);
    fwFileFullPath += PSZ_SIMPLE_FILE_NAME;
    log_info("Rename file %s -> %s", oldFilePath.c_str(), fwFileFullPath.c_str());
    int ec = ::rename(oldFilePath.c_str(), fwFileFullPath.c_str());
    if (ec != 0)
    {
        log_error("Cannot rename file %s -> %s, error code %d"); // TODO: text error desc instead of error code
        return false;
    }

    // Ensure that usbnet interface is UP
    bool bPreviouslyUp = false;
    bool bInterfaceUpSuccess = isInterfaceUp(PSZ_LTE_NETWORK_INTERFACE, bPreviouslyUp);
    if (bInterfaceUpSuccess)
    { // interface status obtained successfully
        if (bPreviouslyUp)
            log_info("Interface %s is UP", PSZ_LTE_NETWORK_INTERFACE);
        else
        {
            log_info("Trying to bring interface %s UP", PSZ_LTE_NETWORK_INTERFACE);
            if (setInterfaceUp(PSZ_LTE_NETWORK_INTERFACE))
                log_info("Interface %s is brought UP", PSZ_LTE_NETWORK_INTERFACE);
            else
                log_error("Cannot bring interface %s UP", PSZ_LTE_NETWORK_INTERFACE);
            sleep(5);
        }
    }
    else
        log_error("Cannot get the status of LTE network interface %s", PSZ_LTE_NETWORK_INTERFACE);

    // ensure that LTE-interface and TFTP-server is in the same subnet
    const char *PSZ_TFTP_SERVER = "192.168.0.1";
    IPADDRESS_TYPE tftpIP = dotNotationToAddress(PSZ_TFTP_SERVER);
    IPADDRESS_TYPE ip = 0, mask = 0;
    bool bIpMaskSuccess = getInterfaceAddressAndMask(PSZ_LTE_NETWORK_INTERFACE, ip, mask);
    if (bIpMaskSuccess)
        log_info("Interface %s: IP-address is %s, mask is %s", PSZ_LTE_NETWORK_INTERFACE, addressToDotNotation(ip).c_str(), addressToDotNotation(mask).c_str());
    else
        log_error("Cannot get the IP-address and mask of LTE network interface %s", PSZ_LTE_NETWORK_INTERFACE);

    if (!bIpMaskSuccess || (ip == 0) || (mask == 0) || !isTheSameSubnet(ip, tftpIP, mask))
    {
        // set IP-address and mask
        const char *PSZ_INTERFACE_IP_ADDRESS = "192.168.0.2";
        if (setInterfaceIpAddress(PSZ_LTE_NETWORK_INTERFACE, dotNotationToAddress(PSZ_INTERFACE_IP_ADDRESS)))
            log_info("IP-address %s assigned to interface %s", PSZ_INTERFACE_IP_ADDRESS, PSZ_LTE_NETWORK_INTERFACE);
        else
            log_error("Cannot assign IP-address to interface %s", PSZ_LTE_NETWORK_INTERFACE);

        const char *PSZ_INTERFACE_MASK = "255.255.255.0";
        IPADDRESS_TYPE lteMask = dotNotationToAddress(PSZ_INTERFACE_MASK);
        if (mask != lteMask)
        {
            if (setInterfaceMask(PSZ_LTE_NETWORK_INTERFACE, lteMask))
                log_info("Subnet mask %s assigned to interface %s", PSZ_INTERFACE_MASK, PSZ_LTE_NETWORK_INTERFACE);
            else
                log_error("Cannot assign subnet mask to interface %s", PSZ_LTE_NETWORK_INTERFACE);
        }
    }

    // 2. upload .prg file to modem's memory by TFTP protocol
    CurlLib &curl = CurlLib::instance();
    log_info("Uploading file %s to LTE-dongle's TFTP-server (%s)", fwFileFullPath.c_str(), PSZ_TFTP_SERVER);
    bool bTftpSuccess = curl.putFileTFTP(fwFileFullPath, PSZ_TFTP_SERVER);
    if (!bTftpSuccess)
        log_error("Could not upload file %s to LTE-dongle's TFTP-server (%s)", fwFileFullPath.c_str(), PSZ_TFTP_SERVER);

    // Restore usb-interface to its previous state if necessary
    if (bInterfaceUpSuccess && !bPreviouslyUp)
    {
        if (putInterfaceDown(PSZ_LTE_NETWORK_INTERFACE))
            log_info("Interface %s is DOWN back", PSZ_LTE_NETWORK_INTERFACE);
        else
            log_error("Cannot put interface %s DOWN back", PSZ_LTE_NETWORK_INTERFACE);
    }

    if (!bTftpSuccess)
        return false;

    // delete file
    int bDeleted = (::remove(fwFileFullPath.c_str()) == 0);
    if (bDeleted)
        log_info("File %s deleted", fwFileFullPath.c_str());
    else
        log_info("Cannot delete file %s", fwFileFullPath.c_str());
    // TODO: delete version and checksum files (???)

    // 3. Upgrade modem FW
    if (!modem.firmwareUpgrade(PSZ_SIMPLE_FILE_NAME))
    {
        log_error("Could not upgrade firmware %s", PSZ_SIMPLE_FILE_NAME);
        return false;
    }

    return true;
}

