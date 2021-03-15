/*
 * net_ci.h   Network Common Interface header
 *
 *
 */

#ifndef	NET_CI_H
#define	NET_CI_H

#define	PORT_ECHO		7
#define	PORT_FTP		21
#define	PORT_TELNET		23
#define	PORT_SMTP		25
#define	PORT_WHOIS		43
#define	PORT_DNS		53
#define	PORT_FINGER		79
#define	PORT_HTTP		80
#define	PORT_POP3		110
#define	PORT_NNTP		119

#ifdef	NET_TEEN
#include	"netteen.h"
#endif

typedef struct netci_udp_tag {
	nethandle_t	handle;		/* ハンドル */
	char far	*buf;		/* 送受信バッファ */
	int			len;		/* 送信データ長 / 受信バッファ長 */
	void far	*r_addr;	/* 送受信先アドレス */
//	unsigned 	r_port;		/* 送受信先ポート */
	WORD 	r_port;		/* 送受信先ポート */
} n_udp_t;


extern nethandle_t net_udp_open(void far *host, WORD myport, WORD peerport, char far *udprbuf, int udprbuf_size);
extern char net_udp_close(nethandle_t handle);
extern int net_udp_send(nethandle_t handle, char far *buf, int len);
extern int net_udp_recv(nethandle_t handle, char far *buf, int len);
extern char *net_get_nameserver(void);
extern void net_resident_check(void);
extern void net_ver_check(void);
extern char *net_get_ipaddr(unsigned char *hostname);
extern int net_udp_get_thisaddr(nethandle_t handle, char far*ipstr,WORD *port);
extern int net_udp_get_peeraddr(nethandle_t handle, char far*ipstr,WORD *port);

int net_udp_sendto(n_udp_t *udp);
int net_udp_recvfrom(n_udp_t *udp);

#endif
