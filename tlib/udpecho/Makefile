#
# BSD Makefile for Open Watcom v2
#

WATCOM=/home/hiroki/ow/open-watcom-v2/rel
.export WATCOM

WCL=bwcl

CFLAGS=-I$(WATCOM)/h -I../include

all: udpecho.exe

udpecho.exe : udpecho.c
	nkf -s udpecho.c > udpecho.c_
	$(WCL) $(CFLAGS) -fe=$@ udpecho.c_ ../source/teen_ow.lib

clean:
	rm -rf *.c_ *.o *.err *.com *.exe
