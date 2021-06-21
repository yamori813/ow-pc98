/************************************************************************
	GA-1024A / GA-1280A グラフィックライブラリ サンプルソース
		X68LOAD.C
	
	X68000 GRAM ベタファイルロ−ダ (X68KのDB.X で W ファイル名,C00000,C7FFFF)

				アイ・オー・データ機器　Device Inc...
*************************************************************************/
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>

#ifndef TURBO
#include <sys\types.h>
#include <sys\stat.h>
#include <malloc.h>
#else
#include <alloc.h>
#endif

#include "gagraph.h"

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

#define	SQR(x)		((x) * (x))

#define SIZE_OF_BUFF 1024*16

#define	COL_CNT		(6 * 6 * 6)

int 	handle;		/* DOS file handle */
int	    buff_len;	/* buff reset */
uchar   buff[SIZE_OF_BUFF]; /* file buffer */
uchar   buff2[1024];
uchar   buff4[1024];
int	    x_wid = 512;		/* screen size */
int	    y_wid = 384;
int 	gmode = 1;

static unsigned int  gaport;
static volatile uchar far *ga_wind;

void    ginit();
void    paletinit();
void    convert();
void    wait(int);
int     freadline(int ,int *,int );
void    error(char *);
void    dither4(int );

/********************************************

	main()

*********************************************/
void main(argc,argv)
int	argc;
char	*argv[];
{
	int i,x,y;
	int dither = 0;
	int pal_flg;

	puts("GA-1024A/GA-1280A X6800 File loader program Ver 1.00 IⅤO DATA DEVICE Inc.");
	if (argc < 2) error("usage : X68LOAD <X68000 file> [-D]");
	if ((handle=open(argv[1],O_RDONLY | O_BINARY))==-1) {
		error("file can't open");
	}
	for (i=2;i<argc;i++) {
		if (argv[i][0] == '-' || argv[i][0] == '/') {
			switch(argv[i][1]) {
			case 'D' :
			case 'd' :
				dither = 1;
			 break;
			default:
				puts("パラメ-タ無効 ");
			}
		}
	}
	gmode = 1;
	ginit();				/* screen init */
	GAcrtSel(1);
	if (dither) dither4(handle); 
	else convert(handle);
	close(handle);

	while (1) {
		switch (getch()) {
			case 'C':
			case 'c':	/* 画面モードを変える */
			if (gmode & 1)
				 	gmode++ ;
				else 
				 	gmode-- ;
				GAinit(gmode);		
				GAsetVisualPlane(-1);
				wait(40);			/* CRTが同期するまで待つ */
				paletinit();
				GAsetVisualPlane(0);
				continue;
			case 'Q':
			case 'q':	/* パレットを変更せずに終了 */
				pal_flg = PAL_USER;
				break;
			default:
				pal_flg = PAL_DEF;
				break;
		}
		break;
	}
	GAcrtSel(CRT_PC);
	GAterm( pal_flg );
	exit(0);
}

/************************************************

	初期化

*************************************************/
void ginit()
{
	struct VDCONFIG vd;

	if (GAinit(gmode)) {
		puts("GAINIT が常駐していません");
	    exit(1);
	}
	GAclrScreen();
	paletinit();
	GAgetVideoConfig((void far *)&vd);
	gaport = vd.port	; /* GA レジスタのポート番号 */
	ga_wind = (void far *) ((long)vd.ga_seg	<< 16); /*グラフィックボードウィンドウのセグメント*/
}

/********************************************************

	パレットの初期化

*********************************************************/
void paletinit()
{
	struct PALETTE pal;
	int i;
	int r,g,b;

/*  R G B 6x6x6 */
	i = 0 ;
	for (r=0;r<6;r++){
		for (g=0;g<6;g++){
			for (b=0;b<6;b++){
				pal.blue = b * 0x33;
				pal.red  = r * 0x33;
				pal.green= g * 0x33;
				GAsetPalette((void far *)&pal,i++);
			}
		}
	}
}

/***************************************************

	32768色のデ−タを誤差拡散により減色する

*****************************************************/
void convert(int handle)
{
	int     x, y, i, c,idx;
	short  *av1, *av2, *av3;
	ushort  cc;

	av1 = malloc((x_wid + 2) * sizeof *av1 * 3);
	av2 = malloc((x_wid + 2) * sizeof *av2 * 3);
	av3 = malloc((x_wid + 2) * sizeof *av3 * 3);
	memset(av2, 0, (x_wid + 2) * sizeof *av2 * 3);
	memset(av3, 0, (x_wid + 2) * sizeof *av3 * 3);

	for (y = 0; y < y_wid; y++) {
		if ((y % 4)==0) freadline(handle, &idx,x_wid * 2) ;
		if (freadline(handle, &idx,x_wid * 2) == 0)
			break;
		for (x = 0; x < x_wid; x++,idx += 2) {
			i = x * 3;
			cc = (buff[idx] << 8) + buff[idx+1];
			av1[i]     = av2[i]     + ((cc & 0x07c0)>>3) ; /* Red */
			av1[i + 1] = av2[i + 1] + ((cc & 0xf800)>>8) ; /* Green */
			av1[i + 2] = av2[i + 2] + ((cc & 0x003e)<<2) ; /* Blue */
			av2[i]     = av3[i];
			av2[i + 1] = av3[i + 1];
			av2[i + 2] = av3[i + 2];
			av3[i]     = 0;
			av3[i + 1] = 0;
			av3[i + 2] = 0;
		}
		for (x = 0; x < x_wid; x++) {
			i = x * 3;
			c = 0;
			if (av1[i] >= 0x33) {
				if (av1[i] >= 0x66) {
					if (av1[i] >= 0x99) {
						if (av1[i] >= 0xcc) {
							if (av1[i] >= 0xff) {
								av1[i] -= 0xff;
								c += 6 * 6 * 5;
							} else {
								av1[i] -= 0xcc;
								c += 6 * 6 * 4;
							}
						} else {
							av1[i] -= 0x99;
							c += 6 * 6 * 3;
						}
					} else {
						av1[i] -= 0x66;
						c += 6 * 6 * 2;
					}
				} else {
					av1[i] -= 0x33;
					c += 6 * 6 * 1;
				}
			}
			if (av1[i + 1] >= 0x33) {
				if (av1[i + 1] >= 0x66) {
					if (av1[i + 1] >= 0x99) {
						if (av1[i + 1] >= 0xcc) {
							if (av1[i + 1] >= 0xff) {
								av1[i + 1] -= 0xff;
								c += 6 * 5;
							} else {
								av1[i + 1] -= 0xcc;
								c += 6 * 4;
							}
						} else {
							av1[i + 1] -= 0x99;
							c += 6 * 3;
						}
					} else {
						av1[i + 1] -= 0x66;
						c += 6 * 2;
					}
				} else {
					av1[i + 1] -= 0x33;
					c += 6 * 1;
				}
			}
			if (av1[i + 2] >= 0x33) {
				if (av1[i + 2] >= 0x66) {
					if (av1[i + 2] >= 0x99) {
						if (av1[i + 2] >= 0xcc) {
							if (av1[i + 2] >= 0xff) {
								av1[i + 2] -= 0xff;
								c += 5;
							} else {
								av1[i + 2] -= 0xcc;
								c += 4;
							}
						} else {
							av1[i + 2] -= 0x99;
							c += 3;
						}
					} else {
						av1[i + 2] -= 0x66;
						c += 2;
					}
				} else {
					av1[i + 2] -= 0x33;
					c += 1;
				}
			}
			switch ((rand() & 0x0300) >> 8) {
			case 0:
			case 1:
				av1[i + 3]     += av1[i] / 2;
				av1[i + 3 + 1] += av1[i + 1] / 2;
				av1[i + 3 + 2] += av1[i + 2] / 2;
				av2[i]     += av1[i] / 2;
				av2[i + 1] += av1[i + 1] / 2;
				av2[i + 2] += av1[i + 2] / 2;
				break;
			case 2:
				av1[i + 3]     += av1[i] / 4 * 3;
				av1[i + 3 + 1] += av1[i + 1] / 4 * 3;
				av1[i + 3 + 2] += av1[i + 2] / 4 * 3;
				av2[i]     += av1[i] / 4;
				av2[i + 1] += av1[i + 1] / 4;
				av2[i + 2] += av1[i + 2] / 4;
				break;
			case 3:
				av1[i + 3]     += av1[i] / 4;
				av1[i + 3 + 1] += av1[i + 1] / 4;
				av1[i + 3 + 2] += av1[i + 2] / 4;
				av2[i]     += av1[i] / 4 * 3;
				av2[i + 1] += av1[i + 1] / 4 * 3;
				av2[i + 2] += av1[i + 2] / 4 * 3;
				break;
			default:
				break;
			}
			buff2[x] = c;
		}
		GArestoreImage(0,y,x_wid-1,y,1,(void far *)buff2);  
	}
} 

/*****************************************************

	32768色のデ−タを2x2のディザ−に変換する

******************************************************/
void dither4(int handle)
{
	uint   r,g,b,cc;
	int    x,y,idx;
	int    xsize = 512;
	int    ysize = 384;

static char dh1b[] = { 0, 1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5 };
static char dh2b[] = { 0, 0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5 };
static char dh3b[] = { 0, 0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5 };
static char dh4b[] = { 0, 0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5 };

static char dh1g[] = { 0, 6,6,6,6,12,12,12,12,18,18,18,18,24,24,24,24,30,30,30,30 };
static char dh2g[] = { 0, 0,0,6,6, 6, 6,12,12,12,12,18,18,18,18,24,24,24,24,30,30 };
static char dh3g[] = { 0, 0,0,0,6, 6, 6, 6,12,12,12,12,18,18,18,18,24,24,24,24,30 };
static char dh4g[] = { 0, 0,6,6,6, 6,12,12,12,12,18,18,18,18,24,24,24,24,30,30,30 };

static char dh1r[] = { 0, 36,36,36,36,72,72,72,72,108,108,108,108,144,144,144,144,180,180,180,180 };
static char dh2r[] = { 0,  0, 0,36,36,36,36,72,72,72,72,108,108,108,108,144,144,144,144,180,180 };
static char dh3r[] = { 0,  0, 0, 0,36,36,36,36,72,72,72,72,108,108,108,108,144,144,144,144,180 };
static char dh4r[] = { 0,  0,36,36,36,36,72,72,72,72,108,108,108,108,144,144,144,144,180,180,180 };

	
	for (y = 0; y < ysize; y++) {
		if ((y % 4)==0) freadline(handle, &idx,xsize * 2) ;
		if (freadline(handle, &idx,xsize * 2) == 0)
				break;
		for (x=0;x<xsize*2;x+=2,idx+=2) {
			cc = (buff[idx]<<8) +buff[idx+1];
			g = ((cc & 0xf800) >> 10) / 3;
			r = ((cc & 0x07c0) >> 5)  / 3;
			b = (cc & 0x003e)         / 3;
			buff2[x]   = dh1r[r]+dh1g[g]+dh1b[b];
			buff2[x+1] = dh2r[r]+dh2g[g]+dh2b[b];
			buff4[x]   = dh3r[r]+dh3g[g]+dh3b[b];
			buff4[x+1] = dh4r[r]+dh4g[g]+dh4b[b];
		}
		GArestoreImage(0,y*2,xsize*2-1,y*2,1,(void far *)buff2);
		GArestoreImage(0,y*2+1,xsize*2-1,y*2+1,1,(void far *)buff4);
	}
}


/*************************************************************

	エラ−処理

*************************************************************/
void error(s)
char *s;
{
	puts(s);
	GAcrtSel(0);
	exit(1);
}


/*****************************************************************

	１ライン分ファイルリード

******************************************************************/
int freadline( handl,idx,size)
int  handl;
int *idx;
int  size;
{
	static int fp = 0;
	static int remain = 0;
	int    ret = size;
	
	if (remain < size) {
		fp = 0;
		remain = ( SIZE_OF_BUFF / size ) * size;
		ret = read(handl,buff,remain);
	} 
	*idx = fp;
	fp += size;
	remain -= size;
	return(ret);
}



/* グラフィック コントロ-ル レジスタ ポ-ト */
#define CRTC_AR 0x1e00
#define CRTC_CR 0x1f00
/***********************************************************

	CRTC の垂直同期信号をカウントしてウエイトする

************************************************************/
void wait(int count) 
{
#if 0
	int  i;
	for (i=0 ; i < count ; i++) {
#ifdef TURBO
		outportb(CRTC_AR+gaport,31);       /* CRTC STATUS register */
		while ( inportb(CRTC_CR+gaport) & 2) ;
		while ( !(inportb(CRTC_CR+gaport) & 2)) ;
#else
		outp(CRTC_AR+gaport,31);       /* CRTC STATUS register */
		while ( inp(CRTC_CR+gaport) & 2) ;
		while ( !(inp(CRTC_CR+gaport) & 2)) ;
#endif
	}
#endif
}


