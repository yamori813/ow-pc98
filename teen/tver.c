#include <dos.h>
#include <stdio.h>
#include <string.h>

void teen_version(int vect)
{
union REGS regs;

	regs.h.ah = 0x01;
	int86(vect, &regs, &regs);
	printf("TEEN Vector %x Version %d.%d", vect, regs.h.ah, regs.h.al);
}

int main()
{
	int vect;
	char far *addr;
	char c;

	vect = 0x22;
	while ( vect < 0x100){
		addr = (char *far)_dos_getvect(vect);
		addr -= 9L;
		c = addr[0];
		if (_fstrcmp(addr, "%TEENAPI") == 0) {
			teen_version(vect);
			return 1;
		}
		++vect;
	}
	printf("NG");
	return 0;
}
