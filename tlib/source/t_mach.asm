;
;	機種種別取得
;

	.186
	.MODEL	SMALL

	.CODE
	INCLUDE teencall.inc


	PUBLIC	teen_machine_


;機種種別取得
;  uint teen_machine(void);
;　入力：
;　出力：TEEN.REF参照
;　破壊：
teen_machine_	PROC

	$TEEN	02H
	RET

teen_machine_	ENDP


;1998/ 4/ 8 
end
