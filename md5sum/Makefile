CC=cc
CFLAGS=-DUNIX
BINDIR=/usr/local/bin

all: md5sum

install: md5sum
	install -m 755 md5sum $(BINDIR)

clean:;
	rm -f md5sum *.o

md5sum: md5sum.o md5.o
	$(CC) -o $@ md5sum.o md5.o

md5sum.o md5.o: md5.h
