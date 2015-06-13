/*
** server.c -- a stream socket server demo
*/

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

#define BACKLOG 3   // how many pending connections queue will hold

size_t gMaxFileSize;

void readFileFromSocket(int s, const char *fileName)
{
    size_t fileSize;
    char buffer[BUFFER_SIZE];
    int fd;

    // create file on server
    if ((fd = creat(fileName, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
    {
        // reporting that 'open' failed because PDF says it is expected
        THREAD_ERROR("open");
    }

    // get file size
    if (readData(s, &fileSize, sizeof(size_t)) != sizeof(size_t))
    {
        THREAD_ERROR("recv");
    }

    // read file from socket
    while (fileSize > 0)
    {
        ssize_t bytesRead;
        size_t bytesToRead = MIN(fileSize, BUFFER_SIZE);

        if ((bytesRead = readData(s, buffer, bytesToRead)) <= 0)
        {
            THREAD_ERROR("recv");
        }

        if (write(fd, buffer, bytesRead) != bytesRead)
        {
            THREAD_ERROR("write");
        }
        
        fileSize -= bytesRead;
    }
}


void *handleConnection(void *socket)
{
    size_t fileNameLen;
    char fileName[PATH_MAX];
    int s = *(int*)socket;

    // send max file size
    if (sendData(s, &gMaxFileSize, sizeof(size_t)) < 0)
    {
        THREAD_ERROR("send");
    }

    // get file name length
    if (readData(s, &fileNameLen, sizeof(size_t)) != sizeof(size_t))
    {
        THREAD_ERROR("recv");
    }

    if (fileNameLen > PATH_MAX)
    {
        fprintf(stderr, "file name on server is too long\n");
        goto done;
    }

    // if filename is empty (only null terminator) do not read file
    if (fileNameLen == 1)
    {
        goto done;
    }

    // get file name on server
    if (readData(s, fileName, fileNameLen) != fileNameLen)
    {
        THREAD_ERROR("recv");
    }

    // read file
    readFileFromSocket(s, fileName);

done:
    close(s);
    pthread_exit(NULL);
}

int establish(unsigned short portnum)
{
    char hostName[HOST_NAME_MAX+1];
    int s;
    struct sockaddr_in sa;
    struct hostent *hp;

    memset(&sa, 0, sizeof(struct sockaddr_in));

    if (gethostname(hostName, HOST_NAME_MAX) < 0)
    {
        MAIN_ERROR("gethostname");
    }

    if ((hp = gethostbyname(hostName)) == NULL)
    {
        MAIN_ERROR("gethostbyname");
    }

    sa.sin_family = hp->h_addrtype;
    memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_port = htons(portnum);

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        MAIN_ERROR("socket");
    }

    if (bind(s, (struct sockaddr*)&sa, sizeof(struct sockaddr_in)) < 0)
    {
        close(s);
        MAIN_ERROR("bind");
    }

    if (listen(s, BACKLOG) < 0) {
        MAIN_ERROR("listen");
    }

    return s;
}

void usage(void)
{
    printf("Usage: srftp server-port max-file-size\n");
    exit(1);
}

int main(int argc, char **argv)
{
    int s, ns;
    unsigned short portnum;
    pthread_t junk;

    if (argc != 3 || strtol(argv[1], NULL, 0) < 1 || strtol(argv[1], NULL, 0) > 65535 ||
        strtol(argv[2], NULL, 0) < 0 || errno)
    {
        usage();
    }

    portnum = (unsigned short)strtol(argv[1], NULL, 0);
    gMaxFileSize = (size_t)strtol(argv[2], NULL, 0);

    s = establish(portnum);

    while (1)
    {
        if ((ns = accept(s, NULL, NULL)) < 0)
        {
            MAIN_ERROR("accept");
        }

        if (pthread_create(&junk, NULL, handleConnection, &ns))
        {
            MAIN_ERROR("pthread_create");
        }
    }

    return 0;
}
