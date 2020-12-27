#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdio>
using namespace std;

const int NONE = -1;
const int UP = 1;
const int LEFT = 2;
const int UP_LEFT = 3;

class Node {	//	stack node
public:
	int pos;		//	GAP이 삽입되는 위치
	Node* next;
	Node(int p) : pos(p), next(NULL) {}
};

Node* top1 = NULL;
Node* top2 = NULL;

bool empty(Node* top) {
	return !top;
}

void push(Node** top, int p) {
	Node* node = new Node(p);
	node->next = *top;
	*top = node;
}

int pop(Node** top) {
	int ret = NONE;
	if (!empty(*top)) {
		Node* del = *top;
		ret = del->pos;
		*top = (*top)->next;
		delete del;
	}
	return ret;
}

int isMatch(char ch1, char ch2, int s, int f) {
	return (ch1 == ch2) ? s : (-1) * f;
}

int max3(int num1, int num2, int num3) {
	//	num1 : match, num2 : str2에 삽입, num3 : str1에 삽입
	int max = num1;
	if (num2 > max) max = num2;
	if (num3 > max) max = num3;
	//	같은 값일 경우 match, str2에 삽입, str1에 삽입 순

	/*
	int max = num1;
	if (num3 > max) max = num3;
	if (num2 > max) max = num2;
	//	같은 값일 경우 match, str1에 삽입, str2에 삽입 순
	*/

	/*
	int max = num2;
	if (num1 > max) max = num1;
	if (num3 > max) max = num3;
	//	같은 값일 경우 str2에 삽입, match, str1에 삽입 순
	*/

	/*
	int max = num2;
	if (num3 > max) max = num3;
	if (num1 > max) max = num1;
	//	같은 값일 경우 str2에 삽입, str1에 삽입, match 순
	*/

	/*
	int max = num3;
	if (num1 > max) max = num1;
	if (num2 > max) max = num2;
	//	같은 값일 경우 str1에 삽입, match, str2에 삽입 순
	*/

	/*
	int max = num3;
	if (num2 > max) max = num2;
	if (num1 > max) max = num1;
	//	같은 값일 경우 str1에 삽입, str2에 삽입, match 순
	*/

	return max;
}

void makeScoreTable(int** score, int** path, char* str1, char* str2, int len1, int len2, int s, int f, int p) {
	if (!score || !path || !str1 || !str2) return;

	for (int i = 0; i < len1 + 1; i++)
		score[i][0] = (-1) * i * p;
	for (int j = 0; j < len2 + 1; j++)
		score[0][j] = (-1) * j * p;

	for (int i = 1; i < len1 + 1; i++) {
		for (int j = 1; j < len2 + 1; j++) {
			int match = isMatch(str1[i - 1], str2[j - 1], s, f) + score[i - 1][j - 1];	//	match
			int insert2 = (-1) * p + score[i - 1][j];	//	str2에 삽입
			int insert1 = (-1) * p + score[i][j - 1];	//	str1에 삽입
			int max = max3(match, insert2, insert1);

			if (max == match) path[i][j] = UP_LEFT;
			else if (max == insert2) path[i][j] = UP;
			else if (max == insert1) path[i][j] = LEFT;
			score[i][j] = max;
		}
	}

	printf("매칭 점수 : %d\n", score[len1][len2]);
}

void printGapPosition(int** path, int& len1, int& len2) {
	if (!path) return;

	int i = len1, j = len2;
	int dir;
	int gap1 = 0, gap2 = 0;	//	str1, str2에 삽입된 gap의 개수
	int pos = 0;
	int matchLen;			//	matching 길이

	printf("len1 : %d, len2 : %d\n", len1, len2);
	while ((dir = path[i][j]) != NONE) {
		//	printf("path 반복문 : ");
		//	printf("(%d, %d)\n", i, j);
		if (dir == UP_LEFT) {
			printf("UP_LEFT\n");
			i--;
			j--;
			++pos;
		}
		else if (dir == LEFT) {
			printf("LEFT\n");
			len1++;
			gap1++;
			push(&top1, ++pos);
			j--;
		}
		else if (dir == UP) {
			printf("UP\n");
			len2++;
			gap2++;
			push(&top2, ++pos);
			i--;
		}
		else {
			printf("Something was wrong!\n");
		}
	}

	printf("len1 : %d, len2 : %d\n", len1, len2);
	matchLen = len1 > len2 ? len1 : len2;

	printf("matching 길이 : %d\n", matchLen);

	int realPos;	//	실제 삽입될 위치
	printf("str1에 삽입된 gap의 개수 : %d\n", gap1);
	while (!empty(top1)) {
		realPos = matchLen - pop(&top1) + 1;
		printf("%d\n", realPos);
	}
	printf("str2에 삽입된 gap의 개수 : %d\n", gap2);
	while (!empty(top2)) {
		realPos = matchLen - pop(&top2) + 1;
		printf("%d\n", realPos);
	}
}


int main(void) {
	char str1[10] = "ATCGGATCT";
	char str2[8] = "ACGGACT";
	str1[9] = 0;
	str2[7] = 0;

	int** score = new int* [10];
	for (int i = 0; i < 10; i++)
		score[i] = new int[8];

	makeScoreTable(score, str2, str1, 7, 9, 2, 1, 2);
	
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 10; j++)
			printf("%3d ", score[i][j]);
		printf("\n");
	}

	return 0;
}
