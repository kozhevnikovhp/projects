/*
 *  serial-connection.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

#include <stddef.h>
#include <termios.h>

class SerialConnection
{
public:
    SerialConnection();
    virtual ~SerialConnection();

    int getFileDescriptor() const { return fd_; }
    bool open(const char *pszDeviceName);
    bool isOpen() const { return (fd_ > 0); }
    bool resetUSB();
    bool close();

    bool setSpeed(speed_t speed);
    bool write(const void *pBuffer, int nBytes);
    bool read(const void *pBuffer, size_t nBufferSize, int timeout, size_t &nBytesReadTotal);

protected:
    int fd_;
    struct termios old_termios_;
    struct termios new_termios_;
};


