#
# BSD Makefile for Open Watcom v2
#

WATCOM=/home/hiroki/ow/open-watcom-v2/rel
.export WATCOM

WCL=bwcl

CFLAGS=-I$(WATCOM)/h -ml

all: tver.exe tdns.exe

tver.exe : tver.c
	$(WCL) $(CFLAGS) $?

tdns.exe : tdns.c
	$(WCL) $(CFLAGS) $?

clean:
	rm -rf *.o *.err *.com *.exe
