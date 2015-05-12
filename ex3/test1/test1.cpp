/*
 * test1.cpp
 *
 *  Created on: May 10, 2015
 *      Author: roigreenberg
 */

#include <unistd.h>
#include <iostream>
#include "../blockchain.h"
#include "../hash.h"
#include <stack>

using namespace std;

#define PHASE_1 50
#define PHASE_2 59



int main() {

	cout << "********* start test **********" << endl;
	init_hash_generator();
	init_blockchain();

	cout << "finish initialing " << endl;


	char l[] = "lll";
	char* pl = l;

	cout << "Phase 1:\n" << endl;

	for (int j = 1; j <= 5; j++)
	{
		cout << "Adding level " << j << ":\n" << endl;

		for (int i = 1; i <= 10; i++)
		{
			cout << "add " << i << ": ";
			cout << add_block(pl, 4) << endl;
		}

		while(chain_size() < j*10);
	}

	cout << "Expected chain size: " << PHASE_1 << ". actual: " << chain_size() << endl;
	prune_chain();

	cout << "Phase 2:\n" << endl;

	int id = 0;

	for (int i = 1; i <= 9; i++)
	{
		cout << "add " << i << ": ";
		id = add_block(pl, 4);
		cout << id << endl;
		to_longest(id);
	}
	//while(chain_size() < 59);
	cout << "Expected chain size: " << PHASE_2 << ". actual: " << chain_size() << endl;
	prune_chain();

	close_chain();
	return_on_close();

	cout << "********* test end **********" << endl;

	return 0;
}


//void printChain(void)
//{
//	cout << "print :" << endl;
//
//
//	Block* cur;
//
//	cout << (*cur).num_block << endl;
//	while(cur->num_block != 0)
//	{
//		cout << cur->num_block << "  ->  " ;
//		cur = cur->father; //move to the father
//	}
//	cout << cur->num_block << endl;
//
//}
