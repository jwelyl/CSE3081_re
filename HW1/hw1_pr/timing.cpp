#include <stdio.h>

#include <windows.h>
#define CHECK_TIME_START QueryPerformanceFrequency ((_LARGE_INTEGER*)&freq); QueryPerformanceCounter((_LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((float) (end - start)/freq)

#define CMAX 1000000000 

void test0(int N, unsigned int *x, unsigned int *xx) {
	int i, j, k;

	for (i = 1; i <= N; i++)
		for (j = 1; j <= i; j++)
			for (k = 1; k <= j; k++) {
				(*x)++; // how many times?
				if (*x == CMAX) {
					(*xx)++;
					*x = 0;
				}
			}
}

void main(void) {
	__int64 start, freq, end;
	float	resultTime = 0;

	unsigned int n, count, count2;

	printf("************* test0 ****************\n");

	n = 512;
	count = count2 = 0;
	CHECK_TIME_START;
	test0(n, &count, &count2);
	CHECK_TIME_END(resultTime);
	printf("*** n = %u --> count2 = %u, count = %u\n", n, count2, count);
	printf("n:512 -> %f(s).. \n", resultTime);

	n = 2048;
	count = count2 = 0;
	CHECK_TIME_START;
	test0(n, &count, &count2);
	CHECK_TIME_END(resultTime);
	printf("*** n = %u --> count2 = %u, count = %u\n", n, count2, count);
	printf("n:2048 -> %f(s).. \n", resultTime);
}