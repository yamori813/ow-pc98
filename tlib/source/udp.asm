;
;	UDP
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL


			;sendto,recvfrom パラメータテーブル
BufAddrTable	STRUC
BAT_Buf		DD ?	;バッファへのポインタ
BAT_Len		DW ?	;バッファのサイズ
BAT_Port	DW ?	;ポート番号
BAT_Addr	DD ?	;IPアドレス構造体へのポインタ
BufAddrTable	ENDS



	.CODE

	PUBLIC	udp_open_,udp_close_
	PUBLIC	udp_send_,udp_recv_,udp_state_
	PUBLIC	udp_sendto_,udp_recvfrom_
	PUBLIC	udp_getmyaddr_,udp_getpeeraddr_

;char udp_open(const UDPPRM far *udpprm);
;
;　入力：
;　出力：AL=ハンドル,-1:エラー
;　破壊：
udp_open_	PROC

	MOV	BX,SP
	PUSH	DI

	LES	DI,[BX+2]
	MOV	AL,0
	$TEEN	50h

	POP	DI
	JC	Error
	RET

udp_open_	ENDP




;エラーがあった場合に呼び出す
;  入力：AL=エラー番号
;  出力：AX=-1

Error	PROC

	MOV	BYTE PTR udperrno_,AL
	MOV	AX,-1
	RET

Error	ENDP


;char udp_close(char handle);
;
;　入力：
;　出力：AL=0:正常,-1:エラー
;　破壊：
udp_close_	PROC

	MOV	BX,SP
	MOV	AL,[BX+2]
	$TEEN	51h
	JC	Error
	RET

udp_close_	ENDP

;int udp_send(char handle, const char far *buf, int len);
;
;　入力：
;　出力：AX=送信バイト数(<32768),-1:エラー
;　破壊：
udp_send_	PROC

	MOV	BX,SP
	PUSH	DI
	
	MOV	AL,[BX+2]
	LES	DI,[BX+4]
	MOV	CX,[BX+8]
	$TEEN	54h
	
	POP	DI
	JC	Error
	RET

udp_send_	ENDP

;int udp_recv(char handle, char far *buf, int len);
;
;　入力：
;　出力：AX=受信バイト数(<32768),-1:エラー
;　破壊：
udp_recv_	PROC

	MOV	BX,SP
	PUSH	DI

	MOV	AL,[BX+2]
	LES	DI,[BX+4]
	MOV	CX,[BX+8]
	$TEEN	55h

	POP	DI
	JC	Error
	RET

udp_recv_	ENDP


;int  udp_sendto(char handle, const BYTE far *buf, int len,
;					  const void far *toaddr, WORD toport);
;　入力：
;　出力：AX=送信バイト数(<32768),-1:エラー
;　破壊：
udp_sendto_	PROC

IF 1
	MOV	BX,SP
	PUSH	[BX+12]	;toaddr SEG
	PUSH	[BX+10]	;toaddr OFS
	PUSH	[BX+14]	;port
	PUSH	[BX+ 8]	;len
	PUSH	[BX+ 6]	;buf SEG
	PUSH	[BX+ 4] ;buf OFS
	MOV	AX,SP
	PUSH	DI

	MOV	DI,AX
	PUSH	DS
	POP	ES

	MOV	AL,[BX+2]	;handle

ELSE
	MOV	BX,SP
	SUB	SP,TYPE BufAddrTable
	MOV	AX,SP
	PUSH	DI

	MOV	DI,AX

	LES	AX,[BX+4]	;buf
	MOV	WORD PTR [DI].BAT_Buf[0],AX
	MOV	WORD PTR [DI].BAT_Buf[2],ES
	MOV	AX,[BX+8]	;len
	MOV	[DI].BAT_Len,AX
	LES	AX,[BX+10]	;toaddr
	MOV	WORD PTR [DI].BAT_Addr[0],AX
	MOV	WORD PTR [DI].BAT_Addr[2],ES
	MOV	AX,[BX+14]	;toport
	MOV	[DI].BAT_Port,AX
	MOV	AL,[BX+2]	;handle

	PUSH	DS
	POP	ES
ENDIF

	PUSH	BX		;念のため
	$TEEN	56h
	POP	BX

	POP	DI
	MOV	SP,BX
	JC	Error
	RET

udp_sendto_	ENDP


;int udp_recvfrom(char handle, char far *buf, int len, 
;		                  IPADDR far *fromaddr, WORD far *fromport);
;　入力：
;　出力：AX=受信バイト数(<32768),-1:エラー
;　破壊：
udp_recvfrom_	PROC

IF 1
	MOV	BX,SP
	PUSH	[BX+12]	;addr SEG
	PUSH	[BX+10]	;addr OFS
	PUSH	0	;Port
	PUSH	[BX+ 8]	;len
	PUSH	[BX+ 6]	;buf SEG
	PUSH	[BX+ 4] ;buf OFS
	MOV	AX,SP
	PUSH	DI

	MOV	DI,AX
	PUSH	DS
	POP	ES

	MOV	AL,[BX+2]	;handle

ELSE

	MOV	BX,SP
	SUB	SP,TYPE BufAddrTable
	MOV	AX,SP
	PUSH	DI

	MOV	DI,AX

	LES	AX,[BX+4]	;buf
	MOV	WORD PTR [DI].BAT_Buf[0],AX
	MOV	WORD PTR [DI].BAT_Buf[2],ES
	MOV	AX,[BX+8]	;len
	MOV	[DI].BAT_Len,AX
	LES	AX,[BX+10]	;toaddr
	MOV	WORD PTR [DI].BAT_Addr[0],AX
	MOV	WORD PTR [DI].BAT_Addr[2],ES
	MOV	AL,[BX+2]	;handle

	PUSH	DS
	POP	ES
ENDIF

	PUSH	BX		;念のため
	$TEEN	57h
	POP	BX

	JC	UDP_RecvFrom_1
		MOV	DX,[DI].BAT_Port
		LES	DI,[BX+14]	;port
		MOV	ES:[DI],DX
UDP_RecvFrom_1:

	POP	DI
	MOV	SP,BX
	JC	Error
	RET

udp_recvfrom_	ENDP


;char udp_state(char handle);
;
;　入力：
;　出力：AL=ステータス,-1:エラー
;　破壊：
udp_state_	PROC

	MOV	BX,SP
	MOV	AL,[BX+2]
	$TEEN	52h
	JC	Error
	RET

udp_state_	ENDP


;char udp_getmyaddr(char handle, IPADDR far *addr, WORD far *port);
;
;　入力：
;　出力：AL=0:正常,-1:エラー
;　破壊：
udp_getmyaddr_	PROC

	MOV	AH,5Ah

UDP_GetAddr:
	MOV	BX,SP
	PUSH	DI

	MOV	AL,[BX+2]	;handle
	LES	DI,[BX+4]	;addr

	$TEEN	;AH

	POP	DI
	JC	Error

	LES	BX,[BX+8]	;port
	MOV	ES:[BX],AX

	XOR	AX,AX
	RET
udp_getmyaddr_	ENDP


;char udp_getpeeraddr(char handle, IPADDR far *addr, WORD far *port);
;
;　入力：
;　出力：AL=0:正常,-1:エラー
;　破壊：
udp_getpeeraddr_	PROC
	MOV	AH,5Bh
	JMP	UDP_GetAddr
udp_getpeeraddr_	ENDP





	.DATA?

	PUBLIC	udperrno_

udperrno_	DW ?			;エラー番号
end
