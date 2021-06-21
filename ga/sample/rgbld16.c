/************************************************************************
	GA-1024A / GA-1280A グラフィックライブラリ サンプルソース
		RGBLD16.C	< 65536 色モード用 >
	
	RGBファイルロ−ダ   

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
ushort  buff2[2048];
int	    x_wid = 640;		/* screen size */
int	    y_wid = 768;
int 	gmode = 1;

static unsigned int  gaport;
static volatile uchar far *ga_wind;

static uchar radj[256];
static uchar gadj[256];
static uchar badj[256];

void ginit();
void convert();
void save();
void    wait(int);
int     freadline(int ,int *,int );
void    error(char *);

/********************************************

	main()

*********************************************/
void main(argc,argv)
int	argc;
char	*argv[];
{
	int i,x,y;
	int pal_flg;

	puts("GA-1024 RGB File loader program Ver 1.00 IⅤO DATA DEVICE Inc.");
	if (argc < 2) error("usage : RGBLOAD <RGB file > -Xxxx -Yxxx");
	if ((handle=open(argv[1],O_RDONLY | O_BINARY))==-1) {
		error("file can't open");
	}
	for (i=2;i<argc;i++) {
		if (argv[i][0] == '-' || argv[i][0] == '/') {
			switch(argv[i][1]) {
			case 'X' :
			case 'x' :
				x_wid = atoi(&argv[i][2]);
			 break;
			case 'Y' :
			case 'y' :
				y_wid = atoi(&argv[i][2]);
			 break;
			default:
				puts("パラメ-タ無効 ");
			}
		}
	}
	gmode = 15; /* 640x480 64k */
	ginit();				/* screen init */
	GAcrtSel(1);
	convert(handle);
	close(handle);

	while (1) {
		switch (getch()) {
			case 'C':
			case 'c':   /* 画面モードを変える */
			if (gmode == 15)
				 	gmode -= 2 ;
				else 
				 	gmode += 2 ;
				GAinit(gmode);
				continue;
			case 'Q':
			case 'q':	/* パレットを変更しないで終了 */
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
		puts("GAINST が常駐していません");
	    exit(1);
	}
	GAclrScreen();
	GAgetVideoConfig((void far *)&vd);
	gaport = vd.port	; /* GA レジスタのポート番号 */
	ga_wind = (void far *) ((long)vd.ga_seg	<< 16); /*グラフィックボードウィンドウのセグメント*/
}

/***************************************************

	フルカラ−のデ−タを誤差拡散により減色する

*****************************************************/
void convert(int handle)
{
	int     x, y, i, c,idx;

	for (y = 0; y < y_wid; y++) {
		if (freadline(handle, &idx,x_wid * 3) == 0)
			break;
		for (x = 0; x < x_wid; x++,idx += 3) {
			buff2[x] = ((buff[idx]   & 0xf8) << 8)   | 
					   ((buff[idx+1] & 0xfc) << 3)   |
					   ((buff[idx+2] & 0xf8) >> 3)   ;
		}
		GArestoreImage(0,y,x_wid-1,y,1,(void far *)buff2);  
	}
} 

/**************************************************

	エラ−処理

***************************************************/
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


