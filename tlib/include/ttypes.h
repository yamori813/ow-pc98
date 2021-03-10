/*
	TEEN / types
	Written by XMaZ Project.  1997,1998

	1998/ 3/ 7 
*/

#if !defined(_TTYPES_H_)
#define _TTYPES_H_

#define	BYTE	unsigned char
#define	uchar	unsigned char
#define	WORD	unsigned short
#define	uint	unsigned short
#define IWORD	unsigned short		/* ネットワーク順序の WORD */
#define	DWORD	unsigned long
#define	DWRD	unsigned long
#define IDWORD	unsigned long		/* ネットワーク順序の DWORD */

#ifndef FAR
#define FAR
#endif /* FAR */

#endif /* _TTYPES_H_ */
