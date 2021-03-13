/*
	UDP ECHO クライアント

        Written by XMaZ Project  2000
*/


/*
	ECHO プロトコルを使って TEEN の UDP API をテストするためのプログラム。
	ECHO プロトコルは、クライアントが送ったデータをそのままサーバがクライ
	アントに返すプロトコルで、RFC 862 で定義されている。TCP/UDP いずれでも
	使用できる。
	プログラムのテストには便利であるが、セキュリティーの問題により、外部か
	らアクセス可能なホストでは ECHO サーバを動かさないのが一般的である。
*/


#include <stdio.h>
#include <string.h>
#include <conio.h>

#include <teen.h>
#include <inet.h>
#include <udp.h>

/***********************
　　　マクロ
***********************/

#define ECHOPORT 7		/* ECHO のポート番号 */
#define RBUFSIZE 256		/* TEEN に与える受信バッファのサイズ */
#define BUF2SIZE 128		/* データを取得するための受信バッファのサイズ*/


/***********************
　　　変数
***********************/

static char rbuf[RBUFSIZE];
static const char *udpstatename[]={"--","UDP_SEARCH","UDP_OPEN","UDP_CLOSED"};


/***********************
　　　コード
***********************/

void dispaddr(IPADDR *ipaddr)
{
	int i;
	IPADDRSTR str;

	inet_tostr(ipaddr, &str);
	printf("アドレス %s\n",str);
}

void dispmyaddr(char handle)
{
	IPADDR ipaddr;
	WORD port;
	char status;

	status=udp_getmyaddr(handle,&ipaddr,&port);
/*	printf("getmyaddr status=%d err=%d\n",status,udperrno);*/
	puts("自分：");
	if (status==0) {
		dispaddr(&ipaddr);
		printf("ポート %u\n",port);
	}
	else {
		printf("取得失敗 errno=%d\n",udperrno);
	}
}

void disppeeraddr(char handle)
{
	IPADDR ipaddr;
	WORD port;
	char status;

	status=udp_getpeeraddr(handle,&ipaddr,&port);
/*	printf("getpeeraddr status=%d err=%d\n",status,udperrno);*/
	puts("相手：");
	if (status==0) {
		dispaddr(&ipaddr);
		printf("ポート %u\n",port);
	}
	else {
		printf("取得失敗 errno=%d\n",udperrno);
	}
}


/*
send/recv を使った ECHO クライアント
	host で指定したホストの ECHO サーバに対して文字列を送信する。
*/
void udpecho(const char *host)
{
	UDPPRM prm;
	char handle;

//	setmem(&prm,sizeof prm,0);
	memset(&prm,0, sizeof prm);
	prm.rbuf=rbuf;
	prm.rbufsize=RBUFSIZE;
	prm.myport  =UDPPORT_ANY;
	prm.peerport=ECHOPORT;
	prm.host    =host;		/* 相手指定 */

	if ( (handle=udp_open(&prm)) < 0 ) {
		printf("オープン失敗 errno=%d",udperrno);
		return;
	}

	dispmyaddr(handle); disppeeraddr(handle);

	printf("ホスト検索中・・・\n");
	while (udp_state(handle)==UDP_SEARCH) {if (kbhit()) break;}

	dispmyaddr(handle); disppeeraddr(handle);

	if (udp_state(handle)!=UDP_OPEN) {
		printf("%s が見つかりません\n",host);
	}
	else {
	    puts("通信準備完了");

	    for(;;) {
		char buf[128],buf2[BUF2SIZE+1];
		int len;

		puts("送信文字列?");
		gets(buf);
		len=strlen(buf);
		if (len==0) break;
		
		len=udp_send(handle,buf,len);
		if (len>=0) printf("送信 %d バイト\n",len);
		else printf("送信失敗 errno=%d\n",udperrno);

		printf("相手からのデータ待ち・・・\n");
		while ( (len=udp_recv(handle,buf2,BUF2SIZE)) < 0 && 
		        udperrno==EUDP_NODATA ) {
			if (kbhit()) {
				puts("受信中断");
				break;
			}
		}
		if (len<0 && udperrno==EUDP_BUFOVER) {
			/* 受信データは BUF2SIZE より大きかった */
			len=BUF2SIZE;
		}
		if (len>=0) {
			printf("受信 %d バイト\n",len);
			buf2[len]='\0';
			puts(buf2);
		}
		else {
			printf("受信失敗 errno=%d\n",udperrno);
		}
	    }
	}
	if (udp_close(handle)) printf("クローズ失敗 errno=%d\n",udperrno);

//	dispmyaddr(handle); disppeeraddr(handle);
}

/*
sendto/recvfrom を使った ECHO クライアント
	毎回、サーバの IPアドレスと文字列の入力をユーザに促し、
	指定されたサーバに対して入力された文字列を送信する。
*/
void udpecho_noconnection(void)
{
	UDPPRM prm;
	char handle;
	IDWORD toipv4addr;
	IPADDR destip;

//	setmem(&prm,sizeof prm,0);
	memset(&prm,0, sizeof prm);
	prm.rbuf=rbuf;
	prm.rbufsize=RBUFSIZE;
	prm.myport  =UDPPORT_ANY;
	prm.peerport=UDPPORT_ANY;
	prm.host    =NULL;		/* 相手指定なし */

	if ( (handle=udp_open(&prm)) < 0 ) {
		printf("オープン失敗 errno=%d",udperrno);
		return;
	}

/*	printf("state=%s\n",udpstatename[udp_state(handle)]);*/

//	dispmyaddr(handle); disppeeraddr(handle);

	puts("通信準備完了");
	for(;;) {
		char buf[128],buf2[BUF2SIZE+1];
		char host[128];
		int len;
		WORD port;
		IPADDR addr;

		puts("送信先IPアドレス(ホスト名不可)?");
		gets(host);
		if (strlen(host)==0) break;

		puts("送信文字列?");
		gets(buf);
		len=strlen(buf);
		if (len==0) break;

/*
		printf("state %d\n", udp_state(handle));
		if ( inet_toipv4addr(host,&toipv4addr)!=0 )
			printf("inet_toipv4addr error\n");
		else
			printf("IP %lx\n", toipv4addr);
*/
		len=udp_sendto(handle,buf,len,host,ECHOPORT);
		if (len>=0) printf("送信 %d バイト\n",len);
		else printf("送信失敗 errno=%d\n",udperrno);

		printf("相手からのデータ待ち・・・\n");
		while ( (len=udp_recvfrom(handle,buf2,BUF2SIZE,&addr,&port))<0
			&& udperrno==EUDP_NODATA ) {
			if (kbhit()) {
				puts("受信中断");
				break;
			}
		}
		if (len<0 && udperrno==EUDP_BUFOVER) {
			/* 受信データは BUF2SIZE より大きかった */
			len=BUF2SIZE;
		}
		if (len>=0) {
			printf("受信 %d バイト\n",len);
			printf("送信者");
			dispaddr(&addr);
			printf("送信者ポート %u\n",port);
			buf2[len]='\0';
			puts(buf2);
		}
		else {
			printf("受信失敗 errno=%d\n",udperrno);
		}
	}
	if (udp_close(handle)) printf("クローズ失敗 errno=%d\n",udperrno);

//	dispmyaddr(handle); disppeeraddr(handle);
}

void usage(void)
{
	printf("UDP ECHO クライアント  for MS-DOS\n"
	       "Written by XMaZ Project  2000\n\n");
	printf("書式： UDPECHO [hostname]\n");
}

void main(int argc, char *argv[])
{
	if (argc>1 && (argv[1][0]=='/' || argv[1][0]=='-')) {
		usage();
		return;
	}

	if (!teen_available()) {
		/* TEEN が常駐していない */
		printf("TEEN が常駐していません。\a\n");
		return;
	}

	{
		uint vmi,vma;

		vmi=teen_version()>>8;
		vma=teen_version()&0xff;
		if (vma<0 && vmi<32) {
			/* バージョンが古い */
			printf("TEEN v0.32 以降を使用してください。\a\n");
			return;
		}
	}
/*
	if (argc>1) {
		udpecho(argv[1]);
	}
	else {
		udpecho_noconnection();
	}
	*/
	udpecho_noconnection();

}


/*
1998/11/11
2000/ 1/22 
2000/ 1/23 
2000/ 2/10 
*/
