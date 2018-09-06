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
    eventDescs_ = inotify_init();
    if (eventDescs_ < 0)
        log_error("Error initializing inotify (Errno: %d)", errno);
    watchDesc_ = inotify_add_watch(eventDescs_, PSZ_FW_UPGRADE_PATH, IN_CLOSE_WRITE);
    if (watchDesc_ < 0)
    {
        log_error("Cannot monitor %s for firmware upgrade (Errno: %d)", errno);
        close(watchDesc_);
        eventDescs_ = -1;
    }
    else
        log_info("Waiting for LTE-dongle firmware upgrade at %s", PSZ_FW_UPGRADE_PATH);
}

//virtual
FirmwareUpgrader::~FirmwareUpgrader()
{
    if (watchDesc_ >= 0)
        inotify_rm_watch(eventDescs_, watchDesc_);

    if (eventDescs_ >= 0)
        close(eventDescs_);
}

void FirmwareUpgrader::configure(const Configuration &cfg)
{
}

bool FirmwareUpgrader::checkForUpdate(std::string &fwFileName)
{
    fwFileName.empty();
    if (eventDescs_ < 0 || watchDesc_ < 0)
        return false;

    printf("Check for update....\n");

    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = eventDescs_;
    fds.events = POLLIN;

    bool bUpdateAvailable = false;
    int ec = poll(&fds, 1, 1000); // 0 means "timeout expired" -> do nothing
    if (ec > 0)
    {
        if (fds.revents == POLLIN)
        {
            long length = ::read(eventDescs_, events_, BUF_LEN);
            printf("%d bytes have been read\n", length);
            if (length < 0)
            {
                log_error("Error reading inotify file descriptor (Errno: %d)\n", errno);
                return false;
            }

            long i = 0;
            while (!bUpdateAvailable && (i < length)) //until first file, matching file name rules, found
            {
                printf("%d of %d\n", i, length);
                struct inotify_event *event = (struct inotify_event *)(events_+i);
                if (event->len)
                {
                    printf("event of length %d\n", event->len);
                    if (event->mask & IN_CLOSE_WRITE)
                    {
                        // event->name is newly created file name
                        // chech it, it can be not FW upgrade file but stranger
                        // TODO: specify file name format rules
                        if (1)
                        {
                            fwFileName = event->name;
                            log_debug("FW upgrade file '%s%s' created\n", PSZ_FW_UPGRADE_PATH, fwFileName.c_str());
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

    printf("Upgrade %s%s is available = %d\n", PSZ_FW_UPGRADE_PATH, fwFileName.c_str(), bUpdateAvailable);
    return bUpdateAvailable;
}

bool FirmwareUpgrader::upgrade(ModemGTC &modem, const std::string &fwFileName)
{
    bool bOK = true;
    // 1. unpack .tgz

    // 2. upload .prg file to modem's memory by TFTP protocol
    KafkaRestProxy &kafkaRestProxy = KafkaRestProxy::instance();
    std::string fwFileFullPath = PSZ_FW_UPGRADE_PATH;
    fwFileFullPath += fwFileName;
#ifndef PSEUDO_MODEM
    const char *PSZ_TFTP_SERVER = "192.168.0.1";
#else
    const char *PSZ_TFTP_SERVER = "10.0.2.15"; // my laptop
#endif
    log_info("FW upgrade: uploading file %s to LTE-dongle's TFTP-server (%s)", fwFileFullPath.c_str(), PSZ_TFTP_SERVER);
    bOK = kafkaRestProxy.putFileTFTP(fwFileFullPath, PSZ_TFTP_SERVER);
    if (!bOK)
    {
        log_error("FW upgrade: could not upload file %s to LTE-dongle's TFTP-server (%s)", fwFileFullPath.c_str(), PSZ_TFTP_SERVER);
        return false;
    }

    // 3. Upgrade modem FW
    bOK = modem.firmwareUpgrade(fwFileName);
    if (!bOK)
    {
        log_error("FW upgrade: could not upgrade firmware %s", fwFileName.c_str());
        return false;
    }


    return true;
}
