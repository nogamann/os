#include <vector>
#include <queue>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "blockchain.h"
#include "blockchain_private.h"

using namespace std;

bool gInitialized = false;
pthread_mutex_t gInitMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gAddMutex;
pthread_mutex_t gRootsMutex;
pthread_cond_t gAddCv;
vector<Block> gBlocks;
vector<int> gRoots;
queue<int> gAddQueue;
queue<int> gAddNowQueue;
bool gCloseFlag;

/*
 * DESCRIPTION: This function initiates the Block chain, and creates the genesis Block.  The genesis Block does not hold any transaction data   
 *      or hash.
 *      This function should be called prior to any other functions as a necessary precondition for their success (all other functions should   
 *      return with an error otherwise).
 * RETURN VALUE: On success 0, otherwise -1.
 */
int init_blockchain()
{
    pthread_mutex_lock(&gInitMutex);
    // If already initialized, return error
    if (gInitialized)
    {
        ERROR("blockchain is already initialized");
        pthread_mutex_unlock(&gInitMutex);
        return -1;
    }

    // Create genesis block
    Block genesis;
    gBlocks.push_back(genesis);
    gRoots.push_back(0);

    pthread_t daemonThread;
    int error = pthread_create(&daemonThread, nullptr, daemon, nullptr);

    if (error)
    {
        ERROR("return code from pthread_create() is " << error);
        pthread_mutex_unlock(&gInitMutex);
        return -1;
    }

    init_hash_generator();

    pthread_mutex_init(&gAddMutex, NULL);
    pthread_mutex_init(&gRootsMutex, NULL);
    pthread_cond_init (&gAddCv, NULL);

    // Initialize random seed
    srand(time(NULL));

    gInitialized = true;
    pthread_mutex_unlock(&gInitMutex);
    return 0;
}

/*
 * DESCRIPTION: Ultimately, the function adds the hash of the data to the Block chain.
 *      Since this is a non-blocking package, your implemented method should return as soon as possible, even before the Block was actually  
 *      attached to the chain.
 *      Furthermore, the father Block should be determined before this function returns. The father Block should be the last Block of the 
 *      current longest chain (arbitrary longest chain if there is more than one).
 *      Notice that once this call returns, the original data may be freed by the caller.
 * RETURN VALUE: On success, the function returns the lowest available block_num (> 0),
 *      which is assigned from now on to this individual piece of data.
 *      On failure, -1 will be returned.
 */
int add_block(char *data , int length)
{
    // Library must be initialized
    if (!gInitialized)
    {
        return -1;
    }

    try
    {
        // Copy data so caller can free it
        char *copiedData = new char[length];
        memcpy(copiedData, data, length);

        
        int father = getFatherNum();
        Block block;
        block.father = father;
        block.data = copiedData;
        block.dataLength = length;
        block.chainSize = gBlocks[father].chainSize + 1;

        pthread_mutex_lock(&gAddMutex);

        // Find a free spot for the block
        int blockNum = -1;
        for (int i = 0 ; i < gBlocks.size(); i++)
        {
            if (gBlocks[i] == nullptr)
            {
                blockNum = i;
                break;
            }
        }

        if (blockNum >= 0)
        {
            gBlocks[blockNum] = block;
        }
        else
        {
            gBlocks.push_back(block);
            blockNum = gBlocks.size() - 1;
        }

        gAddQueue.push(blockNum);
        pthread_cond_signal(&gAddCv);

        pthread_mutex_unlock(&gAddMutex);
    }
    catch (const std::exception& e)
    {
        ERROR(e.what());
        return -1;
    }
}

int getFatherNum()
{
    assert(gRoots.size() > 0);

    vector<int> longestChains;
    int longestChainSize = gBlocks[gRoots[0]].chainSize;

    longestChains.push_back(gRoots[0]);
    for (int i = 1; i < gRoots.size(); i++)
    {
        if (gBlocks[gRoots[i]].chainSize == longestChainSize)
        {
            longestChains.push_back(gRoots[i]);
        }
        else
        {
            break;
        }
    }

    // Get randomly chosen longest chain
    return longestChains[rand() % longestChains.size()];
}

void blockchain_daemon()
{
    while (true)
    {
        pthread_mutex_lock(&gAddMutex);

        int blockNum;
        Block* block;
        bool addNow = false;

        while (gAddQueue.empty() && gAddNowQueue.empty())
        {
            pthread_cond_wait(&gAddCv, &gAddMutex);
        }

        if (!gAddNowQueue.empty())
        {
            blockNum = gAddNowQueue.front();
            gAddNowQueue.pop();
            addNow = true;
        }
        else
        {
            blockNum = gAddQueue.front();
            gAddQueue.pop();
        }

        block = &gBlocks[blockNum];

        pthread_mutex_unlock(&gAddMutex);

        if (block->father < 0)
        {
            block->father = getFatherNum();
        }

        if (block->toLongest)
        {
            pthread_mutex_lock(&gRootsMutex);

            int longestFatherNum = getFatherNum();
            Block& longestFather = gBlocks[longestFatherNum];
            Block& father = gBlocks[block->father];

            if (longestFather.chainSize > father.chainSize)
            {
                block->father = longestFatherNum;
            }

            int nonce = generate_nonce(blockNum, block->father);
            char* newData = generate_hash(block->data, block->dataLength, nonce);

            delete block->data;
            block->data = newData;
            block->dataLength = HASH_LEN;


        }

        int nonce = generate_nonce(blockNum, block->father);
        char* newData = generate_hash(block->data, block->dataLength, nonce);

        delete block->data;
        block->data = newData;
        block->dataLength = HASH_LEN;

        pthread_mutex_lock(&gAddMutex);

        bool requeue = false;

        if (block->father < 0)
        {
            block->father = getFatherNum();
            requeue = true;
        }
        else if (block->toLongest)
        {
            int longestFatherNum = getFatherNum();
            Block& longestFather = gBlocks[longestFatherNum];
            Block& father = gBlocks[block->father];

            if (longestFather.chainSize > father.chainSize)
            {
                block->father = longestFatherNum;
                requeue = true;
            }
        }

        if (requeue)
        {
            if (addNow)
            {
                gAddNowQueue.push()
            }
        }
    }
    
}
