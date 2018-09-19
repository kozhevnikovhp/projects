/*
 *  fw-upgrade.h
 *  
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <sys/inotify.h>
#include <string>

// forward declarations
class ModemGTC;
class Configuration;

typedef enum
{
    FWU_IDLE,
    FWU_READY_FOR_UPGRADE,
    FWU__UPGRADE_COMPLETED,
    FWU_UNKNOWN
} FWU_STATE;

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

// singleton
class FirmwareUpgrader
{
  public:
    static FirmwareUpgrader &instance();

    void configure(const Configuration &config);
    bool checkForUpdate(std::string &fwFileName);
    FWU_STATE getState() const { return state_; }
    bool upgrade(ModemGTC &modem, const std::string &fwFileName);

  private:
    FirmwareUpgrader();
    ~FirmwareUpgrader();

    bool checkFileName(const char *pszFileName) const;

    // disable copy constructor...
    FirmwareUpgrader(FirmwareUpgrader const &) = delete;
    // ... and operator =
    FirmwareUpgrader &operator= (FirmwareUpgrader const &) = delete;

    FWU_STATE state_;
    int notifyDesc_;
    int watchDesc_;
    char events_[BUF_LEN];
};


