
#include "teen.h"
#include "ns.h"
#include "inet.h"
#include "ip.h"

#include <stdio.h>
#include <string.h>
#include <conio.h>	/* kbhit */

void ping(void *dest)
{
	static WORD seq=0;
	int status;

	IPADDR rhost;
	WORD rseq, rid, rcodetype;
	char buf[100];

	uint sentcount=0, receivedcount=0;
	unsigned long last;

	last=teen_mtime();

	while(1) {
		unsigned long now;

		now=teen_mtime();
		if (now-last>=1000) {
			ICMPPRM icmpprm;

			icmpprm.dmy=0;
			icmpprm.src=NULL;
			icmpprm.dest=dest;
			icmpprm.ttl=255;
			icmpprm.type=8;
			icmpprm.code=0;
			icmpprm.id=0;
			icmpprm.seq=seq++;
			icmpprm.buf=&now;
			icmpprm.len=sizeof now;
			status=icmp_send(&icmpprm);
			/*printf("ping_send: status=%d\n",status);*/
			last=now;
			sentcount++;
		}

		now=teen_mtime();

		{
			ICMPPRM icmpprm;

			icmpprm.src=&rhost;
			icmpprm.dest=NULL;
			icmpprm.buf=buf;
			icmpprm.len=sizeof buf;
			status=icmp_recv(&icmpprm);
			rcodetype=(icmpprm.code<<8) + icmpprm.type;
			rid=icmpprm.id;
			rseq=icmpprm.seq;
		}
		if (status>=0 && rcodetype==0) {
			IPADDRSTR rhoststr;
			unsigned long delay;

			inet_tostr(&rhost,&rhoststr);
			delay=now - *(unsigned long *)buf;
			printf("%d bytes from %s: icmp_seq=%d time=%lu ms\n",status,rhoststr,rseq,delay);
			receivedcount++;
		}
		else if (status>=0) {
			IPADDRSTR rhoststr;

			inet_tostr(&rhost,&rhoststr);
			printf("ICMP message from %s: type=%d code=%d\n",rhoststr,rcodetype & 0x0f, rcodetype >>8);
		}

		if (kbhit()) break;
	}

	printf("%d packets transmitted, %d packets received, %d%% packet loss\n",sentcount,receivedcount,(sentcount-receivedcount)*100/sentcount);
}

int checkstr(char *str)
{
int i, len;

	len = strlen(str);
	for (i = 0; i < len; ++i) {
		if (!((str[i] >= '0' && str[i] <= '9') || str[i] == '.')) {
			printf("HOST NAME\n");
			return 0;
		}
	}

	printf("IP ADDRESS\n");
	return 1;
}

int main(int argc, char *argv[])
{
char *dist;
int ver;
IDWORD destipv4;
IPADDR destip;
unsigned char ip[4];
int i;


	dist = argv[1];
	printf("%s\n", dist);
	if(! teen_available()) {
		printf("TEEN not found");
	} else {
		ver = teen_version();
		printf("TEEN Version %d\n", ver);

		if (checkstr(dist))
			inet_toipv4addr(dist, &destipv4);
		else
			destipv4 = getipaddress(dist);
		if (destipv4 != 0) {
			ip[0] = destipv4 >> 24;
			ip[1] = (destipv4 >> 16) & 0xff;
			ip[2] = (destipv4 >> 8) & 0xff;
			ip[3] = destipv4 & 0xff;
			printf("%d.%d.%d.%d\n", ip[3], ip[2], ip[1], ip[0]);

			inet_ipv4addrtoipaddr(destipv4,&destip);
			for(i = 0; i < sizeof(IPADDR); ++i)
				printf("%d ", destip.data[i]);
			printf("\n");

			ping(&destip);
		}
	}

	return 0;
}
