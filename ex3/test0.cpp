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

	cout << "finish initialing " << endl;


	char l[] = "lll";
	char* pl = l;
	cout << "add 1:" << endl;
	cout << add_block(pl, 4) << endl;

	cout << "add 2:" << endl;
	cout << add_block(pl, 4) << endl;
	cout << "add 3:" << endl;
	cout << add_block(pl, 4) << endl;

	cout << "wait for adding to chain" << endl;;

	while (chain_size() == 0);

	cout << "somthing added!" << endl;;

	cout << "3 is now to_longest" << endl;
	to_longest(3);


	sleep(5);

	cout << "add 4:" << endl;
	cout << add_block(pl, 4) << endl;
	cout << "add 5:" << endl;
	cout << add_block(pl, 4) << endl;

	cout << "5 is being attach_now" << endl;
	attach_now(5);

	sleep(5);

	//	printChain();
	cout << "final size: " << chain_size() << endl;
	cout << "add 6:" << endl;
	cout << add_block(pl, 4) << endl;

	cout << "closing" << endl;
	close_chain();
	return_on_close();
	cout << "********* test end **********" << endl;
	return 0;
}



