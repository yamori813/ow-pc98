;
;	インジケータ表示制御
;

	.186
	.MODEL	SMALL

	.CODE
	INCLUDE teencall.inc


	PUBLIC	indicate_sw_

;
;  void indicate_sw(int sw);
;　入力：
;　出力：
;　破壊：
indicate_sw_	PROC

		MOV	BX,SP
		MOV	AL,[BX+2] ;sw
		$TEEN	90h
		RET

indicate_sw_	ENDP

;1998/ 4/ 8 
end
