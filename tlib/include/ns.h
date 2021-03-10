/*
	TEEN / NameService
	Written by XMaZ Project.  1997,1998

	1998/ 3/ 7 
*/

#if !defined(_NS_H_)
#define _NS_H_


#include <ttypes.h>


/*******************************
　　　マクロ
*******************************/

/*------------------------------
　　エラー
------------------------------*/

#define ENS_BUSY	2	/* 処理中 */
#define ENS_NOHOST	6	/* ホストが見つからない */
#define ENS_NORESOURCE	7	/* ハンドル・メモリ不足 */
#define ENS_BADNAME	8	/* 名前が不適当 */
#define ENS_TIMEOUT	9	/* タイムアウト */


/*******************************
　　　変数
*******************************/

extern int nserrno;		/* エラー番号 */


/*******************************
　　　プロトタイプ
*******************************/

IDWORD getipaddress(char far *name);

#endif /*_NS_H_*/
