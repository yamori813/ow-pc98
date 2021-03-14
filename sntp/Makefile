#
# BSD Makefile for Open Watcom v2
#

WATCOM=/home/hiroki/ow/open-watcom-v2/rel
.export WATCOM

WCL=bwcl

CFLAGS=-I$(WATCOM)/h -I../tlib/include -zk0

all: sntpget.exe

sntpget.exe : sntpget.c
	nkf -s sntpget.c > sntpget.c_
	$(WCL) $(CFLAGS) -fe=$@ sntpget.c_ ../tlib/source/teen_ow.lib

clean:
	rm -rf *.c_ *.o *.err *.com *.exe
