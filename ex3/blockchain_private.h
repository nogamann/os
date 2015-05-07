#pragma once

#include <iostream>

#define ERROR(msg) std::cerr << "thread library error: " << \
    msg << " [" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "]" << std::endl

#define HASH_LEN 16

struct Block
{
    int father;
    char *data;
    int dataLength;
    bool attached;
    int chainSize;
    bool toLongest;
    bool deleted;

    Block()
    {
        data = nullptr;
        clear();
        deleted = false;
    }

    void clear()
    {
        if (data != nullptr)
        {
            delete[] data;
            data = nullptr;
        }

        father = 0;
        dataLength = 0;
        attached = false;
        chainSize = 0;
        toLongest = false;
        deleted = true;
    }

    ~Block()
    {
        clear();
    }
};

int getLongestChain();
void* blockchain_daemon(void*);
void doClose();
int getGlobalMutex();