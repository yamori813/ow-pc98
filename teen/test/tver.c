#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEENIFIDENT	"%TEENAPI"

void teen_version(int vect)
{
union REGS regs;

	regs.h.ah = 0x01;
	int86(vect, &regs, &regs);
	printf("TEEN Vector %x Version %d.%d", vect, regs.h.al, regs.h.ah);
}

int main()
{
int vect;
char far *addr;

	vect = 0x22;
	while (vect < 0x100){
		addr = (char *far)_dos_getvect(vect);
		addr -= 9L;
		if (_fstrcmp(addr, TEENIFIDENT) == 0) {
			teen_version(vect);
			return EXIT_SUCCESS;
		}
		++vect;
	}
	printf("NG");
	return EXIT_FAILURE;
}
