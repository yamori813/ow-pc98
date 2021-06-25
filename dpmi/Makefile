#
# BSD Makefile for Open Watcom v2 sample program 
#

WATCOM=$(HOME)/watcom
.export WATCOM
#LIB=$(WATCOM)/lib386
#.export LIB 

WCC=wcc386
WASM=wasm
WLINK=wlink

# now use stack check
CFLAGS=-I$(WATCOM)/h -s

all : dpmi32.exe welcome.exe hello.exe

hello.exe :
	$(WASM) start16.asm
	$(WASM) main.asm
	$(WCC) $(CFLAGS) hello.c
	$(WLINK) op map name hello file start16,main,hello system dos

welcome.exe :
	$(WASM) start16.asm
	$(WASM) welcome.asm
	$(WLINK) name welcome file start16,welcome system dos

dpmi32.exe : dpmi32.asm
	$(WASM) dpmi32.asm
	$(WLINK) file dpmi32 system dos

clean:
	rm -rf *.o *.err *.com *.exe *.map
