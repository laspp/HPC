#include "omp.h"
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int i;

	#pragma omp parallel for
	for(i=0; i<10; i++)
	{
		int id = omp_get_thread_num();
		if(i==0)
			usleep(1000000);
		printf("Iteracija %i koncana (%d).\n", i, id);
	}

	return 0;
}
