/*
 *  fw-upgrade.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/poll.h>

#include "const.h"
#include "fw-upgrade.h"
#include "config.h"
#include "kafka-rest-proxy.h"
#include "modem-gtc.h"
#include "log.h"

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
        log_error("Error initializing inotify (Errno: %d)", errno);
    watchDesc_ = inotify_add_watch(notifyDesc_, PSZ_FW_UPGRADE_PATH, IN_CLOSE_WRITE);
    if (watchDesc_ < 0)
    {
        log_error("Cannot monitor %s for firmware upgrade (Errno: %d)", PSZ_FW_UPGRADE_PATH, errno);
        close(notifyDesc_);
        notifyDesc_ = -1;
    }
    else
        log_info("Waiting for LTE-dongle firmware upgrade at %s", PSZ_FW_UPGRADE_PATH);
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

bool FirmwareUpgrader::checkForUpdate(std::string &fwFileName)
{
    fwFileName.empty();
    if (notifyDesc_ < 0 || watchDesc_ < 0)
        return false;

    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = notifyDesc_;
    fds.events = POLLIN;

    bool bUpdateAvailable = false;
    int ec = poll(&fds, 1, 1000); // 0 means "timeout expired" -> do nothing
    if (ec > 0)
    {
        if (fds.revents == POLLIN)
        {
            long length = ::read(notifyDesc_, events_, BUF_LEN);
            if (length < 0)
            {
                log_error("Error reading inotify file descriptor (Errno: %d)\n", errno);
                return false;
            }

            long i = 0;
            while (!bUpdateAvailable && (i < length)) //until first file, matching file name rules, found
            {
                struct inotify_event *event = (struct inotify_event *)(events_+i);
                if (event->len)
                {
                    if (event->mask & IN_CLOSE_WRITE)
                    {
                        // event->name is newly created file name
                        // check it, it can be not FW upgrade file but stranger
                        // TODO: specify file name format rules
                        if (checkFileName(event->name))
                        {
                            fwFileName = event->name;
                            bUpdateAvailable = true;
                        }
                    }
                }
                i += EVENT_SIZE + event->len;
            }
            fds.revents = 0;
        }
    }
    else if (ec < 0)
    {
        perror("  poll() failed");
    }

    if (bUpdateAvailable)
        log_info("FW upgrade %s%s is available\n", PSZ_FW_UPGRADE_PATH, fwFileName.c_str());

    return bUpdateAvailable;
}

bool FirmwareUpgrader::checkFileName(const char *pszFileName) const
{
    // dummy implementation, just seect for file extension and checks if it is .prg (case insensitively)
    // TODO: invent something more sophisticated according to the file format, like version_number_date.prg, etc
    const char *pszExtension = strchr(pszFileName, '.');
    if (!pszExtension)
        return false;
    return (strcasecmp(pszExtension, ".prg") == 0);
}

bool FirmwareUpgrader::upgrade(ModemGTC &modem, const std::string &fwFileName)
{
    bool bOK = true;
    // 1. unpack .tgz

    // 2. upload .prg file to modem's memory by TFTP protocol
    CurlLib &curl = CurlLib::instance();
    std::string fwFileFullPath = PSZ_FW_UPGRADE_PATH;
    fwFileFullPath += fwFileName;
#ifndef PSEUDO_MODEM
    const char *PSZ_TFTP_SERVER = "192.168.0.1";
#else
    const char *PSZ_TFTP_SERVER = "10.0.2.15"; // my laptop
#endif
    log_info("Uploading file %s to LTE-dongle's TFTP-server (%s)", fwFileFullPath.c_str(), PSZ_TFTP_SERVER);
    bOK = curl.putFileTFTP(fwFileFullPath, PSZ_TFTP_SERVER);
    if (!bOK)
    {
        log_error("Could not upload file %s to LTE-dongle's TFTP-server (%s)", fwFileFullPath.c_str(), PSZ_TFTP_SERVER);
        return false;
    }

    // 3. Upgrade modem FW
    bOK = modem.firmwareUpgrade(fwFileName);
    if (!bOK)
    {
        log_error("Could not upgrade firmware %s", fwFileName.c_str());
        return false;
    }

    // delete file
    int bDeleted = (::remove(fwFileFullPath.c_str()) == 0);
    if (bDeleted)
        log_info("File %s deleted", fwFileFullPath.c_str());
    else
        log_info("Cannot delete file %s", fwFileFullPath.c_str());

    return true;
}
