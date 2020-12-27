#include <iostream>
#include <fstream>
using namespace std;

int main(void) {
	ofstream output("data.bin", ios::out | ios::binary);

	char c1 = 'U';
	char c2 = 'V';
	char c3 = 'W';
	char str[] = "apbak";

	output.write(&c1, sizeof(char));
	output.write(str, sizeof(str) - 1);
	output.write(&c2, sizeof(c2));
	output << "anim" << " ";
	output.put('5');
	output.write(&c3, 1);

	output.close();
	return 0;
}