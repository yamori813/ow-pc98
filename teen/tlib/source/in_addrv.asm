;
;	INET ４バイトIPv4アドレス変換
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	inet_toipv4addr_,inet_ipv4addrtoipaddr_


;char inet_toipv4addr(const void far *addr, IDWORD far *ipv4addr);
;
;　入力：
;　出力：
;　破壊：
inet_toipv4addr_	PROC
;	MOV	BX,SP

	PUSH	BX
	PUSH	CX
	PUSH	DI
	PUSH	ES

;	LES	DI,[BX+2]	;addr
	MOV	DI,AX
	MOV	ES,DX
	$TEEN	22h

	POP	ES
	POP	DI
	POP	CX
	POP	BX

	JC	Error

;	LES	BX,[BX+6]	;ipv4addr
;	MOV	ES:[BX+0],AX
;	MOV	ES:[BX+2],DX
	MOV	ES,CX
	MOV	ES:[BX+0],AX
	MOV	ES:[BX+2],DX
	
	MOV	AL,0
	RET

Error:
	MOV	AL,-1
	RET

inet_toipv4addr_	ENDP


;void inet_ipv4addrtoipaddr(IDWORD ipv4addr, IPADDR far *ipaddr);
;
;　入力：
;　出力：
;　破壊：
inet_ipv4addrtoipaddr_	PROC
;	MOV	BX,SP
	PUSH	DI
	PUSH	ES

;	LES	DI,[BX+6]	;ipaddr
;	MOV	DX,[BX+4]	;ipv4addr 上位
;	MOV	BX,[BX+2]	;ipv4addr 下位
;	MOV	DI,CX
;	MOV	DX,AX
	MOV	DI,BX
	MOV	ES,CX
	MOV	BX,AX
	$TEEN	23h

	POP	ES
	POP	DI
	RET
inet_ipv4addrtoipaddr_	ENDP


end
