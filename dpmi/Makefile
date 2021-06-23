#
# BSD Makefile for Open Watcom v2 sample program 
#

WATCOM=$(HOME)/watcom
.export WATCOM
LIB=$(WATCOM)/lib386
.export LIB 

WCC=wcc386

CFLAGS=-I$(WATCOM)/h

dpmi32.exe : dpmi32.asm
	wasm dpmi32.asm
	wlink file dpmi32.o system dos

clean:
	rm -rf *.o *.err *.com *.exe
