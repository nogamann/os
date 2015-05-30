
make: CachingFileSystem

CachingFileSystem:
	@g++ -Wall -D_FILE_OFFSET_BITS=64 CachingFileSystem.cpp CachingManager.cpp log.c `pkg-config fuse --cflags --libs` -o CachingFileSystem

mountsmall:
	@./CachingFileSystem root /tmp/mount/ 10 8
	
mountlarge:
	@./CachingFileSystem root /tmp/mount/ 10 1024

unmount:
	@-fusermount -u /tmp/mount

test: test.cpp
	g++ -Wall test.cpp -o test

runtest: CachingFileSystem test unmount
	@./CachingFileSystem root /tmp/mount/ 3 1024
	@./test


.PHONY: CachingFileSystem runtest unmount
