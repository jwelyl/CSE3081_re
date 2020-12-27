#include <iostream>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <ctime>
using namespace std;

void makeSquare(int*** arr, int N) {
	*arr = new int* [N];
	for (int i = 0; i < N; i++) {
		(*arr)[i] = new int[N];
	}
}

void freeSquare(int** arr, int N) {
	for (int i = 0; i < N; i++) {
		delete[] arr[i];
	}
	delete[] arr;
}

void inputArr(int** arr, int N) {
	if (!arr) {
		cout << "2차원 배열 arr이 할당되지 않음!" << endl;
		exit(-1);
	}

	
}