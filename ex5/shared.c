#include "shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

int readData(int s, void *buf, size_t len)
{
    size_t bytesCount = 0;
    
    while (bytesCount < len)
    {
        ssize_t bytesRead;

        if ((bytesRead = recv(s, buf, len - bytesCount, 0)) > 0)
        {
            bytesCount += bytesRead;
            buf += bytesRead;
        }
        else if (bytesRead < 0)
        {
            return -1;
        }
    }

    return bytesCount;
}

int sendData(int s, const void *buf, size_t len)
{
    while (len > 0)
    {
        ssize_t bytesWritten;
        if ((bytesWritten = send(s, buf, len, 0)) <= 0)
        {
            return -1;
        }

        len -= bytesWritten;
        buf += bytesWritten;
    }

    return 0;
}
