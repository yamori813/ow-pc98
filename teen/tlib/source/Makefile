#
# BSD Makefile for Open Watcom v2 tools
#

WATCOM=$(HOME)/watcom
.export WATCOM

WCL=wcl
WASM=wasm
WCC=wcc
WLIB=wlib

CFLAGS=-I$(WATCOM)/h -I../include/

OBJS=arp.o dialup.o dns.o in_addr2.o in_addrv.o in_wild.o ind_char.o ind_int.o ind_str.o ind_sw.o ip_icmp.o ip_route.o t_avail.o t_mach.o t_search.o t_versio.o tcp.o timer.o udp.o in_addr.o

all: teen_ow.lib

test.exe : test.c teen_ow.lib
	$(WCL) $(CFLAGS) -fe=$@ test.c teen_ow.lib

teen_ow.lib: $(OBJS)
	$(WLIB) -n teen_ow +arp.o +dialup.o +dns.o +in_addr2.o +in_addrv.o +in_wild.o +ind_char.o +ind_int.o +ind_str.o +ind_sw.o +ip_icmp.o +ip_route.o +t_avail.o +t_mach.o +t_search.o +t_versio.o +tcp.o +timer.o +udp.o +in_addr.o

.asm.o:
	$(WASM) $<

.c.o:
	$(WCC) $(CFLAGS) $<

clean:
	rm -rf *.o *.err *.exe *.com *.lib *.bak
