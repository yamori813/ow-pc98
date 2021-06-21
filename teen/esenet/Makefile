#
# BSD Makefile for Open Watcom v2
#

WATCOM=$(HOME)/watcom
.export WATCOM

WCL=wcl

CFLAGS=-I$(WATCOM)/h -I../tlib/include -zk0

all: esenet.exe

esenet.exe : esenet.c
	nkf -s esenet.c > esenet.c_
	$(WCL) $(CFLAGS) -fe=$@ esenet.c_ ../tlib/source/teen_ow.lib

clean:
	rm -rf *.c_ *.o *.err *.com *.exe
