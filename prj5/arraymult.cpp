#include "simd.p5.h"

//#define ARR_LEN 1024

void MyMul(float * a, float * b, float * c, int len)
{
	for (int i = 0; i < len; i++) {
		c[i] = a[i] * b[i];
	}
}

void MyMulSum(float * a, float * b, int len)
{
	float sum = 0.0;
	for (int i = 0; i < len; i++) {
		sum += a[i] * b[i];
	}
}

int main(int argc, char ** argv)
{
	float a[ARR_LEN], b[ARR_LEN], c[ARR_LEN];

	for (int i = 0; i < ARR_LEN; i++) {
		a[i] = i + 0.17;
		b[i] = i * 0.8 - 0.24;
		c[i] = 0.0;
	}

	double time0 = omp_get_wtime();
	MyMul(a, b, c, ARR_LEN);
	double time1 = omp_get_wtime();

	double madds = (float)(ARR_LEN) / (time1 - time0) / 1000000.;
	printf("Performance = %8.2lf MegaAdds/Sec\n", madds);

	time0 = omp_get_wtime();
	MyMulSum(a, b, ARR_LEN);
	time1 = omp_get_wtime();

	madds = (float)(ARR_LEN) / (time1 - time0) / 1000000.;
	printf("Performance = %8.2lf MegaAdds/Sec\n", madds);

	time0 = omp_get_wtime();
	SimdMul(a, b, c, ARR_LEN);
	time1 = omp_get_wtime();

	madds = (float)(ARR_LEN) / (time1 - time0) / 1000000.;
	printf("Performance = %8.2lf MegaAdds/Sec\n", madds);

	time0 = omp_get_wtime();
	SimdMulSum(a, b, ARR_LEN);
	time1 = omp_get_wtime();

	madds = (float)(ARR_LEN) / (time1 - time0) / 1000000.;
	printf("Performance = %8.2lf MegaAdds/Sec\n", madds);

	printf("-----------------------------------------------\n");

	return 0;
}