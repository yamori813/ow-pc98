;
;	PPP接続・切断
;


	.186
	.MODEL	SMALL

	.CODE
	INCLUDE	teencall.inc

	PUBLIC	dialup_open_,dialup_close_


; int dialup_open(int n);
;　入力：
;　出力：
;　破壊：
dialup_open_	PROC

	$TEEN	11h
	MOV	AH,0
	RET

dialup_open_	ENDP


; int dialup_close(void);
;　入力：
;　出力：
;　破壊：
dialup_close_	PROC

	$TEEN	12h
	MOV	AH,0
	RET

dialup_close_	ENDP



;1997/12/ 3 
;1998/ 3/ 2 
end
