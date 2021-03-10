/*
	TEEN / ARP
	Written by XMaZ Project  2001

	2001/ 1/16 
*/

#if !defined(_ARP_H_)
#define _ARP_H_


#include <ttypes.h>
#include <inet.h>

typedef struct {
	IPADDR ipaddr;
	BYTE macaddr[6];
} ARPENTRY;

char arp_getentry(int n, void far *buf, uint bufsize);

#endif /* _ARP_H_ */
