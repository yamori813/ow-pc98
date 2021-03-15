;
;	インジケータ文字列表示
;

	.186
	.MODEL	SMALL

	.CODE
	INCLUDE teencall.inc


	PUBLIC	indicate_str_

;
;  void indicate_str(int pos, char far *str);
;　入力：
;　出力：
;　破壊：
indicate_str_	PROC

		MOV	BX,SP
		PUSH	DI

		MOV	CH,[BX+2] ;pos
		LES	DI,[BX+4] ;str
		$TEEN	94h

		POP	DI
		RET

indicate_str_	ENDP

;1998/ 4/ 8 
end
