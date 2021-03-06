/*
 *  serial-connection.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <stddef.h>
#include <termios.h>
#include <vector>

class SerialConnection
{
public:
    SerialConnection();
    virtual ~SerialConnection();

    int getFileDescriptor() const { return fd_; }
    bool open(const char *pszDeviceName, bool bNoResetOption);
    bool isOpen() const { return (fd_ > 0); }
    bool resetUSB();
    bool close();

    bool setSpeed(speed_t speed);
    bool write(const void *pBuffer, int nBytes);
    bool read(std::vector<char> &buffer, int timeout);
    bool flushInputBuffer(int timeout);

protected:
    int fd_;
    struct termios old_termios_;
    struct termios new_termios_;
    bool bNoResetOption_; // if set, do not restore termios structure
};


