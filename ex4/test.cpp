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
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

using namespace std;

#define MOUNT(path) "/tmp/mount/" path
#define SZ 1024*10

int main(int argc, char* argv[])
{
	int f;
	ssize_t ret;
	char buf[SZ];

	printf("\ntest begin\n");

	f = open(MOUNT(".filesystem.log"),O_RDONLY);
	assert(f == -1 && errno == ENOENT);

	f = open(MOUNT("qwer"),O_RDONLY);
	assert(f == -1 && errno == ENOENT);

	f = open(MOUNT("small"),O_RDWR);
	assert(f == -1 && errno == EACCES);

	f = open(MOUNT("small"),O_RDONLY);
	assert(f > 0);
	ioctl(f, 0);
	close(f);

	for (int i = 0; i < 5; i++)
	{
		f = open(MOUNT("small"),O_RDONLY);
		memset(buf, 0, SZ);
		ret = pread(f, buf, 1024, 0);
		assert(ret == 11);
		close(f);
	}

	f = open(MOUNT("small"),O_RDONLY);
	ioctl(f, 0);
	memset(buf, 0, SZ);
	ret = pread(f, buf, 2, 2);
	assert(ret == 2);
	ioctl(f, 0);
	close(f);

	f = open(MOUNT("large"),O_RDONLY);
	assert(f > 0);

	memset(buf, 0, SZ);
	ret = pread(f, buf, SZ, 0);
	cout << ret << endl;
	assert(ret == 1024*5+1);

	ioctl(f, 0);

	memset(buf, 0, SZ);
	ret = pread(f, buf, 2, 2);
	assert(ret == 2);

	cout << buf << endl;

	close(f);

	f = open(MOUNT("f/small"),O_RDONLY);
	memset(buf, 0, SZ);
	ret = pread(f, buf, 1024, 0);
	assert(ret == 11);
	ioctl(f, 0);

	rename(MOUNT("f/small"), MOUNT("f/smaller"));
	ioctl(f, 0);
	rename(MOUNT("f/smaller"), MOUNT("f/small"));
	ioctl(f, 0);

	close(f);

	f = open(MOUNT("empty"),O_RDONLY);
	memset(buf, 0, SZ);
	ret = pread(f, buf, 1024, 0);
	assert(ret == 0);
	ioctl(f, 0);
	close(f);


	printf("success!\n");

	return 0;
}

