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
	int max = num1;
	if (num2 > max) max = num2;
	if (num3 > max) max = num3;

	return max;
}

void makeScoreTable(int** score, int** path, char* str1, char* str2,
	int len1, int len2, int s, int f, int p, ofstream& ofs) {
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
	ofs << score[len1][len2] << endl;
}

void printGapPosition(int** path, int len1, int len2, ofstream& ofs) {
	if (!path) return;

	int i = len1, j = len2;
	int dir;
	int gap1 = 0, gap2 = 0;	//	str1, str2에 삽입된 gap의 개수
	int pos = 0;

	while ((dir = path[i][j]) != NONE) {
		if (dir == UP_LEFT) {
			i--;
			j--;
			++pos;
		}
		else if (dir == LEFT) {
			len1++;
			gap1++;
			push(&top1, ++pos);
			j--;
		}
		else if (dir == UP) {
			len2++;
			gap2++;
			push(&top2, ++pos);
			i--;
		}
	}

	if (!(len1 == len2)) {
		printf("something was wrong!\n");
		exit(-1);
	}

	ofs << len1 << endl;

	int realPos;	//	실제 삽입될 위치
	ofs << gap1 << endl;
	while (!empty(top1)) {
		realPos = len1 - pop(&top1) + 1;
		ofs << realPos << endl;
	}
	ofs << gap2 << endl;
	while (!empty(top2)) {
		realPos = len1 - pop(&top2) + 1;
		ofs << realPos << endl;
	}
}

int main(void) {
	string input = "input.txt";
	string output = "output.txt";
	int s, f, p;

	ifstream ifs(input);
	if (ifs.fail()) {
		printf("File open error!\n");
		return -1;
	}

	string fileName;
	getline(ifs, fileName);
	ifs >> s >> f >> p;
	ifs.close();

	char* str1 = NULL, * str2 = NULL;
	int** score = NULL;
	int** path = NULL;
	int len1, len2;

	ifstream ifs2(fileName.data(), ios::in | ios::binary);
	ifs2.read((char*)(&len1), sizeof(int));
	ifs2.read((char*)(&len2), sizeof(int));

	str1 = new char[len1 + 1];
	str1[len1] = 0;
	str2 = new char[len2 + 1];
	str2[len2] = 0;

	score = new int* [len1 + 1];
	path = new int* [len1 + 1];
	for (int i = 0; i < len1 + 1; i++) {
		score[i] = new int[len2 + 1];
		path[i] = new int[len2 + 1];
	}

	for (int i = 0; i < len1 + 1; i++) path[i][0] = UP;
	for (int j = 0; j < len2 + 1; j++) path[0][j] = LEFT;
	path[0][0] = NONE;

	for (int i = 0; i < len1; i++)
		ifs2.read((char*)(&str1[i]), sizeof(char));
	for (int i = 0; i < len2; i++)
		ifs2.read((char*)(&str2[i]), sizeof(char));
	ifs2.close();

	ofstream ofs(output);
	makeScoreTable(score, path, str1, str2, len1, len2, s, f, p, ofs);
	printGapPosition(path, len1, len2, ofs);
	ofs.close();

	delete[] str1;
	delete[] str2;
	for (int i = 0; i < len1 + 1; i++) {
		delete[] score[i];
		delete[] path[i];
	}
	delete[] score;
	delete[] path;

	return 0;
}
