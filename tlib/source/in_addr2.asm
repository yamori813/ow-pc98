;
;	INET IPアドレス構造体-IPアドレス文字列変換
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	inet_tostr_,inet_toipaddr_


;char inet_toipaddr(const void far *addr, IPADDR far *ipaddr);
;
;　入力：
;　出力：
;　破壊：
inet_toipaddr_	PROC

	MOV	AH,20h
INET_To:
	MOV	BX,SP
	PUSH	SI
	PUSH	DI

	LES	SI,[BX+2]	;addr
	MOV	DX,ES
	LES	DI,[BX+6]	;ipaddr

	$TEEN	;AH

	POP	DI
	POP	SI
	JC	Error
	RET

Error:
	MOV	AL,-1
	RET

inet_toipaddr_	ENDP


;char inet_tostr(const void far *addr, IPADDRSTR far *str);
;
;　入力：
;　出力：
;　破壊：
inet_tostr_	PROC
	MOV	AH,21h
	JMP	INET_To
inet_tostr_	ENDP

end
