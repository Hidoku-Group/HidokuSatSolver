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
extern const string AND = " 0\n";
extern const string OR = " ";
extern const string NOT = "-";

struct Field {
	int x;
	int y;
};

//a, b sind die nummern der felder/indizes
int dist(int a, int b, int size) {
	int xa = ((a-1) % size)+1;
	int ya = floor((a-1)/size)+1;
	int xb = ((b-1) % size)+1;
	int yb = floor((b-1) /size)+1;

	int xdif, ydif;
	if (xa>xb) {
		xdif = xa-xb;
	} else xdif=xb-xa;
	if (ya>yb) {
		ydif = ya-yb;
	} else ydif=yb-ya;

	if (xdif>ydif) return xdif;
	else return ydif;
}

int max(int a, int b, int size, vector<int>* values) {
	int distance = dist(a,b,size);
	int value_a = values->at(a);
	int value_b = values->at(b);
	if (value_a > value_b) {
		return ceil((float)((value_a - value_b) - distance)/2);
	} else {
		return ceil((float)((value_b - value_a) - distance)/2);
	}
}

string exec(string cmd) {
	FILE *in;
	char buff[512];

	ostringstream resultStream;

	if(!(in = popen(cmd.c_str(), "r"))) {
		return "error";
	}

	while(fgets(buff, sizeof(buff), in) != NULL) {
		resultStream << buff;
	}
	pclose(in);
	return resultStream.str();
}

int encode(int index, int value, int size) {
	return size * size * index + value;
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

/*int getNumberFromCode(istringstream* code) {
	int result;
	char c;
	string number = "";
	code->get();
	do {
		code->get(c);
//		cout << c;
		if (c != ' ') {
			number += c;
		}
	} while(c != ' ');
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
}*/
void deleteNumberFromStream(istringstream* code) {
code->get();
char c;
do {
code->get(c);
} while(c != ' ');
}      
      
void parseSolution(string solution, int values[], int size) {
     istringstream sol(solution);
     sol.ignore(4);
     int number = 1;
     char sign;
     for (int j = 1; j <= size*size*size*size+size; j++) {
         sol.get(sign);
         if (sign != '-') {
         Field cell = decode(number, size);
         values[(cell.x-1)] = cell.y;
         deleteNumberFromStream(&sol);
         } else {
         deleteNumberFromStream(&sol);
         }
     number++;
     }
}

string getExactlyOne(vector<int>* vars) {
	string result = to_string(vars->at(0));
	for(int i=1; i<vars->size(); i++) {
		result += OR + to_string(vars->at(i));
	}

	for(int i=0; i<vars->size(); i++) {
		for(int j=i+1; j<vars->size(); j++) {
			result += AND + NOT + to_string(vars->at(i)) + OR + NOT + to_string(vars->at(j));
		}
	}
	return result + AND;
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

string computeClauses(vector<int>* values, int size, vector<int>* possibleValues, vector<int>*emptyFields) {
	int n = size * size;

	string result;

	for (int i = 1; i <= n; i++) {
		if (values->at(i - 1) != 0) {
			result = result + to_string(encode(i, values->at(i - 1), size)) + AND;
		}
	}
	//result = "schritt 1: jede zahl kommt im Spielfeld genau einmal vor";
	vector<int>* vars = new vector<int>();
	for (int k = 0; k < possibleValues->size(); k++) { //toggle of ¬
		for (int i = 0; i < emptyFields->size(); i++) { //value
			vars->push_back(encode(emptyFields->at(i), possibleValues->at(k), size));
		}
		result += getExactlyOne(vars);
		vars->clear();
	}



result = result + "\n";

//	result = result + "schritt 2: Jedes Feld hat einen Nachbarn mit einer kleineren Zahl:" + "\n";
for (int i = 1; i <= n; i++) { //i represents the field index
ende:     /**
                 *0-top
                 *1-left
                 *2-right
                 *3-bottom
                 *4-tl
                 *5-tr
                 *6-bl
                 *7-br
                 *
                 */
	int neighbours[8] = {top(size, i, 1), left(size, i, 1),	right(size, i, 1), bottom(size, i, 1)};

	neighbours[4] = top(size, neighbours[1], 1);
	neighbours[5] = top(size, neighbours[2], 1);
	neighbours[6] = bottom(size, neighbours[1], 1);
	neighbours[7] = bottom(size, neighbours[2], 1);


	if(values->at(i-1) == 0) {

		for (int j = 0; j < possibleValues->size(); j++) { // j stands for the value of the field i
hinfort:
			if(possibleValues->at(j) < 2 || find(possibleValues->begin(), possibleValues->end(), possibleValues->at(j)-1) == possibleValues->end() ) {
				continue;
			}

			//result = result + "( " + NOT + "("  + to_string( i ) +  ", "  + to_string( j ) +  ") ";


			for(int k=0; k<8; k++) {
				//	if(neighbours[k] != 0)
				//	result += to_string(possibleValues->at(j)) + " ?= " + to_string(values->at(neighbours[k]-1)+1);
				if(neighbours[k] != 0 && ((possibleValues->at(j) == values->at(neighbours[k] - 1)+1)
				                         )) {
					//result += "weg";
					/* j++;
					if(j>= possibleValues->size()) {
						i++;
						goto ende;
					}
					goto hinfort;*/
				}
			}



			result = result + NOT + to_string(encode(i, possibleValues->at(j), size));
			//	result += "(" + to_string(i) + ", " + to_string(possibleValues->at(j)) + ")";

			for(int k=0; k<8; k++) {
				int nk = neighbours[k];
				if (find(emptyFields->begin(), emptyFields->end(), nk) != emptyFields->end()  ) {
					result = result + OR + to_string(encode(nk, possibleValues->at(j) - 1, size));
					//	result += "(" + to_string(nk) + ", " + to_string(possibleValues->at(j) -1) + ")";

				}


			}
			result = result + AND;
		}
	} else {
		if(values->at(i-1) > 1 && find(possibleValues->begin(), possibleValues->end(), values->at(i-1)-1) != possibleValues->end() ) {

			int tmp=0;
			for(int k=0; k<8; k++) {
				int nk=neighbours[k];
				if(nk != 0 && find(emptyFields->begin(), emptyFields->end(), nk) != emptyFields->end()) {
					if(tmp == 1) {
						result = result + OR;
					}
					result += to_string(encode(nk, values->at(i-1)-1, size));
					//	result += "(" + to_string(nk) + ", " + to_string(values->at(i-1)-1) + ") ";
					tmp=1;
				}

			}
			result = result + AND;
		}

		if(values->at(i-1) < n   && find(possibleValues->begin(), possibleValues->end(), values->at(i-1)+1) != possibleValues->end() ) {

			int tmp=0;
			for(int k=0; k<8; k++) {
				int nk=neighbours[k];
				if(nk != 0 && find(emptyFields->begin(), emptyFields->end(), nk) != emptyFields->end()) {
					if(tmp == 1) {
						result = result + OR;
					}
					result += to_string(encode(nk, values->at(i-1)+1, size));
					//result += "(" + to_string(nk) + ", " + to_string(values->at(i-1)+1) + ") ";
					tmp=1;
				}

			}
			result = result + AND + "\n";
		}
	}

}


//result = result + "\n" + "schritt 3: Nur 1 Zahl pro Feld" + "\n";

for(int i=0; i<emptyFields->size(); i++) {
	for (int k=0; k<possibleValues->size(); k++) {
	    vars->push_back(encode(emptyFields->at(i), possibleValues->at(k), size));
	}
	result += getExactlyOne(vars);
	vars->clear();
		
}

return result;

}

int main(int argc, char* argv[]) {
	vector<int>* values = new vector<int>();
	int size = parseHidoku(argv[1], values);


	vector<int>* possibleValues = new vector<int>();
	vector<int>* emptyFields = new vector<int>();
	fillData(possibleValues, emptyFields, size, values);


	ofstream outfile;
	outfile.open ("/tmp/inteamout.txt");

	outfile << computeClauses(values, size, possibleValues, emptyFields);
	outfile.close();

	int n=size*size;
	int newValues[n];
	string result = exec( "minisat /tmp/inteamout.txt /tmp/inteamresult.txt > /dev/null && cat /tmp/inteamresult.txt");

	if(result == "") {
		return 20;
	}
	parseSolution( result, newValues, size);
	drawBoard(newValues, size);
	return 10;
}
