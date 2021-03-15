/*
	TEEN / Indicater
	Written by XMaZ Project.  1998

	1998/ 4/ 8 
*/

#if !defined(_INDICATE_H_)
#define _INDICATE_H_


#include <ttypes.h>

/*******************************
　　　プロトタイプ
*******************************/

void indicate_sw(int sw);
void indicate_char(int pos, char c);
void indicate_int(int pos, int len, unsigned val);
void indicate_str(int pos, char far *str);

#endif /* _INDICATE_H_ */
