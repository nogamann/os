#pragma once

#define ERROR(msg) std::cerr << "thread library error: " << \
    msg << " [" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "]" << std::endl

struct Block
{
    int father;
    char *data;
    bool attached;
    int chainSize;
    bool toLongest;

    Block()
    {
        this.father = 0;
        this.data = nullptr;
        attached = false;
        chainSize = 0;
        toLongest = false;
    }
};

void blockchain_daemon();
