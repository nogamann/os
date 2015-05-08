/*
* test0.cpp
*
*  Created on: 5 במאי 2015
*      Author: roigreenberg
*/

#include <unistd.h>
#include <iostream>
#include "blockchain.h"

using namespace std;

int main() {

	cout << "********* start test **********" << endl;
	init_blockchain();

	cout << "finish initializing " << endl;


	char l[] = "lll";
	char* pl = l;

	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);

	to_longest(3);
	attach_now(2);

	sleep(5);

	to_longest(add_block(pl, 4));
	to_longest(add_block(pl, 4));
	to_longest(add_block(pl, 4));
	to_longest(add_block(pl, 4));
	to_longest(add_block(pl, 4));
	to_longest(add_block(pl, 4));

	attach_now(5);

	sleep(5);

	cout << add_block(pl, 4) << endl;

	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	prune_chain();
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);
	add_block(pl, 4);

	while (!was_added(8));

	cout << "final size: " << chain_size() << endl;

	cout << "close_chain()" << endl;
	close_chain();
	cout << "return_on_close()" << endl;
	return_on_close();
	cout << "********* test end **********" << endl;
	pthread_exit(NULL);
	return 0;
}



