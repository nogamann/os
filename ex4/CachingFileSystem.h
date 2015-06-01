#ifndef _CACHINGFILESYSTEM_H_
#define _CACHINGFILESYSTEM_H_

// The FUSE API has been changed a number of times.  So, our code
// needs to define the version of the API that we assume.  As of this
// writing, the most current API version is 26
#define FUSE_USE_VERSION 26

// maintain bbfs state in here
#include <limits.h>
#include <stdio.h>
#include <fuse.h>

struct block
{
    char filename[PATH_MAX];
    int blocknum;
    int usedBytes;
    int frequency;
    char *data;
};

struct caching_state
{
    FILE *logfile;
    char rootdir[PATH_MAX];
    int numOfBlocks;
    int blockSize;
    struct block *blocks;
};

#define SYSERR(msg) fprintf(stderr, "System Error " msg "\n")

#define LOG_NAME ".filesystem.log"
#define SLASH_LOG_NAME "/" LOG_NAME

#define CACHING_DATA ((struct caching_state *) fuse_get_context()->private_data)

FILE *log_open(const char *rootdir);
void log_msg(const char *format, ...);
void log_function(const char *func);

int cachingmanager_read(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi);
struct block *cachingmanager_allocate(const char *path, int blocknum);
void cachingmanager_rename(const char *path, const char *newpath);
void cachingmanager_log(void);

#endif
