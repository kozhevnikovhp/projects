/*
 *  serial-connection.cpp
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/poll.h>

#include <linux/usbdevice_fs.h>

#include "serial-connection.h"

SerialConnection::SerialConnection()
{
    fd_ = -1; // invalid value
    bNoResetOption_ = false; // restore by default
}

//virtual
SerialConnection::~SerialConnection()
{
    close();
}

bool SerialConnection::open(const char *pszDeviceName, bool bNoResetOption)
{
    fd_ = ::open(pszDeviceName, O_RDWR | O_NOCTTY);
    if (fd_ < 0)
    {
        fprintf(stderr, "error, counldn't open file %s\n", pszDeviceName);
        perror("open");
        return false;
    }

    // To reset or not to reset original options upon device closure? It is a question! The same as -r option of picocom, for example
    bNoResetOption_ = bNoResetOption;
    if (!bNoResetOption_)
    {
        if (tcgetattr(fd_, &old_termios_) != 0)
        {
            fprintf(stderr, "tcgetattr(fd, &old_termios) failed: %s\n", strerror(errno));
            perror("tcgetattr");
            close();
            return false;
        }
    }

    memset(&new_termios_, 0, sizeof(new_termios_));
    new_termios_.c_iflag = IGNPAR;
    new_termios_.c_oflag = 0;
    new_termios_.c_cflag = CS8 | CREAD | CLOCAL | HUPCL;
    new_termios_.c_lflag = 0;
    new_termios_.c_cc[VINTR]    = 0;
    new_termios_.c_cc[VQUIT]    = 0;
    new_termios_.c_cc[VERASE]   = 0;
    new_termios_.c_cc[VKILL]    = 0;
    new_termios_.c_cc[VEOF]     = 4;
    new_termios_.c_cc[VTIME]    = 0;
    new_termios_.c_cc[VMIN]     = 1;
    new_termios_.c_cc[VSWTC]    = 0;
    new_termios_.c_cc[VSTART]   = 0;
    new_termios_.c_cc[VSTOP]    = 0;
    new_termios_.c_cc[VSUSP]    = 0;
    new_termios_.c_cc[VEOL]     = 0;
    new_termios_.c_cc[VREPRINT] = 0;
    new_termios_.c_cc[VDISCARD] = 0;
    new_termios_.c_cc[VWERASE]  = 0;
    new_termios_.c_cc[VLNEXT]   = 0;
    new_termios_.c_cc[VEOL2]    = 0;

    if (tcsetattr(fd_, TCSANOW, &new_termios_) != 0)
    {
        fprintf(stderr, "tcsetattr(fd, TCSANOW, &new_termios) failed: %s\n", strerror(errno));
        perror("tcsetattr");
        close();
        return false;
    }

    return true;
}

bool SerialConnection::resetUSB()
{
    if (!isOpen())
        return false;
    int ec = ioctl(fd_, USBDEVFS_RESET, 0);
    if (ec < 0)
    {
        perror("Error in ioctl (resetUSB)");
        return false;
    }
    return true;
}

bool SerialConnection::close()
{
    if (isOpen())
    {
        if (!bNoResetOption_)
        {
            // Before leaving, reset the old serial settings.
            tcsetattr(fd_, TCSANOW, &old_termios_);
        }
        ::close(fd_);
    }
    fd_ = -1;
    return true;
}

bool SerialConnection::setSpeed(speed_t speed)
{
    if (cfsetispeed(&new_termios_, speed) != 0)
    {
        fprintf(stderr, "cfsetispeed(&new_termios, B57600) failed: %s\n", strerror(errno));
        perror("cfsetispeed");
        return false;
    }

    if (cfsetospeed(&new_termios_, speed) != 0)
    {
        fprintf(stderr, "cfsetospeed(&new_termios, B57600) failed: %s\n", strerror(errno));
        perror("cfsetospeed");
        return false;
    }

    if (tcsetattr(fd_, TCSANOW, &new_termios_) != 0)
    {
        fprintf(stderr, "tcsetattr(fd, TCSANOW, &new_termios) failed: %s\n", strerror(errno));
        perror("tcsetattr");
        return false;
    }
    return true;
}

bool SerialConnection::write(const void *pBuffer, int nBytes)
{
    if (!isOpen())
        return false;
    int nWritten = ::write(fd_, pBuffer, nBytes);
    return (nWritten == nBytes);
}

bool SerialConnection::read(const void *pBuffer, size_t nBufferSize, int timeout, size_t &nBytesReadTotal)
{
    if (!isOpen())
        return false;

    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = fd_;
    fds.events = POLLIN;

    size_t nRead = 0;
    char *pCurrentPos = (char *)pBuffer;
    do
    {
        nRead = 0;
        int ec = poll(&fds, 1, timeout); // 0 means "timeout expired" -> do nothing
        if (ec > 0)
        {
            if (fds.revents == POLLIN)
            {
                nRead = ::read(fds.fd, pCurrentPos, 1);
                if (nRead)
                {
                    pCurrentPos += nRead;
                    nBytesReadTotal += nRead;
                }
                fds.revents = 0;
            }
        }
        else if (ec < 0)
        {
          perror("  poll() failed");
          break;
        }
    } while (nRead && (nBytesReadTotal < nBufferSize));
    return true;
}




