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
inet_toipv_4addr	PROC
	MOV	BX,SP

	PUSH	DI

	LES	DI,[BX+2]	;addr
	$TEEN	22h

	POP	DI

	JC	Error

	LES	BX,[BX+6]	;ipv4addr
	MOV	ES:[BX+0],AX
	MOV	ES:[BX+2],DX

	MOV	AL,0
	RET

Error:
	MOV	AL,-1
	RET

inet_toipv_4addr	ENDP


;void inet_ipv4addrtoipaddr(IDWORD ipv4addr, IPADDR far *ipaddr);
;
;　入力：
;　出力：
;　破壊：
inet_ipv_4addrtoipaddr	PROC
	MOV	BX,SP
	PUSH	DI

	LES	DI,[BX+6]	;ipaddr
	MOV	DX,[BX+4]	;ipv4addr 上位
	MOV	BX,[BX+2]	;ipv4addr 下位
	$TEEN	23h

	POP	DI
	RET
inet_ipv_4addrtoipaddr	ENDP


end
