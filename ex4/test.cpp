/*
 * test.cpp
 *
 *  Created on: 25 במאי 2015
 *      Author: roigreenberg
 */
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include  <assert.h>
#include <cstring>
#include <string>
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
	if (strcmp(argv[1], "0") == 0)
	{
		cout << "*******test 0**********" << endl;
		int f = open("/tmp/mount/dira/testa",O_RDONLY);
		char * buf = (char*) malloc(5010);
		ssize_t i;

		i = pread(f, buf, 10, 0);
		assert (i == 10);
		cout << "read: " << i << " bytes\n" << buf << "***" << endl;
		i = pread(f, buf, 27, 10);
		assert (i == 27);
		cout << "read: " << i << " bytes\n" << buf << "***" << endl;
		close(f);
		free(buf);
		cout << "########end test 0#######" << endl;
	}
	if (strcmp(argv[1], "1") == 0)
	{
		cout << "*******test 1**********" << endl;
		int f = open("/tmp/mount/dira/dira1/testa1",O_RDONLY);
		char * buf = (char*) malloc(5010);
		ssize_t i;

		i = pread(f, buf, 37, 0);
		assert (i == 37);
		cout << "read: " << i << " bytes\n" << buf << "***" << endl;
		i = pread(f, buf, 74, 37);
		assert (i == 74);
		cout << "read: " << i << " bytes\n" << buf << "***" << endl;
		i = pread(f, buf, 111, 111);
			assert (i == 76);
		cout << "read: " << i << " bytes\n" << buf << "***" << endl;
		close(f);
		free(buf);
		cout << "########end test 1#######" << endl;
	}
	if (strcmp(argv[1], "2") == 0)
	{
		cout << "*******test 2**********" << endl;
		int f = open("/tmp/mount/dirb/testb",O_RDONLY);
		char * buf = (char*) malloc(5010);
		ssize_t i;

		i = pread(f, buf, 100, 0);
		assert (i == 39);
		cout << "read: " << i << " bytes\n" << buf << "***" << endl;
		i = pread(f, buf, 100, 100);
		assert (i <= 0);
		cout << "offset to big: " << i << endl;
		close(f);
		free(buf);
		cout << "########end test 2#######" << endl;
	}
	if (strcmp(argv[1], "3") == 0)
	{
		cout << "*******test 3**********" << endl;
		rename("/tmp/mount/dira/dira1/dira1a/testa1a", \
				"/tmp/mount/dira/dira1/dira1a/test");
		int f = open("/tmp/mount/dira/dira1/dira1a/test",O_RDONLY);
		char * buf = (char*) malloc(10000);
		ssize_t i;

		i = pread(f, buf, 4096, 4096);
		assert (i == 4096);
		cout << "read: " << i << " bytes\n" << buf << "***" << endl;
		close(f);
		free(buf);
		rename("/tmp/mount/dira/dira1/dira1a/test", \
						"/tmp/mount/dira/dira1/dira1a/testa1a");
		cout << "########end test 3#######" << endl;
	}
	return 0;
}

