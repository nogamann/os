#include <string>
#include <vector>
#include <list>
#include <deque>
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hash.h"
#include "blockchain.h"
#include "blockchain_private.h"

using namespace std;

bool gInitialized = false;
pthread_mutex_t gInitMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gMutex;
pthread_cond_t gAddCv;
vector<Block> gBlocks;
list<int> gRoots;
deque<int> gAddQueue;
deque<int> gAddNowQueue;
pthread_t gDaemonThread;
bool gShouldClose;
int gNumOfBlocks;

/**
 * DESCRIPTION: This function initiates the Block chain, and creates the genesis Block.  The genesis Block does not hold any transaction data   
 *      or hash.
 *      This function should be called prior to any other functions as a necessary precondition for their success (all other functions should   
 *      return with an error otherwise).
 * RETURN VALUE: On success 0, otherwise -1.
 */
int init_blockchain()
{
    if (pthread_mutex_lock(&gInitMutex))
    {
        return -1;
    }
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

    // Initialize hash generator 
    init_hash_generator();

    int res = 0;
    // Initialize Mutexes
    if ((res = pthread_mutex_init(&gMutex, NULL)))
    {
        goto done;
    }
    if ((res = pthread_cond_init(&gAddCv, NULL)))
    {
        goto done;
    }

    // Initialize random seed
    srand(time(NULL));

    gShouldClose = false;
    gNumOfBlocks = 0;

    if ((res = pthread_create(&gDaemonThread, nullptr, blockchain_daemon, nullptr)))
    {
        goto done;
    }

    gInitialized = true;

    

done:
    pthread_mutex_unlock(&gInitMutex);
    return res;
}

/**
 * DESCRIPTION: This function closes the Block chain. 
 */
 // TODO i'm not sure if this functon should return an int with error -1 in case something happens
void doClose()
{
    gInitialized = false;
    close_hash_generator();
    pthread_mutex_destroy(&gMutex);
    pthread_cond_destroy(&gAddCv);
    gBlocks.clear();
    gRoots.clear();
    gAddQueue.clear();
    gAddNowQueue.clear();
    gNumOfBlocks = 0;
}

/**
 * DESCRIPTION: This function trys to get the global mutex.
 * RETURN VALUE: 0 in success, -2 if it was called when the library is closing and -1 on other errors.
 */
int getGlobalMutex()
{
    int res;
    if ((res = pthread_mutex_lock(&gInitMutex)))
    {
        ERROR("pthread_mutex_lock returned " << res);
        return -1;
    }

    if (gShouldClose)
    {
        pthread_mutex_unlock(&gInitMutex);
        return -2;
    }

    if ((res = pthread_mutex_lock(&gMutex)))
    {
        ERROR("pthread_mutex_lock returned " << res);
        return -1;
    }

    if ((res = pthread_mutex_unlock(&gInitMutex)))
    {
        ERROR("pthread_mutex_lock returned " << res);
        return -1;
    }

    return 0;
}

/**
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
    if (!gInitialized | gShouldClose)
    {
        return -1;
    }

    if (getGlobalMutex())
    {
        return -1;
    }

    int res;

    try
    {
        // Find a free spot for the block
        int blockNum = -1;
        for (size_t i = 0; i < gBlocks.size(); i++)
        {
            if (gBlocks[i].deleted)
            {
                gBlocks[i].deleted = false;
                blockNum = i;
                break;
            }
        }

        if (blockNum < 0)
        {
            gBlocks.emplace_back();
            blockNum = gBlocks.size() - 1;
        }

        // Copy data so caller can free it
        char *copiedData = (char*)malloc(length);
        std::copy(data, data + length, copiedData);

        int father = getLongestChain();
        gBlocks[blockNum].father = father;
        gBlocks[blockNum].data = copiedData;
        gBlocks[blockNum].dataLength = length;
        gBlocks[blockNum].chainSize = gBlocks[father].chainSize + 1;

        gAddQueue.push_back(blockNum);
        res = blockNum;

        cout << "*** add_block " << blockNum << endl;

        if (pthread_cond_signal(&gAddCv))
        {
            // TODO what to do here?
            throw std::runtime_error("pthread_cond_signal returned an error");
        }
    }
    catch (const std::exception& e)
    {
        ERROR(e.what());
        res = -1;
    }

    pthread_mutex_unlock(&gMutex);

    return res;
}

/**
 * DESCRIPTION: Returns random block_num of one of the current longest chains last blocks.
 * RETURN VALUE: a block_num of a block that is the last block in a longest chain.
 */
int getLongestChain()
{
    assert(gRoots.size() > 0);

    vector<int> longestChains;
    int longestChainSize = gBlocks[gRoots.front()].chainSize;

    // Collect all blocks of longest chain size (gRoots is sorted by chain size)
    for (int root : gRoots)
    {
        if (gBlocks[root].chainSize == longestChainSize)
        {
            longestChains.push_back(root);
        }
        else
        {
            break;
        }
    }

    // Get randomly chosen longest chain
    return longestChains[rand() % longestChains.size()];
}

/**
 * DESCRIPTION: The library's daemon function.
 */
void* blockchain_daemon(void*)
{
    while (true)
    {
        if (pthread_mutex_lock(&gMutex))
        {
            pthread_exit(NULL);
        }

        int blockNum;
        bool addNow = false;

        // If queues are empty, wait for them to fill
        while (gAddQueue.empty() && gAddNowQueue.empty())
        {
            if (!gShouldClose)
            {
                if (pthread_cond_wait(&gAddCv, &gMutex))
                {
                    pthread_exit(NULL); // TODO- is this ok? not too nested?
                }
            }

            // If no more stuff in queue and need to close, close chain and return
            if (gAddQueue.empty() && gAddNowQueue.empty() && gShouldClose)
            {
                doClose();
                pthread_exit(NULL);
            }
        }

        // If there is an attach_now block, pick it
        if (!gAddNowQueue.empty())
        {
            blockNum = gAddNowQueue.front();
            gAddNowQueue.pop_front();
            addNow = true;
        }
        else
        {
            // else, take a regular block to attach
            blockNum = gAddQueue.front();
            gAddQueue.pop_front();
        }

        // Copy block so vector can be reallocated during hash...
        Block block = gBlocks[blockNum];

        // If block's father was pruned, get another father
        if (block.father < 0)
        {
            block.father = getLongestChain();
        }

        // If block is toLongest, make sure its father is in the longest chain
        if (block.toLongest)
        {
            int longestFatherNum = getLongestChain();
            Block& longestFather = gBlocks[longestFatherNum];
            Block& father = gBlocks[block.father];

            if (longestFather.chainSize > father.chainSize)
            {
                block.father = longestFatherNum;
            }
        }

        // Unlock before hash
        if (pthread_mutex_unlock(&gMutex))
        {
            pthread_exit(NULL);
        }

        int nonce = generate_nonce(blockNum, block.father);
        char* newData = generate_hash(block.data, block.dataLength, nonce);

        free(block.data);
        block.data = newData;
        block.dataLength = HASH_LEN;
        cout << "*** hashed " << blockNum << endl;

        if (gShouldClose)
        {
            string data(block.data, HASH_LEN);
            cout << "*** Block #" << blockNum << ", data: " << data << endl;
            continue;
        }

        // Lock after hash
        if (pthread_mutex_lock(&gMutex))
        {
            pthread_exit(NULL);
        }

        // If father was pruned, requeue and continue
        if (block.father < 0)
        {
            block.father = getLongestChain();
            if (addNow)
            {
                gAddNowQueue.push_back(blockNum);
            }
            else
            {
                gAddQueue.push_back(blockNum);
            }

            if(pthread_mutex_unlock(&gMutex))
            {
                 pthread_exit(NULL);
            }
            continue;
        }

        // Attached block is the new root of the chain, replace father
        gRoots.remove(block.father);

        block.chainSize = gBlocks[block.father].chainSize + 1;

        // Add block to Roots, sorted by chain size
        if (gRoots.empty() || gBlocks[gRoots.back()].chainSize >= block.chainSize)
        {
            gRoots.push_back(blockNum);
        }
        else
        {
            for (auto it = gRoots.begin(); it != gRoots.end(); it++)
            {
                if (gBlocks[*it].chainSize <= block.chainSize)
                {
                    gRoots.insert(it, blockNum);
                }
                break;
            }
        }
 
        cout << "*** attaching " << blockNum << " to " << block.father << endl;
        block.attached = true;
        gBlocks[blockNum] = std::move(block);
        gNumOfBlocks++; 
        cout << "was added: " << gBlocks[blockNum].attached << endl;
        cout << "chain size is " << gNumOfBlocks << endl;

        if (pthread_mutex_unlock(&gMutex))
        {
             pthread_exit(NULL);
        }
    }
}

/**
 * DESCRIPTION: Without blocking, enforce the policy that this block_num should be attached to the longest chain at the time of attachment of 
 *      the Block. For clearance, this is opposed to the original add_block that adds the Block to the longest chain during the time that 
 *      add_block was called.
 *      The block_num is the assigned value that was previously returned by add_block. 
 * RETURN VALUE: If block_num doesn't exist, return -2; In case of other errors, return -1; In case of success return 0; In case block_num is 
 *      already attached return 1.
 */
int to_longest(int block_num)
{
    int res = 0;

    // Library must be initialized
    if (!gInitialized || gShouldClose)
    {
        return -1;
    }

    if (getGlobalMutex())
    {
        return -1;
    }

    // If block_num doesn't exist, return -2
    if (block_num >= (int)gBlocks.size() || gBlocks[block_num].deleted)
    {
        res = -2;
        goto done;
    }

    // In case block_num is already attached return 1
    if (gBlocks[block_num].attached == true)
    {
        res = 1;
        goto done;
    }

    gBlocks[block_num].toLongest = true;

done:
    pthread_mutex_unlock(&gMutex);
    return res;
}

/**
 * DESCRIPTION: Search throughout the tree for sub-chains that are not the longest chain,
 *      detach them from the tree, free the blocks, and reuse the block_nums.
 * RETURN VALUE: On success 0, otherwise -1.
 */
int prune_chain()
{
    // Library must be initialized
    if (!gInitialized || gShouldClose)
    {
        return -1;
    }

    if (getGlobalMutex())
    {
        return -1;
    }

    vector<bool> blocksToDelete(gBlocks.size(), true);

    // Mark longest chain's blocks
    int longestChain = getLongestChain();
    int cur = longestChain;
    while (cur != 0)
    {
        blocksToDelete[cur] = false;
        cur = gBlocks[cur].father;
    }
    blocksToDelete[0] = false;

    // Delete unmarked blocks, start from every root, progress toward genesis and delete
    // every block that was marked to delete
    for (int root : gRoots)
    {
        int cur = root;
        while (blocksToDelete[cur])
        {
            cout << "*** prune " << cur << endl;
            int father = gBlocks[cur].father;
            gBlocks[cur].clear();
            blocksToDelete[cur] = false;
            cur = father;
        }
    }

    gRoots.clear();
    gRoots.push_back(longestChain);

    // Go through queues and updated blocks that their father was deleted
    for (int blockNum : gAddNowQueue)
    {
        if (blocksToDelete[gBlocks[blockNum].father])
        {
            gBlocks[blockNum].father = -1;
        }
    }

    for (int blockNum : gAddQueue)
    {
        if (blocksToDelete[gBlocks[blockNum].father])
        {
            gBlocks[blockNum].father = -1;
        }
    }

    if (pthread_mutex_unlock(&gMutex))
    {
        return -1;
    }

    return 0;
}

/**
 * DESCRIPTION: Without blocking, check whether block_num was added to the chain.
 *      The block_num is the assigned value that was previously returned by add_block.
 * RETURN VALUE: 1 if true and 0 if false. If the block_num doesn't exist, return -2; In case of other errors, return -1.
 */
int was_added(int block_num)
{
    int res;
    // Library must be initialized
    if (!gInitialized)
    {
        return -1;
    }

    if (getGlobalMutex())
    {
        return -1;
    }

    // res = block_num < (int)gBlocks.size() ? (int)gBlocks[block_num].attached : 0; TODO what is this? delete please

    // If block_num doesn't exist, return -2
    if (block_num >= (int)gBlocks.size() || gBlocks[block_num].deleted)
    {
        res = -2;
    }
    else if (gBlocks[block_num].attached)
    {
        res = 1;
    }
    else
    {
        res = 0;
    }

    pthread_mutex_unlock(&gMutex);
    return res;
}

/**
 * DESCRIPTION: Return how many Blocks were attached to the chain since init_blockchain.
 *      If the chain was closed (by using close_chain) and then initialized (init_blockchain) again this function should return 
 *      the new chain size.
 * RETURN VALUE: On success, the number of Blocks, otherwise -1.
 */
int chain_size()
{
    // Library must be initialized
    if (!gInitialized || gShouldClose)
    {
        return -1;
    }
    return gNumOfBlocks;
}

/**
 * DESCRIPTION: This function blocks all other Block attachments, until block_num is added to the chain. The block_num is the assigned value 
 *      that was previously returned by add_block.
 * RETURN VALUE: If block_num doesn't exist, return -2;
 *      In case of other errors, return -1; In case of success or if it is already attached return 0.
 */
int attach_now(int block_num)
{
    // Library must be initialized
    if (!gInitialized || gShouldClose)
    {
        return -1;
    }

    int res = 0;

    if (getGlobalMutex())
    {
        return -1;
    }

    // If block_num doesn't exist, return -2
    if (block_num >= (int)gBlocks.size() || gBlocks[block_num].deleted)
    {
        res = -2;
        goto done;
    }

    // In case block_num is already attached return 0
    if (gBlocks[block_num].attached == true)
    {
        res = 0;
        goto done;
    }

    // Do nothing if block_num is already in addNowQueue
    for (int b : gAddNowQueue)
    {
        if (b == block_num)
        {
            goto done;
        }
    }

    for (auto it = gAddQueue.begin(); it != gAddQueue.end(); it++)
    {
        if (*it == block_num)
        {
            gAddQueue.erase(it);
            break;
        }
    }
    gAddNowQueue.push_back(block_num);

done:
    pthread_mutex_unlock(&gMutex);

    return res;
}

/**
 * DESCRIPTION: Close the recent blockchain and reset the system, so that it is possible to call init_blockchain again. Non-blocking.
 *      All pending Blocks should be hashed and printed to terminal (stdout).
 *      Calls to library methods which try to alter the state of the Blockchain are prohibited while closing the Blockchain. e.g.: Calling   
 *      chain_size() is ok, a call to prune_chain() should fail.
 *      In case of a system error, the function should cause the process to exit.
*/
void close_chain()
{
    int res;

    // Library must be initialized
    if (!gInitialized)
    {
        return;
    }

    if (pthread_mutex_lock(&gInitMutex))
        {
            pthread_exit(NULL);
        }

    if (gShouldClose)
    {
        pthread_mutex_unlock(&gInitMutex);
        return;
    }

    gShouldClose = true;

    if ((res = pthread_cond_signal(&gAddCv)))
    {
        ERROR("pthread_cond_signal returned " << res);
    }

    pthread_mutex_unlock(&gInitMutex);
}

/**
 * DESCRIPTION: The function blocks and waits for close_chain to finish.
 * RETURN VALUE: If closing was successful, it returns 0.
 *      If close_chain was not called it should return -2. In case of other error, it should return -1.
*/
int return_on_close()
{
    // Library must be initialized
    if (!gInitialized)
    {
        return 0;
    }

    if (!gShouldClose)
    {
        return -2;
    }

    if (pthread_mutex_lock(&gInitMutex))
    {
        return -1;
    }

    // If libarary is not initialized, blockchain is already closed
    if (gInitialized)
    {
        if (pthread_join(gDaemonThread, NULL))
        {
            return -1;
        }
    }

    pthread_mutex_unlock(&gInitMutex);

    return 0;
}
