/************************************************************************
	GA-1024A / GA-1280A グラフィックライブラリ サンプルソース
		TIFFLOAD.C
	
	TIFFファイルロ−ダ (FM-TOWNS TIFFファイル)

				アイ・オー・データ機器　Device Inc...
*************************************************************************/
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "gagraph.h"

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

#define BUFF_SIZE 16*1024
static  uchar buff[BUFF_SIZE];         /* ファイルバッファ */
static  uchar buff2[1024];
static  uchar buff4[1024];
static  uint  gaport;	               /* GA-1024i I/Oポ-トアドレス 下位バイト*/
static  volatile uchar far *ga_wind;    /* グラフィックメモリウィンドウアドレス*/
static  uchar lut[256*3];               /* パレットデ−タ */
static  int   gmode = HRESO_256C;       /* 画面モ−ド 1:1024x768 2:640x480 */
static  int   colorMax;                 /* 色数 */
static  int   dither = 0;              /* ディザ- フラグ */

void    ginit();
void    wait(int);
int     freadline(int ,int *,int );
void    error(char *);
void    palinit(int);
int     gaload(int);
void    dither16(int ,int ,int );

/**************************************

	main()

***************************************/
void main(argc,argv)
int   argc;
char  *argv[];
{
	int handl,i;
	int pal_flg;

	puts ("GA-1024A/1280A TIFF file loader Ver 1.00   I・O DATA DEVICE Inc.");
	if (argc < 2) error("usage : TIFFLOAD <TIFF file>  [/D]");
	if ((handl = open(argv[1], O_RDONLY | O_BINARY )) == -1) {
		error("can't open file");
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
	ginit();
	gaload(handl);
	while (1) {
		switch (getch()) {
			case 'C':
			case 'c':	/* 画面モードをえる */
			if (gmode & 1)
				 	gmode++ ;
				else 
				 	gmode-- ;
				GAinit(gmode);		
				GAsetVisualPlane(-1);
				wait(40);			/* CRTが同期するまで待つ */
				palinit(colorMax);
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

/***********************************************

	初期化

************************************************/
void ginit()
{
	struct VDCONFIG vd;

	if (GAinit(gmode)) error("GAINIT が常駐していません");
	GAclrScreen();
	GAgetVideoConfig((void far *)&vd);
	gaport = vd.port ;                   /* GA-1024A レジスタのポート番号 */
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
void palinit(int colors)
{
	int i;

	if (colors > 256) colors = 256; 
	for (i=0 ; i < colors ; i++) { 
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
	int 	xsize=0,ysize=0;
	int	    yy,xx,i,j,k,scan;
	int 	tag;
	int 	rslt;
	uint	cnt;
	ulong	rofs = 0;
	ulong	rdat = 0;
    ulong   pofs = 0;
    int     comp = 1;
	int     bit  = 1;
	int     sampl = 1;
	int     newsubf = 1;
	int     rsmode;
	int     idx;
	int     FillOrd = 1;
	int     pal = 0;
	ushort  cc;

		/* ファイルヘッダ */
	if ( !read(handl,buff,0x200)) goto endr;
	if ( buff[0] != 'I' || buff[1] != 'I' ) {
		puts("not TIFF File!");
		goto endr;
	}
	idx = * (long *)&buff[4];     /* タグ開始オフセット */
	cnt = * (short *)&buff[idx];  /* タグ数 */
	idx += 2;
	for (i = 0; i < cnt; i++) {
		tag = * (ushort *)&buff[idx];
		idx += 8;
		rdat = *(long *)&buff[idx];
		idx += 4;
		if (tag == 0x100) {     /* ｘサイズ */
			xsize = rdat;
		} else if(tag == 0x101) { /* ｙサイズ */
			ysize = rdat;
		} else if (tag == 0x102) { /* Bit/Pixel */
			bit = rdat;
		} else if (tag == 0xfe) { /* New Subfile */
			newsubf = rdat;
		} else if (tag == 0x103) { /* 圧縮モ−ド */
			comp = rdat;
		} else if (tag == 0x106) { /* パレット有無 */
			pal = rdat;
		} else if (tag == 0x10a) { /* TOWNS 16色モ-ド */
			FillOrd = rdat;
		} else if (tag == 0x111) { /* 画像デ−タオフセット */
			rofs = rdat;
		} else if (tag == 0x115) { /* byte/Pixel */
			sampl = rdat;
		} else if (tag == 0x140) { /* パレットオフセット */
			pofs = rdat;
		}
	}
	bit = bit * sampl ;
	colorMax = 1 << bit;
	printf("X Size %d Y Size %d Colors %d\n",xsize,ysize,colorMax);
	if (comp != 1) {
		puts("圧縮デ−タはサポ−トしていません");
		goto endr;
	}
	if (pal == 0) { /* モノクロ */
		lut[0] = lut[1] = lut[2] = 0;
		lut[3] = lut[4] = lut[5] = 255;
	} else if (pal == 1 || pal == 2) { /* パレットデータ無し */
		if (bit == 1) { /* 2色パレット */
			lut[0] = lut[1] = lut[2] = 0;
			lut[3] = lut[4] = lut[5] = 255;
		} else if (bit == 4) { /* 16色パレット */
			if (FillOrd==2) {
				for (i=0;i<16;i++) {
					lut[i*3]   = (i & 2) ? ( (i & 8) ? 255 : 128) : 0 ;
					lut[i*3+1] = (i & 4) ? ( (i & 8) ? 255 : 128) : 0 ;
					lut[i*3+2] = (i & 1) ? ( (i & 8) ? 255 : 128) : 0 ;
				}
				lut[8*3] = lut[8*3+1] = lut[8*3+2] = 192;
			} else { /* モノクロ１６階調 */
				for (i=0;i<16;i++) {
					lut[i*3] = lut[i*3+1] = lut[i*3+2] = i << 4;
				}
			}
		} else if (bit==8) { /* ２５６色パレット */
			if (newsubf) {
				for (i=0;i<256;i++) lut[i*3] = lut[i*3+1] = lut[i*3+2] = i;
			} else {
				for (i=0;i<256;i++) {
					lut[i*3]   = ((i & 0x1c) >> 2) * 36;
					lut[i*3+1] = ((i & 0xe0) >> 5) * 36;
					lut[i*3+2] = (i & 3) * 85;
				}
			}
		} else { /* FULL color */
			idx = 0;
			for (i=0 ; i < 6 ; i++) {
				for (j=0 ; j<6 ; j++) {
					for (k=0 ; k<6 ; k++ ) {
						lut[idx++] = k * 0x33;	/* R */
						lut[idx++] = j * 0x33;	/* G */
						lut[idx++] = i * 0x33;	/* B */
					}
				}
			}
			colorMax = 6*6*6;
		}
	} else if (pal == 3) { /* パレット リード */
		lseek(handl, pofs, SEEK_SET);
		read(handl,buff,colorMax*3*2);
		for (i=0;i<colorMax;i++) {
			lut[i*3]   = buff[i*2+1];
			lut[i*3+1] = buff[i*2+colorMax*2+1];
			lut[i*3+2] = buff[i*2+colorMax*4+1];
		}
	}
	lseek(handl,rofs,SEEK_SET);
	if (bit == 1) {      /* モノクロ */
		scan = (xsize + 7) / 8 ;
		rsmode = 0;
		gmode = 7;   /* 1024x768 mono */
		GAinit(gmode) ;
	} else if (bit == 4) {  /* １６色 */
		scan = (xsize + 1) / 2 ;
		rsmode = 1;
		gmode = 3;   /* 1024x768 mono */
		GAinit(gmode) ;
	} else if (bit == 8) {		/*	256色 */
		scan = xsize  ;
		rsmode = 1;
	} else if (bit == 16) {       /* 15Bit フルカラー */
		scan = xsize * 2 ;
		rsmode = 1;
	} else if (bit == 24) {       /* 24Bit フルカラー */
		scan = xsize * 3 ;
		rsmode = 1;
	}
	if (xsize > 1024) xsize = 1024;
	palinit(colorMax);

	/* 画像をグラフィックメモリに展開する */
	for (yy=0;yy<ysize;yy++) {
		if (freadline(handl,&idx,scan)==0) break ; 
		if (bit == 4) {  /* 16色 */
			if (FillOrd == 2) { /* TOWNS ONLY */
				for (xx=0 ; xx < xsize ; xx += 2) {
					buff4[xx+1] = buff[idx] >> 4 ;
					buff4[xx+0] = buff[idx++] & 0x0f ;
				}
			} else { /* Not TOWNS */
				for (xx=0 ; xx < xsize ; xx += 2) {
					buff4[xx+0] = buff[idx] >> 4 ;
					buff4[xx+1] = buff[idx++] & 0x0f ;
				}
			}
			GArestoreImage(0,yy,xsize-1,yy,1,(void far *)buff4);
		} else if (bit == 16) { /* フルカラ- -> 216色 */
			if (dither) dither16(xsize,yy,idx); /* 2x2ディザ- */
			else {
				for (xx=0 ; xx < xsize ; xx++,idx += 2) {
					cc = * (ushort *)&buff[idx];
					buff4[xx] =  (((cc & 0x03e0) >> 3) / 21)   
				           + ((((cc & 0x7c00) >> 8) / 21) * 6  )
				           + ((((cc & 0x001f) << 2) / 21) * 36 );
				}
				GArestoreImage(0,yy,xsize-1,yy,1,(void far *)buff4);
			}
		} else if (bit == 24) { /* 24Bit フルカラ- */
			for (xx=0 ; xx < xsize ; xx++,idx += 3) {
				buff4[xx] =  ((buff[idx  ]+25) / 51)  
				           + (((buff[idx+1]+25) / 51) * 6  )
				           + (((buff[idx+2]+25) / 51) * 36 ) ;
			}
			GArestoreImage(0,yy,xsize-1,yy,1,(void far *)buff4);
		} else {  /* モノクロ    ２５６色 */
		    GArestoreImage(0,yy,xsize-1,yy,rsmode,(void far *)(buff+idx));  
		}
	}
	close( handl );
	return(0);
endr:
	close( handl );
	error("error!  load abote");
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
		remain = ( BUFF_SIZE / size ) * size;
		ret = read(handl,buff,remain);
	} 
	*idx = fp;
	fp += size;
	remain -= size;
	return(ret);
}

/**********************************************************

	32768色のデ−タを2x2のディザに変換する
	
***********************************************************/
void dither16(int xsize,int y,int idx)
{
	uint   r,g,b,cc;
	int    x;

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


	for (x=0;x<xsize*2;x+=2,idx+=2) {
		cc = *(ushort *)&buff[idx];
		g = ((cc & 0x7c00) >> 9) / 3;
		r = ((cc & 0x03e0) >> 4) / 3;
		b = ((cc & 0x001f) << 1) / 3;
		buff2[x]   = dh1r[b]+dh1g[g]+dh1b[r];
		buff2[x+1] = dh2r[b]+dh2g[g]+dh2b[r];
		buff4[x]   = dh3r[b]+dh3g[g]+dh3b[r];
		buff4[x+1] = dh4r[b]+dh4g[g]+dh4b[r];
	}
	GArestoreImage(0,y*2,xsize*2-1,y*2,1,(void far *)buff2);
	GArestoreImage(0,y*2+1,xsize*2-1,y*2+1,1,(void far *)buff4);
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

