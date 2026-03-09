#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "omp.h"

#define N 1000000

int main(void)
{
	double pi;
	int i, factor;

	pi = 0;
	factor = 1;
	#pragma omp parallel for
	for (i = 0; i < N; i++, factor = -factor)
		pi += 4.0 * factor / (2 * i + 1);

	printf("pi = %lf\n", pi);
	return 0;
}
