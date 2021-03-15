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

	PUSH	SI
	PUSH	DI
	PUSH	ES
	MOV	SI,AX
;	MOV	DX,DX
	MOV	DI,BX
	MOV	ES,CX
	MOV	AH,20h
INET_To:
;	POP	SI
;	MOV	BX,SP
;	PUSH	SI
;	PUSH	DI
;	PUSH	ES

;	LES	SI,[BX+2]	;addr
;	MOV	DX,ES
;	LES	DI,[BX+6]	;ipaddr
	MOV	DI,BX
	MOV	ES,CX

	$TEEN	;AH

	POP	ES
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
	PUSH	SI
	PUSH	DI
	PUSH	ES
	MOV	SI,AX
;	MOV	DX,DX
	MOV	DI,BX
	MOV	ES,CX
	MOV	AH,21h
	JMP	INET_To
inet_tostr_	ENDP

end
