;
;	INET ワイルドカードIPアドレス
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	inet_iswildcard_


;char inet_iswildcard(const void far *addr);
;
;　入力：
;　出力：
;　破壊：
inet_iswildcard_	PROC
	MOV	BX,SP
	PUSH	DI
	LES	DI,[BX+2]	;addr
	$TEEN	24h
	POP	DI
	RET
inet_iswildcard_	ENDP


end
