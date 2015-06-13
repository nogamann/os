#ifndef SHARED_H
#define SHARED_H

#include <stdlib.h>

#define PRINT_ERROR(syscall) fprintf(stderr, "Error: function:%s, errno:%d.\n", syscall, errno)
#define MAIN_ERROR(syscall) PRINT_ERROR(syscall); exit(1)
#define THREAD_ERROR(syscall) PRINT_ERROR(syscall); pthread_exit(NULL)

#define BUFFER_SIZE 4096
#define MIN(x, y) ((x) < (y) ? (x) : (y))

int readData(int s, void *buf, size_t len);
int sendData(int s, const void *buf, size_t len);

#endif