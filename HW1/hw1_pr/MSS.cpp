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
		cout << "2���� �迭 arr�� �Ҵ���� ����!" << endl;
		exit(-1);
	}

	
}