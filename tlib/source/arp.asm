;
;	ARP
;

	INCLUDE	teencall.inc

	.186
	.MODEL	SMALL

	.CODE

	PUBLIC	arp_getentry_

;char arp_getentry(int n, void far *buf, uint bufsize);
;　入力：
;　出力：char 0..正常 0以外..エラー
;　破壊：
arp_getentry_	PROC

		MOV	BX,SP
		PUSH	DI

		MOV	CL,[BX+2]	;n
		LES	DI,[BX+4]	;buf
		MOV	CH,[BX+8]	;bufsize

		$TEEN	0A0h

		POP	DI
		RET

arp_getentry_	ENDP
end
