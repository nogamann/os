#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "CachingFileSystem.h"

#define BLOCKNUM(offset) ((offset) / CACHING_DATA->blockSize)
#define OFFSET(blocknum) ((blocknum) * CACHING_DATA->blockSize)

/** Read from cache if data is cached, or read from disk and cache if data is not cached. */
int cachingmanager_read(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi)
{
    size_t readSize = 0;
    int firstBlock = BLOCKNUM(offset);
    int lastBlock = BLOCKNUM(offset + size - 1);
    int blockStart = offset % CACHING_DATA->blockSize;

    // for each block
    for (int blocknum = firstBlock; blocknum <= lastBlock; blocknum++)
    {
        struct block *block = NULL;

        // get block from cache
        for (int i = 0; i < CACHING_DATA->numOfBlocks; i++)
        {
            struct block* b = &CACHING_DATA->blocks[i];

            if (b->frequency && strcmp(b->filename, path) == 0 && b->blocknum == blocknum)
            {
                // if trying to read after the used part of the block, there's nothing to be read
                if (blockStart >= b->usedBytes)
                {
                    return 0;
                }

                block = b;
                block->frequency++;
                break;
            }
        }

        // if block is not in cache, read from disk
        if (block == NULL)
        {
            char *data = (char*)malloc(CACHING_DATA->blockSize);
            if (data == NULL)
            {
                return -ENOMEM;
            }

            int retstat = pread(fi->fh, data, CACHING_DATA->blockSize, OFFSET(blocknum));
            // if read failed or block is empty, we're done reading
            if (retstat <= 0)
            {
                free(data);
                break;
            }

            block = cachingmanager_allocate(path, blocknum);
            memcpy(block->data, data, CACHING_DATA->blockSize);
            free(data);

            block->usedBytes = retstat;
        }

        int blockSize = block->usedBytes - blockStart;

        // don't read more than "size" bytes overall
        if (readSize + blockSize > size)
        {
            blockSize = size - readSize;
        }

        memcpy(buf + readSize, block->data + blockStart, blockSize);
        readSize += blockSize;

        if (block->usedBytes < CACHING_DATA->blockSize)
        {
            break;
        }

        blockStart = 0;
    }

    return readSize;
}

/** Allocate a block in cache, return allocated block */
struct block *cachingmanager_allocate(const char *path, int blocknum)
{
    struct block* block = NULL;

    for (int i = 0; i < CACHING_DATA->numOfBlocks; i++)
    {
        // if block is not in use, use it
        if (!CACHING_DATA->blocks[i].frequency)
        {
            block = &CACHING_DATA->blocks[i];
            break;
        }

        // find LFU block
        if (block == NULL || CACHING_DATA->blocks[i].frequency < block->frequency)
        {
            block = &CACHING_DATA->blocks[i];
        }
    }

    strcpy(block->filename, path);
    block->blocknum = blocknum;
    block->frequency = 1;
    return block;
}

/** Rename a file in cache */
void cachingmanager_rename(const char *path, const char *newpath)
{
    // check if block is in cache
    for (int i = 0; i < CACHING_DATA->numOfBlocks; i++)
    {
        struct block* b = &CACHING_DATA->blocks[i];
        if (b->frequency && strcmp(b->filename, path) == 0)
        {
            strcpy(b->filename, newpath);
        }
    }
}

/** Log all cached blocks (in response to IOCTL) */
void cachingmanager_log(void)
{
    // check if block is in cache
    for (int i = 0; i < CACHING_DATA->numOfBlocks; i++)
    {
        struct block* b = &CACHING_DATA->blocks[i];
        if (b->frequency)
        {
            log_msg("%s %d %d\n", b->filename+1, b->blocknum, b->frequency);
        }
    }
}
