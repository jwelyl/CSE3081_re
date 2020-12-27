#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <Windows.h>

#define CHECK_TIME_START QueryPerformanceFrequency ((_LARGE_INTEGER*)&freq); QueryPerformanceCounter((_LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((float) (end - start)/freq)
_int64 start, freq, end;
#define NONE	-1

float part1 = 0;
float part2 = 0;
float total = 0;

int main(void) {
	CHECK_TIME_START;
	for (int i = 0; i < 200000; i++) {
		for (int j = 0; j < 10; j++) {
			int* p = new int;
			delete p;
		}
	}
	CHECK_TIME_END(part1);

	CHECK_TIME_START;
	for (int i = 200000; i < 1000000; i++) {
		for (int j = 0; j < 10; j++) {
			int* p = new int;
			delete p;
		}
	}
	CHECK_TIME_END(part2);
	total = part1 + part2;
	printf("part1 time : %f(s)\n", part1);
	printf("part2 time : %f(s)\n", part2);
	printf("total time : %f(s)\n", total);
	return 0;
}