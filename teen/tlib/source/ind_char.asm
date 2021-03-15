;
;	インジケータ文字表示
;

	.186
	.MODEL	SMALL

	.CODE
	INCLUDE teencall.inc


	PUBLIC	indicate_char_

;
;  void indicate_char(int pos, char c);
;　入力：
;　出力：
;　破壊：
indicate_char_	PROC

		MOV	BX,SP
		MOV	DH,[BX+2] ;pos
		MOV	DL,[BX+4] ;c
		$TEEN	92h
		RET

indicate_char_	ENDP

;1998/ 4/ 8 
end
