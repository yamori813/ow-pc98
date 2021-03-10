;
;	TEEN バージョン取得
;


	.186
	.MODEL	SMALL

	.CODE
	INCLUDE teencall.inc

	PUBLIC	teen_version_


;TEEN バージョン取得
; uint teen_version(void);
;  出力： (バージョン小数部 << 8) | バージョン整数部
teen_version_	PROC

	$TEEN	01h
	RET

teen_version_	ENDP
end
