/*
	PING for TEEN

	Written by XMaZ Project  2001
*/

#include <teen.h>
#include <ns.h>
#include <inet.h>
#include <ip.h>

#include <stdio.h>
#include <string.h>
#include <conio.h>	/* kbhit */

#define VERSION "0.2"

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

void usage(void)
{
	printf("PING for TEEN  v"VERSION"\n");
	printf("書式： PING ホスト名またはIPアドレス\n\n");
	printf("       指定したホストにパケット(ICMPエコー要求)を送信し、\n"
	       "       受信した応答を表示する。指定したホストとの通信が\n"
	       "       可能かどうかを確認できる。\n\n");
}

int main(int argc, char *argv[])
{
//	char *dest=0;
	char dest[128];
	IDWORD destipv4;
	IPADDR destip;

	if (argc<2) {
		usage();
		return -1;
	}

//	dest=argv[1];
	strcpy(dest, argv[1]);

	if (!teen_available()) {
		/* TEEN が常駐していない */
		printf("TEEN が常駐していません。\a\n");
		return -1;
	}

	{
		uint vmi,vma;

		vmi=teen_version()>>8;
		vma=teen_version()&0xff;
		if (vma==0 && vmi<37) {
			/* バージョンが古い */
			printf("TEEN v0.37 以降を使用してください。\a\n");
			return -1;
		}
	}

	destipv4=getipaddress(dest);
	if (destipv4==0) {
		static char *emes[]={"",
			/*1*/	"エラーが発生しました。",
			/*2*/	"TEENがビジーです。",
			/*3*/	"エラーが発生しました。",
			/*4*/	"エラーが発生しました。",
			/*5*/	"エラーが発生しました。",
			/*6*/	"指定されたホストが見つかりません。",
			/*7*/	"TEENの資源不足により実行できません。",
			/*8*/	"指定された名前は不適当です。",
			/*9*/	"ネームサーバから返答が得られませんでした。"
		};

		if (nserrno<10) puts(emes[nserrno]);
		else puts("ホスト名からIPアドレスを取得する際にエラーが発生しました。");
	}
	else {
		inet_ipv4addrtoipaddr(destipv4,&destip);
		ping(&destip);
	}

	return 0;
}

/*
2001/ 4/ 8 
2001/ 4/10 
2001/ 4/14 v0.1
2001/ 4/23 v0.2 icmp_send/recv の仕様変更に伴う修正
*/

/*
バグ：
	応答到着順が送信順と異なる場合や、往復時間が1秒を超える場合、往復時間が正しく測れない
	エコー応答以外のICMPメッセージはタイプ番号とコード番号のみ表示する
	往復時間の統計はとらない
*/

