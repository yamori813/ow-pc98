#include <stdio.h>

#include "teen.h"

int main()
{
int ver;

	if(! teen_available()) {
		printf("TEEN not found");
	} else {
		ver = teen_version();
		printf("TEEN Version %d", ver);
	}

	return 0;
}
