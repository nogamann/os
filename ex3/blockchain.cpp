#include <vector>
#include <queue>
#include <pthread.h>
#include <string.h>

#include "blockchain.h"
#include "blockchain_private.h"

using namespace std;

bool gInitialized = false;
pthread_mutex_t gInitMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gMutex;
vector<Block> gBlocks;
queue<int> gAddQueue;
queue<int> gAddNowQueue;
bool gCloseFlag;
int gLongestChain; // TODO

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

    pthread_t daemonThread;
    int error = pthread_create(&daemonThread, nullptr, daemon, nullptr);

    if (error)
    {
        ERROR("return code from pthread_create() is " << error);
        pthread_mutex_unlock(&gInitMutex);
        return -1;
    }

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

        // TODO set father
        int father = ...;
        Block block;
        block.father = father;
        block.data = copiedData;
        block.chainSize = gBlocks[father].chainSize + 1;

        pthread_mutex_lock(&gInitMutex);
        gAddQueue.push(block);
        pthread_mutex_unlock(&gInitMutex);
    }
    catch (const std::exception& e)
    {
        ERROR(e.what());
        return -1;
    }
}

void blockchain_daemon()
{
    
}
