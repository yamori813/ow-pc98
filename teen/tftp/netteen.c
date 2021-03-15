/*
 * netteen.c
 *
 *
 */

#include <stdio.h>
#include <string.h>

#include "net_ci.h"
#include "netteen.h"

UDPPRM udpprm;

unsigned ver_maj, ver_min;


/* UDP OPEN
 *
 * host:	接続先ホスト名
 * peerport:接続先ポート番号
 *
 */
nethandle_t net_udp_open(void far *host, WORD myport, WORD peerport, char far *udprbuf, int udprbuf_size)
{
	char state, handle;
	
	udpprm.rbank = udpprm.sbank = udpprm.sbufsize = udpprm.callbackid = 0;
	udpprm.sbuf = 0;
	udpprm.callback = 0;
	udpprm.myport = myport;
	udpprm.peerport = peerport;
	udpprm.rbuf = udprbuf;
	udpprm.rbufsize = udprbuf_size;
	udpprm.host = host;
	
	handle = udp_open(&udpprm);
	if (handle != 0xff)
	{
		while((state = udp_state(handle)) != 0xff)
		{
			switch(state)
			{
				case UDP_OPEN:	return handle;
				case UDP_CLOSED:return -1;
			}
		}
		return -1;
	}
	else
		return -1;
}

/* UDP CLOSE */
char net_udp_close(nethandle_t handle)
{
	return udp_close(handle);
}

/* UDP SEND
 *
 * handle: ハンドル
 * buf: 送信データ
 * len: データ長
 * ret: 送信したバイト数
 * 送信されるまでブロックする。
 *
 */
int net_udp_send(nethandle_t handle, char far *buf, int len)
{
	return udp_send(handle, buf, len);
}


/* UDP RECV without BLOCK
 *
 * handle: ハンドル
 * buf: 受信バッファ
 * len: 受信バッファサイズ
 * ret: 0以上==受信したバイト数  -1==データがない -1未満==エラー
 *
 */
int net_udp_recv(nethandle_t handle, char far *buf, int len)
{
	int rbytes;
	rbytes = udp_recv(handle, buf, len);
	return rbytes;
}


/* UDP SEND TO
 *
 * 
 * 送信されるまでブロックする。
 *
 */
int net_udp_sendto(n_udp_t *udp)
{
	return
		udp_sendto(udp->handle, udp->buf, udp->len, udp->r_addr, udp->r_port);
}


IPADDR far addr;

/* UDP RECV FROM without BLOCK
 *
 * udp: UDP情報格納構造体
 * ret: 0以上==受信したバイト数  -1==データがない -1未満==エラー
 *
 */
int net_udp_recvfrom(n_udp_t *udp)
{
	int rbytes;
	
	rbytes =
	udp_recvfrom(udp->handle, udp->buf, udp->len, &addr, &udp->r_port);
	if (rbytes >= 0) inet_tostr(&addr, udp->r_addr);
	
	return rbytes;
}

/* UDP アドレスを得る
 *
 *
 */
int net_udp_get_peeraddr(nethandle_t handle, char far *ipstr, WORD *port)
{
	udp_getpeeraddr(handle, &addr, port);
	inet_tostr(&addr, ipstr);
	
	return 0;
}

/* UDP アドレスを得る
 *
 *
 */
int net_udp_get_thisaddr(nethandle_t handle, char far *ipstr, WORD *port)
{
	udp_getmyaddr(handle, &addr, port);
	inet_tostr(&addr, ipstr);
	
	return 0;
}


/* net_get_nameserver() - 設定されているネームサーバのIPを得る
 *
 * ret:	NULL=失敗, それ以外=静的バッファへのアドレス
 *
 */
char *net_get_nameserver(void)
{
	IPADDR addr;
	WORD port;
	static IPADDRSTR ipstr;
	
	if (udp_getpeeraddr(0, &addr, &port) == 0)
	{
		inet_tostr(&addr, &ipstr);
		return ipstr;
	}
	else
		return 0;
}

/* net_get_ipaddr() - IP アドレスを調べる
 * hostname: IP を調べるホスト名
 * ret: 静的バッファへのアドレス. 失敗時は NULL.
 *
 */
char *net_get_ipaddr(unsigned char *hostname)
{
	static char ipstr[16];
	IDWORD ip;
	
	if (inet_aton(hostname) != 0)
	{
		strcpy(ipstr, hostname);
		return ipstr;
	}
	
	ip = getipaddress(hostname);
	if (ip == 0) return 0;
	strcpy(ipstr, inet_ntoa(ip));
	
	return ipstr;
}

/* 常駐チェック */
void net_resident_check(void)
{
	if (!teen_available())
	{
		printf("TEEN が常駐していません。\a\n");
		exit(250);
	}
}

/* バージョンチェック */
void net_ver_check(void)
{
	ver_maj = teen_version();
	ver_min = ver_maj >> 8;
	ver_maj &= 0xff;
	
	if (ver_maj < 0 && ver_min < 32)
	{
		printf("TEEN v0.32 以降を使用してください。\a\n");
		exit(251);
	}
}
