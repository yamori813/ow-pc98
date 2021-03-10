/*
	TEEN / misc
	Written by XMaZ Project.  1997,1998,2000

	2001/ 4/13 
*/

#if !defined(_TEEN_H_)
#define _TEEN_H_


#include <ttypes.h>

#include <inet.h>		/* 1998/4/8版との互換性のため */

/*******************************
　　　プロトタイプ
*******************************/


/*
一般
*/
int  teen_available(void);
uint teen_version(void);	/* 上位:バージョン小数部 下位:整数部 */
uint teen_machine(void);
unsigned long teen_mtime(void);

/*
接続切断
*/
int  dialup_open(int n);
int  dialup_close(void);

#endif /*_TEEN_H_*/
