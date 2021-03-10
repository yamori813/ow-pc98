/*
	TEEN / IP, ICMP
	Written by XMaZ Project  2000,2001

	2001/ 4/23
*/

#if !defined(_IP_H_)
#define _IP_H_


#include <ttypes.h>
#include <inet.h>

/*******************************
　　　型
*******************************/

				/* ルーティングテーブルエントリ */
typedef struct {
	IPADDR	dest;		/* 宛て先ホスト/ネットアドレス 
				   ワイルドカードアドレス(0.0.0.0)
				   		..デフォルトルート */
	uchar destmask;		/* dest のネットマスクの1の部分の
				   ビット数
				   0(0.0.0.0)..デフォルトルート
				   32(255.255.255.255)..ホストルート
				   その他..ネットワークルート */
	IPADDR	gateway;	/* Gateway (flag.gateway=ON時のみ) */
	struct {
		int valid:1;	/* このエントリは有効 */
		int gateway:1;	/* indirect(gateway使用)ルート */
		int reserved:14;
	} flag;
	uchar interface;	/* ネットワークインタフェース番号 */
} RTENTRY;


				/* icmp_send/recv パラメータ */
typedef struct {
	void far *src;		/* IPパケットの送信元(ホスト名不可) 
				   送信時は NULL 
				   受信時は IPADDR への　ポインタ */
	void far *dest;		/* IPパケットの送信先(ホスト名不可) 
				   送信時はIPADDRまたはIPアドレス文字列への、
				   受信時はIPADDRへの　ポインタ */
	uchar ttl;		/* TTL(最長ホップ数) */
	uchar dmy;		/* 未使用 */

	uchar type;		/* ICMPタイプ */
	uchar code;		/* ICMPコード */
	uint id;		/* ICMPメッセージ識別番号 */
	uint seq;		/* ICMPメッセージシーケンス番号 */
	void far *buf;		/* ICMPオプションデータへのポインタ */
	uint len;		/* 上記オプションデータのサイズ(バイト) */
				/* オプションデータとは、ここでは、
				   ICMPパケット先頭から8バイト目以降のデータを
				   指す。*/
} ICMPPRM;


/*******************************
　　　プロトタイプ
*******************************/

char ip_getrtentry(int n, void far *buf, uint bufsize);

#if 0
/* v0.37 */
char icmp_send(const void far *dmy, const void far *dest, WORD ttl, WORD codetype, WORD id, WORD seq, const char far *buf, int len);
int  icmp_recv(IPADDR far *src, IPADDR far *dest, WORD *dmy, WORD *codetype, WORD *id, WORD *seq, char far *buf, int len);
#endif


/* v0.37a */
char icmp_send(const ICMPPRM far *icmpprm);
int  icmp_recv(ICMPPRM far *icmpprm);

#endif /* _IP_H_ */
