#include <stdio.h>
#include <conio.h>

void main(void)
{

    // YM2203,YM2608,YMF288,YMF297
    int x;

    x = inp(0x0188);

    printf("0x0188 %02x\n", x);
}
