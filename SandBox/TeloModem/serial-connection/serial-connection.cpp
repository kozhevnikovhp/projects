#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <ctype.h>

int runSerialDeviceCommand(const char *pszDevice, const char *pszCommand)
{
    struct termios old_termios;
    struct termios new_termios;

    printf("1\n");
    int fd = open(pszDevice, O_RDWR | O_NOCTTY);
    printf("1.5 fd = %d\n", fd);
    if (fd < 0)
    {
        fprintf(stderr, "error, counldn't open file %s\n", pszDevice);
        perror("open");
        return 1;
    }
    printf("2\n");

    if (tcgetattr(fd, &old_termios) != 0)
    {
        fprintf(stderr, "tcgetattr(fd, &old_termios) failed: %s\n", strerror(errno));
        perror("tcgetattr");
        return 1;
    }
    printf("3\n");

    memset(&new_termios, 0, sizeof(new_termios));
    new_termios.c_iflag = IGNPAR;
    new_termios.c_oflag = 0;
    new_termios.c_cflag = CS8 | CREAD | CLOCAL | HUPCL;
    new_termios.c_lflag = 0;
    new_termios.c_cc[VINTR]    = 0;
    new_termios.c_cc[VQUIT]    = 0;
    new_termios.c_cc[VERASE]   = 0;
    new_termios.c_cc[VKILL]    = 0;
    new_termios.c_cc[VEOF]     = 4;
    new_termios.c_cc[VTIME]    = 0;
    new_termios.c_cc[VMIN]     = 1;
    new_termios.c_cc[VSWTC]    = 0;
    new_termios.c_cc[VSTART]   = 0;
    new_termios.c_cc[VSTOP]    = 0;
    new_termios.c_cc[VSUSP]    = 0;
    new_termios.c_cc[VEOL]     = 0;
    new_termios.c_cc[VREPRINT] = 0;
    new_termios.c_cc[VDISCARD] = 0;
    new_termios.c_cc[VWERASE]  = 0;
    new_termios.c_cc[VLNEXT]   = 0;
    new_termios.c_cc[VEOL2]    = 0;
    if (cfsetispeed(&new_termios, B115200) != 0)
    {
        fprintf(stderr, "cfsetispeed(&new_termios, B57600) failed: %s\n", strerror(errno));
        perror("cfsetispeed");
        return 1;
    }
    printf("4\n");

    if (cfsetospeed(&new_termios, B115200) != 0)
    {
        fprintf(stderr, "cfsetospeed(&new_termios, B57600) failed: %s\n", strerror(errno));
        perror("cfsetospeed");
        return 1;
    }
    printf("5\n");

    if (tcsetattr(fd, TCSANOW, &new_termios) != 0)
    {
        fprintf(stderr, "tcsetattr(fd, TCSANOW, &new_termios) failed: %s\n", strerror(errno));
        perror("tcsetattr");
        return 1;
    }
    printf("6\n");

    int nWritten = write(fd, pszCommand, strlen(pszCommand));
    printf("%d bytes written\n", nWritten);
    perror("write");
    const char *pszCRLF = "\r\n";
    nWritten = write(fd, pszCRLF, strlen(pszCRLF));
    printf("%d bytes written\n", nWritten);
    perror("write");

    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = fd;
    fds.events = POLLIN;
    int timeout = (3 * 1000); // 3 seconds
    printf("7\n");

    int nRead = 0;
    char c;
    do
    {
        nRead = 0;
        int rc = poll(&fds, 1, timeout); // 0 means "timeout expired" -> do nothing
        if (rc > 0)
        {
            if (fds.revents == POLLIN)
            {
                nRead = read(fds.fd, &c, sizeof(c));
                if (nRead)
                {
                    if (isalnum(c) || isspace(c) || ispunct(c) || c == 0x0A || c == 0x0D)
                        printf("%c", c);
                }
                fds.revents = 0;
            }
        }
        else if (rc < 0)
        {
          perror("  poll() failed");
          break;
        }
    } while (nRead);
    printf("8\n");

    // Before leaving, reset the old serial settings.
    tcsetattr(fd, TCSANOW, &old_termios);
    printf("9\n");
    close(fd);
    printf("10\n");
    return 0;
}
