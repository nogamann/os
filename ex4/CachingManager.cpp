#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "CachingFileSystem.h"

#define BLOCKNUM(offset) ((offset) / CACHING_DATA->blockSize)
#define OFFSET(blocknum) ((blocknum) * CACHING_DATA->blockSize)
#define ALIGNED(offset) (BLOCKNUM(offset) * CACHING_DATA->blockSize)

int cachingmanager_read(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi)
{
    int readsize = 0;
    int firstBlock = BLOCKNUM(offset);
    int lastBlock = BLOCKNUM(offset + size - 1);

    printf("cachingmanager_read(path='%s', size=%d, offset=%d)\n", path, (int)size, (int)offset);
    printf("  first: %d, last: %d\n", firstBlock, lastBlock);

    // for each block
    for (int blocknum = firstBlock; blocknum <= lastBlock; blocknum++)
    {
        struct block *block = NULL;

        printf("   block %d\n", blocknum);

        // check if block is in cache
        for (int i = 0; i < CACHING_DATA->numOfBlocks; i++)
        {
            struct block* b = &CACHING_DATA->blocks[i];

            if (b->frequency)
            {
                printf("   - index %d, num %d, name %s, freq %d\n", i, b->blocknum, b->filename, b->frequency);
            }

            if (b->frequency && strcmp(b->filename, path) == 0 && b->blocknum == blocknum)
            {
                block = b;
                printf("   frequency=%d\n", block->frequency);
                block->frequency++;
                break;
            }
        }

        // block is not in cache, read from disk
        if (block == NULL)
        {
            printf("   read, path=%s, blocknum=%d\n", path, blocknum);
            block = cachingmanager_add(path, blocknum);

            int retstat = pread(fi->fh, block->data, CACHING_DATA->blockSize, OFFSET(blocknum));
            if (retstat < 0)
            {
                return retstat;
            }

            block->usedBytes = retstat;
        }

        printf("   block->usedBytes=%d\n", block->usedBytes);

        // calculate what range we need to copy from the block to user buffer
        int blockStart = (blocknum == firstBlock) ? offset % CACHING_DATA->blockSize : 0;
        int blockEnd = (blocknum == lastBlock) ?
                       (offset + size) % CACHING_DATA->blockSize : CACHING_DATA->blockSize;
        if (blockEnd > block->usedBytes)
        {
            blockEnd = block->usedBytes;
        }

        memcpy(buf + readsize, block->data + blockStart, blockEnd - blockStart);
        readsize += block->usedBytes;

        if (block->usedBytes < CACHING_DATA->blockSize)
        {
            break;
        }
    }

    printf("   readsize=%d\n", readsize);

    return readsize;
}

struct block *cachingmanager_add(const char *path, int blocknum)
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

void cachingmanager_log(void)
{
    // check if block is in cache
    for (int i = 0; i < CACHING_DATA->numOfBlocks; i++)
    {
        struct block* b = &CACHING_DATA->blocks[i];
        if (b->frequency)
        {
            log_msg("%s %d %d\n", b->filename, b->blocknum, b->frequency);
        }
    }
}
