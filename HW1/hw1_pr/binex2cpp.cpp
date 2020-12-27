#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
	ofstream output("data2.bin", ios::out | ios::binary);

	int n1 = 500;
	int n2 = 1000;
	double f = 3.14;
	int arr[5] = { 1,2,4,8,16 };
	string str = "ABCDE";

	output.write((char*)&n1, sizeof(int));
	output.write((char*)arr, sizeof(arr));
	output << n2 << f << str;
	output << n1;

	output.close();
	return 0;
}