/*
	TEEN / UDP
	Written by XMaZ Project  1998,2000

	2000/ 1/22
*/

#if !defined(_UDP_H_)
#define _UDP_H_


#include <ttypes.h>
#include <inet.h>

/*******************************
　　　マクロ
*******************************/

/*------------------------------
ポート番号
------------------------------*/

#define UDPPORT_ANY 0

/*------------------------------
udp_state が返す状態
------------------------------*/

#define UDP_SEARCH	1	/* IP アドレス検索中（送受信不可）	*/
#define UDP_OPEN	2	/* 送受信可能				*/
#define UDP_CLOSED	3	/* 相手ホストがみつからず送受信不可	*/

/*------------------------------
　　エラー
------------------------------*/

#define EUDP_BUSY      2	/* 処理中			*/
#define EUDP_BADHANDLE 6	/* ハンドル不正			*/
#define EUDP_NORESOURCE 7	/* ハンドル不足			*/
#define EUDP_RANGE     8	/* アドレス/バンク不正		*/
#define EUDP_BADSTATE 11	/* 実行できる状態でない		*/
#define EUDP_BADPORT  12	/* 使用不可能なポート番号	*/
#define EUDP_NOPEER   13	/* 相手指定なし			*/
#define EUDP_NODATA   14	/* 受信データなし		*/
#define EUDP_BUFOVER  15	/* 受信データが大きすぎ		*/
#define EUDP_DATASIZE 16	/* データサイズが不適当		*/


/*******************************
　　　型
*******************************/

typedef struct {
				/* 受信バッファ */
	void far *rbuf;		/* アドレス */
	uint rbank;		/* バンク */
	uint rbufsize;		/* サイズ(バイト) */
				/* 送信バッファ */
	void far *sbuf;		/* アドレス */
	uint sbank;		/* バンク */
	uint sbufsize;		/* サイズ(バイト) */

	void (huge *callback)();/* コールバック関数 */
	WORD myport;		/* 自分のポート番号 */
	WORD peerport;		/* 相手のポート番号 */
	void far *host;		/* 相手のホスト記述子 */
	int  callbackid;
} UDPPRM;


/*******************************
　　　変数
*******************************/

extern int udperrno;		/* エラー番号 */


/*******************************
　　　プロトタイプ
*******************************/

char udp_open(const UDPPRM far *prm);
char udp_close(char handle);
char udp_state(char handle);
int  udp_send(char handle, const char far *buf, int len);
int  udp_recv(char handle, char far *buf, int len);
int  udp_sendto(char handle, const BYTE far *buf, int len,
					  const void far *toaddr, WORD toport);
int  udp_recvfrom(char handle, char far *buf, int len, 
				     IPADDR far *fromaddr, WORD far *fromport);
char udp_getmyaddr(char handle, IPADDR far *addr, WORD far *port);
char udp_getpeeraddr(char handle, IPADDR far *addr, WORD far *port);


#endif /* _UDP_H_ */
