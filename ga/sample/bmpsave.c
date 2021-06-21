/************************************************************************
	GA-1024A / GA-1280A グラフィックライブラリ サンプルソース
		BMPSAVE.C
	
    BMP FILE 画像セ−ブプログラム  

				アイ・オー・データ機器　Device Inc...
*************************************************************************/
#include <fcntl.h>

#ifndef TURBO
#include <sys\types.h>
#endif

#include <sys\stat.h>
#include <io.h>
#include "gagraph.h"

/*
 * header file for .bmp file header
 */
typedef	unsigned short	WORD;
typedef	unsigned long	DWORD;

typedef	struct	tagBITMAPFILEHEADER {
	WORD	bfType;
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
} BITMAPFILEHEADER;

typedef	struct	tagBITMAPINFOHEADER {
	DWORD	biSize;
	DWORD	biWidth;
	DWORD	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	DWORD	biXPelsPerMeter;
	DWORD	biYPelsPerMeter;
	DWORD	biCirUsed;
	DWORD	biCirImportant;
} BITMAPINFOHEADER;

typedef unsigned char  UCHAR;
typedef unsigned short USHORT;
typedef unsigned int   UINT;
typedef unsigned long  ULONG;

#define SIZE_OF_BUFF 1024
#define	COL_CNT      256

#define swap(x,y) { int z ; z = x ; x = y ; y = z; }

int 	handle;		/* DOS file handle */
UCHAR   buff[SIZE_OF_BUFF]; /* file buffer */
UCHAR   lut[COL_CNT*3];      /* palette */
int	    x_wid = 320;		/* screen size */
int	    y_wid = 240;
int     x_start = 0;
int     y_start = 0;
int     gmode = 0;

static UINT  gaport;
static volatile UCHAR far *ga_wind;

BITMAPFILEHEADER bf = {
	0x4d42,				/* ID ('B', 'M') */
	0x00000000,			/* file size */
	0x0000,				/* reserved */
	0x0000,				/* reserved */
	sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + COL_CNT * 4
};

BITMAPINFOHEADER bi = {
	sizeof(BITMAPINFOHEADER),	/* header size */
	0x00000000,			/* width */
	0x00000000,			/* height */
	0x0001,				/* planes */
	0x0008,				/* bit count */
	0x00000000,			/* compression */
	0x00000000,			/* size of image */
	0x00000000,			/* x-resolution */
	0x00000000,			/* y-resolution */
	COL_CNT,			/* color index count */
	0x00000000			/* important index count */
};

UCHAR col_idx[COL_CNT][4]; /* palette data */

extern int	far GetMouse(int far *,int far *,int far *,int far *);
extern int	far InitMouse();


void ginit();
void paletinit();
void paletget();
void hdrinit();
void save(int ,int ,int,int);
void error();
void wait(int);

/********************************************

	main()

*********************************************/
void main(argc,argv)
int	argc;
char	*argv[];
{
	int i,x,y,sx,sy,ox,oy,right,left,on;

    ox = oy = sx = sy =0;
	puts("GA-1024A/GA-1280A BMP File Saver program Ver 1.00  I・O DATA DEVICE Inc");
	if (argc < 2) error("usage : BMPSAVE <dst file> ");
	ginit();				/* screen init */
	GAcrtSel(1);
	if (InitMouse()) {
		error("マウスが使用できません");
	}
	on = 0;
    gmode=1;
	while (1) {
		if (kbhit()) {
			switch (getch()) {
			case 'S':
			case 's':
				break;
			case 'C':
			case 'c':   /* 画面モードを変える */
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
			default:
				error("save aborted.");
			}
			break;
		} else {
			GetMouse(&x, &y, &left, &right);
			if (x != ox || y != oy) GAdrawCursor(x,y);
			if (on) {	/* 既に左ボタンを押している */
				if (right) { /* 右を押したらキャンセル */
					GAeraseCursor();
					GAbox(sx,sy,ox,oy,-1,DASH_LINE);
					GAdrawCursor(x,y);
					on = 0;
				} else if (left) { /* 左ボタンを押し続ける */
					if (x != ox || y != oy) {
						GAeraseCursor();
						GAbox(sx,sy,ox,oy,-1,DASH_LINE);
						GAbox(sx,sy,x,y,-1,DASH_LINE);
						GAdrawCursor(x,y);
					}
				} else {
					if (sx > ox) swap(sx,ox);
					if (sy > oy) swap(sy,oy);
					x_wid   = ox - sx + 1;
					y_wid   = oy - sy + 1;
					x_start = sx;
					y_start = sy;
					GAeraseCursor();
					GAbox(sx,sy,ox,oy,-1,DASH_LINE);
					break;
				}
			} else {
				if (left) {
					sx = x;
					sy = y;
					on = 1;
					GAeraseCursor();
					GAbox(sx,sy,x,y,-1,DASH_LINE);
				}
			}
			ox = x;
			oy = y;
		}
	}
	GAeraseCursor();
	GAcrtSel(0);
	if ((handle=open(argv[1],O_CREAT | O_WRONLY | O_BINARY | O_TRUNC,S_IREAD | S_IWRITE))==-1) {
		error("file can't open");
	}
	puts("now saving.");
	hdrinit();
	save(x_start,y_start,x_wid,y_wid);
	close(handle);
	GAterm( PAL_DEF );
	exit(0);
}

/****************************************

	初期化

*****************************************/
void ginit()
{
	struct VDCONFIG vd;

	if (GAinit(gmode)) {
		puts("GAINST が常駐していません");
	    exit(1);
	}
	paletget();
	GAgetVideoConfig((void far *)&vd);
	gaport = vd.port	; /* GA レジスタのポート番号 */
	ga_wind = (void far *) ((long)vd.ga_seg	<< 16); /*グラフィックボードウィンドウのセグメント*/
}

/************************************************

	パレット設定

*************************************************/
void paletinit()
{
	int i;
	for (i = 0 ; i<COL_CNT;i++) {
		GAsetPalette((void far *)&lut[i*3],i);
	}
}

/****************************************************

	パレットデ−タを得る

****************************************************/
void paletget()
{
	int i;

	for (i = 0 ; i<COL_CNT;i++) {
		GAgetPalette((void far *)&lut[i*3],i);
	}
}

/****************************************************

	ＢＭＰファイルのヘッダを作る

****************************************************/
void	hdrinit()
{
	int 	i;

	bi.biWidth = (ULONG)x_wid;
	bi.biHeight = (ULONG)y_wid;
	bi.biSizeImage = bi.biWidth * bi.biHeight;
	bf.bfSize = bf.bfOffBits + bi.biSizeImage;
	for (i = 0; i < COL_CNT ; i++) {
		col_idx[i][0] = lut[i*3+2]; /* BLUE  */
		col_idx[i][1] = lut[i*3+1]; /* GREEN */
		col_idx[i][2] = lut[i*3+0]; /* RED   */
		col_idx[i][3] = 0x00;
	}
}


/********************************************************

	GA-1024A/GA-1280Aの画像をファイルに書き込む

*********************************************************/
void	save(int xstart,int ystart,int xwid,int ywid)
{
	int y;
	int scan;

	scan = (xwid + 3) & 0xfffc ;
	write(handle, (char *)&bf, sizeof bf);
	write(handle, (char *)&bi, sizeof bi);
	write(handle, (char *)col_idx[0], sizeof col_idx);
	for (y = ystart+ywid - 1; y >= ystart; y--) {
		GAsaveImage(xstart, y, xstart+scan-1, y, 1, buff);
		write(handle,buff,scan);
	}
}

/********************************************************

	エラ−処理

*********************************************************/
void error(s)
char *s;
{
	puts(s);
	GAcrtSel(0);
	exit(1);
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

