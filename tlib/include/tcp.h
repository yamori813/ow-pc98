/*
	TEEN / TCP
	Written by XMaZ Project.  1997,1998

	1998/ 9/17 
*/

#if !defined(_TCP_H_)
#define _TCP_H_


#include <ttypes.h>


/*******************************
　　　マクロ
*******************************/

/*------------------------------
tcp_state が返す状態
状態遷移通知関数に与えられるイベント番号 ([]内)
------------------------------*/

#define TCP_LISTEN     1	/*LISTEN	相手からの接続要求待ち*/
#define TCP_WAITACCEPT 2	/*WAITACCEPT	接続受理待ち
						[相手から接続が要求された]*/
#define TCP_SEARCH     3        /*              IPアドレス検索中 */
#define TCP_OPENING    4	/*SYNSENT	接続処理中 
				  SYNRCVD */
#define TCP_OPEN       5	/*ESTAB		接続している
						[接続が完了した]*/
#define TCP_HISCLOSING 6	/*CLOSEWAIT	相手から切断が要求された
						(TCP_SHUTDOWNまち)
						(相手からの受信はもうない)
						[切断が相手から要求された]*/
#define TCP_MYCLOSING  7	/*FINWAIT1	切断を要求した
				  FINWAIT2	(こちらからの送信はもうない)*/
#define TCP_CLOSING    8	/*LASTACK	切断処理中(送受信はもうない)
				  CLOSING
				  TIMEWAIT*/
#define TCP_CLOSED     9	/*CLOSED	完全に切断した
						[完全に切断した]*/


/*------------------------------
　　切断原因(tcp_downstate)
------------------------------*/

#define TCP_NOTDOWN	0	/* まだ切断は開始されていない */
#define TCP_HISDOWN	1	/* 相手が切断を要求した */
#define TCP_MYDOWN	2	/* こちらが切断を要求した */
#define TCP_REJECT	3	/* 接続が拒否された */
#define TCP_TIMEOUT	4	/* タイムアウト */
#define TCP_NOHOST	5	/* 接続先が見つからない */


/*------------------------------
　　エラー
------------------------------*/

#define ETCP_BUSY       2/* 処理中					*/
#define ETCP_BADHANDLE  6/* ハンドル不正	 send,recv,state,close	*/
		         /*			 shutodown,accept	*/
#define ETCP_NORESOURCE 7/* ハンドル不足	 aopen,popen,accept	*/
#define ETCP_RANGE      8/* アドレス/バンク不正	 aopen,accept		*/

#define ETCP_EXIST      9/* オープン済み	 aopen,popen		*/
#define ETCP_CLOSE     10/*クローズしている	 send,shutdown		*/
#define ETCP_BADSTATE  11/*acceptできない状態	 accept			*/
 		         /*接続先未指定		 send,recv		*/
#define ETCP_BADPORT   12/*ポート番号不正	 aopen,popen		*/


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

	void (huge *notice)();	/* 状態遷移通知関数 */
	WORD thisport;		/* ポート番号 */
	WORD peerport;		/* 接続先ポート番号 */
	char far *host;		/* 接続先ホスト名 */
				/* accept では、thisport,peerport,hostは不要 */
} TCPPRM;


typedef struct {
	WORD port;		/* ポート番号 */
	IDWORD ip;		/* IPアドレス */
} TCPADDR;



/*******************************
　　　変数
*******************************/

extern int tcperrno;		/* エラー番号 */


/*******************************
　　　プロトタイプ
*******************************/

char tcp_activeopen(TCPPRM far *tcpprm);
char tcp_passiveopen(WORD thisport, void (huge *notice)());
char tcp_accept(char handle, TCPPRM far *tcpprm);
char tcp_shutdown(char handle);
int  tcp_send(char handle, char far *buf, int len, int flag);
int  tcp_recv(char handle, char far *buf, int len);
char tcp_close(char handle);
char tcp_state(char handle);
char tcp_downstate(char handle);
char tcp_getaddr(char handle, TCPADDR far *addr);
		/* addr には TCPADDR型配列で要素数2のものへのポインタを与える*/

#endif /* _TCP_H_ */
