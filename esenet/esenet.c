/*
	ESENET on TEEN  [似非telnet]
	for MS-DOS,Turbo-C

        Written by XMaZ Project.  1997-1999
*/

#include <stdio.h>
#include <stdlib.h>
//#include <alloc.h>	/* farmalloc */
#include <conio.h>	/* kbhit */
#include <string.h>	/* strupr */
#include <jstring.h>	/* strupr */
#include <ctype.h>	/* toupper */
#include <dos.h>	/* intr,setvect,enable */
#include <jctype.h>	/* nthctype */
#include <process.h>	/* system */

#include <teen.h>
#include <tcp.h>


/***********************
　　　マクロ
***********************/

#define VERSION "0.5f"

#define ON  1
#define OFF 0

#define SENDBIN  0
#define SENDTEXT 1

#define RETRYSEND 16		/* データ送信が失敗したときのリトライ回数 */

#define TCPBUFSIZE (1024*8)	/* TCP送信/受信バッファサイズ */
#define RECVBUFLEN 160		/* TCPからデータを得る時に使うバッファサイズ*/

#define TEXTMODE 0
#define COMMANDMODE 1

#define COM_WILL 251
#define COM_WONT 252
#define COM_DO   253
#define COM_DONT 254

#define OPT_ACK 1

#define OPT_ECHO 1
#define OPT_SUPPRESS_GO_AHEAD 3

#define OPTIONS 4

/***********************
　　　変数
***********************/

static char ahandle=-1;
static TCPPRM tcpprm;

static char sw_vb=OFF;		/* 送信・受信バイト数を表示 */
static char sw_vh=OFF;		/* 受信データを HEX で表示 */
static char sw_line=OFF;	/* 行(CRまで)毎に送信 */
static char sw_echo=ON;		/* ローカルエコー */
static char sw_va=OFF;		/* VA TEXT BIOS に出力/ ^C処理をVAに合わせる */
static char sw_j=OFF;		/* 送受信に JIS を使う */

static FILE *recvfile=NULL;	/* 受信ファイル */

static unsigned char opt_do[OPTIONS]={0};
				/* オプションの状態
				　(相手が実行することを要求するもの) */
/*static unsigned char opt_will[OPTIONS]={0};*/


/*
static char n_handle;
static int n_kind=0;
*/

/***********************
　　　プロトタイプ
***********************/

static char *tcpread(void);
static void output(char *buf, uint len);



/***********************
　　　コード
***********************/

#if 0
void huge notice(BYTE handle, int kind, void far *vp)
{
	n_handle=handle;
	n_kind=kind;
}
#endif


/*
1文字コンソール入力(キー入力を待たない)
　出力：int		0..キー入力なし その他..キー入力あり
	char c		入力されたキー
*/
static int getchnw(char *c)
{
	union REGPACK reg;

	reg.w.ax=0x0600;
	reg.w.dx=0x00ff;
	intr(0x21,&reg);
	*c=(char)reg.w.ax;

	return ~reg.w.flags & INTR_ZF;	/* ZF フラグ */
}


/*
ESCキーがおされたか？
*/
static int eschit(void)
{
	char c;

	return getchnw(&c) && c==0x1b ;
}


/*
ログon
*/
static void log_on(void)
{
	union REGPACK reg;

	reg.w.ax=0x0c01;
	intr(0x70,&reg);
}


/*
ログoff
*/
static void log_off(void)
{
	union REGPACK reg;

	reg.w.ax=0x0c00;
	intr(0x70,&reg);
}



/*
接続先指定
*/
static void settarget(char *host, char *port)
{
	tcpprm.host=host;
	tcpprm.peerport=atoi(port);
	printf("[接続先指定] %s:%d\n",host,tcpprm.peerport);
}





/*
送信
  出力：送信バイト数
  注意：obuf は olen+1 バイト以上の長さの領域が確保されている必要がある
*/
static void send(char *obuf, uint olen)
{
	int a,retry;
	char *recvstr;

	if (ahandle<0) {
		printf("[接続していません]\n");
		return;
	}

	retry=RETRYSEND;
	do {
		if ( (a=tcp_send(ahandle,obuf,olen,1))>0 ) {
			if (sw_vb) printf("[送信]%dbytes\n",a);
			olen-=a;
			obuf+=a;
		}
		else {
			puts("[0]");
		}
		retry--;
	} while (olen>0 && retry>0);

	if (olen>0) {
		printf("[送信失敗しました。送信できるまで再試行します。"
		       "ESC で中止]\n");
		do {
			if ( (a=tcp_send(ahandle,obuf,olen,1))>0 ) {
				if (sw_vb) printf("[送信]%dbytes\n",a);
				olen-=a;
				obuf+=a;
			}
			if (eschit()) break;
		} while(olen>0);
	}
}


/*
ファイル送信
  入力：int mode		SENDBIN..バイナリファイル SENDTEXT..テキスト
*/
static void filesend(int mode)
{
#define BUFLEN 4096 /*80*/
	char buf[BUFLEN+1];
	FILE *fp;
	int i;
	long l=0;
	int dc=0;

	printf("送信ファイル名>");
	gets(buf);

	if ( (fp=fopen(buf,(mode==SENDTEXT) ? "rt" : "rb"))==NULL ) {
		printf("[%s がオープンできません。]\n",buf);
		return;
	}

	printf("[ESCで中止]\n");
	for(;;) {
		i=fread(buf,1,BUFLEN,fp);
		if (i==0) break;

		send(buf,i);
		l+=i;
		if (++dc>1) {printf("[%lu bytes]",l); dc=0;}
		if (eschit()) break;
	}
	
	fclose(fp);
	printf("[ファイル送信終了]\n");

#undef BUFLEN
}


/*
ファイル受信開始
*/
static void filerecv_start(void)
{
#define BUFLEN 80
	char buf[BUFLEN];

	if (recvfile!=NULL) {
		printf("[受信中です。]\n");
		return;
	}

	printf("受信ファイル名>");
	gets(buf);

	if ( (recvfile=fopen(buf,"ab"))==NULL ) {
		printf("[%s がオープンできません。]\n",buf);
		return;
	}
	printf("[ファイル受信開始]\n");

#undef BUFLEN
}


/*
ファイル受信終了
*/
static void filerecv_end(void)
{
	if (recvfile!=NULL) {
		fclose(recvfile);
		recvfile=NULL;
		printf("[ファイル受信終了]\n");
	}
}


/*
^@メニュー
*/
int menu(void)
{
#define BUFLEN 25
	char c;
	char buf1[BUFLEN],buf2[BUFLEN];

	printf("\n[-- enter menu --]\n");
	printf(
"1.切断終了 2.バイナリファイル送信 3.テキストファイル送信   4.ファイル受信開始   5.ファイル受信終了\n");
	printf(
"6.16進入力 7.即送信        8.ローカルエコー切替    9.行単位送信切替 A.バイト数表示\n");
	printf(
"B.HEX表示  C.ログ無        D.ログ有         E.子プロセス\n");

	while(!getchnw(&c));

	c=toupper(c);
	if (c>='A') c=c-'A'+10; else c=c-'0';
	switch(c) {
	case 1: return -1;
	case 6: {
			int i;
			uint a=0;
			char c;

			/*buf1[0]=BUFLEN;*/
			printf("Hex:");
			gets(buf1);
			strupr(buf1);
			for (i=0; i<2; i++) {
				a<<=4;
				c=buf1[i];
				if (c>='0' && c<='9') c-='0';
				else if (c>='A' && c<='F') c=c-'A'+10;
				else c=0;
				a+=c;
			}
			printf("\nHex:0x%x\n",a);

			return 0x100+a;
		}
	case 7:	return 0x200;
	case 2: filesend(SENDBIN); break;
	case 3: filesend(SENDTEXT); break;
	case 4: filerecv_start(); break;
	case 5: filerecv_end(); break;
	case 10:sw_vb=(sw_vb)?OFF:ON; break;
	case 11:sw_vh=(sw_vh)?OFF:ON; break;
	case 8: sw_echo=(sw_echo)?OFF:ON; break;
	case 9: sw_line=(sw_line)?OFF:ON; break;
	case 12:
		log_off();
		break;
	case 13:
		log_on();
		break;
	case 14:
#if 0
		system("");
		enable();	/* 88VAで子プロセス呼び出し後に割込みが禁止
				   されるのを防ぐ */
#endif
		break;
	}

	printf("[-- leave menu --]\n");
	return 0;
#undef BUFLEN
}


/*
文字入力、送信
　出力：int 			=0 そのまま続行
				=-1 切断して終了せよ
*/
static int input(void)
{
	static char obuf[160];
	static uint olen=0;
	int a;
	char c,enter;

	enter=ON;
	if (getchnw(&c)) {
		if (c==0) {
			a=menu();
			if (a<0) return -1;
			
			switch(a&0x0f00) {
			case 0x0100:
				c=a&0xff;
				break;
			case 0x0200:
				if (sw_line) {send(obuf,olen); olen=0;}
				enter=OFF;
				break;
			default:
				enter=OFF;
				break;
			}
		}
		else if (c==0x0d) {
			if (sw_echo) fputs("\n",stdout);
			obuf[olen++]=0x0d;
			obuf[olen++]=0x0a;
			send(obuf,olen);
			olen=0;
			enter=OFF;
		}

		if (enter) {
			if (sw_echo) fputc(c,stdout);
			obuf[olen++]=c;
			if (!sw_line) {send(obuf,olen); olen=0;}
		}
	}
	return 0;
}



/*
受信文字列(SJIS)を画面・ファイルに出力
　注意：buf は len+1バイトの領域が確保してあると仮定
*/
static void loutput(char *buf, uint len)
{
	if (sw_va) {
#if 0
		struct REGPACK reg;

		buf[len]='\0';
		reg.r_si=(unsigned int)buf;
		reg.r_ds=FP_SEG((char far *)buf);
		reg.r_dx=0x8000;
		reg.r_ax=0x0200;
		intr(0x83,&reg);	/* VA TEXT BIOS */
#endif
	}
	else fwrite(buf,len,1,stdout);
	if (recvfile!=NULL) fwrite(buf,len,1,recvfile);
}


/*
受信文字列(SJIS)を画面・ファイルに出力
　補足：最後の１バイトが漢字１バイト目なら適切な処理をする
　注意：buf は len+1バイトの領域が確保してあると仮定
*/
static void output_sjis(char *buf, uint len)
{
	static char back[3];
	static int backuse=0;

	if (len==0) return;

	if (backuse) {
		back[1]=*buf;
		buf++;
		len--;
		loutput(back,2);
		backuse=0;
	}

	if (len==0) return;
/*
	if (nthctype(buf,len-1)==CT_KJ1) {
		back[0]=buf[len-1];
		len--;
		backuse=1;
	}
	*/

	loutput(buf,len);
}


/*
受信文字列(JIS)を画面・ファイルに出力
　補足：bufがJISの途中で終わっていても適切な処理をする
*/
static void output_jis(char *buf,uint len)
{
	static char sjisbuf[RECVBUFLEN+2+1];	/* 2= ESC+? の分 */
	static enum {jis1,jis2,ascii,esc,esc_kakko,esc_dollar} context=ascii;
	static unsigned char firstbyte=0;
	unsigned char c;
	char *sjis=sjisbuf;
	char *jis=buf;
	unsigned short a;

	for (; len>0; len--) {
		c=*jis++;
		switch(context) {
		case ascii:
			if (c==0x1b) {
				context=esc;
			}
			else {
				*sjis++=c;
			}
			break;
		case jis1:
			if (c==0x1b) {
				context=esc;
			}
			else if (c<0x21) {
				*sjis++=c;
				context=ascii;
			}
			else {
				firstbyte=c;
				context=jis2;
			}
			break;
		case jis2:
			if (c==0x1b) {
				*sjis++=firstbyte;
				context=esc;
			}
			if (c<0x21) {
				*sjis++=firstbyte;
				*sjis++=c;
				context=ascii;
			}
			else {
				a=jistojms((((unsigned short)firstbyte)<<8)|c);
				if (a!=0) {
					*sjis++=a>>8;
					*sjis++=a&0xff;
				}
				context=jis1;
			}
			break;
		case esc:
			switch(c) {
			case '(':
				context=esc_kakko;
				break;
			case '$':
				context=esc_dollar;
				break;
			default:
				*sjis++=0x1b;
				*sjis++=c;
				context=ascii;
				break;
			}
			break;
		case esc_kakko:
			switch(c) {
			case 'B':
			case 'J':
				context=ascii;
				break;
			default:
				*sjis++=0x1b;
				*sjis++='(';
				*sjis++=c;
				context=ascii;
				break;
			}
			break;
		case esc_dollar:
			switch(c) {
			case 'B':
				context=jis1;
				break;
			default:
				*sjis++=0x1b;
				*sjis++='$';
				*sjis++=c;
				context=ascii;
				break;
			}
			break;
		}
	}
	loutput(sjisbuf,sjis-sjisbuf);
}


/*
受信文字列を画面・ファイルに出力
　補足：最後の１バイトが漢字１バイト目なら適切な処理をする
　注意：buf は len+1バイトの領域が確保してあると仮定
*/
static void output(char *buf, uint len)
{
	if (sw_j) output_jis(buf,len); else output_sjis(buf,len);
}


/*
バイト列を16進で画面に表示
*/
static void putbinary(BYTE *buf, int len)
{
	for (; len>0; len--, buf++) {
		printf("%02X ",*buf);
	}
	printf("\n");
}


/*
telnet コマンド処理
　出力：uint			受信データ消費量
        int mode		１コマンド全体を受信し処理した時TEXTMODE
        			まだ処理できない時 COMMANDMODE
*/
uint recv_command(char *buf, uint len, int *mode)
{
	static char obuf[3+1];
	static uint olen=0;
	unsigned char code;
	uint used=0;


	if (olen<2) {
		obuf[olen]=*buf;
		olen++;
		buf++;
		len--;
		used++;
	}
	if (olen>=2) {
		if ((unsigned char)obuf[1]<240) {
			/* 異常なコマンドコード */
			output(obuf,2);
			olen=0;
			*mode=TEXTMODE;
		}
		else if ((unsigned char)obuf[1]==255) {
			output(obuf,1);
			olen=0;
			*mode=TEXTMODE;
		}
		else if ((unsigned char)obuf[1]<251) {
			/* 無視 */
			printf("[コマンド%d 無視]\n",(unsigned char)obuf[1]);
			olen=0;
			*mode=TEXTMODE;
		}
		else {
			if (olen<3 && len>0) {
				obuf[olen]=*buf;
				olen++;
				buf++;
				len--;
				used++;
			}
			if (olen>=3) {
			    *mode=TEXTMODE;
			    olen=0;
			    code=obuf[2];
			    switch((unsigned char)obuf[1]) {
			    case COM_WILL:
				switch (code) {
				case OPT_ECHO:
					sw_echo=OFF;
				case OPT_SUPPRESS_GO_AHEAD:
					if (!(opt_do[code]&OPT_ACK)) {
						opt_do[code]|=OPT_ACK;
						obuf[1]=COM_DO;	/* 許可 */
						obuf[2]=code;
						send(obuf,3);
						printf("[WILL %d 許可]\n",code);
					}
					else {
						printf("[WILL %d 無視]\n",code);
					}
					break;
				default:
					obuf[1]=COM_DONT;	/* 拒否 */
					obuf[2]=code;
					send(obuf,3);
					printf("[WILL %d 拒否]\n",code);
					break;
				}
				break;

			    case COM_WONT:
				printf("[WON'T %d 無視]\n",code);
				break;

			    case COM_DONT:
				printf("[DON'T %d 無視]\n",code);
				break;

			    case COM_DO:
				obuf[1]=COM_WONT;	/* 拒否 */
				obuf[2]=code;
				send(obuf,3);
				printf("[DO %d 拒否]\n",code);
				break;
			    }
			}
		}

	}

	
	return used;
}




/*
bufからlenバイトのデータの中から 0x00または0xffを探し、そのポインタを返す。
　補足：みつからない時は buf+len を返す。
*/
static char *terminateFFor00(char *buf, uint len)
{
	for (; *buf!='\xff' && *buf!=0 && len>0 ; buf++,len--);
	return buf;
}



/*
TCPから文字列を受信し、受信データを適切なルーチンに渡す
*/
static void mainloop(void)
{
	static char buf[RECVBUFLEN+1];
	static char *top=buf;			/* buf 内未消費データ */
	static uint len=0;			/* buf内未消費データバイト数*/
	static int mode=TEXTMODE;		/* 受信データの解釈 */
	char state;
	char closing=OFF;			/*相手からTCP切断要求があった*/

    while(input()==0) {
	if (len==0) {
		len=tcp_recv(ahandle,buf,RECVBUFLEN);
		top=buf;
		if (len>0) {
			if (sw_vb) printf("[受信]%dbytes\n",len);
			if (sw_vh) putbinary(buf,len);
		}
	}

	if (len>0) {
		switch(mode) {
		case COMMANDMODE: {
			uint a;

			a=recv_command(top,len,&mode);
			/*if (a>ilen) a=ilen;*/
			top+=a;
			len-=a;
			}
			break;

		case TEXTMODE: {
			char *tail;

			if (*top=='\0') {top++; len--; break;}
			if (*top=='\xff') {mode=COMMANDMODE; break;}

			tail=terminateFFor00(top,len);
			output(top,tail-top);
			len-=tail-top;
			top=tail;
			}
			break;
		}
	}
	else {
		if (closing) break;
			/* TCP から読みだせるデータがなく、かつ、
			   切断要求がすでにあった場合、終了する */
	}

	if ( !closing && (state=tcp_state(ahandle))!=TCP_OPEN ) {
		switch(state) {
		case TCP_HISCLOSING:
			printf("[相手からの切断要求]\n");
			break;
		default:
			printf("[原因不明の切断]\n");
			break;
		}
		closing=ON;
	}

    }
}




/*
telnet本体
*/
void telnet(void)
{
	char state;

	tcpprm.thisport=0;
	tcpprm.notice=NULL; /*notice;*/
//	tcpprm.rbuf=farmalloc(TCPBUFSIZE);
	tcpprm.rbuf=malloc(TCPBUFSIZE);
	tcpprm.rbank=0;
	tcpprm.rbufsize=TCPBUFSIZE;
//	tcpprm.sbuf=farmalloc(TCPBUFSIZE);
	tcpprm.sbuf=malloc(TCPBUFSIZE);
	tcpprm.sbank=0;
	tcpprm.sbufsize=TCPBUFSIZE;

	if (tcpprm.rbuf==NULL || tcpprm.sbuf==NULL) {
		printf("[メモリ確保失敗]\n");
		return;
	}

	printf("[接続中・・・]\n");
	if ( (ahandle=tcp_activeopen(&tcpprm)) < 0 ) {
		printf("[ハンドル確保失敗]\n");
		return;
	}
	/*printf("[ハンドル %d]\n",ahandle);*/
	while( state=tcp_state(ahandle),
	       (state!=TCP_OPEN && state!=TCP_CLOSED) ) {
		if (eschit()) {
			printf("[キャンセル]\n");
			tcp_close(ahandle);
			return;
		}
	}
	if (state!=TCP_OPEN) {
		static char *downmess[]={"",
			/*1*/	"相手が切断を要求",
			/*2*/	"ユーザが切断を要求",
			/*3*/	"相手が接続を拒否",
			/*4*/	"タイムアウト",
			/*5*/	"ホストが見つからない"
			};

		printf("[接続失敗：%s]\n",downmess[tcp_downstate(ahandle)]);
		tcp_close(ahandle);
		return;
	}
#if 0
	{
		TCPADDR addr[2];
		char ipstr[16];

		tcp_getaddr(ahandle,addr);
		strcpy(ipstr,inet_ntoa(addr[0].ip));
		printf("[%s:%u-%s:%u]\n",ipstr,addr[0].port,inet_ntoa(addr[1].ip),addr[1].port);
	}
#endif

	printf("[接続完了]\n");
	printf("[^@でメニュー]\n");

	mainloop();

	/* 切断 */
	printf("[切断中・・・]\n");
	tcp_shutdown(ahandle);
	while(tcp_state(ahandle)!=TCP_CLOSED) {
		if (kbhit()) break;
	}
	printf("[切断]\n");
	tcp_close(ahandle);
#undef BUFLEN
}


/*
^Cハンドラ
*/
void interrupt ctrlc_handler(uint bp, uint di, uint si, uint ds, uint es,
	 uint dx, uint cx, uint bx, uint ax)
{
	if (sw_va && ((ax & 0xff00) == 0x3f00) && bx==0) {
				/* VA+MSE でコンソールからのファイル読みだし
				   (AH=3Fh)が、^Cにより 
				   	CY=0,AX=呼び出し時のまま
				   で帰るのを防ぐ。
				*/
		ax=0;
	}
	return;			/* 継続実行 */
}



/*
使い方
*/
void usage(void)
{
	printf("ESENET on TEEN v"VERSION"  for MS-DOS/PC-88VA\n"
	       "Written by XMaZ Project. 1997-1999\n\n");
	printf("書式： ESENET [option] hostname [port]\n"
	       "         /VB 送受信バイト数を表示\n"
	       "         /VH 受信データを16進数でも表示\n");
/*	printf("         /VA PC-88VAのTEXT BIOSを使用\n");*/
	printf("         /J  受信時JIS->ShiftJIS変換をする\n");
}


/*
MAIN
*/
int main(int argc, char *argv[])
{
	char far *host=NULL,*port=NULL;
	int i;

	for (i=1; i<argc; i++) {
		if (argv[i][0]=='-' || argv[i][0]=='/') {
			switch(toupper(argv[i][1])) {
			case 'V':
				switch (toupper(argv[i][2])) {
				case 'A': sw_va=ON; break;
				case 'H': sw_vh=ON; break;
				case 'B': sw_vb=ON; break;
				default: usage(); return -1;
				}
				break;
			case 'J':
				sw_j=ON;
				break;
			default:
				usage();
				return -1;
			}
		}
		else if (host==NULL) host=argv[i];
		else if (port==NULL) port=argv[i];
		else {usage(); return -1;}
	}
	if (host==NULL && port==NULL) {usage(); return -1;}
	if (port==NULL) port="23";

	if (!teen_available()) {
		/* TEEN が常駐していない */
		printf("TEEN が常駐していません。\a\n");
		return -1;
	}

	if ((teen_machine() & 0x000f) == 0) sw_va=ON;
	if (sw_va) {printf("[PC-88VA mode]\n");}

	settarget(host,port);
#if 0
	setvect(0x23,ctrlc_handler);
#endif
	telnet();

	return 0;
}

/*
1997/12/ 3 v0.0
1997/12/ 4 v0.1  echoのon/off,行入力のon/off,メニューの整理,teletコマンド
1997/12/ 5 v0.2  /VA,tcpからの読みだし部分の整理,
		 2バイト文字が分割されて表示されるのを防止
1997/12/22 v0.3  TEENv0.10対応。getipaddress,tcp_state使用
1997/12/23 v0.3a TEEN常駐検査
1998/ 1/ 3 v0.3b tcpread:出力文字列中の\0 を \x01 に変換
1998/ 1/15 v0.3c TCPバッファサイズを拡大
1998/ 1/25 v0.3d ファイル受信を行った後、再び行おうとすると失敗するのを修正
		 既存ファイルにファイル受信するときはファイル末尾に追加する
		 データ送信中に送信バッファが一杯になっても再試行する
		 ファイル送信中でも受信データを画面に表示する
		 DOSの^C監視をOFFにする
1998/ 3/ 7 v0.4  TEEN v0.20対応。/I廃止(ホスト名にIPアドレスを指定できるため)
		 DOSの^C監視OFFを取り止め。^Cルーチンを作成。
		 98+MS-DOSで ^C 送信が可能。
1998/ 3/ 8 v0.4a VAで /VA 指定時の ^C 処理を修正
1998/ 3/10 v0.4b 接続失敗時にその原因を表示
1998/ 7/ 2 v0.4c TELNETオプションの扱いの変更：相手からの DOはすべて拒否する。
		 WILLは ECHO/SUPPRESS-GO-AHEAD を除いて拒否する。
1998/ 7/18 v0.5  受信処理があまりに汚かったので書き換え。
		 98で sx キャラクタシンボルが表示されていたのを修正
		 (\0 を \x01 に置き換えていたのが原因、v0.3bからのバグ)
1998/ 7/19 v0.5a 2バイト文字が分割されて表示されるのを防止(v0.5からのバグ)
1998/ 7/20 v0.5b ローカルエコーはデフォルトでオンとする。
		 nthctype を自作のものに置き換え(一部非互換)速くなった？
		 TELNETオプション：相手からのWILL ECHO/SUPPRESS-GO-AHEADを許可
		 する(DOを返す)。WILL ECHOを受信するとローカルエコーをオフにす
		 る。
1998/ 8/18 v0.5c キー入力を AH=06h,INT 21h で取得。(EPSON DOS 5.0 で ^P,^N な
		 どが入力できなかったため変更した。これに伴い、VA で PC キーに
		 よるメニュー起動ができなくなった(;_;) )
		 相手から切断要求があったとき、受信済みの文字列を表示せずに
		 終了していたのを修正。
		 キャンセルに使っていた ^@ を ESC に変更。
		 メニューに子プロセスの起動を追加。
1998/ 9/16 v0.5d コネクションの IPアドレスとポート番号を表示(TEEN v0.23)
1999/ 5/12 v0.5e IPアドレス・ポート番号表示を削除
		 ファイル送信時、送信済みバイト数を表示
		 ソースの整理
1999/ 6/27 v0.5f 受信時JIS->ShiftJIS変換機能(/J オプション)
                 VAで子プロセス起動後TEENのRS受信とりこぼしが頻発するのを修正
                 (子プロセス実行後割込み許可命令を実行するようにした
                  Thanks団栗さん)
*/

