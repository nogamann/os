
make: CachingFileSystem

CachingFileSystem:
	g++ -Wall -std=c++11 -D_FILE_OFFSET_BITS=64 CachingFileSystem.cpp CachingManager.cpp log.c `pkg-config fuse --cflags --libs` -o CachingFileSystem

mount1:
	./CachingFileSystem test/rootdir /tmp/mount/ 10 10
	
mount2:
	./CachingFileSystem test/rootdir /tmp/mount/ 10 1000

unmount:
	fusermount -u /tmp/mount

#test: CachingFileSystem 
#	fusermount -u /home/roigreenberg/workspace/ex4/src//tmp/mount/	
#	./CachingFileSystem test/rootdir /tmp/mount/ 10 4096
OUT= > output

test0:
	g++ -Wall test.cpp -o mytest

test1: make mount1
	./journey
	./mytest 0

test2: unmount mount2
	./mytest 0
	
test3: unmount mount1
	./mytest 1
	./mytest 2

test4: unmount mount2
	./mytest 3

test: test0 test1 test2 test3 test4 unmount

ftest1: make mount1
	./journey $(OUT)
	./mytest 0 >$(OUT)

ftest2: unmount mount2
	./mytest 0 >$(OUT)
	
ftest3: unmount mount1
	./mytest 1 >$(OUT)
	./mytest 2 >$(OUT)

ftest4: unmount mount2
	./mytest 3 >$(OUT)

test_to_file: test0 ftest1 ftest2 ftest3 ftest4 unmount

testdebug: test0
	./journey
	./mytest 0
	./mytest 1
	./mytest 2
	./mytest 3


.PHONY: CachingFileSystem CachingFileSystem.o test test0
