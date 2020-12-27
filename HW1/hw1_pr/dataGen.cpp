#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <string>

#define Is_2D true
#define N_Sequence 1600
#define Final_n Is_2D?N_Sequence*N_Sequence:N_Sequence
#define Max_Sequence_Elements 100
#define Positive_Probability 0.25

void main(void) {
	int target;
	float prob;
	char c_string[256];
	srand((unsigned)time(NULL));

	for (int idx = 1; idx < 6; idx++) {
		
		sprintf(c_string, "HW1_MAX100_%d_0%d.in.bin", N_Sequence, idx);
		std::ofstream File(std::string(c_string).data(), std::ios::out | std::ios::binary);

		if (File.is_open()) {
			target = N_Sequence;
			File.write((char*)(&target), sizeof(int));
			for (int i = 0; i < (Final_n); i++) {
				target = rand() % (Max_Sequence_Elements);

				prob = (float)rand() / RAND_MAX;
				if (prob > Positive_Probability)
					target *= -1;
				File.write((char*)(&target), sizeof(int));

			}

			File.close();
		}
/*
		std::ifstream inFile(std::string(c_string).data(), std::ios::in | std::ios::binary);
		if (inFile.is_open()) {
			std::string str;
			int n;
			inFile.read((char*)(&n), sizeof(int));
			std::cout << "Generated Data Count: " << n << std::endl;
			for (int i = 0; i < n * n; i++) {
				inFile.read((char*)(&target), sizeof(int));
				std::cout << target << " ";
			}
			std::cout << std::endl;
			inFile.close();
		}
*/
	}
}


