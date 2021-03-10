/*
 * Copyright (c) 1997  XMaZ Project.
 *
 *  isoternet Protocol Library / Common Routines / isoternet addreess
 *
 *  $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ttypes.h>


static int getbyte(char *str)
{
  char *s;
  int a;

  if ( (s = strrchr(str, '.'))==NULL ) return -1;

  *s = '\0';
  s++;

  if (*s == '\0') return -1;
  for (a = 0; *s != '\0'; s++) {
    if (*s < '0' || *s > '9') return -1;
    if ( a *= 10, (a += *s - '0') > 255 ) return -1;
  }

  return a;
}


DWRD inet_aton(char *str_)
{
  DWRD addr;
  BYTE *b = (BYTE *) &addr;
  char str[17];
  char *s;
  int a,i;

  strncpy(str+1,str_,15);
  str[0] = '.';
  str[16] = '\0';
  for (i=3; i>=0; i--) {
    if ( (a = getbyte(str)) < 0 ) return 0;
    b[i] = a;
  }
  return ( str[0] == '\0' ) ? addr : 0;
}


char *inet_ntoa(DWRD addr)
{
  static char s[16];
  BYTE *a;
   
  a = (BYTE *) & addr;
  sprintf(s, "%d.%d.%d.%d", a[0],a[1],a[2],a[3]);

  return s;
}
