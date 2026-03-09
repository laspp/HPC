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
	double startTime = omp_get_wtime();
	#pragma omp parallel for
	for (i = 0; i < N; i++) {
		factor = 1 - 2 * (i % 2);
		#pragma omp critical
		pi += 4.0 * factor / (2 * i + 1);
	}
	double endTime = omp_get_wtime();
	printf("pi=%lf, time taken: %lf seconds\n", endTime - startTime);

	return 0;
}
