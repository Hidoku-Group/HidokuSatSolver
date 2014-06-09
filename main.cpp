#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int parseHidoku(char* filename) {
	string line;
	ifstream myfile(filename);

	if (myfile.is_open()) {
		//erste Zeile
		getline(myfile,line);
		int size;
		sscanf (line.c_str(), "hidoku with size %d",&size);
		cout << size;

		vector<int> result;
		int i=0, linecounter = 1;
		while ( getline (myfile,line) )	{
			linecounter++;
			//nur die zeilen in denen zahlen kodiert sind
			if(linecounter%4 != 0) {
				continue;
			}
			
			//zahlen extrahieren
			for(int j=0;j<size;j++) {
				
	
			}
			result[i] = line;
			cout << line << '\n';
		}

		myfile.close();
	}

}

int main() {
	parseHidoku("easy/hidoku-3-6-1.txt");
	return 10;
}


