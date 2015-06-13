#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "shared.h"

int callSocket(const char *hostname, unsigned short portnum)
{
    struct sockaddr_in sa;
    struct hostent *hp;
    int s;

    if ((hp = gethostbyname(hostname)) == NULL)
    {
        MAIN_ERROR("gethostbyname");
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = hp->h_addrtype;
    memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_port = htons(portnum);

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        MAIN_ERROR("socket");
    }

    if (connect(s, (struct sockaddr*)&sa, sizeof(sa)) < 0)
    {
        close(s);
        MAIN_ERROR("connect");
    }

    return s;
}

void sendFileName(int s, const char *filename)
{
    size_t len = strlen(filename) + 1;

    // send filename len (including null terminator)
    if (sendData(s, &len, sizeof(size_t)) < 0)
    {
        MAIN_ERROR("send");
    }

    // send filename
    if (sendData(s, filename, len) < 0)
    {
        MAIN_ERROR("send");
    }
}


void sendFile(int s, const char *fileName, size_t fileSize)
{
    char buffer[BUFFER_SIZE];
    int fd;

    // send file size
    if (sendData(s, &fileSize, sizeof(size_t)) < 0)
    {
        MAIN_ERROR("send");
    }

    if ((fd = open(fileName, O_RDONLY)) < 0)
    {
        MAIN_ERROR("open");
    }

    while (1)
    {
        // read data into buffer
        int bytesRead = read(fd, buffer, BUFFER_SIZE);
        if (bytesRead == 0) // We're done reading from the file
        {
            break;
        }

        if (bytesRead < 0)
        {
            MAIN_ERROR("read");
        }

        // send data through socket
        if (sendData(s, buffer, bytesRead) < 0)
        {
            MAIN_ERROR("send");
        }
    }
}

size_t getFileSize(const char *fileName)
{
    int fd;
    off_t fileSize;

    if ((fd = open(fileName, O_RDONLY)) < 0)
    {
        MAIN_ERROR("open");
    }

    if ((fileSize = lseek(fd, 0L, SEEK_END)) == (off_t)-1)
    {
        MAIN_ERROR("lseek");
    }

    close(fd);
    return (size_t)fileSize;
}

void usage(void)
{
    printf("Usage: clftp server-port server-hostname file-to-transfer filename-in-server\n");
    exit(1);
}

int main(int argc, char **argv)
{
    char *hostname;
    char *fileToTransfer;
    char *fileOnServer;
    unsigned short portnum;
    size_t maxFileSize;
    size_t fileToTransferSize;
    int s;


    if (argc != 5 || strtol(argv[1], NULL, 0) < 1 || strtol(argv[1], NULL, 0) > 65535 ||
        access(argv[3], F_OK) == -1 || strlen(argv[4]) > PATH_MAX)
    {
        usage();
    }

    portnum = strtol(argv[1], NULL, 0);
    hostname = argv[2];
    fileToTransfer = argv[3];
    fileOnServer = argv[4];

    s = callSocket(hostname, portnum);

    // Get maximum file size
    if (readData(s, &maxFileSize, sizeof(size_t)) < 0)
    {
        MAIN_ERROR("recv");
    }

    // Get transfer file size
    fileToTransferSize = getFileSize(fileToTransfer);
    
    if (fileToTransferSize > maxFileSize || strlen(fileOnServer) > maxFileSize)
    {
        // send empty file name to inform server we're not sending the file
        sendFileName(s, "");
        close(s);
        printf("Transmission failed: too big file\n");
        return 0;
    }

    sendFileName(s, fileOnServer);
    sendFile(s, fileToTransfer, fileToTransferSize);

    close(s);

    return 0;
}