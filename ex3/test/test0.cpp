/*
* test0.cpp
*
*  Created on: 5 במאי 2015
*      Author: roigreenberg
*/

#include <unistd.h>
#include <iostream>
#include "../blockchain.h"
#include "../hash.h"
#include <cassert>
#define FAIL -1
#define SUCCESS 0

int firstAdds = 4;
int secondAdds = 4;
char l[] = "lll";
char* pl = l;
size_t len = 4;

using namespace std;
void add(int add, char* charStar, size_t length, bool consequntial = false)
{
	for(int i = 1; i <= add; i++)
	{
		cout << "add "<< i << endl;
		if(consequntial)
			{
				assert(add_block(charStar, length) == i);
			}
			else
			{
				cout << add_block(charStar, length) << endl;
			}
	}
}

void notInitiated()
{
	assert(add_block(pl, 4) == FAIL);
	assert(to_longest(0) != SUCCESS);
	assert(to_longest(1) != SUCCESS);
	assert(to_longest(-1) != SUCCESS);
	assert(attach_now(0) != SUCCESS);
	assert(attach_now(1) != SUCCESS);
	assert(attach_now(-1) != SUCCESS);
	assert(was_added(0) != SUCCESS);
	assert(was_added(1) != SUCCESS);
	assert(was_added(-1) != SUCCESS);
	assert(chain_size() == FAIL);
	assert(return_on_close() == SUCCESS);
	close_chain();
	assert(return_on_close() == SUCCESS);
	assert(chain_size() == FAIL);
}
int main() {	
	cout << "********* start test **********" << endl;
	
	notInitiated();
	cout << "works fine before initiating" << endl;

	//init close and recheck
	assert(init_blockchain() == SUCCESS);
	close_chain();
	assert(return_on_close() == SUCCESS);
	cout << "reclosed" << endl;
	notInitiated();
	cout << "works fine after reclosing" << endl;
	//init
	assert(init_blockchain() == SUCCESS);

	//tests state after init
	assert(init_blockchain() == FAIL);

	assert(return_on_close() == -2);
	assert(to_longest((firstAdds+secondAdds)*2) == -2);
	cout << "finish initialing " << endl;

	add(firstAdds, pl, len, true);
	cout << "wait for adding to chain" << endl;;

	while (chain_size() == 0);
	cout << "somthing added!" << endl;;
	
	// check pruning does not dec chain size
	while(chain_size() != firstAdds);

	cout << "3 is now to_longest" << endl;
	to_longest(3);

	cout << "sleeps" << endl;
	sleep(firstAdds);
	for(int i = 1; i <= firstAdds; i++)
	{
		assert(was_added(i) == 1);
	}
	assert(chain_size() >= firstAdds);
	assert(to_longest(1) == 1);
	
	//check pruning something
	prune_chain();
	bool pruned = false;
	for(int i = 1; i <= firstAdds; i++)
	{
		if(was_added(i) == -2)
		{
			pruned = true;
		}
	}
	assert(pruned);
	assert(chain_size() >= firstAdds); //might be caused by unfunctional was_added

	//note that at the end prints only should point to 0

	add(secondAdds, pl, len);
	
	cout << "5 is being attach_now" << endl;
	attach_now(5);

	sleep(secondAdds);

	assert(chain_size() == (firstAdds + secondAdds));
	cout << "add 6:" << endl;
	cout << add_block(pl, len) << endl;

	cout << "closing" << endl;
	close_chain();
	cout << "closed wait for return" << endl;
	return_on_close();
	cout << "********* test end **********" << endl;
	return 0;
}



