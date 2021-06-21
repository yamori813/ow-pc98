/*************************************************************
	GA-1024A / GA-1280A グラフィックライブラリ サンプルソース

		SAMPLE.C
			
				アイ・オー・データ機器　Device Inc...
**************************************************************/
#include "gagraph.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef TURBO
#include <malloc.h>
#else
#include <alloc.h>
#endif

#define	XO	256
#define	YO	128

extern void far VWait(int);    /* 時間待ち */
extern int	far GetMouse(int far *,int far *,int far *,int far *);
extern int	far InitMouse();

void ball();
void moveball();
void line_box();
void zukei();
void gradation();
void save_restore();
void gacursor();
void painttest();
/****************************
	マウスカーソルのデータ
****************************/
static char andmask[] = {
	0xff,0x80,0xff,0xff,
	0xfe,0x00,0x3f,0xff,
	0xfc,0x00,0x1f,0xff,
	0xf8,0x00,0x0f,0xff,
	0xf0,0x00,0x0f,0xff,
	0xf0,0x00,0x07,0xff,
	0xe0,0x00,0x07,0xff,
	0xe0,0x00,0x07,0xff,
	0xe0,0x00,0x07,0xff,
	0xe0,0x00,0x07,0xff,
	0xe0,0x00,0x03,0xff,
	0xe0,0x00,0x03,0xff,
	0xf0,0x00,0x03,0xff,
	0xf0,0x00,0x03,0xff,
	0xf8,0x00,0x07,0xff,
	0xf8,0x00,0x07,0xff,
	0xf8,0x00,0x03,0xff,
	0xf0,0x00,0x01,0xff,
	0xe0,0x00,0x01,0xff,
	0xe0,0x00,0x00,0xff,
	0xe0,0x00,0x07,0xff,
	0xe0,0x00,0x07,0xff,
	0xe3,0x00,0x07,0xff,
	0xf7,0x00,0x07,0xff,
	0xff,0x80,0x07,0xff,
	0xff,0xc0,0x07,0xff,
	0xff,0xe0,0x07,0xff,
	0xff,0xf0,0x00,0x7f,
	0xff,0xfc,0x00,0x3f,
	0xff,0xff,0x80,0x7f,
	0xff,0xff,0xc7,0xff,
	0xff,0xff,0xe7,0xff,
};


static char xormask[] = {
	0x00,0x00,0x00,0x00,
	0x00,0x7f,0x00,0x00,
	0x01,0xff,0xc0,0x00,
	0x03,0xff,0xe0,0x00,
	0x07,0xff,0x60,0x00,
	0x07,0xef,0x90,0x00,
	0x0f,0x9f,0xf0,0x00,
	0x0f,0xff,0x10,0x00,
	0x0f,0x8f,0x50,0x00,
	0x0f,0xaf,0x18,0x00,
	0x0f,0x8f,0xf8,0x00,
	0x0f,0xff,0x78,0x00,
	0x07,0xfe,0x18,0x00,
	0x07,0xf8,0xf8,0x00,
	0x03,0xff,0xf0,0x00,
	0x03,0x9f,0xc0,0x00,
	0x03,0xe0,0x38,0x00,
	0x07,0xff,0xfc,0x00,
	0x0f,0xff,0xec,0x00,
	0x0f,0xff,0xf6,0x00,
	0x0f,0xff,0xf0,0x00,
	0x0e,0xff,0xf0,0x00,
	0x08,0x7f,0xf0,0x00,
	0x00,0x7f,0xf0,0x00,
	0x00,0x3f,0xf0,0x00,
	0x00,0x1f,0xf0,0x00,
	0x00,0x0f,0xf0,0x00,
	0x00,0x07,0xf8,0x00,
	0x00,0x00,0xff,0x80,
	0x00,0x00,0x38,0x00,
	0x00,0x00,0x10,0x00,
	0x00,0x00,0x00,0x00,
};



/********************************
	タイルパターンのデータ
*********************************/
static char tile[]={
		0x00, 0x00, 0x66, 0x00, 0x00, 0x42, 0x3c, 0x00, 
		0x00, 0x00, 0x66, 0x00, 0x00, 0x42, 0x3c, 0x00, 
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
		0x00, 0x00, 0x66, 0x00, 0x00, 0x42, 0x3c, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};
/*********************************
	main()
**********************************/
void main()
{
	if ( GAinit( HRESO_256C ) ) {  /* 1024x768   256Color */
		printf("\nGAINIT.EXEが常駐していません\n");
		exit(1);
	}
	GAclrScreen();
	GAcrtSel(CRT_GA);    /* CRT GA-1024Aに切換 */
	VWait(50);

	line_box(200,200,824,568,-1);
	zukei();
	painttest();
	scroll();
	moveball();		/*  この関数はカラーパレットを変更する  */
	save_restore();
	gradation();	/*  この関数はカラーパレットを変更する  */
	gacursor();

	GAcrtSel(CRT_PC);    /* CRT 本体に切換 */
	GAterm( PAL_DEF );		/*	終了関数	*/
//	exit(0);
}


/**********************************************************
	立体球を描画　
***********************************************************/
void ball( x, y, r)
int		x;
int		y;
int		r;		/*  1 - 256  */
{
	int		i, j, c;
	double  z,rr,zz,xx;

	z = 255.0 / r;
	rr = (double)r * (double)r;
	for (i = 0; i < r; i++) {
		xx = r - i;
		zz = sqrt( rr - xx * xx ) ;
		c = zz * z;
		GAcircleFill(x, y, r-i, c);
	}
}



/*******************************************************
	GAcopyImage でボールで動かす
********************************************************/
void moveball()
{
	int		x, y, c;
	int		i, j;
	struct	PALETTE pal;


	x = 0;
	y = 384;


	for (i = 0; i < 256; i++){
		c = i >> 4;
		pal.red   = c*c;
		pal.green = i;
		pal.blue  = i;
		GAsetPalette((void far *)&pal, i);
	}
	GAclrScreen();
	GAputStr( 450,0,"コピ−",255,TRANSPARENT,BOLD);
	ball(x+32,y+32,32);
	for(j = 0; j < 1; j++){
		for(i = 0; i < 1023-63-16; i+=16){
			GAcopyImage(x, y, x+63+16, y+63, i, y);
			x = i;
		}
		for(i = 1023-63-16; i > 0 ; i-=16){
			GAcopyImage(x, y, x+63+16, y+63, i, y);
			x = i;
		}
	}
	for(j = 0; j < 2; j++){
		for(i = 0; i < 1023-63-16; i+=16){
			GAcopyImage(x, y, x+63+16, y+63, i, y);
			x = i;
		}
		for(i = 1023-63-16; i > 0 ; i-=16){
			GAcopyImage(x, y, x+63+16, y+63, i, y);
			x = i;
		}
	}
	x = 512;
	y = 0;
	ball(x+32,y+32,32);
	for(j = 0; j < 3; j++){
		for(i = 0; i < 766-64-16; i+=16){
			GAcopyImage(x, y, x+63, y+63+16, x, i);
			y = i;
		}
		for(i = 766-64-16; i > 0 ; i-=16){
			GAcopyImage(x, y, x+63, y+63+16, x, i);
			y = i;
		}
	}
	VWait(100);
}



/******************************************************
	すべての方向にライン描画
*******************************************************/
void line_box(x0,y0,x1,y1, style)
int		x0,y0,x1,y1;
short	style;
{
	int		i;
	int		x,y;

	x = (x1 - x0) / 2 + x0;
	y = (y1 - y0) / 2 + y0;

	GAputStr( 450,0,"ライン描画",7,TRANSPARENT,BOLD );
	for(i = x0; i <= x1; i++){
		GAline(x, y, i, y0, 1, style);
	}
	for(i = y0; i <= y1; i++){
		GAline(x, y, x1, i, 2, style);
	}
	for(i = x1; i >= x0; i--){
		GAline(x, y, i, y1, 3, style);
	}
	for(i = y1; i >= y0; i--){
		GAline(x, y, x0, i, 4, style);
	}
	VWait(100);
}



/***********************************************************
	円、三角、四角 
************************************************************/
void zukei()
{
	int		i;
	int		sx,sy;
	long	size;

	GAclrScreen();
	GAputStr( 450,0,"円、三角、四角",7,TRANSPARENT,BOLD );

	GAsetTile((void far *)tile);

	GAcircle(256,172,100, 1, 0xfccc);
	GAcircleFill(256,384,100, 2);
	GAtiledCircle(256,596,100);

	GAtriangle(512,72,412,272,612,272, 3, 0x5555);
	GAtriangleFill(512,284,412,484,612,484, 4);
	GAtiledTriangle(512,496,412,696,612,696);

	GAbox(668,72,868,272, 5, 0x0f0f);
	GAboxFill(668,284,868,484, 6);
	GAtiledBox(668,496,868,696);
	VWait(100);
}


/***************************************************
	パレットを変えて 256色グラデーションを表示
****************************************************/
void gradation()
{

	struct	PALETTE pal;

	int i;
	int r,g,b,hol,ver;

	hol = 1024/16;
	ver = 768/16;
	GAclrScreen();

	GAsetVisualPlane(-1);
	for (i=0;i<256;i++) {
			GAboxFill( (i&15)*hol,(i/16)*ver,(i&15)*hol+(hol-2),(i/16)*ver+(ver-2),i);
	}
	GAputStr( 450,0,"グラデ−ション",255,TRANSPARENT,BOLD );
	for (i=0;i<256;i++) {
		r = (15 - (i&15) - (i/16))*16 ;
		if (r < 0) r = -r;
		g = (i / 16) * 16;
		b = (i & 15) * 16;
		pal.red   = r;
		pal.green = g;
		pal.blue  = b;
		GAsetPalette((void far *)&pal, i);
	}
	GAsetVisualPlane(0);
	VWait(100);


	GAsetVisualPlane(-1);
	for (i=0;i<256;i++) {
		r = ((i&15) + (i/16)- 15)*16 ;
		if (r < 0) r = 0;
		b = ((i&15) - (i/16))*16 ;
		if (b < 0) b = 0;
		g = ((i/16)-(i&15))*16 ;
		if (g < 0) g = 0;
		pal.red   = r;
		pal.green = g;
		pal.blue  = b;
		GAsetPalette((void far *)&pal, i);
	}
	GAsetVisualPlane(0);
	VWait(100);
}

/************************************************************
	文字 スクロール
*************************************************************/
static char moji[]={ " ＧＡ−１０２４Ａ　（株）アイ・オー・データ機器　Device Inc...  "};

scroll()
{
	int		dy;
	int		x,y;

	GAinit(1);
	GAclrScreen();
	GAputStr( 450,0,"文字 スクロール",7,TRANSPARENT,BOLD );

	GAbox(XO-1,YO-1,XO+504,YO+514,7,-1);
	GAsetClipRgn(XO,YO,XO+503,YO+511);

	dy = 2;
	for(y = 140; y; y-=dy){ 
		GAcopyImage(XO, YO+dy, XO+511, YO+513, XO, YO);
		GAputStr( XO, YO+y%20+495, moji, 7-(y/20), y > 60 ? 7-(y/20)+1 : -1 , (y/20)%4);
	}
	for(y = 560; y; y-=dy){
		GAcopyImage(XO, YO+dy, XO+511, YO+513, XO, YO);
	}
	GAsetClipRgn(-1,0,0,0);
}



/***************************************************************
	セーブ、リストア
*****************************************************************/
void save_restore()
{
	int		x,y;
	int		mode;
	long	size;
#ifndef TURBO
	void huge	*savep;
#else
	void far	*savep;
#endif


/*	セーブ リストア 	モード０  */
	GAclrScreen();
	GAputStr( 400,0,"セーブ リストア モード０ (プレ-ン)",255,TRANSPARENT,BOLD);

	ball(32,32,32);
	size = GAsaveImage(0,0,63,63,PLANE_MODE,0L);
#ifndef TURBO
	savep = halloc(size,1);
#else
	savep = farmalloc(size);
#endif
	GAsaveImage(0,0,63,63,PLANE_MODE,savep);
	GAsetClipRgn(80,83,1001,602);
	for(y = 0; y < 768; y+=64){
		for(x = 0; x < 1024; x+=64){
			GArestoreImage(x,y,x+63,y+63,PLANE_MODE,savep);
		}
	}
#ifndef TURBO
	hfree(savep);
#else
	farfree(savep);
#endif
	VWait(100);
	GAsetClipRgn(-1,0,0,0);


/*	セーブ リストア 	モード１  */
	GAclrScreen();
	GAputStr( 400,0,"セーブ リストア モード１ (ピクセル)",255,TRANSPARENT,BOLD);

	ball(32,32,32);
	size = GAsaveImage(0,0,63,63,PIXEL_MODE,0L);
#ifndef TURBO
	savep = halloc(size,1);
#else
	savep = farmalloc(size);
#endif
	GAsaveImage(0,0,63,63,PIXEL_MODE,savep);
	GAsetClipRgn(80,83,1001,602);
	for(y = 0; y < 768; y+=64){
		for(x = 0; x < 1024; x+=64){
			GArestoreImage(x,y,x+63,y+63,PIXEL_MODE,savep);
		}
	}
#ifndef TURBO
	hfree(savep);
#else
	farfree(savep);
#endif
	VWait(100);
	GAsetClipRgn(-1,0,0,0);


/*	セーブ リストア 	モード２  */
	GAclrScreen();
	GAputStr( 400,0,"セーブ リストア モード２ (プッシュ ポップ)",255,TRANSPARENT,BOLD);

	ball(32,32,32);
	GAsetClipRgn(80,83,1001,602);
	for(y = 0; y < 768; y+=64){
		for(x = 0; x < 1024; x+=64){
			GAsaveImage(0,0,63,63,PUSH_POP,0L);
			GArestoreImage(x,y,x+63,y+63,PUSH_POP,0L);
		}
	}
	VWait(100);
	GAsetClipRgn(-1,0,0,0);




/*	セーブ リストア    モード８  */
	GAclrScreen();
	GAputStr( 400,0,"セーブ リストア モード８ (バイト アライン)",255,TRANSPARENT,BOLD);
	ball(32,32,32);
	size = GAsaveImage(0,0,63,63,BYTE_ALIGN,0L);
#ifndef TURBO
	savep = halloc(size,1);
#else
	savep = farmalloc(size);
#endif
	GAsaveImage(0,0,63,63,BYTE_ALIGN,savep);
	GAsetClipRgn(80,83,1001,602);
	for(y = 0; y < 768; y+=64){
		for(x = 0; x < 1024; x+=64){
			GArestoreImage(x,y,x+63,y+63,BYTE_ALIGN,savep);
		}
	}
#ifndef TURBO
	hfree(savep);
#else
	farfree(savep);
#endif
	GAsetClipRgn(-1,0,0,0);
	VWait(100);
}

/******************************************************
	カーソル表示
********************************************************/
void gacursor()
{
	int 	i,x,y;
	struct	PALETTE pal;
	int     wx=0,wy=0,left,right=0;

	pal.red   = 255;
	pal.green = 255;
	pal.blue  = 255;
	GAsetPalette((void far *)&pal, 255);

	GAclipControl( 0,0,0,0,0,0);
	GAputStr( 440,0,"カ ー ソ ル 表 示",255, 0,BOLD);
	GAsetCursor((char far *)andmask,(char far *)xormask);
	if (InitMouse()){ /* MOUSE.SYSが登録されていない場合 */
		for(i = 0; i < 500; i+=8) {
			GAdrawCursor(i,i);
			VWait(5);
		}
		VWait(50);
	} else { /* MOUSE.SYSを使用 */
		GAputStr( 440,30,"右クリックで終了します",255, 0, 0);
		while(!right){
			GetMouse(&x, &y, &left, &right);
			if (wx != x || wy != y){
				GAdrawCursor(x,y);
			}
			wx = x;
			wy = y;
		}
	}
	GAeraseCursor();
}

/**************************************************
  ペイント
****************************************************/
void painttest()
{
	int		i;
	int		sx,sy;
	long	size;

	GAclrScreen();
	GAputStr( 450,0,"ペイント",7,TRANSPARENT,BOLD);
	GAsetTile((void far *)tile);

	GAcircle(256,272,200, 3, 0xffff);
	GAtriangle(512,72,412,272,612,272, 3, 0xffff);
	GAbox(368,572,868,760, 5, 0xffff);
	VWait(20);
	GApaint(256,272,3,6);
	GAtiledPaint(512,172,3);
	GApaint(700,600,5,1);
	VWait(100);


}


#if 0
/********************************************************************

	グラフィックメモリに直接描画する場合はこのソ−スを参考にして下さい
	
      TURBO C の場合は outp()を outportb()に、outpw()を outportw()に
      変更してください
*********************************************************************/

/* グラフィック コントロ-ル レジスタ ポ-ト */
#define SRW_    0x0100
#define SRR_    0x0200
#define WPM     0x0300
#define WBM_    0x0500
#define RPS     0x0600
#define RPE     0x0700
#define COL     0x0900
#define TILE_   0x0b00
#define MOD1    0x0e00
#define MOD2    0x0e01
#define CRTC_AR 0x1e00
#define CRTC_CR 0x1f00

direct()
{
	unsigned int  gaport;
	volatile char far *ga_wind; /* グラフィックRAMのポインタは volatileを付ける */

	struct VDCONFIG vd;
	int    x,y;
	int    wpm_save;
	char   a,b;

	GAgetVideoConfig((void far *)&vd);
	gaport = vd.port	;                           /* GA レジスタのポート番号 */
	ga_wind = (void far *) ((long)vd.ga_seg	<< 16); /*グラフィックボードウィンドウのセグメント*/

	x = 100;
	y = 512;
	a = 0xfc;
	wpm_save = inp(WPM+gaport);  /* ライト プレ-ン マスク 保存   */
	outp(MOD1+gaport,0);         /* グラフィック コントロ-ラのモ-ド */
	outpw(WBM_+gaport,0x3f3f);   /* ライト ビットマスク           */
	outpw(SRW_+gaport,y );       /* ライト スタ-トラスタ (Y)      */
	outp(WPM+gaport,7);          /* ライト プレ-ン マスク        */
	*(ga_wind + (x >> 3)) = a;   /* グラフィックメモリにライトアクセス   */

	outpw(SRR_+gaport,y);        /* リ-ド スタ-トラスタ */
	outp(RPS+gaport,2);          /* リ-ド プレ-ン   */
	b = *(ga_wind + (x >> 3));   /* グラフィックメモリにリ-ドアクセス   */

/* MOD1 WBM WPM をデフォルトにもどす */
	outp(MOD1+gaport,1);          /* グラフィック コントロ-ラ モ-ド 1 */
	outpw(WBM_+gaport,0xffff) ;   /* ライト ビットマスク           */
	outp(WPM+gaport,wpm_save);    /* ライト プレ-ン マスク         */
}

/*****************************************
	ピクセル セットをｃ言語で行なう場合
*****************************************/
void setPixel(x1,y1,color)     
int x1,y1,color;
{
 static unsigned char smask[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	int j,bx;
	volatile uchar far *p;

	outp(gaport+MOD1,1) ;
	outpw(gaport+SRW_,y1);
	outp(gaport+WPM,-1);
	outp(gaport+COL,color);
	j =  x1 & 7 ;
	bx = x1 >> 3;
	p = ga_wind + bx ;
	*p = smask[j];
}


#endif

#if 0
/****************************************************************************

	マウスカ−ソルを割り込みで処理する場合は以下のソ−スを参考にしてください

  MS-DOS添付のMOUSE.SYSが組み込まれていることが必要です

  CallMouse() を呼び出せばマウスカ−ソルを割り込みで処理されます
  ReleaseMouse()で割り込みを解除する

*****************************************************************************/

/*マウスを移動するとこの関数が呼ばれる*/
void far intMouse()
{
	struct	HWREG hw;
	int	x,y,left,right;

	GAsaveHwRegister(&hw);
	GetMouse(&x,&y,&left,&right);
	if (disp_flag) GAdrawCursor(x,y);
	GArestoreHwRegister(&hw);
}


/*  アセンブラ記述部 */

;  割り込みの設定
_CallMouse	proc	far
		mov 	ax,cs
		mov 	es,ax
		mov 	dx,offset mouse_move  ;下の関数
		mov 	ax,0ch		; 
		mov 	cx,1	    ; コール条件
		int 	33h
		ret
_CallMouse	endp

mouse_move		proc	far
		push	ds
		mov 	ax,DGROUP
		mov 	ds,ax
		call	far ptr _intMouse   ; ｃの関数
		pop 	ds
		ret
mouse_move		endp

_ReleaseMouse	proc	far
		mov 	cx,0	;  コール条件
		mov 	ax,0ch
		int 	33h
		ret
_ReleaseMouse	endp

#endif
