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
extern const string OR = " ";
extern const string NOT = "-";

int andCtr=0;

string AND() {
	andCtr++;
	return " 0\n";
}

/*
 * vector<int> encode/decode global
 * vector->at(i) mehrmals benutzt -> in int zwischenspeichern
 * int tmp außerhalb von for schleifen deklarieren
 * (x, y) anstatt wert
 *
 * encode funktion sodass keine unnützen variablen verwendet werden
 *
 * */

int size;

struct Field {
	int x;
	int y;
};

vector<int> values;

int* valueMap;
/**
 * returns the number of the field that is "steps" steps in the left direction of "from"
 * if there is no such field in this direction simply 0 will be returned
 *
 * performance hint: if ever the left top (which means a combination of two funcitions is needed)
 * try to store the result of left (it's slow in computing) and use top twice (it's fast :D )
 */
int left(int from, int steps) {
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

int right(int from, int steps) {
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

int top(int from, int steps) {
	if (from == 0) {
		return from;
	}
	int top = from - steps * size;
	if (top > 0) {
		return top;
	}
	return 0;
}

int bottom(int from, int steps) {
	if (from == 0) {
		return from;
	}
	int bottom = from + steps * size;
	if (bottom <= size * size) {
		return bottom;
	}
	return 0;
}





int getXPos(int a) {
	return ((a - 1) % size) + 1;
}

int getYPos(int a) {
	return floor((a - 1) / size) + 1;
}




int convertXYToIndex(int x, int y) {
	return (y-1)*size + x ;
}


//////////////////////////////////////////////////////////i
//
//
int leftM(int from, int steps) {
	int mod = from % size;

	//prevents from "overflow, which means that you can substract 3 from 7, get 4,
	//and 4 would be left of 4, but not in the same row
	if (mod > size) {
		return convertXYToIndex(1, getYPos(from));
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
	return convertXYToIndex(1, getYPos(from));
}

int rightM(int from, int steps) {
	int right = from + steps, d = from / size;

	if (from % size == 0) {
		return convertXYToIndex(size, getYPos(from));
	} else if (right <= (d + 1) * size) {
		return right;
	}
	return convertXYToIndex(size, getYPos(from));
}

int topM(int from, int steps) {
	int top = from - steps * size;
	if (top > 0) {
		return top;
	}
	return convertXYToIndex(getXPos(from), 1);
}

int bottomM(int from, int steps) {
	int bottom = from + steps * size;
	if (bottom <= size * size) {
		return bottom;
	}
	return convertXYToIndex(getXPos(from), size);
}


//a, b sind die nummern der felder/indizes
//returns biggest difference in x or y values
int dist(int a, int b) {
	int xa = getXPos(a);
	int ya = getYPos(a);
	int xb = getXPos(b);
	int yb = getYPos(b);

	int xdif, ydif;
	if (xa > xb) {
		xdif = xa - xb;
	} else
		xdif = xb - xa;
	if (ya > yb) {
		ydif = ya - yb;
	} else
		ydif = yb - ya;

	if (xdif < ydif)
		return xdif;
	else
		return ydif;
}

int xdiff(int a, int b) {
	int xa = getXPos(a);
	int xb = getXPos(b);
	if(xa > xb) {
		return xa-xb;
	}
	return xb-xa;
}


int ydiff(int a, int b) {
	int xa = getYPos(a);
	int xb = getYPos(b);
	if(xa > xb) {
		return xa-xb;
	}
	return xb-xa;

}
//a=6, b=1
//ceil((6-1)-dist(a,b))
//---------------------
//       2
int max(int a, int b) {
	int distance = dist(a, b);
	int valueA = values.at(a-1);
	int valueB = values.at(b-1);

	if (valueA > valueB) {
		return floor((float) ((valueA - valueB) - distance) / 2);

	} else {
		return floor((float) ((valueB - valueA) - distance) / 2);
	}
}


// order = 0: a tl, b br
// order = 1: a bl, b tr
void getRectangle(int a, int b, int valuesMap[], int order, int x, int y) {
	int xa = getXPos(a);
	int ya = getYPos(a);
	int xb = getXPos(b);
	int yb = getYPos(b);

	int index;
	if(order == 0) {
		for(int j=ya; j<=yb; j++) {
			for(int i=xa; i<=xb; i++) {
				index = convertXYToIndex(i, j);
				if(values.at(index-1) == 0) {
					for(int k = x+1; k<y; k++) {
						if(valuesMap[index*size*size+k] == 2) {
							cout <<"break"<< endl;
							break;
						}
						valuesMap[index*size*size + k] = 1;
					}
				}
			}
		}
	} else if (order == 1) {
		for(int i=xa; i<=xb; i++) {
			for(int j=yb; j<=ya; j++) {
				index = convertXYToIndex(i, j);
				if(values.at(index-1) == 0) {
					for(int k = x+1; k<y; k++) {
						if(valuesMap[index*size*size+k] == 2) {
							cout <<"break"<<endl;
							break;
						}
						valuesMap[index*size*size + k] = 1;
					}
				}

			}
		}
	}

}

void getMaxRange(int a, int b, int valuesMap[]) {
	int xa = getXPos(a);
	int ya = getYPos(a);
	int xb = getXPos(b);
	int yb = getYPos(b);
	int m = max(a, b);
	int valDif;
	int valA = values.at(a-1);
	int valB = values.at(b-1);
	if(valA > valB)
		valDif = valA-valB;
	else valDif = valB - valA;
	int xdif = xdiff(a,b);
	if(m == 0 && (xdif == ydiff(a,b) && valDif == xdif )) {
		//a links
		if(xa <= xb) {
			//a links oben
			if(ya <= yb) {
				if(valA>valB) {
					for(int i=1; i<xb-xa; i++) {
						values.at(a+(size+1)*i-1) = valA-i;
						valuesMap[size*size*(a+(size+1)*i) + valA-i] = 2;
					}
				} else {
					for(int i=1; i<xb-xa; i++) {
						values.at(a+(size+1)*i-1) = valA+i;
						valuesMap[size*size*(a+(size+1)*i) + valA+i] = 2;
					}
				}
			} else { //a links unten
				if(valA>valB) {
					for(int i=1; i<xb-xa; i++) {
						values.at(a+(-size+1)*i-1) = valA-i;
						valuesMap[size*size*(a+(-size+1)*i) + valA-i] = 2;
					}
				} else {
					for(int i=1; i<xb-xa; i++) {
						values.at(a+(-size+1)*i-1) = valA+i;
						valuesMap[size*size*(a+(-size+1)*i) + valA+i] = 2;
					}
				}

			}
		} else {//a rechts
			if(ya<=yb) { //a rechts oben
				if(valA>valB) {
					for(int i=1; i<xa-xb; i++) {
						values.at(a+(size-1)*i-1)= valA-i;
						valuesMap[size*size*(a+(size-1)*i) + valA-i] = 2;
					}
				} else {
					for(int i=1; i<xa-xb; i++) {
						values.at(a+(size-1)*i-1)= valA+i;
						valuesMap[size*size*(a+(size-1)*i) + valA+i] = 2;
					}
				}

			} else { // a rechtes unten
				if(valA>valB) {
					for(int i=1; i<xa-xb; i++) {
						values.at(a+(-size-1)*i-1)= valA-i;
						valuesMap[size*size*(a+(-size-1)*i) + valA-i] = 2;
					}
				} else {
					for(int i=1; i<xa-xb; i++) {
						values.at(a+(-size-1)*i-1)= valA+i;
						valuesMap[size*size*(a+(-size-1)*i) + valA+i] = 2;
					}
				}
			}

		}
		return;

	}


	int cornerA;
	int cornerB;
	//a links
	if(xa <= xb) {
		//a links oben
		if(ya <= yb) {
			cornerA = topM(leftM(a, m), m);

			cornerB = bottomM(rightM(b,m), m);
			getRectangle(cornerA, cornerB, valuesMap, 0, values.at(a-1), values.at(b-1));
		} else { //a links unten
			cornerA = bottomM(leftM(a, m), m);
			cornerB = topM(rightM(b, m), m);
			getRectangle(cornerA, cornerB, valuesMap, 1, values.at(a-1), values.at(b-1));
		}
	} else {//a rechts
		if(ya<=yb) { //a rechts oben
			cornerA = topM(rightM(a, m), m);
			cornerB = bottomM(leftM(b, m), m);
			getRectangle(cornerB, cornerA, valuesMap, 1, values.at(a-1), values.at(b-1));
		} else { // a rechtes unten
			cornerA = bottomM(rightM(a, m), m);
			cornerB = topM(leftM(b, m), m);
			getRectangle(cornerB, cornerA, valuesMap, 0, values.at(a-1), values.at(b-1));
		}

	}
}


Field* encoding;
int* encodingReverse;
int encodingCount = 1;

int encode(int index, int value) {
	//if encoding contains already (index, value) return it
	int tmp = encodingReverse[index * size * size + value];
	if (tmp != 0) {
		return tmp;
	}
	encodingReverse[index * size * size + value] = encodingCount;

	Field f = { index, value };
	encoding[encodingCount] = f;
//	cout << "(" << index << ", " << value << ") \t\t->" << encodingCount << endl;
	encodingCount++;

	return encodingCount - 1;
}

Field decode(int number) {
	//cout <<number << "\t\t-> (" << encoding[number].x << ", " << encoding[number].y << ")" << endl;
	return encoding[number];
}

//the vector need to have the first element on position with index 1 -> write 0 in index 0
void fillData(vector<int> * possibleValues, vector<int>* emptyFields) {
	//fill all Values in possible Values field
	for (int j = 1; j <= size * size; j++) {
		possibleValues->push_back(j);
	}
	for (int i = 1; i < size * size + 1; i++) {
		int value = values.at(i - 1);
		//if field is empty add it to emptyField
		if (value == 0) {
			emptyFields->push_back(i);
		}
		//if not empty create a Point and save it to values and remove it from possible values
		else {
			possibleValues->erase(
			    remove(possibleValues->begin(), possibleValues->end(),
			           value), possibleValues->end());
		}
	}
}

void parseHidoku(char* path) {
	ifstream myfile(path);
	char c;
	string number;
	int flags = 0;
	int lineIndex = 0;
	int linesize = 0;
	int count;

	// read out size
	myfile.ignore(17);
	myfile >> size;
	linesize = (3 + ceil(log10(size * size + 0.5))) * size + 2;
	//cout << "size: " << size << "linesize: " << linesize << endl;
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
				values.push_back(0);
				flags |= EMPTY_FIELD;
				lineIndex++;
				count++;
				//cout << " EMPTY ";
				break;
			default:
				flags = 0;
				values.push_back(atoi(number.c_str()));
				int ignores = ceil(log10(size * size + 0.5));
				ignores -= ceil(log10(atoi(number.c_str()) + 0.5));
//				cout << ignores;
				myfile.ignore(ignores);
				lineIndex++;
				count++;
				//	cout << " " << number << " ";
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
	do {
		code->get(c);
		cout << c;
		if (c != ' ') {
			number += c;
		}
	} while (c != ' ');
	result = atoi(number.c_str());
	return result;
}

int parseSolution(string solution, int values[]) {
	ifstream solFile(solution);
	string line;
	string number;
	if (solFile.is_open()) {
		getline(solFile, line);
		if (line == "UNSAT") {
			return 0;
		}
		bool skip = 0;
		while (getline(solFile, line)) {
			for (size_t i = 0; i < line.size(); ++i) {
				char c = line[i];

				//new number read
				if (c == ' ') {
					skip = 0;
					if (!number.empty()) {
						Field cell = decode(atoi(number.c_str()));
						//										cout << cell.x << "," << cell.y   << " -> " << number << endl;
						values[(cell.x - 1)] = cell.y;
						number.clear();
					}
				} else //negative number, skip it!
					if (c == '-') {
						skip = 1;
						i++;
					} else {
						if (skip == 0) {
							number += c;
						}
					}
			}
		}
		solFile.close();
	}
}

string getExactlyOne(vector<int>* vars) {
	Field a = decode(vars->at(0));
	string result = to_string(vars->at(0));
//	result += ": (" + to_string(a.x) + ", " + to_string(a.y) + ") ";
	for (int i = 1; i < vars->size(); i++) {
		a = decode(vars->at(i));
		result += OR + to_string(vars->at(i));
		//	result += ": (" + to_string(a.x) + ", " + to_string(a.y) + ") ";
	}

	for (int i = 0; i < vars->size(); i++) {
		for (int j = i + 1; j < vars->size(); j++) {
			result += AND() + NOT + to_string(vars->at(i)) + OR + NOT
			          + to_string(vars->at(j));
		}
	}
	return result + AND();
}

string computeClauses(vector<Field>*sortedValues) {
	int n = size * size;


	vector<int>* possibleValues = new vector<int>();

	for (int j = 1; j <= size * size; j++) {
		possibleValues->push_back(j);
	}
	for (int i = 1; i < size * size + 1; i++) {
		int value = values.at(i - 1);
		possibleValues->erase(
		    remove(possibleValues->begin(), possibleValues->end(),
		           value), possibleValues->end());
	}

	string result;

	for (int i = 1; i <= n; i++) {
		if (values.at(i - 1) != 0) {
			result += to_string(encode(i, values.at(i - 1))) + AND();
		}
	}


	Field a,b;
	vector<int>* vars = new vector<int>();

	valueMap = new int[size*size*size*size*size];


	for(int i=0; i<sortedValues->size()-1; i++) {
		a=sortedValues->at(i);
		b=sortedValues->at(i+1);
		if(i == 0) {
			for(int k=1; k<a.y+1-1; k++) {
				for(int l=1; l<=size*size; l++) {
					valueMap[l*size*size + k] = 1;
				}
			}
		} else if (i == sortedValues->size()-2) {
			for(int k=b.y+1; k<=size*size; k++) {
				for(int l=1; l<=size*size; l++) {
					valueMap[l*size*size + k] = 1;
				}
			}
		}

		if(a.y+1 == b.y) {
			continue;
		}
//		cout << "a: " << a.y << " b: " << b.y << endl;


		getMaxRange(a.x, b.x, valueMap);

		/*
				cout <<"val map:" <<endl;
			for(int k=1; k<=size*size; k++) {
				for(int j=1; j<=size*size; j++) {
					if(valueMap[size*size*k+j] !=0)
						cout << "(" << k << ", " << j << ") \t-> " << valueMap[size*size*k + j] << endl;
				}
				cout << endl;
			}*/

	}

	int ctr = 0, tmp=0, ctr2 =0, v;
	for(int i=0; i<n; i++) {
		for(int j=0; j<n; j++) {
			v = valueMap[n*i+j];
			if(v == 1) {
				if(ctr == 0) {
					tmp=j;
				}
				ctr++;
			}
			if(v == 2) {
				if(ctr2 == 1) {
					return "ausDieMaus";
				}
				ctr2++;
			}
		}
		if(ctr == 1) {
			values.at(i)= tmp;
		}
		ctr2 = 0;
		ctr = 0;
	}

	//result = "schritt 1: jede zahl kommt im Spielfeld genau einmal vor";

	//nach sortieren wird diese for schleife nicht von k=0 bis possibleValues->size() gehen, sondern in einzelnen
	//schritten von a bis b, und so weiter
	for (int k = 1; k <= size*size; k++) { //possible Value
		for (int i = 1; i <=size*size; i++) { //empty Fields
			if(valueMap[i*size*size + k] != 0) {
				vars->push_back(encode(i, k));
			}
		}
		if(vars->size() > 0) {
			result += getExactlyOne(vars);
			vars->clear();
		}
	}

	result += "\n";





//	result = result + "schritt 2: Jedes Feld hat einen Nachbarn mit einer kleineren Zahl:" + "\n";
	for (int i = 1; i <= n; i++) { //i represents the field index
		/**
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
ende2:
		int neighbours[8] = { top(i, 1), left(i, 1), right(i, 1), bottom(i, 1) };

		neighbours[4] = top(neighbours[1], 1);
		neighbours[5] = top(neighbours[2], 1);
		neighbours[6] = bottom(neighbours[1], 1);
		neighbours[7] = bottom(neighbours[2], 1);
//	cout << i << ": "<< values.at(i-1) <<endl;
		if (values.at(i - 1) == 0) {
			for (int j = 2; j <=size*size; j++) { // possible values
ende:
				if(find(possibleValues->begin(), possibleValues->end(), j-1) == possibleValues->end()) {
					continue;
				}
				if(valueMap[size*size*i + j] != 0) {
					//wenn der mögliche kleinere nachbar sowieso kein möglicher wert ist
					//die eins hat keinen unteren nachbarn
					if (j < 2) {
						//cout << "lasse aus: (" << i << ", " << j-1 << "), " << endl;
						continue;
					}

					int nk;
					for (int k = 0; k < 8; k++) {
						nk = neighbours[k];
						if(nk != 0) {
							//wenn es bereits einen unteren nachbarn gibt, oder es keinen geben kann, kannstes lassen
							if(values.at(nk-1) == j-1) {
								//cout << "lasse aus: (" << nk << ", " << j-1 << "), " << endl;
								j++;
								if(j > size*size) {
									i++;
									goto ende2;
								}
								goto ende;
							}
						}
					}

					result += NOT + to_string(encode(i, j));
					//	result += ":(" + to_string(i) + ", " + to_string(j) + ") ";

					for (int k = 0; k < 8; k++) {
						nk = neighbours[k];
						if(nk != 0) {
							if ( values.at(nk-1) ==  0  && valueMap[size*size*nk + j-1] != 0) {
								result += OR
								          + to_string(
								              encode(nk, j - 1));

								//result += ":(" + to_string(nk) + ", " + to_string(j-1) + ") ";
							}
						}

					}
					result += AND();
				}
			}
		} else {
			if (values.at(i - 1) > 1 && find(values.begin(), values.end(), values.at(i - 1) - 1) == values.end()) {
				int tmp = 0, nk;
				for (int k = 0; k < 8; k++) {
					nk = neighbours[k];
					if(nk != 0) {
						if (  valueMap[size*size*nk + values.at(i-1)-1] != 0) {
							if (tmp == 1) {
								result += OR;
							}
							result += to_string(encode(nk, values.at(i - 1) - 1));
							//result += "(" + to_string(nk) + ", " + to_string(values.at(i-1)-1) + ") ";
							tmp = 1;
						}
					}

				}
				result += AND();
			}

			if (values.at(i - 1) < n && find(values.begin(), values.end(), values.at(i - 1) + 1) == values.end()) {
				int tmp = 0, nk;
				for (int k = 0; k < 8; k++) {
					nk = neighbours[k];
					if(nk != 0) {
						if (  valueMap[size*size*nk + values.at(i-1)+1] != 0) {
							if (tmp == 1) {
								result += OR;
							}
							result +=to_string(
							             encode(nk, values.at(i - 1) + 1));
							//result += "(" + to_string(nk) + ", " + to_string(values.at(i-1)+1) + ") ";
							tmp = 1;
						}
					}

				}
				result += AND() + "\n";
			}
		}

	}


//result = result + "\n" + "schritt 3: Nur 1 Zahl pro Feld" + "\n";

	for (int i = 1; i <=size*size; i++) { //empty fields
		for (int k = 1; k <= size*size; k++) { //possibleValues
			if(valueMap[i*size*size + k] == 1) {
				vars->push_back(encode(i,k));
			}
		}
		if(vars->size() > 0) {
			result += getExactlyOne(vars);
			vars->clear();
		}

	}

	return result;

}


bool compareFields (Field i,Field j) {
	return (i.y < j.y);
}

int main(int argc, char* argv[]) {


	parseHidoku(argv[1]);

	int newValues[size*size];
	//test auf triviales hidoku
	int linecounter = -1;
	bool test = true;
	for(int i=0; i<values.size(); i++) {
		if(i%size == 0)
			linecounter++;
		if(linecounter%2 == 1) {
			if(values.at(i) != 0 && values.at(i) != i+1) {
				test = false;
				break;
			}
		} else {
			if(values.at(i) != 0 && values.at(i) != size*linecounter-i%size) {
				test = false;
				break;
			}
		}
	}

	linecounter = 0;
	if(test == true) {
		for(int i=0; i<size*size; i++) {
			if(i%size == 0) {
				linecounter++;
			}
			if(linecounter %2 == 0) {
				newValues[i] = size*(linecounter-1) + i%size+  1;
			} else {
				newValues[i] = size*linecounter - i%size ;
			}
		}
	} else {

		//sort values
		vector<Field>* sortedValues = new vector<Field>();

		int tmp;
		for(int i=0; i<values.size(); i++) {
			tmp = values.at(i);
			if(tmp == 0) {
				continue;
			}
			Field x = {i+1, tmp};
			sortedValues->push_back(x);
		}

		sort (sortedValues->begin(), sortedValues->end(), compareFields);

		encoding = new Field[size * size * size * size*size]();
		encodingReverse = new int[size * size * size * size *size* size]();


		string clauses = computeClauses(sortedValues);
		if(clauses == "ausDieMaus") {
			cout << "unsatAusDieMaus";
			return 20;
		}
		ofstream outfile;
		outfile.open("out.txt");
		outfile << "p cnf " << encodingCount - 1 << " " << andCtr << " \n" + clauses;
		outfile.close();

		int n = size * size;
		system("minisat out.txt result.txt > stat.txt");

		int res = parseSolution("result.txt", newValues);

		/*  	cout <<"newvalues" <<endl;
			for(int i=0; i<n; i++) {
				cout << newValues[i] << ", ";
			}
			cout <<"fertsch" <<endl;*/
		delete[] encoding;
		delete[] encodingReverse;

//weil es nachts um fünfe ist - UND SCHON DRAUẞEN DIE SONNE SCHEINT!!!!
		if (res == 0) {
			cout << "unsat";
			return 20;
		}
	}

	drawBoard(newValues, size);
	return 10;
}
