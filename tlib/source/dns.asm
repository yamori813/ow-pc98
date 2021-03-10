;
;	DNS
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	getipaddress_


;IDWORD getipaddress(char far *name);
;　入力：
;　出力：
;　破壊：
getipaddress_	PROC

	MOV	BX,SP
	PUSH	DI

	LES	DI,[BX+2]
	$TEEN	30h
	JC	GIA_Err

	POP	DI
	RET

GIA_Err:
	MOV	BYTE PTR _nserrno,AL
	XOR	AX,AX
	MOV	DX,AX

	POP	DI
	RET

getipaddress_	ENDP



	.DATA?

	PUBLIC	_nserrno

_nserrno	DW ?				;エラー番号
end
