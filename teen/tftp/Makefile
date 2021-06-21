#
# BSD Makefile for Open Watcom v2
#

WATCOM=$(HOME)/watcom
.export WATCOM

WCL=wcl

CFLAGS=-I$(WATCOM)/h -I../tlib/include -DNET_TEEN -zk0

all: tftp.exe

tftp.exe : tftp.c netteen.c
	nkf -s tftp.c > tftp.c_
	nkf -s netteen.c > netteen.c_
	$(WCL) $(CFLAGS) -fe=$@ tftp.c_ netteen.c_ ../tlib/source/teen_ow.lib

clean:
	rm -rf *.c_ *.o *.err *.com *.exe
