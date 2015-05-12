#pragma once

#include <iostream>
#include <stdlib.h>

#define ERROR(msg) std::cerr << "thread library error: " << \
    msg << " [" << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "]" << std::endl

#define HASH_LEN 16

int gId = 0;

struct Block
{
    int father;
    char *data;
    int dataLength;
    bool attached;
    int chainSize;
    bool toLongest;
    bool deleted;
    int id;

    Block()
        : data(nullptr), id(gId++)
    {
        clear();
        deleted = false;
    }

    Block(const Block& other) 
        : father(other.father),
          data(other.dataLength ? (char*)malloc(other.dataLength) : nullptr),
          dataLength(other.dataLength),
          attached(other.attached),
          chainSize(other.chainSize),
          toLongest(other.toLongest),
          deleted(other.deleted), id(gId++)
    {

        if (dataLength > 0)
        {
            std::copy(other.data, other.data + dataLength, data);
        }
    }

    Block(Block&& other)
        : Block()
    {
        swap(*this, other);
    }

    void clear()
    {
        if (data != nullptr)
        {
            free(data);
            data = nullptr;
        }

        father = 0;
        dataLength = 0;
        attached = false;
        chainSize = 0;
        toLongest = false;
        deleted = true;
    }

    Block& operator=(Block other)
    {
        swap(*this, other);

        return *this;
    } 

    ~Block()
    {
        clear();
    }

    friend void swap(Block& first, Block& second)
    {
        using std::swap; 

        // by swapping the members of two classes,
        // the two classes are effectively swapped
        swap(first.father, second.father);
        swap(first.data, second.data);
        swap(first.dataLength, second.dataLength);
        swap(first.attached, second.attached);
        swap(first.chainSize, second.chainSize);
        swap(first.toLongest, second.toLongest);
        swap(first.deleted, second.deleted);
    }
};

int getLongestChain();
void* blockchain_daemon(void*);
int doClose();
int getGlobalMutex();