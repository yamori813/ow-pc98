/*
    sntpget.c 0.01

    Usage: sntpget <host>
*/

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
//#include <farstr.h>
#include <time.h>
#include <stdlib.h>

#include "teen.h"
#include "udp.h"

#define SNTPPORT 123		/* SNTP のポート番号 */
#define RBUFSIZE 256		/* TEEN に与える受信バッファのサイズ */
#define BUF2SIZE 128		/* データを取得するための受信バッファのサイズ */
#define C1970 2208988800	/* RFC868 で定義された 1970/01/01 を示す秒数 */

#define Cf (regs.x.cflag)
//#define teen( a)  regs.h.ah = a; int86y( teen_int, &regs, &regs) ;

static char copyright[]="sntpget version 0.01 Copyright (C) 2002 by Toshiki.N";
static char rbuf[RBUFSIZE];
union  REGS  regs;
unsigned short teen_int;
//short udperrno = 0;

typedef struct {
    unsigned long head;
    unsigned long root_delay;
    unsigned long root_dispersion;
    unsigned long reference_identifier;
    unsigned long ref1;
    unsigned long ref2;
    unsigned long org1;
    unsigned long org2;
    unsigned long rcv1;
    unsigned long rcv2;
    unsigned long xmit1;
    unsigned long xmit2;
} NTPDATA;

union NTP_pkt {
	NTPDATA pkt;
	char s[48];
} ;

union LR {
	unsigned long n;
	char s[4];
};

unsigned long flip(unsigned long x){
	union LR a,b;
	
	a.n=x;
	b.s[3]=a.s[0];
	b.s[2]=a.s[1];
	b.s[1]=a.s[2];
	b.s[0]=a.s[3];
	return b.n;
}

#if 0
char udp_open(const UDPPRM far *udpprm){
	regs.x.es = FP_SEG( udpprm);
	regs.x.di = FP_OFF( udpprm);
	teen( 0x50);
	if (Cf){
		udperrno = regs.h.al;
		return -1;
	}
	return regs.h.al;
}

schar udp_close(schar handle){
	regs.h.al = handle;
	teen( 0x51);
	if (Cf){
		udperrno = regs.h.al;
		return -1;
	}
	return 0;
}

schar udp_state(schar handle){
	regs.h.al = handle;
	teen( 0x52);
	if (Cf){
		udperrno = regs.h.al;
		return -1;
	}
	return regs.h.al;
}

sshort udp_send(schar handle, const char far *buf,short len){
	regs.h.al = handle;
	regs.x.es = FP_SEG( buf);
	regs.x.di = FP_OFF( buf);
	regs.x.cx = len;
	teen( 0x54);
	if (Cf){
		udperrno = regs.h.al;
		return -1;
	}
	return regs.x.ax;
}

sshort udp_recv(schar handle, char far *buf,short len){
	regs.h.al = handle;
	regs.x.es = FP_SEG( buf);
	regs.x.di = FP_OFF( buf);
	regs.x.cx = len;
	teen( 0x55);
	if (Cf){
		udperrno = regs.h.al;
		return -1;
	}
	return regs.x.ax;
}

static ushort teen_search(void){
	int vect;
	char far *addr;

	vect = 0x22;
	while ( vect < 0x100){
		addr = (char far *)_dos_getvect( vect);
		addr -= 9L;
		if ( far_strcmp( addr, "%TEENAPI" ) == 0){
			return vect;
		}
		vect++;
	}
	return 0;
}

short teen_available(void){
	teen_int = teen_search();
	if ( teen_int){
		return 1;
	}
	return 0;
}

ushort teen_version(void){
	teen( 0x01);
	return regs.x.ax;
}
#endif

void set_dostimedate(struct dostime_t *t,struct dosdate_t *d,struct tm tmp){
	t->second=tmp.tm_sec;
	t->minute=tmp.tm_min;
	t->hour=tmp.tm_hour;
	d->day=tmp.tm_mday;
	d->month=tmp.tm_mon+1;
	d->year=tmp.tm_year+1900;
}

double calc_time(NTPDATA pkt,struct dosdate_t d4,struct dostime_t t4){
	struct tm tmp;
	double t; /* 誤差 */

	struct dostime_t t1,t2,t3;
	struct dosdate_t d1,d2,d3;
	unsigned long sec;
	
	sec=flip(pkt.org1)-C1970;
	memcpy(&tmp,localtime(&sec),sizeof(tmp));
	t1.hsecond=flip(pkt.org2)/42949673;
	set_dostimedate(&t1,&d1,tmp);
	
	sec=flip(pkt.rcv1)-C1970;
	memcpy(&tmp,localtime(&sec),sizeof(tmp));
	t2.hsecond=flip(pkt.rcv2)/42949673;
	set_dostimedate(&t2,&d2,tmp);
	
	sec=flip(pkt.xmit1)-C1970;
	memcpy(&tmp,localtime(&sec),sizeof(tmp));
	t3.hsecond=flip(pkt.xmit2)/42949673;
	set_dostimedate(&t3,&d3,tmp);

	printf("\n");

	printf("クライアントのリクエスト送信時刻 = %04d-%02d-%02d %02d:%02d:%02d.%02d\n",d1.year,d1.month,d1.day,t1.hour,t1.minute,t1.second,t1.hsecond);
	printf("サーバのリクエスト受信時刻       = %04d-%02d-%02d %02d:%02d:%02d.%02d\n",d2.year,d2.month,d2.day,t2.hour,t2.minute,t2.second,t2.hsecond);
	printf("サーバの応答送信時刻             = %04d-%02d-%02d %02d:%02d:%02d.%02d\n",d3.year,d3.month,d3.day,t3.hour,t3.minute,t3.second,t3.hsecond);
	printf("クライアントの応答受信時刻       = %04d-%02d-%02d %02d:%02d:%02d.%02d\n",d4.year,d4.month,d4.day,t4.hour,t4.minute,t4.second,t4.hsecond);

/*
	printf("伝送遅延時間（往復）             = %10.6f 秒\n",
	(mktime(&tmp)+C1970-flip(pkt.org1))
		-(flip(pkt.rcv1)-flip(pkt.xmit1))+
	((t4.hsecond*42949673-flip(pkt.org2))
		-(flip(pkt.rcv2)-flip(pkt.xmit2)))/4294967297.0);
*/
	tmp.tm_mday=d4.day;
	tmp.tm_mon=d4.month-1;
	tmp.tm_year=d4.year-1900;
	tmp.tm_sec=t4.second;
	tmp.tm_min=t4.minute;
	tmp.tm_hour=t4.hour;
/* Can not calucrate corectory on open watcom v2
	t=((int)(flip(pkt.rcv1)-flip(pkt.org1)
		+flip(pkt.xmit1)-mktime(&tmp)-C1970)+
	((flip(pkt.rcv2)/4294967297.0-flip(pkt.org2)/4294967297.0
		+flip(pkt.xmit2)/4294967297.0-t4.hsecond*0.01)))/2.0;
		*/
	t=((int)(flip(pkt.rcv1)-flip(pkt.org1)
		+flip(pkt.xmit1)-mktime(&tmp)-C1970)
		) / 2.0;
	
	printf("サーバとローカルマシンとの誤差   = %10.6lf 秒\n",t);
	
	return t;
}

void udptime(const char *host){
	UDPPRM prm;
	char handle;

//	setmem(&prm,sizeof prm,0);
	memset(&prm,0,sizeof prm);
	prm.rbuf=rbuf;
	prm.rbufsize=RBUFSIZE;
	prm.myport  =UDPPORT_ANY;
	prm.peerport=SNTPPORT;
	prm.host    =(void far *)host;

	if ( (handle=udp_open(&prm)) < 0 ) {
		char buf[128];
		fputs("オープン失敗 errno=",stderr);
		fputs(itoa(udperrno,buf,10),stderr);
		exit(EXIT_FAILURE);
	}

	fputs("ホスト検索中・・・\n",stderr);
	while (udp_state(handle)==UDP_SEARCH) {if (kbhit()) break;}

	if (udp_state(handle)!=UDP_OPEN) {
		fputs(host,stderr);
		fputs(" が見つかりません\n",stderr);
	} else {
	    int i;
		union NTP_pkt snd,rcv;
	    
	    snd.pkt.head=flip(0x23000000);
		snd.pkt.root_delay=0;
		snd.pkt.root_dispersion=0;
		snd.pkt.reference_identifier=0;
		snd.pkt.ref1=0;
		snd.pkt.ref2=0;
		snd.pkt.org1=0;
		snd.pkt.org2=0;
		snd.pkt.rcv1=0;
		snd.pkt.rcv2=0;
		
		for(i=0;i<10;i++){
			int len;
			unsigned long sec;
			struct dostime_t t1;
			struct dosdate_t d1;
			struct tm tmp;
			
			_dos_getdate(&d1);
			_dos_gettime(&t1);
			if (t1.hour == 0 && t1.minute == 0 && t1.second == 0){
				_dos_getdate(&d1);
				_dos_gettime(&t1);
			}
			
			tmp.tm_mday=d1.day;
			tmp.tm_mon=d1.month-1;
			tmp.tm_year=d1.year-1900;
			tmp.tm_sec=t1.second;
			tmp.tm_min=t1.minute;
			tmp.tm_hour=t1.hour;
			snd.pkt.xmit1=flip(mktime(&tmp)+C1970);
			snd.pkt.xmit2=flip(t1.hsecond*42949673);
	    
			len=udp_send(handle,snd.s,sizeof(snd));
			if (len>0) printf("時刻問い合わせ中・・・");
			else {
				char buf[128];
				fputs("送信失敗 errno=",stderr);
				fputs(itoa(udperrno,buf,10),stderr);
			}
	
			printf("相手からのデータ待ち・・・\n");
			len=udp_recv(handle,rcv.s,sizeof(rcv));
			_dos_gettime(&t1);
			_dos_getdate(&d1);
			
			if (len<0 && udperrno==EUDP_BUFOVER) {
				/* 受信データは BUF2SIZE より大きかった */
				len=sizeof(rcv);
			}
			if (len>=0) {
				double t; /* 誤差 */
				int s;    /* hsecond から second へのくり上がり */
				
				t=calc_time(rcv.pkt,d1,t1);
				
				_dos_getdate(&d1);
				_dos_gettime(&t1);
				if (t1.hour == 0 && t1.minute == 0 && t1.second == 0){
					_dos_getdate(&d1);
					_dos_gettime(&t1);
				}
				
				if (t1.hsecond+((int)((t-(int)t)*100))<0){
					s=-1;
					t1.hsecond=t1.hsecond+((int)((t-(int)t)*100))+100;
				} else if (t1.hsecond+((int)((t-(int)t)*100))>=100){
					s=1;
					t1.hsecond=t1.hsecond+((int)((t-(int)t)*100))-100;
				} else {
					s=0;
					t1.hsecond=t1.hsecond+((int)((t-(int)t)*100));
				}
				tmp.tm_sec=t1.second;
				tmp.tm_min=t1.minute;
				tmp.tm_hour=t1.hour;
				tmp.tm_mday=d1.day;
				tmp.tm_mon=d1.month-1;
				tmp.tm_year=d1.year-1900;
				
				sec=mktime(&tmp)+(int)t+s; /* 正しい時刻の計算 */
				
				memcpy(&tmp,localtime(&sec),sizeof(tmp));
				t1.second=tmp.tm_sec; /* わずかでもオーバーヘッドによる */
				t1.minute=tmp.tm_min; /* 誤差を減らしたいため、ここでは */
				t1.hour=tmp.tm_hour;  /* set_dostimedate をコールせず、 */
				d1.day=tmp.tm_mday;   /* 直接代入文の羅列とする         */
				d1.month=tmp.tm_mon+1;
				d1.year=tmp.tm_year+1900;
				_dos_settime(&t1);
				_dos_setdate(&d1);
				if (t>0) {
					printf("\nパソコンの時計を %10.2f 秒、進ませました\n",t);
				} else if (t<0) {
					printf("\nパソコンの時計を %10.2f 秒、遅らせました\n",-t);
				}
				
				break;
			}
		}
		if (i==10) fputs("サーバから応答がありませんでした\n",stderr);
	}
	if (udp_close(handle)) {
		char buf[128];
		
		fputs("クローズ失敗 errno=",stderr);
		fputs(itoa(udperrno,buf,10),stderr);
	    exit(EXIT_FAILURE);
	}
}

void usage(void){
    fputs(copyright, stderr);
    fputs("\nUsage: ", stderr);
    fputs("sntpget", stderr);
    fputs(" <host>\n", stderr);
    exit(EXIT_FAILURE);
}

void main(int argc, char *argv[]){
char ntpsvr[64];
int havesvr = 0;
	
	if (argc>1 && (argv[1][0]=='/' || argv[1][0]=='-')) {
		usage();
	    exit(EXIT_FAILURE);
	}
	if (argc == 2 && strlen(argv[1]) < 63) {
		strcpy(ntpsvr, argv[1]);
		printf("NTP Server: %s\n", ntpsvr);
		havesvr = 1;
	}

	if (!teen_available()) {
		/* TEEN が常駐していない */
		fputs("TEEN が常駐していません。\a\n",stderr);
	    exit(EXIT_FAILURE);
	}

	{
		unsigned short vmi,vma;

		vmi=teen_version()>>8;
		vma=teen_version()&0xff;
		if (vma<0 && vmi<32) {
			/* バージョンが古い */
			fputs("TEEN v0.32 以降を使用してください。\a\n",stderr);
			exit(EXIT_SUCCESS);
		}
	}

	if (havesvr) {
		udptime(ntpsvr);
	} else {
		usage();
	}

	exit(EXIT_SUCCESS);
}
