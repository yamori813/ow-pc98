;
;	TEEN 割り込み番号検索
;


	.186
	.MODEL	SMALL

	.CODE
	;INCLUDE	teencall.inc

	PUBLIC	teen_search_


;TEEN API 割り込み番号取得
;BYTE _teen_search(void);
;　入力：
;　出力：BYTE				割り込み番号 0..常駐していない
;　破壊：AH,BX,CX,ES
teen_search_	PROC

	PUSH	SI
	PUSH	DI

	MOV	BX,18h*4		;ES:BX=割り込みベクタアドレス
	CLD

Re:
		XOR	AX,AX
		MOV	ES,AX
		LES	DI,ES:[BX]	;ES:DI=割り込みベクタ

		MOV	CX,9
		SUB	DI,CX
		MOV	SI,OFFSET IDENT
		REPE	CMPSB
		JE	Found

		ADD	BX,4
		CMP	BX,100h*4
	JB	Re

	MOV	AL,0
	JMP	Exit
Found:
	SHR	BX,2
	MOV	AL,BL
Exit:
	POP	DI
	POP	SI
	RET

teen_search_	ENDP


	.DATA

IDENT	DB "%TEENAPI",0


;1998/ 2/21 
end
