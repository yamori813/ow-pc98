#
# BSD Makefile for Open Watcom v2
#

WATCOM=/home/hiroki/ow/open-watcom-v2/rel
.export WATCOM

WCL=bwcl

CFLAGS=-I$(WATCOM)/h -ml

all: tver.exe

tver.exe : tver.c
	$(WCL) $(CFLAGS) $?

clean:
	rm -rf *.o *.err *.com *.exe
