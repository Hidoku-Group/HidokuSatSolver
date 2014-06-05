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
	linesize = (3 + ceil(log10(size * size))) * size + 2;
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
				values->push_back(0);
				flags |= EMPTY_FIELD;
				lineIndex++;
				count++;
				break;
			default:
				flags = 0;
				values->push_back(atoi(number.c_str()));
				lineIndex++;
				count++;
				break;
			}
			if (lineIndex == size) {
				myfile.ignore(linesize * 3 + 1);
				if ((flags & EMPTY_FIELD) == 0) {
					myfile.ignore(2);
				}
				lineIndex = 0;
				flags = 0;
			}
		}

	}
	myfile.close();
	return size;
}

void drawLine(int linesize, char c) {
	for (int i = 0; i < linesize; i++) {
		cout.put('-');
	}
	cout << endl;
}

/**
 * This function should write the head (or the footer) of a cell of the Hidoku-Board.
 *
 * @param numbersize Count of characters for a Number in the Hidoku
 * @param size Number of cells in a row of the Hidoku
 */
void drawCellBody(int numbersize, int cellCount) {

}

int drawBoard(vector<int>* values, int size) {
	int linesize = (3 + ceil(log10(size * size))) * size + 1;
	cout << "hidoku with size " << size << endl;
	drawLine(linesize, '-');
	drawCellBody(ceil(log10(size * size)), size);
	return 0;
}

int main() {
	vector<int>* values = new vector<int>();
	int size = parseHidoku("easy/hidoku-4-6-1.txt", values);
	drawBoard(values, size);
	return 10;
}

