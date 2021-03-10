;
;	TCP
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	tcp_activeopen_,tcp_passiveopen_,tcp_accept_
	PUBLIC	tcp_shutdown_,tcp_close_,tcp_send_,tcp_recv_,tcp_state_
	PUBLIC	tcp_downstate_,tcp_getaddr_


;char tcp_activeopen(TCPPRM far *tcpprm);
;
;　入力：
;　出力：AL=ハンドル,-1:エラー
;　破壊：
tcp_activeopen_	PROC

	MOV	BX,SP
	PUSH	DI

	LES	DI,[BX+2]
	$TEEN	40h

	POP	DI
	JC	Error
	RET

tcp_activeopen_	ENDP


;char tcp_passiveopen(WORD thisport, void (huge *notice)());
;
;　入力：
;　出力：AL=ハンドル,-1:エラー
;　破壊：
tcp_passiveopen_	PROC

	MOV	BX,SP
	PUSH	DI
	
	MOV	DX,[BX+2]
	LES	DI,[BX+4]
	$TEEN	41h
	
	POP	DI
	JC	Error
	RET

tcp_passiveopen_	ENDP


;char tcp_accept(char handle, TCPPRM far *tcpprm);
;
;　入力：
;　出力：AL=ハンドル,-1:エラー
;　破壊：
tcp_accept_	PROC

	MOV	BX,SP
	PUSH	DI
	
	MOV	AL,[BX+2]
	LES	DI,[BX+4]
	$TEEN	42h
	
	POP	DI
	JC	Error
	RET

tcp_accept_	ENDP


;char tcp_shutdown(char handle);
;
;　入力：
;　出力：AL=0:正常,-1:エラー
;　破壊：
tcp_shutdown_	PROC

	MOV	BX,SP
	MOV	AL,[BX+2]
	$TEEN	43h
	JC	Error
	RET

tcp_shutdown_	ENDP


;エラーがあった場合に呼び出す
;  入力：AL=エラー番号
;  出力：AX=-1

Error	PROC

	MOV	BYTE PTR _tcperrno,AL
	MOV	AX,-1
	RET

Error	ENDP


;char tcp_close(char handle);
;
;　入力：
;　出力：AL=0:正常,-1:エラー
;　破壊：
tcp_close_	PROC

	MOV	BX,SP
	MOV	AL,[BX+2]
	$TEEN	44h
	JC	Error
	RET

tcp_close_	ENDP

;int tcp_send(char handle, char far *buf, uint len, uint flag);
;
;　入力：
;　出力：AX=送信バイト数(<32768),-1:エラー
;　破壊：
tcp_send_	PROC

	MOV	BX,SP
	PUSH	DI
	
	MOV	AL,[BX+2]
	LES	DI,[BX+4]
	MOV	CX,[BX+8]
	MOV	DX,[BX+10]
	$TEEN	45h
	
	POP	DI
	JC	Error
	RET

tcp_send_	ENDP

;int tcp_recv(char handle, char far *buf, int len);
;
;　入力：
;　出力：AX=受信バイト数(<32768),-1:エラー
;　破壊：
tcp_recv_	PROC

	MOV	BX,SP
	PUSH	DI

	MOV	AL,[BX+2]
	LES	DI,[BX+4]
	MOV	CX,[BX+8]
	$TEEN	46h

	POP	DI
	JC	Error
	RET


tcp_recv_	ENDP


;char tcp_state(char handle);
;
;　入力：
;　出力：AL=ステータス,-1:エラー
;　破壊：
tcp_state_	PROC

	MOV	BX,SP
	MOV	AL,[BX+2]
	$TEEN	47h
	JC	Error
	RET

tcp_state_	ENDP


;char tcp_downstate(char handle);
;
;　入力：
;　出力：AL=切断原因,-1:エラー
;　破壊：
tcp_downstate_	PROC

	MOV	BX,SP
	MOV	AL,[BX+2]
	$TEEN	48h
	JC	Error
	RET

tcp_downstate_	ENDP


;char tcp_getaddr(char handle, TCPADDR far *addr);
;
;　入力：
;　出力：AL=0:正常終了,-1:エラー
;　破壊：
tcp_getaddr_	PROC

	MOV	BX,SP
	PUSH	DI

	MOV	AL,[BX+2]
	LES	DI,[BX+4]
	$TEEN	49h

	POP	DI
	JC	Error
	RET

tcp_getaddr_	ENDP





	.DATA?

	PUBLIC	_tcperrno

_tcperrno	DW ?			;エラー番号
end
