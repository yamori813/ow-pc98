/*
 * tftp.c - Trivial FTP
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#include "net_ci.h"
#include "netteen.h"

#define	VERSION	"0.01"

char *arg_remote = NULL;
char *arg_local = NULL;
char opt_opcode = 'r';		/* 's'=送信  'r'=受信 */
char opt_mode = 'b';		/* 'a'=netascii  'b'=octet */

char *remote_host = NULL;
char *remote_file = NULL;
char *local_file = NULL;
char *trans_mode = NULL;

int tftp_analyze_oack(void);

#define	UDP_RBUFSIZE	5000
unsigned char far udp_rbuf[UDP_RBUFSIZE];

#define	LEN_PKTMAX	1500
unsigned char	spkt[LEN_PKTMAX];
int				spkt_len;
unsigned char	rpkt[LEN_PKTMAX];
int				rpkt_len;

unsigned port_tftp = 69;
unsigned peer_port = 0;			/* ブロック送受信時相手のポート番号 */

unsigned opt_blocksize = 512;	/* ブロックサイズ */
unsigned opt_timeout = 0;		/* タイムアウトオプション */
long	 opt_tsize = -1;		/* 転送サイズオプション */

unsigned blocksize = 512;		/* 実際のブロックサイズ */
unsigned retry_max = 30;		/* 再試行最大回数 */

nethandle_t udphandle;

enum tftp_opcode_t {
	TFTP_RRQ = 1,		/* 1 読み出し要求 */
	TFTP_WRQ,			/* 2 書き込み要求 */
	TFTP_DATA,			/* 3 データブロック */
	TFTP_ACK,			/* 4 アクノウリッジ */
	TFTP_ERROR,			/* 5 エラー報告 */
	TFTP_OACK,			/* 6 オプションACK */
};

n_udp_t udp;

void usage(int detail);


char *strpcpy(char *dest, char *src, unsigned *pos)
{
	while(*src)
	{
		*(dest + *pos) = *src++;
		(*pos)++;
	}
	*(dest + *pos) = '\0';
	(*pos)++;
	return dest-1;
}

int udp_connect(unsigned myport, unsigned peerport)
{
	udphandle =
		net_udp_open(remote_host, myport, peerport, udp_rbuf, UDP_RBUFSIZE);
	
	if (udphandle == 0xff)
	{
		printf("Not enough network resource.\n");
		return 0;
	}
	return 1;
}

/* tftp_wait_ack() - ACK を待つ
 * blknum: ブロック番号
 * 戻り値: 成功=1, 失敗=0
 *
 */
int tftp_wait_ack(unsigned blknum)
{
	unsigned ackblknum, pkttyp;
	
	udp.buf = rpkt;
	udp.len = LEN_PKTMAX;
	
	do {
		while ((rpkt_len = net_udp_recvfrom(&udp)) == -1)
		{
			if (kbhit() && getch() == 0x1b) return 0;
		}
		if (rpkt_len < 4)
		{
			printf("tftp: ACK error: too small packet length (%u)", rpkt_len);
			return 0;
		}
		pkttyp = ((unsigned)rpkt[0] << 8) | rpkt[1];
		ackblknum = ((unsigned)rpkt[2] << 8) | rpkt[3];
		switch(pkttyp)
		{
			case TFTP_ERROR:
				printf("tftp: ERROR received (%u: %s)\n",
					((unsigned)rpkt[2] << 8) | rpkt[3], &rpkt[4]);
				return 0;
			case TFTP_OACK:
				if (ackblknum == blknum)
				{
					tftp_analyze_oack();
					return 1;
				}
				break;
			case TFTP_ACK:
				if (ackblknum == blknum) return 1;
				break;
			default:
				printf("tftp: ACK error: unknown opcode (%u)\n", pkttyp);
				return 0;
		}
	} while (ackblknum < blknum);
	
	return 0;
}

/* tftp_send_ack() - ACK を送信する
 *
 *
 */
int tftp_send_ack(unsigned blknum)
{
	spkt[0] = 0;
	spkt[1] = TFTP_ACK;
	spkt[2] = blknum >> 8;
	spkt[3] = blknum & 0xff;
	udp.buf = spkt;
	udp.len = 4;
	net_udp_sendto(&udp);
	return 1;
}

/* tftp_analyze_oack() - OACK を解析する
 *
 *
 */
int tftp_analyze_oack(void)
{
	unsigned i;
	uchar *name;
	uchar *val;
	
	printf("\n");
	i = 2;
	while (i < rpkt_len)
	{
		val = name = rpkt + i;
		while (*val) val++, i++;
		val++;
		i++;
		while (*(rpkt + i)) i++;
		i++;
		printf("OACK: %s=%s\n", name, val);
		if (stricmp(name, "blksize") == 0)	/* ブロックサイズオプション */
		{
			blocksize = atoi(val);
		}
		else
		if (stricmp(name, "timeout") == 0)	/* タイムアウトオプション */
		{
			opt_timeout = atoi(val);
		}
		else
		if (stricmp(name, "tsize") == 0)	/* 転送サイズオプション */
		{
			switch(opt_opcode)
			{
				case 'r':		/* 受信時 */
					opt_tsize = atol(val);
					break;
				case 's':		/* 送信時 */
					break;
			}
		}
		else
		{
			printf("WARNING: Unknown tftp option (%s=%s)\n", name, val);
		}
	}
	return 1;
}

unsigned make_req_packet(unsigned opcode)
{
	unsigned pos = 0;
	char tmp[10];
	
	spkt[pos++] = 0;
	spkt[pos++] = opcode;
	strpcpy(spkt, remote_file, &pos);	/* ファイル名 */
	strpcpy(spkt, trans_mode, &pos);	/* モード (netsascii, octet) */
	
	/* ブロックサイズオプション */
	if (opt_blocksize != 512)
	{
		sprintf(tmp, "%u", opt_blocksize);
		strpcpy(spkt, "blksize", &pos);
		strpcpy(spkt, tmp, &pos);
	}
	/* タイムアウトオプション */
	if (opt_timeout)
	{
		sprintf(tmp, "%u", opt_timeout);
		strpcpy(spkt, "timeout", &pos);
		strpcpy(spkt, tmp, &pos);
	}
	/* 転送サイズオプション */
	if (opt_tsize >= 0)
	{
		sprintf(tmp, "%u", opt_tsize);
		strpcpy(spkt, "tsize", &pos);
		strpcpy(spkt, tmp, &pos);
	}
	
	spkt_len = pos;
	return 0;
}

/* 送信
 *
 *
 */
int tftp_send(void)
{
	unsigned blknum;	/* ブロック番号 */
	unsigned pkttyp;	/* パケットタイプ */
//	unsigned myport;	/* こちらのポート番号 */
	WORD myport;	/* こちらのポート番号 */
	unsigned retry;		/* 再試行回数 */
	size_t readbytes;	/* ファイルから読み込んだサイズ */
	FILE *fpi;			/* 入力元ファイル */
	int result = 0;
	long total = 0;		/* 送信したトータルファイルサイズ */
	long fsiz = 0;		/* ファイルサイズ */
	char ipstr[16];		/* IP アドレス */
	time_t timer, diff;	/* タイムアウト測定用 */
	time_t start, now, prev;	/* 経過時間測定用 */
	
	if (!remote_host || !remote_file || !local_file) return -1;
	fpi = fopen(local_file, "rb");
	if (fpi == NULL)
	{
		printf("tftp: %s - Can\'t open a file.\n", local_file);
		return -2;
	}
	fseek(fpi, 0, SEEK_END);
	fsiz = ftell(fpi);
	rewind(fpi);
	
	if (!udp_connect(0, port_tftp)) return -1;
	
	make_req_packet(TFTP_WRQ);
	net_udp_send(udphandle, spkt, spkt_len);
	net_udp_get_thisaddr(udphandle, ipstr, &myport);
	net_udp_close(udphandle);
	if (opt_timeout == 0) opt_timeout = 3;
	
	if (!udp_connect(myport, 0)) return -1;
	
	udp.handle = udphandle;
	udp.r_addr = ipstr;
	udp.r_port = 0;
	
	printf("begin data transfer... (ESC to abort)\n");
	
	spkt[0] = 0;
	spkt[1] = TFTP_DATA;
	
	blknum = 0;
	retry = 0;
	time(&start);
	prev = 0;
	do {
		if (!tftp_wait_ack(blknum)) break;
		blknum++;
		spkt[2] = blknum >> 8;
		spkt[3] = blknum & 0xff;
		readbytes = fread(&spkt[4], 1, blocksize, fpi);
		spkt_len = 4 + readbytes;
		udp.buf = spkt;
		udp.len = spkt_len;
		net_udp_sendto(&udp);
		total += readbytes;
		now = time(NULL) - start;
		if (prev != now)
		{
			printf(
			"[%2lu:%02lu][%u blocks][%lu bytes][%lu bytes/sec][retry %u]\r",
			now / 60, now % 60, blknum, total, total / max(now,1), retry);
			prev = now;
		}
	} while(readbytes == blocksize);
	
	fclose(fpi);
	
	if (readbytes == 0)
	{
		if (!tftp_wait_ack(blknum)) result = -1;
		printf(
		"[%2lu:%02lu][%u blocks][%lu bytes][%lu bytes/sec][retry %u]\r",
		now / 60, now % 60, blknum, total, total / max(now,1), retry);
	}
	else
	{
		result = -1;
	}
	
	net_udp_close(udphandle);
	printf("\nend data transfer.\n");
	
	return result;
}

/* tftp_recv() - ファイル受信
 *
 */
int tftp_recv(void)
{
	unsigned blknum;	/* ブロック番号 */
	unsigned pkttyp;	/* パケットタイプ */
	unsigned datlen;	/* データ長 */
//	unsigned myport;	/* こちらのポート番号 */
	WORD myport;	/* こちらのポート番号 */
	unsigned retry;		/* 再試行回数 */
	FILE *fpo;			/* 出力先ファイル名 */
	int result = 0;
	unsigned long total = 0;	/* 受信したトータルファイルサイズ */
	char *msg;			/* ステータスメッセージ */
	char ipstr[16];		/* IP アドレス */
	time_t timer, diff;	/* タイムアウト測定用 */
	time_t start, now, prev;	/* 経過時間測定用 */
	
	
	if (!remote_host || !remote_file || !local_file) return -1;
	fpo = fopen(local_file, "wb");
	if (fpo == NULL)
	{
		printf("tftp: %s - Can\'t open a file.\n", local_file);
		return -2;
	}
	
	/*printf("connect..\n");*/
	if (!udp_connect(0, port_tftp)) return -1;
	
	/*printf("send req..\n");*/
	make_req_packet(TFTP_RRQ);
	net_udp_send(udphandle, spkt, spkt_len);
	net_udp_get_thisaddr(udphandle, ipstr, &myport);
	net_udp_close(udphandle);
	/*fwrite(spkt, spkt_len, 1, stdout);*/
	
	if (!udp_connect(myport, 0)) return -1;
	
	udp.handle = udphandle;
	udp.r_addr = ipstr;
	
	printf("begin data transfer... (ESC to abort)\n");
	/*printf("recv blocks...\n");*/
	blknum = 0;
	retry = 0;
	time(&start);
	prev = 0;
	do {
		udp.buf = rpkt;
		udp.len = LEN_PKTMAX;
		time(&timer);
		while ((rpkt_len = net_udp_recvfrom(&udp)) == -1)
		{
			/* キーボードキャンセルチェック */
			if (kbhit()) if (getch() == 0x1b) break;
			
			/* 再試行多すぎ? */
			if (retry > retry_max)
			{
				printf("\ntftp: ERROR: too many retris.\n");
				break;
			}
			
			/* タイムアウトチェック */
			diff = time(NULL) - timer;
			if (diff >= 2 && blknum)
			{
				tftp_send_ack(blknum);
				retry++;
				diff = 0;
			}
		}
		if (rpkt_len == -1) {result = -1; break;}
		if (rpkt_len < 4)
		{
			printf("tftp: recieved packet size too small (%u)\n", rpkt_len);
			break;
		}
	/*	printf("rpkt %u: %x %x %x %x %x %x :", 
		rpkt_len, rpkt[0], rpkt[1], rpkt[2], rpkt[3], rpkt[4], rpkt[5]);*/
		
		pkttyp = ((unsigned)rpkt[0]) << 8 | rpkt[1];
		blknum = ((unsigned)rpkt[2]) << 8 | rpkt[3];
		datlen = rpkt_len - 4;
		switch(pkttyp)
		{
			case TFTP_DATA:
				tftp_send_ack(blknum);
				total += datlen;
				msg = "ACK ";
				break;
			case TFTP_ERROR:
				printf("\ntftp: ERROR received (%u: %s)\n", datlen, &rpkt[4]);
				result = -1;
				break;
			case TFTP_OACK:
				msg = "OACK";
				tftp_analyze_oack();
				tftp_send_ack(0);
				break;
			default:
				printf("\ntftp: ERRROR: unknown opcode (%u)\n", pkttyp);
				pkttyp = TFTP_ERROR;
				result = -1;
				break;
		}
		if (pkttyp == TFTP_ERROR) break;
		fwrite(&rpkt[4], datlen, 1, fpo);
		now = time(NULL) - start;
		if (prev != now)
		{
			printf(
			"[%2lu:%02lu][%u blocks][%lu bytes][%lu bytes/sec][retry %u] %s\r",
			now / 60, now % 60, blknum, total, total / max(now,1), retry, msg);
			prev = now;
		}
	} while(datlen == blocksize || pkttyp == TFTP_OACK);
	
	if (pkttyp == TFTP_DATA)
	{
		printf(
		"[%2lu:%02lu][%u blocks][%lu bytes][%lu bytes/sec][retry %u] %s\r",
		now / 60, now % 60, blknum, total, total / max(now,1), retry, msg);
	}
	
	fclose(fpo);
	
	printf("\nend data transfer.\n");
	net_udp_close(udphandle);
	
	return result;
}


int commandmode()
{
	usage(0);
	return 0;
}

int automode()
{
	char *ptr;
	
	if (arg_remote) remote_host = arg_remote;
	if (arg_local)  local_file = arg_local;
	
	
	/* リモートの情報設定  host[:file] */
	
	ptr = strchr(arg_remote, ':');
	if (!ptr)
	{
		commandmode();
		return 0;
	}
	
	*ptr++ = '\0';
	
	remote_host = arg_remote;
	remote_file = ptr;
	
	if (*remote_host == '\0') {printf("invalid hostname\n"); return -1;}
	if (*remote_file == '\0') {printf("invalid remote filename\n"); return -1;}
	
	/* ローカルの情報設定 */
	
	if (!local_file)
	{
		local_file = strrchr(remote_file, '/');
		if (local_file == NULL) local_file = remote_file;
		else					local_file++;
	}
	
	if (*local_file == '\0') {printf("invalid local filename\n"); return -1;}
	
	/* その他の設定 */
	switch(opt_mode)
	{
		case 'a':	trans_mode = "netascii";	break;
		case 'b':	trans_mode = "octet";		break;
		case 'm':	trans_mode = "mail";		break;
	}
	
	/* 転送指示 */
	
	switch(opt_opcode)
	{
		case 's':
		case 'w':
			tftp_send();
			break;
		case 'r':
		case '\0':
			tftp_recv();
			break;
	}
	return 0;
}



/* 使用方法表示＆終了 */
void usage(int detail)
{
	if (detail)
	printf("tftp v"VERSION" for TEEN  Copyright (C) 2000 by Kattyo@ABK.\n");
	printf("usage: tftp [-?absrBTS] [host[:path] [localfile]]\n");
	if (detail)
	printf("options:\n"
		   "-a    set mode to ascii      -B(blksize) set transmit blocksize\n"
		   "-b    set mode to binary     -T(seconds) set timeout to seconds\n"
		   "-s    send a file            -S          use tftp tsize option\n"
		   "-r    retrieve a file\n"
		   "-?    show this help\n"
	);
	exit(250);
}

/* パラメータ解析 */
int para(int argc, char *argv[])
{
	int i, j;
	char ch, *ptr;
	
	i = 1;
	while(i < argc)
	{
		j = 0;
		ch = *(argv[i] + j);
		if (ch == '-' || ch == '/')
		{
			ch = *(argv[i] + ++j);
			switch(ch)
			{
				case '8':	opt_mode = 'b';		break;
				case 'a':	opt_mode = 'a';		break;
				case 'b':	opt_mode = 'b';		break;
				case 'm':	opt_mode = 'm';		break;
				case 's':	opt_opcode = 's';	break;
				case 'r':	opt_opcode = 'r';	break;
				case 'B':	opt_blocksize = atoi(argv[i] + ++j);	break;
				case 'T':	opt_timeout = atoi(argv[i] + ++j);		break;
				case 'S':	opt_tsize = 0;		break;
				case '?':
					usage(1);
					break;
				default:
					printf("%c - unknown option\n", ch);
					usage(0);
					break;
			}
		}
		else
		{
			if (arg_remote == NULL) arg_remote = argv[i];
			else
			if (arg_local == NULL)  arg_local = argv[i];
			else
			{
				printf("%s - extra patameter\n", argv[i]);
			}
		}
		
		if (opt_blocksize < 8 && opt_blocksize > 4500)
		{
			printf(	"WARNING: -B option ignored - Illegal Blocksize "
					"(min.8 - max.4500)\n");
			opt_blocksize = 512;
		}
		if (opt_timeout < 1 && opt_timeout > 255)
		{
			printf(	"WARNING: -T option ignored - Illegal Timeout Seconds "
					"(min.1 - max.255)\n");
			opt_timeout = 0;
		}
		i++;
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	para(argc, argv);

	net_resident_check();
	net_ver_check();
	
	if (arg_remote)
	{
		automode();
	}
	commandmode();
	
	return 0;
}
