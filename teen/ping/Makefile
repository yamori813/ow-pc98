#
# BSD Makefile for Open Watcom v2
#

WATCOM=$(HOME)/watcom
.export WATCOM

WCL=wcl

CFLAGS=-I$(WATCOM)/h -I../tlib/include -zk0

all: ping.exe

ping.exe : ping.c
	nkf -s ping.c > ping.c_
	$(WCL) $(CFLAGS) -fe=$@ ping.c_ ../tlib/source/teen_ow.lib

clean:
	rm -rf *.c_ *.o *.err *.com *.exe
