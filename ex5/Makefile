CC=gcc

SRC=srftp.c clftp.c shared.c
OBJ=$(SRC:.c=.o)

INCS=-I.
CFLAGS = -Wall $(INCS)

TARGETS = srftp clftp

TAR=tar
TARFLAGS=-cvf
TARNAME=ex5.tar
TARSRCS=$(SRC) shared.h Makefile README


all: $(TARGETS) 

srftp.o: shared.h
clftp.o: shared.h
shared.o: shared.h

srftp: srftp.o shared.o
	$(CC) $(CFLAGS) $^ -lpthread -o $@

clftp: clftp.o shared.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(TARGETS) $(OBJ) *~ *core

depend:
	makedepend -- $(CPPFLAGS) -- $(SRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
