#	$Id:	$	#

FILES=iter.c
OBFILES=${FILES.c=.o}
CC=gcc
CFLAGS=-g -Wall

iter:$(OBFILES)

clean:
	rm -f $(OBFILES) iter
