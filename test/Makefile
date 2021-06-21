#
# BSD Makefile for Open Watcom v2 sample program 
#

WATCOM=$(HOME)/watcom
.export WATCOM

WCL=wcl

CFLAGS=-I$(WATCOM)/h

all: a.com b.com a.exe b.exe c.exe fm.exe

a.exe : a.c
	$(WCL) $(CFLAGS) $?

a.com : a.c
	$(WCL) $(CFLAGS) -l=com $?

b.exe : b.cpp
	$(WCL) $(CFLAGS) $?

b.com : b.cpp
	$(WCL) $(CFLAGS) -l=com $?

c.exe : c.asm
	$(WCL) c.asm

fm.exe : fm.c
	$(WCL) $(CFLAGS) fm.c

clean:
	rm -rf *.o *.err *.com *.exe
