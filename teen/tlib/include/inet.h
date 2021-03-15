/*
	TEEN / Internet Address
	Written by XMaZ Project  2000

	2000/ 1/23 
*/

#if !defined(_INET_H_)
#define _INET_H_

#include <ttypes.h>


/*******************************
　　　型
*******************************/

typedef struct {			/* IPアドレス構造体 */
	BYTE data[18];
} IPADDR;

typedef char IPADDRSTR[40];		/* IPアドレス文字列 */



/*******************************
　　　プロトタイプ
*******************************/

/*
API
*/
char inet_toipaddr(const void far *addr, IPADDR far *ipaddr);
char inet_tostr(const void far *addr, IPADDRSTR far *str);
char inet_toipv4addr(const void far *addr, IDWORD far *ipv4addr);
void inet_ipv4addrtoipaddr(IDWORD ipv4addr, IPADDR far *ipaddr);
char inet_iswildcard(const void far *addr);



/*
ライブラリ
*/
IDWORD inet_aton(char *str);
char *inet_ntoa(IDWORD addr);


#endif /*_INET_H_*/
