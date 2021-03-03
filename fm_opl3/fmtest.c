#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "fm.h"

int main()
{
	int i;

	if( ! fm_detect() )
	{
		printf("ERROR: FM not detected \n");
		return 1;
	}
	else
	{
		printf("FM detected \n");
	}
	fm_reset();

	fm_load_patch( 0, fm_get_patch_sine() );

	for( i = 1; i < 20000; i += 1 )
	{
		if( ! fm_play_tone(0, i, 63) )
		{
			printf("ERROR: Can't play frequency %i \n", i);
		}
		else
		{
			printf("%i \n", i);
		}
		if( kbhit() )
		{
			getch();
			break;
		}
	}

	fm_stop_tone( 0 );

	return 0;
}


