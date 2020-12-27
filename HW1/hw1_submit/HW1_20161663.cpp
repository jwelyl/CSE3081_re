#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdio>
#include <windows.h>

#define CHECK_TIME_START QueryPerformanceFrequency ((_LARGE_INTEGER*)&freq); QueryPerformanceCounter((_LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((float) (end - start)/freq)
_int64 start, freq, end;

void make1DArray(int** arr, int N) {
	*arr = new int[N];
}

void make2DArray(int*** arr, int N) {
	*arr = new int* [N];
	for (int i = 0; i < N; i++)
		(*arr)[i] = new int[N];
}

void inputSquare(int** square, int N, std::ifstream& ifs) {
	if (!square) {
		std::cout << "inputSquare 함수, 2차원 배열 square이 할당되지 않음!" << std::endl;
		exit(-1);
	}
	for (int i = 0; i < N; i++) 
		for (int j = 0; j < N; j++) 
			ifs.read((char*)(&square[i][j]), sizeof(int));
}

void free1DArray(int* arr) {
	delete[] arr;
}

void free2DArray(int** arr, int N) {
	for (int i = 0; i < N; i++)
		delete[] arr[i];
	delete[] arr;
}

void makeTable(int** table, int** square, int N) {
	if (!square || !table) {
		std::cout << "2차원 배열 table 또는 square이 할당되지 않음!" << std::endl;
		exit(-1);
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i == 0 && j == 0) table[i][j] = square[i][j];
			else if (i == 0) table[i][j] = table[i][j - 1] + square[i][j];
			else if (j == 0) table[i][j] = table[i - 1][j] + square[i][j];
			else {
				table[i][j] = table[i][j - 1] + table[i - 1][j] 
								- table[i - 1][j - 1] + square[i][j];
			}
		}
	}
}

int max3(int left, int right, int mid, 
	   int ls, int le, int rs, int re, 
	   int ms, int me, int& s, int& e) {
	int cand;
	if (left > right) {
		cand = left;
		s = ls;	e = le;
	}
	else {
		cand = right;
		s = rs;	e = re;
	}
	if (cand < mid) {
		cand = mid;
		s = ms;	e = me;
	}
	return cand;
}

//	DNC Algorithm O(NlogN)
int mss1(const int seq[], int& start, int& end, int left, int right) {
	int maxLeftSum, ls, le;
	int maxRightSum, rs, re;
	int maxMidLeftSum, ms;
	int maxMidRightSum, me;
	int midLeftSum, midRightSum;
	int maxMidSum;
	int center;

	if (left == right) { /* Base Case */
		start = left; end = right;
		return seq[left];
	}

	center = (left + right) / 2;
	maxLeftSum = mss1(seq, ls, le, left, center);
	maxRightSum = mss1(seq, rs, re, center + 1, right);

	maxMidLeftSum = seq[center]; midLeftSum = 0;
	ms = center;
	for (int i = center; i >= left; i--) {
		midLeftSum += seq[i];
		if (midLeftSum > maxMidLeftSum) {
			maxMidLeftSum = midLeftSum;
			ms = i;
		}
	}

	maxMidRightSum = seq[center + 1]; midRightSum = 0;
	me = center + 1;

	for (int i = center + 1; i <= right; i++) {
		midRightSum += seq[i];
		if (midRightSum > maxMidRightSum) {
			maxMidRightSum = midRightSum;
			me = i;
		}
	}

	maxMidSum = maxMidLeftSum + maxMidRightSum;

	return max3(maxLeftSum, maxRightSum, maxMidSum,
					ls, le, rs, re, ms, me, start, end);
}

int mss1(const int seq[], int& start, int& end, int N) {
	return mss1(seq, start, end, 0, N - 1);
}

//	Kadane's Algorithm O(N)
int mss2(const int seq[], int& start, int& end, int N) {
	int sum = 0; int maxSum = INT_MIN;
	end = -1;

	int localStart = 0;
	for (int i = 0; i < N; i++) {
		sum += seq[i];
		if (sum < 0) {
			sum = 0;
			localStart = i + 1;
		}
		else if (sum > maxSum) {
			maxSum = sum;
			start = localStart;	end = i;
		}
	}
	if (end != -1) return maxSum;

	maxSum = seq[0]; start = end = 0;
	for (int i = 1; i < N; i++) {
		if (seq[i] > maxSum) {
			maxSum = seq[i]; start = end = i;
		}
	}
	return maxSum;
}

//	O(N^4) Algorithm 
int findMaxSum(int** square, int** table, int N,
	int& finalLeft, int& finalRight, int& finalTop, int& finalBottom) {

	make2DArray(&table, N);
	makeTable(table, square, N);
/*
	int maxSum = table[0][0], sum;
	finalLeft = 0; finalRight = 0;
	finalTop = 0; finalBottom = 0;
*/
	int maxSum = INT_MIN, sum;

	for (int i = 0; i < N; i++) {
		for (int j = i; j < N; j++) {
			for (int k = 0; k < N; k++) {
				for (int l = k; l < N; l++) {
					if (i == 0 && k != 0) sum = table[j][l] - table[j][k - 1];
					else if (i != 0 && k == 0) sum = table[j][l] - table[i - 1][l];
					else if (i == 0 && k == 0) sum = table[j][l];
					else {
						sum = table[j][l] - table[j][k - 1] 
							- table[i - 1][l] + table[i - 1][k - 1];
					}
					if (sum > maxSum) {
						maxSum = sum;
						finalLeft = k; finalRight = l;
						finalTop = i; finalBottom = j;
					}
				}
			}
		}
	}
	
	free2DArray(table, N);
	return maxSum;
}

//	O(N^3logN) or O(N^3) Algorithm
int findMaxSum(int** square, int N,
	int (*mss)(const int seq[], int& start, int& end, int N),
	int& finalLeft, int& finalRight, int& finalTop, int& finalBottom) {
	
	int maxSum = INT_MIN;
	int left, right;
	int sum, start, finish;
	int* temp;
	
	make1DArray(&temp, N);
	
	for (left = 0; left < N; left++) {
		for (int i = 0; i < N; i++)
			temp[i] = 0;
		for (right = left; right < N; right++) {
			for (int i = 0; i < N; i++)
				temp[i] += square[i][right];
			sum = mss(temp, start, finish, N);

			if (sum > maxSum) {
				maxSum = sum;
				finalLeft = left; finalRight = right;
				finalTop = start; finalBottom = finish;
			}
		}
	}

	free1DArray(temp);
	return maxSum;
}

int main(void) {
	int** square = NULL;
	int** table = NULL;
	int T, N;
	int maxSum, left, right, top, bottom;

	std::string config = "HW1_config.txt";
	
	std::ifstream ifs(config);
	if (ifs.fail()) {
		std::cout << "File Open Error!" << std::endl;
		return -1;
	}
	ifs >> T;

	for (int i = 1; i <= T; i++) {
		int algo;
		float duration = 0;
		char inData[33], outData[33];
		ifs >> algo >> inData >> outData;

		std::ifstream input(std::string(inData).data(), std::ios::in | std::ios::binary);
		std::ofstream output(std::string(outData).data(), std::ios::out | std::ios::binary);
		if (input.fail()) {
			std::cout << inData << " Open Error!" << std::endl;
			exit(-1);
		}
		else if (output.fail()) {
			std::cout << outData << " Open Error!" << std::endl;
			exit(-1);
		}
		input.read((char*)(&N), sizeof(int));

		make2DArray(&square, N);
		inputSquare(square, N, input);
		input.close();

		CHECK_TIME_START;
		printf("[%d]\n", algo);
		switch (algo) {
		case 3:
			maxSum = findMaxSum(square, table, N, left, right, top, bottom);
			break;
		case 4:
			maxSum = findMaxSum(square, N, mss1, left, right, top, bottom);
			break;
		case 5:
			maxSum = findMaxSum(square, N, mss2, left, right, top, bottom);
			break;
		default:
			std::cout << "잘못된 알고리즘 입력!" << std::endl;
			break;
		}
		CHECK_TIME_END(duration);

//
		std::cout << "Test " << i << std::endl;
		std::cout << "크기 : " << N << " x " << N << ", 사용 알고리즘 : " << algo << std::endl;
		std::cout << "최대 부분 사각형 합 : " << maxSum << std::endl;
		std::cout << "top : " << top << ", left : " << left << ", bottom : " << bottom << ", right : " << right << std::endl;
		std::cout << "duration : " << duration * 1000 << "msec" << std::endl;
//

		output.write((char*)(&maxSum), sizeof(int));
		output.write((char*)(&top), sizeof(int));
		output.write((char*)(&left), sizeof(int));
		output.write((char*)(&bottom), sizeof(int));
		output.write((char*)(&right), sizeof(int));
		output.close();
	
		free2DArray(square, N);
	}

	ifs.close();
	return 0;
}
