#
# BSD Makefile for Open Watcom v2 tools
#

WATCOM=/home/hiroki/ow/open-watcom-v2/rel
.export WATCOM

WCL=bwcl
WASM=bwasm
WCC=bwcc
WLIB=bwlib

CFLAGS=-I$(WATCOM)/h -I../include/

OBJS=arp.o dialup.o dns.o in_addr2.o in_addrv.o in_wild.o ind_char.o ind_int.o ind_str.o ind_sw.o ip_icmp.o ip_route.o t_avail.o t_mach.o t_search.o t_versio.o tcp.o timer.o udp.o in_addr.o

all: test.exe 

test.exe : test.c teen_ow.lib
	$(WCL) $(CFLAGS) -fe=$@ test.c teen_ow.lib

.asm.o:
	$(WASM) $<

.c.o:
	$(WCC) $(CFLAGS) $<

teen_ow.lib: $(OBJS)
	$(WLIB) -n teen_ow +arp.o +dialup.o +dns.o +in_addr2.o +in_addrv.o +in_wild.o +ind_char.o +ind_int.o +ind_str.o +ind_sw.o +ip_icmp.o +ip_route.o +t_avail.o +t_mach.o +t_search.o +t_versio.o +tcp.o +timer.o +udp.o


clean:
	rm -rf *.o *.err *.exe *.com *.lib *.bak
