#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#define EMPTY_FIELD 1
#define BOTTOM_LINE 2

using namespace std;

extern const string XOR = "@";
extern const string AND = ".";
extern const string OR = "+";
extern const string NOT = "~";

struct Field {
	int x;
	int y;
};

string exec(string cmd) {
    FILE *in;
	char buff[512];

	ostringstream resultStream;

	if(!(in = popen(cmd.c_str(), "r"))){
		return "error";
	}

	while(fgets(buff, sizeof(buff), in) != NULL){
		resultStream << buff;
	}
	pclose(in);
	return resultStream.str();
}

string encode(int index, int value, int size) {
	int e = size * size * index + value;
	return "a" + to_string(e);
}

Field decode(int number, int size) {
	int n = size * size;
	int newValue = ((number-1) % (n)) +1;
	int newNumber = floor((number-1) / n);
	Field f = { newNumber, newValue };
	return f;
}

//the vector need to have the first element on position with index 1 -> write 0 in index 0
void fillData(vector<int> * possibleValues, vector<int>* emptyFields, int size, vector<int>* data) {
	//fill all Values in possible Values field
	for (int j = 1; j <= size * size ; j++) {
		possibleValues->push_back(j);
	}
	//erasecounter necessary because of erasing elements is changing the index
	int erasecounter = 1;
	for (int i = 1; i < size * size + 1; i++) {
		int value = (*data).at(i-1);
		//if field is empty add it to emptyField
		if (value == 0) {
			emptyFields->push_back(i);
		}
		//if not empty create a Point and save it to values and remove it from possible values
		else {
			possibleValues->erase(remove(possibleValues->begin(), possibleValues->end(), value), possibleValues->end());
			erasecounter++;
		}
	}
}

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
	linesize = (3 + ceil(log10(size * size + 0.5))) * size + 2;

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
	for (int i = 0; i < cellCount; i++) {
		cout.put('|');
		for (int j = 0; j < (numbersize + 2); j++) {
			cout.put(' ');
		}
	}
	cout << '|' << endl;
}

void drawNumbers(int numsize, int size, int values[], int linenumber) {
	for (int i = 0; i < size; i++) {
		cout << "| ";
		int number = values[i + (linenumber * size)];
		if (number != 0) {
			cout << number;
			int numberLength = ceil(log10(number + 0.5));
			while (numberLength < numsize) {
				cout.put(' ');
				numberLength++;
			}
		} else {
			for (int j = 0; j < (numsize); j++) {
				cout.put(' ');
			}
		}
		cout << " ";
		//		cout << values->at(i);
	}
	cout << '|' << endl;
}

void drawBoard(int values[], int size) {
	int linesize = (3 + ceil(log10(size * size + 0.5))) * size + 1;
	cout << "hidoku with size " << size << endl;
	drawLine(linesize, '-');
	for (int i = 0; i < size; i++) {
		drawCellBody(ceil(log10(size * size + 0.5)), size);
		drawNumbers(ceil(log10(size * size + 0.5)), size, values, i);
		drawCellBody(ceil(log10(size * size + 0.5)), size);
		drawLine(linesize, '-');
	}
}

int getNumberFromCode(istringstream* code) {
	int result;
	char c;
	string number = "";
	code->get();
	do
	{
		code->get(c);
//		cout << c;
		if (c != ' '){
			number += c;
		}
	}while(c != ' ');
	result = atoi(number.c_str());
	return result;
}

void parseSolution(string solution, int values[], int size) {
	istringstream sol(solution);
	sol.ignore(3);
	int number = 0;
	char sign;
	for (int j = 1; j <= size*size; j++) {
		for (int i = 1; i <= size * size; i++) {
			sol.get(sign);
			if (sign == ' ') {
				Field cell = decode(getNumberFromCode(&sol), size);
				values[(cell.x-1)] = cell.y;
			} else {
				getNumberFromCode(&sol);

			}

		}
	}
}

/**
 * returns the number of the field that is "steps" steps in the left direction of "from"
 * if there is no such field in this direction simply 0 will be returned
 *
 * performance hint: if ever the left top (which means a combination of two funcitions is needed)
 * try to store the result of left (it's slow in computing) and use top twice (it's fast :D )
 */
int left(int size, int from, int steps) {
	if (from == 0) {
		return from;
	}
	int mod = from % size;

	//prevents from "overflow, which means that you can substract 3 from 7, get 4,
	//and 4 would be left of 4, but not in the same row
	if (mod > size) {
		return 0;
	}
	int left = from - steps;
	if (left > from - mod) {
		return left;
	} else if (mod == 0) { //special case for the last entries in each row
		int d = from / steps;
		if (left > from - d * size) {
			return left;
		}
	}
	return 0;
}

int right(int size, int from, int steps) {
	if (from == 0) {
		return from;
	}
	int right = from + steps, d = from / size;

	if (from % size == 0) {
		return 0;
	} else if (right <= (d + 1) * size) {
		return right;
	}
	return 0;
}

int top(int size, int from, int steps) {
	if (from == 0) {
		return from;
	}
	int top = from - steps * size;
	if (top > 0) {
		return top;
	}
	return 0;
}

int bottom(int size, int from, int steps) {
	if (from == 0) {
		return from;
	}
	int bottom = from + steps * size;
	if (bottom <= size * size) {
		return bottom;
	}
	return 0;
}

string computeClauses(vector<int>* values, int size) {
	int n = size * size;

	string result;

	result += "def ";
	for (int i = size * size + 1; i <= size * size * size * size + size * size;
			i++) {
		result += "a" + to_string(i) + " ";
	}
	result += ";";

	for (int i = 1; i <= n; i++) {
		if (values->at(i - 1) != 0) {
			result = result + encode(i, values->at(i - 1), size) + AND + " ";
		}
	}

	result = result + "(";

	//result = "schritt 1: jede zahl kommt im Spielfeld genau einmal vor";
	for (int k = 1; k <= n; k++) { //toggle of ¬
		//result = result + "k: " + to_string(k) + " val[k]:"  + to_string( values->at(k-1)) + "\n";
		if (values->at(k - 1) != 0) {
			continue;
		}

		result = result + "(";

		for (int i = 1; i <= n; i++) { //value
			result = result + "(";
			for (int j = 1; j <= n; j++) { //field index
				if (k == j) {
					result = result + encode(j, i, size);
					//result = result + " ("  + to_string( j ) +  ", "  + to_string( i ) +  ") ";
				} else {
					result = result + NOT + encode(j, i, size);
					//result = result + NOT + "("  + to_string( j ) +  ", "  + to_string( i ) +  ") ";
				}
				if (j != n) {
					result = result + AND + " ";
				}
			}

			if (i == n) {
				result = result + ") " + "\n";
			} else {
				result = result + ") " + OR + "\n";

			}
		}
		result = result + ")" + AND + "\n";

	}

	result = result + "\n";

	//	result = result + "schritt 2: Jedes Feld hat einen Nachbarn mit einer kleineren Zahl:" + "\n";
	for (int i = 1; i <= n; i++) { //i represents the field index
		for (int j = 2; j <= n; j++) { // j stands for the value of the field i
			//result = result + "( " + NOT + "("  + to_string( i ) +  ", "  + to_string( j ) +  ") ";

			result = result + "( " + NOT + encode(i, j, size) + " ";

			int t = top(size, i, 1);
			int l = left(size, i, 1);
			int r = right(size, i, 1);
			int b = bottom(size, i, 1);
			int tl = top(size, l, 1);
			int tr = top(size, r, 1);
			int bl = bottom(size, l, 1);
			int br = bottom(size, r, 1);

			if (t != 0) {
				//	result = result + OR + " ("  + to_string( t ) +  ", "  + to_string( j-1 ) +  ") ";
				result = result + OR + encode(t, j - 1, size) + " ";

				if (tl != 0) {
					//	result = result + OR + " ("  + to_string( tl ) +  ", "  + to_string( j-1 ) +  ") ";
					result = result + OR + encode(tl, j - 1, size) + " ";
				}

				if (tr != 0) {
					//	result = result + OR + " ("  + to_string( tr ) +  ", "  + to_string( j-1 ) +  ") ";
					result = result + OR + encode(tr, j - 1, size) + " ";
				}

			}

			if (l != 0) {
				//result = result + OR + " ("  + to_string( l ) +  ", "  + to_string( j-1 ) + ") ";
				result = result + OR + encode(l, j - 1, size) + " ";
			}

			if (r != 0) {
				//result = result + OR + " ("  + to_string( r ) +  ", "  + to_string( j-1 ) +  ") ";
				result = result + OR + encode(r, j - 1, size) + " ";
			}

			if (b != 0) {
				//result = result + OR + " ("  + to_string( b ) +  ", "  + to_string( j-1 ) +  ") ";
				result = result + OR + encode(b, j - 1, size) + " ";

				if (bl != 0) {
					//result = result + OR + " ("  + to_string( bl ) +  ", "  + to_string( j-1 ) +  ") ";
					result = result + OR + encode(bl, j - 1, size) + " ";
				}

				if (br != 0) {
					//	result = result + OR + " ("  + to_string( br ) +  ", "  + to_string( j-1 ) +  ") ";
					result = result + OR + encode(br, j - 1, size) + " ";
				}
			}

			result = result + " )" + AND + "\n";
		}

	}

	//result = result + "\n" + "schritt 3: Nur 1 Zahl pro Feld" + "\n";

	for (int i = 1; i <= n; i++) { //index of field
		result = result + "(";
		for (int j = 1; j <= n; j++) { //value of field
			//result = result + "( ("  + to_string( i ) +  ", "  + to_string( j ) +  ") ";
			result = result + "( " + encode(i, j, size);
			for (int k = 1; k <= n; k++) { //value of other fields
				if (k == j) {
					continue;
				}
				//	result = result + AND + " " + NOT + "("  + to_string( i ) +  ", "  + to_string( k ) +  ") ";
				result = result + AND + NOT + encode(i, k, size);
			}
			if (j == n) {
				result = result + ") " + "\n";
			} else {
				result = result + ") " + OR + "\n";
			}
		}
		if (i != n) {
			result = result + ") " + AND + "\n";
		} else {
			result = result + ")";
		}
	}

	return result + ");";

}

int main(int argc, char* argv[]) {
	vector<int>* values = new vector<int>();
	int size = parseHidoku(argv[1], values);


	vector<int>* possibleValues = new vector<int>();
	vector<int>* emptyFields = new vector<int>();
	fillData(possibleValues, emptyFields, size, values);




	ofstream outfile;
    outfile.open ("out.txt");

    outfile << computeClauses(values, size);
    outfile.close();

	int n=size*size;
    int newValues[n];
	parseSolution( exec( "cat out.txt | ./logic2cnf -j1"), newValues, size);
	drawBoard(newValues, size);
	return 10;
}
