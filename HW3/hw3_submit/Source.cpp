#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>
#include <Windows.h>

#define CHECK_TIME_START QueryPerformanceFrequency ((_LARGE_INTEGER*)&freq); QueryPerformanceCounter((_LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((float) (end - start)/freq)
_int64 start, freq, end;
#define NONE	-1

float total = 0;
float partial = 0;
clock_t startTime;
clock_t endTime;

int main(void) {
	CHECK_TIME_START;
	for (int i = 0; i < 1000000; i++) {
		if (i == 100000) startTime = clock();
		if (i == 200000) endTime = clock();
		for (int j = 0; j < 10; j++) {
			int* p = new int;
			delete p;
		}
	}
	CHECK_TIME_END(total);
	partial = (float)((endTime - startTime) / CLOCKS_PER_SEC);
	printf("partial time : %f(s)\n", partial);
	printf("total time : %f(s)\n", total);
	return 0;
}