#pragma once

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

    Block()
    {
        father = 0;
        data = nullptr;
        dataLength = 0;
        attached = false;
        chainSize = 0;
        toLongest = false;
    }
};

int getFatherNum();
void blockchain_daemon();
