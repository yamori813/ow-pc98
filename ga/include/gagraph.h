/**********************************************************
   GAGRAPH.H

    GA-1024A / GA-1280A グラフィックライブラリ

	              1993/03/31	I・O DATA DEVICE Inc.
**********************************************************/

struct HWREG {
	short	wbm;
	short	srw;
	short	srr;
	char	mode;
	short	wpm;
	short	rps;
	short	rpe;
	short	color;
	short	dac_msk;
};


struct VDCONFIG {
	int	scr_width;
	int	scr_height;
	int	plane;
	int	palette;
	int	font_width;
	int	font_height;
	int	internal;
	int	port;
	int	ga_seg;
	int	port2;
	int	gainit_int;
	int	ga_vram;
	char	rfu[54];
};



struct PALETTE {
	char red ;
	char green;
	char blue;
};


#ifdef __cplusplus
extern "C" {
#endif

extern	void	cdecl	far 	GAsetPixel( int, int, int );
extern	int 	cdecl	far 	GAgetPixel( int, int );
extern	void	cdecl	far 	GAclrScreen( void );
extern	void	cdecl	far 	GAcrtSel( int );
extern	void	cdecl	far 	GAsetClipRgn( int, int, int, int );
extern	void	cdecl	far 	GAgetClipRgn( int far *,int far *,int far *,int far * );
extern	void	cdecl	far 	GAsetPalette( struct PALETTE far *, int );
extern	void	cdecl	far 	GAgetPalette( struct PALETTE far *, int );
extern	void	cdecl	far 	GAsetVisualPlane( int );
extern	int 	cdecl	far 	GAgetVisualPlane( void );
extern	void	cdecl	far 	GAsetActivePlane( int );
extern	int 	cdecl	far 	GAgetActivePlane( void );
extern	void	cdecl	far 	GAgetVideoConfig( struct VDCONFIG far * );
extern	int 	cdecl	far 	GAsaveHwRegister( struct HWREG far * );
extern	void	cdecl	far 	GArestoreHwRegister( struct HWREG far * );
extern	int 	cdecl	far 	GAinit( int );
extern	void	cdecl	far 	GAline( int, int, int, int, int, short );
extern	void	cdecl	far 	GAsetTile( char far * );
extern	void	cdecl	far 	GAcircle( int, int, int, int, short );
extern	void	cdecl	far 	GAcircleFill( int, int, int, int );
extern	void	cdecl	far 	GAtiledCircle( int, int, int );
extern	void	cdecl	far 	GAtriangle( int, int, int, int, int, int, int, short );
extern	void	cdecl	far 	GAtriangleFill( int, int, int, int, int, int, int );
extern	void	cdecl	far 	GAtiledTriangle( int, int, int, int, int, int );
extern	void	cdecl	far 	GAbox( int, int, int, int, int, short );
extern	void	cdecl	far 	GAboxFill( int, int, int, int, int );
extern	void	cdecl	far 	GAtiledBox( int, int, int, int );
extern	void	cdecl	far 	GAtiledLine( int, int, int, int );
extern	void	cdecl	far 	GAcopyImage( int, int, int, int, int, int );
extern	long	cdecl	far 	GAsaveImage( int, int, int, int, int, void far * );
extern	void	cdecl	far 	GArestoreImage( int, int, int, int, int, void far * );
extern	void	cdecl	far 	GAputChar( int, int, char, int, int, short );
extern	void	cdecl	far 	GAputStr( int, int, char far *, int, int, short );
extern	void	cdecl	far 	GAsetCursor( char far *, char far * );
extern	void	cdecl	far 	GAdrawCursor( int ,int );
extern	void	cdecl	far 	GAeraseCursor( void );
extern	void	cdecl	far 	GApaint( int, int, int, int );
extern	void	cdecl	far 	GAtiledPaint( int, int, int );
extern	void	cdecl	far 	GAclipControl( int, int, int, int, int, int );
extern	void	cdecl	far 	GAropLine( int, int, int, int, int, short, int, int ,int );
extern	void	cdecl	far 	GAfepSet( int, char far * );
extern	void	cdecl	far 	GAfepReset();
extern	void	cdecl	far 	GAterm( int );


#ifdef __cplusplus
}
#endif


/* GAinit() 解像度 色数 */
#define DEFAULT
#define HRESO_256C  	1	/*	1024*768	256色	*/
#define LRESO_256C  	2	/*	 640*480	256色	*/
#define HRESO_16C   	3	/*	1024*768	 16色	*/
#define LRESO_16C   	4	/*	 640*480	 16色	*/
#define HRESO_4C    	5	/*	1024*768	  4色	*/
#define LRESO_4C    	6	/*	 640*480	  4色	*/
#define HRESO_2C    	7	/*	1024*768	 モノクロ	*/
#define LRESO_2C    	8	/*	 640*768	 モノクロ	*/
#define MRESO_256C  	9	/*	 800*600	256色	*/
#define MRESO_16C   	10	/*	 800*600	 16色	*/
#define MRESO_4C    	11	/*	 800*600	 4色	*/
#define MRESO_2C    	12	/*	 800*600	 モノクロ	*/
#define HRESO_65536C	13	/*	1024*768	65536色	*/
#define MRESO_65536C	14	/*	 800*600	65536色	*/
#define LRESO_65536C	15	/*	 640*480	65536色	*/
#define SHRESO960_256C	16	/*	1280*960	256色	*/
#define SHRESO960_16C	17	/*	1280*960	 16色	*/
#define SHRESO960_4C	18	/*	1280*960	  4色	*/
#define SHRESO960_2C	19	/*	1280*960	 モノクロ	*/
#define SHRESO1024_256C	20	/*	1280*1024	256色	*/
#define SHRESO1024_16C	21	/*	1280*1024	 16色	*/
#define SHRESO1024_4C	22	/*	1280*1024	  4色	*/
#define SHRESO1024_2C	23	/*	1280*1024	 モノクロ	*/



/*  GAcrtSel()  CRT 切り替え　*/
#define CRT_PC   0
#define CRT_GA   1

/* Line style */
#define SOLID_LINE      0xffff
#define DASH_LINE       0xe7e7
#define DOT_LINE        0xaaaa
#define DASHDOT_LINE    0xe427
#define DASHDOTDOT_LINE 0xea57

/* GAsaveImage() GArestoreImage() モ−ド */
#define PLANE_MODE  0
#define PIXEL_MODE  1
#define PUSH_POP    2
#define BYTE_ALIGN  8

/*	XOR TYPE	8 or 16 plane mode */
#define XOR_WRITE8   0xffff
#define XOR_WRITE16  0xffdf

/*   文字種別 */
#define BOLD        1
#define ITALIC      2
#define TRANSPARENT -1

/*	クリッピング	*/
#define CLIP_OFF	0
#define CLIP_ON		1
#define CLIP_IN		0
#define CLIP_OUT	1

/*	パレット初期化フラグ	*/
#define PAL_DEF		0	/* パレットを初期状態に戻す */
#define PAL_USER	1	/* パレットを初期状態に戻さない */
