#
# BSD Makefile for Open Watcom v2 sample program 
#

WATCOM=$(HOME)/watcom
.export WATCOM
LIB=$(WATCOM)/lib386
.export LIB 

WCC=wcc386
WASM=wasm

CFLAGS=-I$(WATCOM)/h

all : dpmi32.exe welcome.exe hello.exe

hello.exe :
	$(WASM) start.asm
	$(WASM) main.asm
	$(WCC) hello.c
	wlink op map name hello file start.o,main.o,hello system dos

welcome.exe :
	$(WASM) start.asm
	$(WASM) welcome.asm
	wlink name welcome file start.o,welcome.o system dos

dpmi32.exe : dpmi32.asm
	wasm dpmi32.asm
	wlink file dpmi32.o system dos

clean:
	rm -rf *.o *.err *.com *.exe *.map
