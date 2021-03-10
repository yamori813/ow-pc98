;
;	ICMP
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	icmp_send_,icmp_recv_

;;
;; v0.37a
;;

;TEEN API 64h/65h 呼び出し時に渡すパラメータ
ICMPPRM	STRUC
P_Src	DD ?
P_Dest	DD ?
P_TTL	DW ?
P_CodeType	DW ?
P_ID	DW ?
P_Seq	DW ?
P_Buf	DD ?
P_Len	DW ?
ICMPPRM	ENDS


;char icmp_send(const ICMPPRM far *icmpprm);
;
;　入力：
;　出力：char 0..正常 -1..エラー
;　破壊：
icmp_send_	PROC

	MOV	BX,SP
	PUSH	DI

	LES	DI,[BX+2]	;icmpprm

	$TEEN	64h

	POP	DI

	JNC	IS_Exit

	MOV	AL,-1

IS_Exit:
	RET

icmp_send_	ENDP


;int icmp_recv(ICMPPRM far *icmpprm);
;
;　入力：
;　出力：int >=0..正常 -1..エラー
;　破壊：
icmp_recv_	PROC

	MOV	BX,SP
	PUSH	DI

	LES	DI,[BX+2]	;icmpprm

	$TEEN	65h
	MOV	AX,[DI].P_Len	;受信バイト数

	POP	DI

	JNC	IR_Exit

	MOV	AX,-1

IR_Exit:
	RET

icmp_recv_	ENDP



IF 0
;;
;; v0.37
;;

;char icmp_send(const void far *dmy, const void far *dest, WORD ttl, WORD codetype, WORD id, WORD seq, const char far *buf, int len);
;　入力：
;　出力：char 0..正常 -1..エラー
;　破壊：
icmp_send_	PROC

	PUSH	DI

	PUSH	SS
	POP	ES
	MOV	DI,SP
	ADD	DI,2+2

	$TEEN	64h

	POP	DI

	JNC	PS_Exit
	MOV	AL,-1

PS_Exit:
	RET

icmp_send_	ENDP



;TEEN API 65h 呼び出し時に渡すパラメータ
ICMPPRM	STRUC
P_Src	DD ?
P_Dest	DD ?
P_TTL	DW ?
P_CodeType	DW ?
P_ID	DW ?
P_Seq	DW ?
P_Buf	DD ?
P_Len	DW ?
ICMPPRM	ENDS

;icmp_recv呼び出し元がスタックに積むパラメータ
PR_PRM	STRUC
PR_Src	DD ?
PR_Dest	DD ?
PR_TTL	DW ?
PR_CodeType	DW ?
PR_ID	DW ?
PR_Seq	DW ?
PR_Buf	DD ?
PR_Len	DW ?
PR_PRM	ENDS

;int  icmp_recv(IPADDR far *src, IPADDR far *dest, WORD *dmy, WORD *codetype, WORD *id, WORD *seq, char far *buf, int len);
;　入力：
;　出力：int >=0..正常 -1..エラー
;　破壊：
icmp_recv_	PROC

	MOV	BX,SP
	INC	BX
	INC	BX		;SS:BX=icmp_recvパラメータ(PR_PRM)先頭

	PUSH	DI

	SUB	SP,TYPE ICMPPRM
	MOV	DI,SP		;SS:DI=API 63hパラメータ(ICMPPRM)先頭

	LES	AX,[BX].PR_Src
	MOV	WORD PTR [DI].P_Src[0],AX
	MOV	WORD PTR [DI].P_Src[2],ES
	LES	AX,[BX].PR_Dest
	MOV	WORD PTR [DI].P_Dest[0],AX
	MOV	WORD PTR [DI].P_Dest[2],ES
	LES	AX,[BX].PR_Buf
	MOV	WORD PTR [DI].P_Buf[0],AX
	MOV	WORD PTR [DI].P_Buf[2],ES
	MOV	AX,[BX].PR_Len
	MOV	[DI].P_Len,AX

	MOV	AX,SS
	MOV	ES,AX

	$TEEN	65h

	JC	PR_Err

	PUSH	SI

IF 0
	MOV	SI,[BX].PR_TTL
	MOV	AX,[DI].P_TTL
	MOV	[SI],AX
ENDIF

	MOV	SI,[BX].PR_CodeType
	MOV	AX,[DI].P_CodeType
	MOV	[SI],AX

	MOV	SI,[BX].PR_ID
	MOV	AX,[DI].P_ID
	MOV	[SI],AX

	MOV	SI,[BX].PR_Seq
	MOV	AX,[DI].P_Seq
	MOV	[SI],AX

	POP	SI

	MOV	AX,[DI].P_Len

PR_Exit:
	ADD	SP,TYPE ICMPPRM
	POP	DI
	RET

PR_Err:
	MOV	AX,-1
	JMP	PR_Exit

icmp_recv_	ENDP

ENDIF


;2001/ 4/ 8 
;2001/ 4/10 
;2001/ 4/23 icmp_recv/send のパラメータを構造体で渡すよう仕様変更
end
