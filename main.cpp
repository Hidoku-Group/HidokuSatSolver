#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define EMPTY_FIELD 1
#define BOTTOM_LINE 2

using namespace std;

int parseHidoku(char* path, vector<int>* values) {
	ifstream myfile(path);
	char c;
	string number;
	int size;
	int flags = 0;
	int lineIndex = 0;
	int linesize = 0;
	int count;
	// read out size
	myfile.ignore(17);
	myfile >> size;
//	cout << "SIZE: "<< size << endl;
	linesize = (3+ceil(log10(size*size)))*size+2;
//	cout << "linesize: " << linesize << endl;
	// ignore heading
	myfile.ignore(1); // ignore linebreak
	myfile.ignore(linesize * 2); // ignore next two lines
	while (!myfile.eof()) {

		if ((flags & EMPTY_FIELD) == 0) {
			myfile.get(c);
		}

		if (c == '|') {
			myfile >> number;
			switch (number[0]) {
			case '|':
//				cout << "EMPTY" << endl;
				values->push_back(0);
				flags |= EMPTY_FIELD;
				lineIndex++;
				count++;
				break;
			default:
				flags = 0;
//				cout << number << endl;
				values->push_back(atoi(number.c_str()));
				lineIndex++;
				count++;
				break;
			}
			if (lineIndex == size){
				myfile.ignore(linesize*3+1);
				if ((flags & EMPTY_FIELD) == 0){
					myfile.ignore(2);
				}
				lineIndex = 0;
				flags = 0;
			}
		}

	}
	return size;
}

int main() {
	vector<int>* values = new vector<int>();
	int size = parseHidoku("easy/hidoku-4-6-1.txt", values);
	return 10;
}

