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
	int pos;		//	GAP�� ���ԵǴ� ��ġ
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
	//	num1 : match, num2 : str2�� ����, num3 : str1�� ����
	int max = num1;
	if (num2 > max) max = num2;
	if (num3 > max) max = num3;
	//	���� ���� ��� match, str2�� ����, str1�� ���� ��

	/*
	int max = num1;
	if (num3 > max) max = num3;
	if (num2 > max) max = num2;
	//	���� ���� ��� match, str1�� ����, str2�� ���� ��
	*/

	/*
	int max = num2;
	if (num1 > max) max = num1;
	if (num3 > max) max = num3;
	//	���� ���� ��� str2�� ����, match, str1�� ���� ��
	*/

	/*
	int max = num2;
	if (num3 > max) max = num3;
	if (num1 > max) max = num1;
	//	���� ���� ��� str2�� ����, str1�� ����, match ��
	*/

	/*
	int max = num3;
	if (num1 > max) max = num1;
	if (num2 > max) max = num2;
	//	���� ���� ��� str1�� ����, match, str2�� ���� ��
	*/

	/*
	int max = num3;
	if (num2 > max) max = num2;
	if (num1 > max) max = num1;
	//	���� ���� ��� str1�� ����, str2�� ����, match ��
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
			int insert2 = (-1) * p + score[i - 1][j];	//	str2�� ����
			int insert1 = (-1) * p + score[i][j - 1];	//	str1�� ����
			int max = max3(match, insert2, insert1);

			if (max == match) path[i][j] = UP_LEFT;
			else if (max == insert2) path[i][j] = UP;
			else if (max == insert1) path[i][j] = LEFT;
			score[i][j] = max;
		}
	}

	printf("��Ī ���� : %d\n", score[len1][len2]);
}

void printGapPosition(int** path, int& len1, int& len2) {
	if (!path) return;

	int i = len1, j = len2;
	int dir;
	int gap1 = 0, gap2 = 0;	//	str1, str2�� ���Ե� gap�� ����
	int pos = 0;
	int matchLen;			//	matching ����

	printf("len1 : %d, len2 : %d\n", len1, len2);
	while ((dir = path[i][j]) != NONE) {
		//	printf("path �ݺ��� : ");
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

	printf("matching ���� : %d\n", matchLen);

	int realPos;	//	���� ���Ե� ��ġ
	printf("str1�� ���Ե� gap�� ���� : %d\n", gap1);
	while (!empty(top1)) {
		realPos = matchLen - pop(&top1) + 1;
		printf("%d\n", realPos);
	}
	printf("str2�� ���Ե� gap�� ���� : %d\n", gap2);
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
