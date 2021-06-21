;--------------------------------------------------------------
;
;	sub.asm
;
;--------------------------------------------------------------
;
;
DGROUP	GROUP	_DATA
	assume	CS:_TEXT, DS:DGROUP
;
_DATA	segment  word public 'DATA'
;
_DATA	ends
;
;	引数
X		equ	[bp + 6 + 0]
Y		equ	[bp + 6 + 4]
LEFT		equ	[bp + 6 + 8]
RIGHT		equ	[bp + 6 + 12]
;
;
_TEXT	segment	byte public 'CODE'
	assume	CS:_TEXT


public		_GetMouse
;----------------------------------------------------------------
;
;    マウス座標の収得
;
;	int	GetMouse(int *X, int *Y, int *LEFT, int *RIGHT)
;
;	entry
;		*X	水平座標を格納するアドレス
;		*Y 	垂直座標を格納するアドレス
;		*LEFT	左ボタンの状態を格納するアドレス
;		*RIGHT	右ボタンの状態を格納するアドレス
;
;	return
;		0	
;
;------------------------------------------------------------------
_GetMouse	proc	far
		push	bp
		mov	bp, sp
		push	es
		push	di

		mov	ax,03h		; カーソルの位置
		int	33h
		les	di,LEFT		; 左ボタンの状態
		mov	es:[di],ax
		les	di,RIGHT	; 右ボタンの状態
		mov	es:[di],bx
		les	di,X		; 水平位置
		mov	es:[di],cx
		les	di,Y		; 垂直位置
		mov	es:[di],dx
		xor	ax,ax

		pop	di
		pop	es
		pop	bp
		ret

_GetMouse	endp

;-------------------------------------------------------------
;  マウスの初期化
;
;  int InitMouse()
;
; リタ−ン  0: 正常   -1:異常 
;-------------------------------------------------------------
public		_InitMouse

_InitMouse	proc	far

		mov	ax,0		; 環境チェック
		int	33h
		cmp	ax,-1
		je	usems ;@f
		mov	ax,-1
		jmp	endmouse

usems:		mov	ax,10h		; 移動範囲設定（水平）
		mov	cx,0		; min:0
		mov	dx,1024-1	; max:1023(0x3ff)
		int	33h

		mov	ax,11h		; 移動範囲設定（垂直）
		mov	cx,0		; min:0
		mov	dx,768-1	; max:767(0x2ff)
		int	33h
		xor	ax,ax
endmouse:
		ret

_InitMouse	endp



;------------------------------------------------------------
;
;  垂直同期信号をカウントして時間待ちをする
;          20mSec x 引数
;
;   void VWait(int  time)
;
;------------------------------------------------------------
public		_VWait

	assume	CS:_TEXT, DS:DGROUP
;
;
;	引数
COUNT		equ	[bp + 6]
;
_VWait	proc	far
		push	bp
		mov	bp, sp
		push	es
		push	ds
		mov	ax,DGROUP
		mov	ds,ax

		mov	cx,COUNT
vwait_10:
lop1:		in	al, 60h
		jmp	short $+2
		jmp	short $+2
		jmp	short $+2
		test	al, 20h
		jz	lop1
lop2:		in	al, 60h
		jmp	short $+2
		jmp	short $+2
		jmp	short $+2
		test	al, 20h
		jnz	lop2
		loop	vwait_10
		xor	ax, ax

		pop	ds
		pop	es
		pop	bp
		ret
_VWait	endp

_TEXT		ends
		end

