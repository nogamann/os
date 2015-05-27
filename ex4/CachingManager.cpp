#define BLOCKNUM(offset) (offset / CACHING_DATA->blockSize)
#define ALIGNED(offset) (BLOCKNUM(offset) * CACHING_DATA->blockSize)

int getCachedBlock(const char *path, char *buf, size_t size, off_t offset)
{
    for (int i = 0; i < CACHING_DATA->numOfBlocks; i++)
    {
        struct block* b = &CACHING_DATA->blocks[i];
        if (b->used && strcmp(b->filename, path) == 0 && b->blocknum == BLOCKNUM(offset))
        {
            // TODO copy cache to buf

            b->frequency++;
            return 0;
        }
    }
}
