#
# BSD Makefile for fm_opl3.exe
#

WATCOM=$(HOME)/watcom
.export WATCOM

WCC=wcc
WCL=wcl

CFLAGS=-I$(WATCOM)/h

fm_opl3.exe : fmtest.o fm.o
	$(WCL) -fe=$@ fmtest.o fm.o

fm.o : fm.c fm.h
	$(WCC) $(CFLAGS) $<

fmtest.o : fmtest.c fm.h
	$(WCC) $(CFLAGS) $<

clean:
	rm -rf *.o *.err *.exe
