#include "CachingFileSystem.h"

#include <fuse.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

FILE *log_open(const char *rootdir)
{
    FILE *logfile;
    char logpath[PATH_MAX];

    strcpy(logpath, rootdir);
    strncat(logpath, SLASH_LOG_NAME, PATH_MAX);
    
    // very first thing, open up the logfile and mark that we got in
    // here.  If we can't open the logfile, we're dead.
    logfile = fopen(logpath, "a");
    if (logfile == NULL)
    {
        SYSERR("unable to open log file");
        exit(EXIT_FAILURE);
    }
    
    // set logfile to line buffering
    setvbuf(logfile, NULL, _IOLBF, 0);

    return logfile;
}

void log_msg(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    vfprintf(CACHING_DATA->logfile, format, ap);
}

void log_function(const char *func)
{
    log_msg("%d %s\n", time(NULL), func);
}
