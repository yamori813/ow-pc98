#
# BSD Makefile for Open Watcom v2
#

WATCOM=$(HOME)/watcom
.export WATCOM

WCL=wcl
WASM=wasm

CFLAGS=-I$(WATCOM)/h -I../include -zk0
# Turbo C compatible mode
CFLAGS+=-ecc -zp1

all: sample.exe

sample.exe : sample.c sub.o
	nkf -s sample.c > sample.c_
	$(WCL) $(CFLAGS) -fe=$@ sample.c_ sub.o ../turboc/galib.lib

sub.o: sub.asm
	$(WASM) sub.asm

clean:
	rm -rf *.c_ *.o *.err *.com *.exe
