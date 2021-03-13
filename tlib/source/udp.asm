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

;	MOV	BX,SP
	PUSH	DI
	PUSH	ES

;	LES	DI,[BX+2]
	MOV	DI,AX
	MOV	ES,DX
	MOV	AL,0
	$TEEN	50h

	POP	ES
	POP	DI
	JC	Error
	RET

udp_open_	ENDP




;エラーがあった場合に呼び出す
;  入力：AL=エラー番号
;  出力：AX=-1

Error	PROC

	MOV	BYTE PTR _udperrno,AL
	MOV	AX,-1
	RET

Error	ENDP

Error6	PROC

	MOV	BYTE PTR _udperrno,AL
	MOV	AX,-1
	RET	6

Error6	ENDP

Error4	PROC

	MOV	BYTE PTR _udperrno,AL
	MOV	AX,-1
	RET	4

Error4	ENDP

Error8	PROC

	MOV	BYTE PTR _udperrno,AL
	MOV	AX,-1
	RET	8

Error8	ENDP

;char udp_close(char handle);
;
;　入力：
;　出力：AL=0:正常,-1:エラー
;　破壊：
udp_close_	PROC

;	MOV	BX,SP
;	MOV	AL,[BX+2]
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

;	MOV	BX,SP
	PUSH	DI
	PUSH	ES
	
;	MOV	AL,[BX+2]
;	LES	DI,[BX+4]
;	MOV	CX,[BX+8]
	MOV	DI,BX
	MOV	ES,CX
	MOV	CX,DX
	$TEEN	54h
	
	POP	ES
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

;	MOV	BX,SP
	PUSH	DI
	PUSH	ES

;	MOV	AL,[BX+2]
;	LES	DI,[BX+4]
;	MOV	CX,[BX+8]
	MOV	DI,BX
	MOV	ES,CX
	MOV	CX,DX
	$TEEN	55h

	POP	ES
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

	MOV	TMP,BX
	MOV	BX,SP
	PUSH	ES
	PUSH	DI
	MOV	DI,OFFSET WBAT
;	MOV	WORD PTR [DI+0], BX
	MOV	WORD PTR [DI+2], CX
	MOV	WORD PTR [DI+4], DX
	MOV	CX,[BX+6]
	MOV	WORD PTR [DI+6], CX
	MOV	CX,[BX+2]
	MOV	WORD PTR [DI+8], CX
	MOV	CX,[BX+4]
	MOV	WORD PTR [DI+10], CX
	MOV	CX,TMP
	MOV	WORD PTR [DI+0], CX

	PUSH	DS
	POP	ES

;	MOV	AL,[BX+2]	;handle

	PUSH	BX		;念のため
	$TEEN	56h
	POP	BX

	POP	DI
	POP	ES
;	MOV	SP,BX
	JC	Error6
	RET	6

udp_sendto_	ENDP


;int udp_recvfrom(char handle, char far *buf, int len, 
;		                  IPADDR far *fromaddr, WORD far *fromport);
;　入力：
;　出力：AX=受信バイト数(<32768),-1:エラー
;　破壊：
udp_recvfrom_	PROC

	MOV	TMP,BX
	MOV	BX,SP
	PUSH	ES
	PUSH	DI
	MOV	DI,OFFSET WBAT
;	MOV	WORD PTR [DI+0], BX
	MOV	WORD PTR [DI+2], CX
	MOV	WORD PTR [DI+4], DX
	MOV	CX,0
	MOV	WORD PTR [DI+6], CX
	MOV	CX,[BX+2]
	MOV	WORD PTR [DI+8], CX
	MOV	CX,[BX+4]
	MOV	WORD PTR [DI+10], CX
	MOV	CX,TMP
	MOV	WORD PTR [DI+0], CX

	PUSH	DS
	POP	ES

	PUSH	BX		;念のため
	$TEEN	57h
	POP	BX

	JC	UDP_RecvFrom_1
	MOV	DX,[DI+6]
	LES	DI,[BX+8]	;port SEG
	MOV	ES,DI
	LES	DI,[BX+6]	;port OFS
	MOV	ES:[DI],DX
UDP_RecvFrom_1:

	POP	DI
	POP	ES
;	MOV	SP,BX
	JC	Error8
	RET	8

udp_recvfrom_	ENDP


;char udp_state(char handle);
;
;　入力：
;　出力：AL=ステータス,-1:エラー
;　破壊：
udp_state_	PROC

;	MOV	BX,SP
;	MOV	AL,[BX+2]
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
;	MOV	BX,SP
	PUSH	DI
	PUSH	ES

;	MOV	AL,[BX+2]	;handle
;	LES	DI,[BX+4]	;addr
	MOV	DI,BX
	MOV	ES,CX

	$TEEN	;AH

	POP	ES
	POP	DI
	JC	Error4

;	LES	BX,[BX+8]	;port
;	MOV	ES:[BX],AX
	MOV	BX,SP
	LES	CX,[BX+4]	;port seg
	MOV	ES,CX
	LES	BX,[BX+2]	;port off
	MOV	ES:[BX],AX
	XOR	AX,AX
	RET	4
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

	PUBLIC	_udperrno


_udperrno	DW ?			;エラー番号
WBAT		DW 6 DUP(10)
TMP		DW ?
end
