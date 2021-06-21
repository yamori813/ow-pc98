/************************************************************************
	GA-1024A / GA-1280A グラフィックライブラリ サンプルソース
		BMPLOAD.C

	BMPファイルロ−ダ

				アイ・オー・データ機器　Device Inc...
*************************************************************************/
#include <stdio.h>
#include <fcntl.h>
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

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

#define BUFF_SIZE 16*1024

static  uchar buff[BUFF_SIZE];          /* ファイルバッファ */
static  uchar buff4[2048];
static  uint  gaport;                /* GA-1024A/GA-1280A I/Oポ-トアドレス */
static  volatile uchar far *ga_wind; /* グラフィックメモリウィンドウアドレス*/
static  uchar lut[256*3];            /* パレットデ−タ */

static  int   gmode = 1;	        /* 画面モ−ド 1:1024x768  2:640x480 */
									/*           13:1024x768 15:640x480 */
static  uint  colorMax;				/* 色数 */
static  int	  mode_flg = 0;

static	int	  xsize,ysize,scan;
static	int   bit;
static	int   rsmode;
static	int   idx;
static	int	  remain;
static	char  *fname;

void    ginit();
void    palinit();
void    error(char *);
int     gaload(int);
void	rle_4(int);
void	rle_8(int);
void	nomal(int);
int     freadline(int ,int *,int );
void    wait(int);

/**************************************

	main()

***************************************/
void main(argc,argv)
int   argc;
char  *argv[];
{
	int handl;
	int i, pal_flg;


	fname = argv[1];
	puts ("GA-1024A/GA-1280A BMP loader Ver 1.00  I・O DATA DEVICE Inc.");
	if ( argc < 2 ) {
		puts("usage : BMPLOAD <file> /xx");
		puts("    /xx  = /8  ( 8 plane mode )");
		puts("           /16 (16 plane mode )");
		error("");
	}
	if ( argc > 3 ) error("パラメータ 無効");
		if ( argv[2][0] == '/' ) {
			if ( argv[2][1] == '1' && argv[2][2] == '6' ) {
				mode_flg = 1;
				gmode = 15;
			}
			else {
				 if ( argv[2][1] == '8' )
					mode_flg = 0;
				else
					error("パラメータ 無効");
			}
		}

	if ((handl = open(argv[1], O_RDONLY | O_BINARY )) == -1) {
		error("can't open file");
	}
	ginit();
	gaload(handl);
	while (1) {
		switch (getch()) {
			case 'C':
			case 'c':	/* 画面モードを変える */
			if ( mode_flg == 0 ) {
				if (gmode & 1)
					 	gmode++ ;
					else 
					 	gmode-- ;
				GAinit(gmode);		
				GAsetVisualPlane(-1);
				wait(40);			/* CRTが同期するまで待つ */
				palinit();
				GAsetVisualPlane(0);
			}
			else {
				if ( gmode == 15 )
					gmode -= 2;
				else
					gmode += 2;
				GAinit(gmode);
			}
				continue;
			case 'q':
			case 'Q':	/* パレット設定を変更しないで終了 */
				pal_flg = PAL_USER;
				break;
			default:
				pal_flg = PAL_DEF;
				break;
		}
		break;
	}
	GAcrtSel(CRT_PC);
	GAterm( pal_flg );	/* 終了関数 */
	exit(0);
}

/***********************************************

	初期化

************************************************/
void ginit()
{
	struct VDCONFIG vd;

	if (GAinit(gmode)) error("GAINIT が常駐していません");
	GAclrScreen();
	GAgetVideoConfig((void far *)&vd);
	gaport = vd.port ;	/* GA-1024A/GA-1280A レジスタのポート番号 */
	ga_wind = (void far *) ((ulong)vd.ga_seg << 16); /* グラフィックメモリウィンドウアドレス */
	GAcrtSel(CRT_GA);
}

/***************************************************

	エラ−処理

****************************************************/
void error(s)
char *s;
{
	puts(s);
	GAcrtSel(0);
	exit(1);
}


/****************************************************

	パレット設定

*****************************************************/
void palinit()
{
	int i;
	for (i=0 ; i < colorMax ; i++) { 
		GAsetPalette((void far *)&lut[i*3],i);
	}
}



#define RED   2 
#define GREEN 1 
#define BLUE  0 
/*****************************************************************

	画像データロード

*****************************************************************/
int  gaload(handl)
int  handl;
{
	int 	i,j,k,rle_code;
	long 	lut_sz;
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER inf;

idx = 0;
remain = 0;

		/* ファイルヘッダ */
	if ( !read(handl,(char *)&fh,sizeof(BITMAPFILEHEADER))) goto endr;
	if ( fh.bfType != 'B' + 'M'*256) {
		puts("not BMP File!");
		goto endr;
	}
	if ( !read(handl,(char *)&inf,sizeof(BITMAPINFOHEADER))) goto endr;
	rle_code = inf.biCompression;
	xsize = inf.biWidth ;
	ysize = (inf.biHeight > 768) ? 768 : inf.biHeight ;
	colorMax = inf.biCirUsed ? inf.biCirUsed : 256;
    bit = inf.biBitCount;

	if (bit == 1) {      /* モノクロ */
		scan = ((xsize + 31) / 8) & 0xfffc ;
		rsmode = PLANE_MODE;
		gmode = 7;   /* 1024x768 mono */
		GAinit(gmode) ;
	} else  if (bit == 4) {  /* １６色 */
		scan = ((xsize + 7) / 2) & 0xfffc ;
		rsmode = PIXEL_MODE;
		colorMax = 16;
		gmode = 3;   /* 1024x768 mono */
		GAinit(gmode) ;
	} else if (bit ==8 ) {		/*	256色 */
		scan = (xsize + 3) & 0xfffc ;
		rsmode = PIXEL_MODE;
	} else if (bit == 24) {       /* 24Bit フルカラー */
			scan = (xsize * 3 + 3) & 0xfffc ;
			rsmode = PIXEL_MODE;
			if ( mode_flg == 0 ) {
				colorMax = 6*6*6;
				idx = 0;
				for (i=0 ; i < 6 ; i++) {
					for (j=0 ; j<6 ; j++) {
						for (k=0 ; k<6 ; k++ ) {
							lut[idx++] = i * 0x33;	/* R */
							lut[idx++] = j * 0x33;	/* G */
							lut[idx++] = k * 0x33;	/* B */
						}
					}
				}
			}
	}
	lut_sz = fh.bfOffBits - 54 ;

	if (bit != 24) {	
		/* パレット設定             */
		if (!read(handl,buff,(int)lut_sz)) goto endr;
		for (i=0; i < colorMax ; i++ ) { 
			lut[i*3+0] = buff[RED+  (i*4)];
			lut[i*3+1] = buff[GREEN+(i*4)];
			lut[i*3+2] = buff[BLUE+ (i*4)];
		}
	}
	if ( mode_flg == 0 ) palinit();

#define BI_RGB  0
#define BI_RLE8 1
#define BI_RLE4 2
/*---------------------------------------*/
/*	 画像をグラフィックメモリに展開する	 */
/*---------------------------------------*/

	switch(rle_code){
		case BI_RGB  : 
				nomal(handl);
				break;
		case BI_RLE8 :
				rle_8(handl);
				break;
		case BI_RLE4 :
				rle_4(handl);
				break;
		default:
				break;
	}
	close( handl );
	return(0);
endr:
	close( handl );
	error("error!  load abote");
}


/*****************************************************************

	圧縮フォーマット

******************************************************************/

/*----------------------------------*/
/*			<< BI_RLE8 >>			*/
/*----------------------------------*/

void rle_8(int handl)
{
	uchar 	count,color,i,endf=1;
	uint	x,y;

	x=0;
	y=ysize;
	while(endf) {
		if (read(handl,buff,2) == 0  ) return;
		count = buff[0];
		color = buff[1];
		if ( count == 0 ) {
			switch(color) {
				case 0:						/* 改行 */
						x = 0;
						y--;
						break;
				case 1:						/* 終了 */
						endf=0;
						break;
				case 2:						/* (+x,+y) 移動 */
						read(handl,buff,2);
						x += buff[0];
						y += buff[1];
						break;
				default:					
		 				count = color;
						read(handl,buff,((count+1) & 0x00fe));
						GArestoreImage(x,y,x+count-1,y,rsmode,(void far *)buff);
				        x += count;
						break;
			}
		} else {
				for(i = 0; i < count; i++)	{
					buff[i] = color;
				}
				GArestoreImage(x,y,x+count-1,y,rsmode,(void far *)buff);
				x += count;
		}
	}
}

/*----------------------------------*/
/*			<< BI_RLE4 >>			*/
/*----------------------------------*/

void rle_4(int handl)
{
	uchar 	color_1,color_2,color,count,i,endf=1;
	uint	x,y;

	x=0;
	y=ysize;
	while(endf) {
		if (read(handl,buff,2) == 0  ) return;
		count = buff[0];
		color = buff[1];
		if (count == 0) {
			switch(color){
				case 0:
					x = 0;
					y--;
					break;
				case 1:
					endf=0;
					break;
				case 2:
					read(handl,buff,2);
					x += buff[0];
					y += buff[1];
					break;
				default:
					count = color;
					read(handl,buff4,((count/2+1) & 0x00fe));
					for(i = 0; i < count; i++){
						buff[i*2]   = buff4[i] >> 4;
						buff[i*2+1] = buff4[i] & 0x0f;
					}
					GArestoreImage(x,y,x+count-1,y,rsmode,(void far *)buff);
			        x += count;
					break;
			}
		} else {
				color_1 = color >> 4;
				color_2 = color & 0x0f;
				for(i = 0; i < count; i+=2){
					buff[i]   = color_1;
					buff[i+1] = color_2;
				}
				GArestoreImage(x,y,x+count-1,y,rsmode,(void far *)buff);
				x += count;
		}
	}
}



/*****************************************************************

	ノーマルフォーマット

******************************************************************/
void nomal(int handl)
{

	int		yy,xx,i,j,k;

	for (yy=ysize-1;yy;yy--) {
		if (freadline(handl,&idx,scan)==0) break ; 
		if (bit == 4) {  /* 16色 */
			for (xx=0 ; xx < xsize ; xx += 2) {
				buff4[xx  ] = buff[idx] >> 4 ;
				buff4[xx+1] = buff[idx++] & 0x0f ;
			}
			GArestoreImage(0,yy,xsize-1,yy,rsmode,(void far *)buff4);
		} else	if (bit == 24) { /* フルカラ- -> 216色 */
			for (xx=0 ; xx < xsize ; xx++,idx += 3) {
				if ( mode_flg == 0 ) {																buff4[xx] =  ((buff[idx  ]+25) / 51)  
					           + (((buff[idx+1]+25) / 51) * 6  )
	    				       + (((buff[idx+2]+25) / 51) * 36 ) ;
				} else {
					*((ushort *)buff4 + xx) = 
									((buff[RED+idx]   & 0xf8) << 8) |
					                ((buff[GREEN+idx] & 0xfc) << 3) |
	    				            ((buff[BLUE+idx]  & 0xf8) >> 3) ;
				}
			}
			GArestoreImage(0,yy,xsize-1,yy,rsmode,(void far *)buff4);
		} else {  /* モノクロ    ２５６色 */
		    GArestoreImage(0,yy,xsize-1,yy,rsmode,(void far *)(buff+idx));  
	    }
	}
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
	int    ret = size;
	
	if (remain < size) {
		fp = 0;
		remain = ( BUFF_SIZE / size ) * size;
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

