;
;	インジケータ整数表示
;

	.186
	.MODEL	SMALL

	.CODE
	INCLUDE teencall.inc


	PUBLIC	indicate_int_

;
;  void indicate_int(int pos, int len, unsigned val);
;　入力：
;　出力：
;　破壊：
indicate_int_	PROC

		MOV	BX,SP
		MOV	CH,[BX+2] ;pos
		MOV	CL,[BX+4] ;len
		MOV	DX,[BX+6] ;val
		$TEEN	93h
		RET

indicate_int_	ENDP

;1998/ 4/ 8 
end
