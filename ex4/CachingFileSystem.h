#ifndef _CACHINGFILESYSTEM_H_
#define _CACHINGFILESYSTEM_H_

// The FUSE API has been changed a number of times.  So, our code
// needs to define the version of the API that we assume.  As of this
// writing, the most current API version is 26
#define FUSE_USE_VERSION 26

// maintain bbfs state in here
#include <limits.h>
#include <stdio.h>

struct block
{
    char filename[PATH_MAX];
    int blocknum;
    int frequency;
    bool used;
    char data[];
};

struct caching_state
{
    FILE *logfile;
    char *rootdir;
    int numOfBlocks;
    int blockSize;
    struct block *blocks;
};

#define LOG_NAME ".filesystem.log"

#define CACHING_DATA ((struct caching_state *) fuse_get_context()->private_data)

FILE *log_open(const char *rootdir);
void log_msg(const char *format, ...);
void log_function(const char *func);

#endif
