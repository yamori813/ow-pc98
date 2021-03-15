;
;	IP Routing
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	ip_getrtentry_

;char ip_getrtentry(int n, void far *buf, uint bufsize);
;　入力：
;　出力：char 0..正常 -1..エラー
;　破壊：
ip_getrtentry_	PROC

		MOV	BX,SP
		PUSH	DI

		MOV	CH,[BX+2]	;n
		LES	DI,[BX+4]	;buf
		MOV	CL,[BX+8]	;bufsize

		$TEEN	60h

		POP	DI
		RET

ip_getrtentry_	ENDP

;2000/ 9/22 
;2001/ 1/28 関数名変更
end
