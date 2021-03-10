;
;	TIMER
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	teen_mtime_

;unsigned long teen_mtime(void)
;　入力：
;　出力：unsigned long msec単位の増加カウンタの値
;　破壊：

teen_mtime_	PROC
	$TEEN	03h
	RET
teen_mtime_	ENDP

end
