;
;	TEEN 存在判定
;	TEEN API 呼び出し
;


	.186
	.MODEL	SMALL

	.CODE
	;INCLUDE teencall.inc

	PUBLIC	teen_available_
	PUBLIC	TEENAPI
	EXTRN	teen_search_:NEAR


;TEENが存在するか
; int teen_available(void);
;　入力：
;　出力：0..利用不可 0以外..利用可
;　破壊：
;  補足：このファンクションを最初に実行しないと、他の TEEN 呼び出し関数が
;	 利用できない。
teen_available_	PROC

	CALL	teen_search_
	OR	AL,AL
	JNZ	Available

	XOR	AX,AX
	RET

Available:
	MOV	CS:INTNo,AL
	MOV	AX,-1
	RET

teen_available_	ENDP



;TEEN API 呼び出し
;　入力：
;　出力：
;　破壊：
TEENAPI	PROC

	DB	0CDH	;INT
INTNo	DB	00h	;TEEN API 割り込み番号
	RET

TEENAPI	ENDP


;1997/12/ 5 
;1998/ 3/ 2 
end
