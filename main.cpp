#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#define EMPTY_FIELD 1
#define BOTTOM_LINE 2

using namespace std;

extern const string XOR = "@";
extern const string AND = ".";
extern const string OR = "+";
extern const string NOT = "~";

struct Field 
{
	int x;
	int y;
};

struct Hidoku {
	int size;
	vector<Field> values;
	vector<int> possibleValues;
	vector<int> emptyFields;
};

int encode(int index, int value, int size) {
	return size*size*index+value;
}

Field decode(int number, int size){
	int n = size*size;
	int newValue = number % n;
	int newNumber = floor(number / n);
	Field f = {newNumber, newValue};
	return f;
}

//the vector need to have the first element on position with index 1 -> write 0 in index 0
Hidoku fillData(Hidoku h, int size, vector<int> data){
     h.size = size;
     //fill all Values in possible Values field
     for (int j=1; j < size*size+1; j++){
         h.possibleValues.push_back(j);
         }
     //erasecounter necessary because of erasing elements is changing the index
     int erasecounter = 1;
     for (int i=1; i < size*size+1; i++){
         int value = data.at(i);
         //if field is empty add it to emptyField
         if (value == 0){
             h.emptyFields.push_back(i);       
             }
         //if not empty create a Point and save it to values and remove it from possible values
         else {
              Field p = {i,value};
              h.values.push_back(p); 
              h.possibleValues.erase(h.possibleValues.begin()+value-erasecounter);
              erasecounter++;
             }
         }
     return h;
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

void drawNumbers(int numsize, int size, vector<int>* values, int linenumber) {
	for (int i = 0; i < size; i++) {
		cout << "| ";
		int number = values->at(i + (linenumber * size));
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

void drawBoard(vector<int>* values, int size) {
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


/**
 * returns the number of the field that is "steps" steps in the left direction of "from"
 * if there is no such field in this direction simply 0 will be returned
 *
 * performance hint: if ever the left top (which means a combination of two funcitions is needed) 
 * try to store the result of left (it's slow in computing) and use top twice (it's fast :D )
 */
int left(int size, int from, int steps) {
	if(from == 0) {
		return from;
	}
	int mod = from%size;

	//prevents from "overflow, which means that you can substract 3 from 7, get 4, 
	//and 4 would be left of 4, but not in the same row
	if(mod > size) {
		return 0;
	}
	int left = from-steps;
	if(left > from - mod ){
		return left;
	} else if(mod == 0) { //special case for the last entries in each row
		int d = from / steps;
		if(left > from - d*size) {
			return left;
		}
	}
	return 0;
}

int right(int size, int from, int steps) {
	if(from == 0) {
		return from;
	}
	int right = from+steps, d = from / size;

	if(from%size == 0) {
		return 0;
	} else if(right <= (d+1)*size){
		return right;
	}
	return 0;
}

int top(int size, int from, int steps) {
	if(from == 0) {
		return from;
	}
	int top = from-steps*size;
	if(top > 0) {
		return top;
	}
	return 0;
}

int bottom(int size, int from, int steps) {
	if(from == 0) {
		return from;
	}
	int bottom = from + steps * size;
	if(bottom <= size*size) {
		return bottom;
	}
	return 0;
}

string computeClauses(vector<int>* values, int size) {
	int n = size*size;

	ostringstream result;

	result << "schritt 1: jede zahl kommt im Spielfeld genau einmal vor";
	for(int k=1; k<=n; k++) {//toggle of ¬
		result << "k: " << k << " val[k]:" << values->at(k-1) << endl;
		if(values->at(k-1) != 0) {
			continue;
		}
		for(int i=1; i<=n; i++) { //value
			result << "(";
			for(int j=1; j<=n; j++) { //field index
				if(k == j) {
					result << " (" << j << ", " << i << ") ";
				} else {
					result << NOT + "(" << j << ", " << i << ") ";
				}
				if(j != n) {
					result << AND + " ";
				}
			}
			//remove last AND

			result << ") " + OR << endl;
		}
		result << endl;
	}

	result << endl;


	result << "schritt 2: Jedes Feld hat einen Nachbarn mit einer kleineren Zahl:" << endl;
	for(int i=1; i<=n; i++) { //i represents the field index
		for(int j=1; j<=n; j++) { // j stands for the value of the field i
			result << "( " + NOT + "(" << i << ", " << j << ") ";
			//printf("(%d, %d) → (", i, j);

			int t = top(size, i, 1);
			int l = left(size, i, 1);
			int r = right(size, i, 1);
			int b = bottom(size, i, 1);
			int tl = top(size, l, 1);
			int tr = top(size, r, 1);
			int bl = bottom(size, l, 1);
			int br = bottom(size, r, 1);

			if(t != 0) {
				result << OR + " (" << t << ", " << j-1 << ") ";

				if(tl != 0) {
					result << OR + " (" << tl << ", " << j-1 << ") ";
				}

				if(tr != 0) {
					result << OR + " (" << tr << ", " << j-1 << ") ";
				}


			}

			if(l != 0) {
				result << OR + " (" << l << ", " << j-1 <<") ";
			}

			if(r != 0) {
				result << OR + " (" << r << ", " << j-1 << ") ";
			}

			if( b != 0) {
				result << OR + " (" << b << ", " << j-1 << ") ";


				if(bl != 0) {
					result << OR + " (" << bl << ", " << j-1 << ") ";
				}

				if(br != 0) {
					result << OR + " (" << br << ", " << j-1 << ") ";
				}
			}

			result << " )" + AND << endl;
		}


	}
	result << endl << "schritt 3: Nur 1 Zahl pro Feld" << endl;

	for(int i=1; i<=n; i++) { //index of field
		for(int j=1; j<=n; j++) { //value of field
			result << "( (" << i << ", " << j << ") ";
			for(int k=1; k<=n; k++) { //value of other fields
				if(k==j) {
					continue;
				}
				result << AND + " " + NOT << "(" << i << ", " << k << ") "; 
			}
			result << ") " + OR << endl;
		}
	}

	return result.str();

}


int main() {
	vector<int>* values = new vector<int>();
	int size = parseHidoku("easy/hidoku-3-6-1.txt", values);
	drawBoard(values, size);
	cout << computeClauses(values, size);
	return 10;
}

