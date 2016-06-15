#include <stdio.h>
#include <omp.h>
#include <math.h>

//#define NUMS			1000
//#define NUMT	         64
#define NUMTRIES        10

#define XMIN	0.
#define XMAX	3.
#define YMIN	0.
#define YMAX	3.

#define Z00	0.
#define Z10	1.
#define Z20	0.
#define Z30	0.

#define Z01	1.
#define Z11	6.
#define Z21	1.
#define Z31	0.

#define Z02	0.
#define Z12	1.
#define Z22	0.
#define Z32	4.

#define Z03	3.
#define Z13	2.
#define Z23	3.
#define Z33	3.

float
Height(int iu, int iv)	// iu,iv = 0 .. NUMS-1
{
	float u = (float)iu / (float)(NUMS - 1);
	float v = (float)iv / (float)(NUMS - 1);

	// the basis functions:

	float bu0 = (1. - u) * (1. - u) * (1. - u);
	float bu1 = 3. * u * (1. - u) * (1. - u);
	float bu2 = 3. * u * u * (1. - u);
	float bu3 = u * u * u;

	float bv0 = (1. - v) * (1. - v) * (1. - v);
	float bv1 = 3. * v * (1. - v) * (1. - v);
	float bv2 = 3. * v * v * (1. - v);
	float bv3 = v * v * v;

	// finally, we get to compute something:

	float height = bu0 * (bv0 * Z00 + bv1 * Z01 + bv2 * Z02 + bv3 * Z03)
	               + bu1 * (bv0 * Z10 + bv1 * Z11 + bv2 * Z12 + bv3 * Z13)
	               + bu2 * (bv0 * Z20 + bv1 * Z21 + bv2 * Z22 + bv3 * Z23)
	               + bu3 * (bv0 * Z30 + bv1 * Z31 + bv2 * Z32 + bv3 * Z33);

	return height;
}

int
main()
{
#ifndef _OPENMP
	fprintf(stderr, "OpenMP is not supported here -- sorry.\n");
	return 1;
#endif

	omp_set_num_threads(NUMT);
	//fprintf(stderr, "Using %d threads\n", NUMT);

	double maxmmults = 0.;
	double summmults = 0.;
	float sum_v = 0.;

	for (int t = 0; t < NUMTRIES; t++)
	{
		double time0 = omp_get_wtime();

		float fullTileArea = (((XMAX - XMIN) / (float)(NUMS - 1))  *  ((YMAX - YMIN) / (float)(NUMS - 1)));
		float volume = 0;

		#pragma omp parallel for reduction(+:volume)
		for (int i = 0; i < NUMS * NUMS; i++)
		{
			int iu = i % NUMS;
			int iv = i / NUMS;

			if (((iu % (NUMS - 1)) == 0) || ((iv % (NUMS - 1)) == 0)) {
				if (((iu % (NUMS - 1)) == 0) && ((iv % (NUMS - 1)) == 0)) {
					volume += 0.25 * fullTileArea * Height(iu, iv);
				}
				else {
					volume += 0.5 * fullTileArea * Height(iu, iv);
				}
			}
			else {
				volume += fullTileArea * Height(iu, iv);
			}
		}

		sum_v += volume;
		//printf("TRY_%d, The total volume is: %8.2lf\n", t, volume);

		double time1 = omp_get_wtime();
		double mmults =  (NUMS * NUMS) / (time1 - time0) / 1000000.;
		summmults += mmults;
		if (mmults > maxmmults)
			maxmmults = mmults;
	}
	printf("Volume is: %8.2lf\n", sum_v / 10.0);
	printf("   Peak Performance = %8.2lf MegaMults/Sec\n", maxmmults);
	printf("Average Performance = %8.2lf MegaMults/Sec\n", summmults / (double)NUMTRIES);
	printf("\n");
	return 0;
}