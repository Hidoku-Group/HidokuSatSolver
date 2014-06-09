#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int parseHidoku(char* filename) {
/* /	string line;
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
*/
}

int computeClauses(int size) {
	int n = size*size;
	//schritt 1: Jede Zahl kommt im Spielfeld genau einmal vor
	//es fehlt noch einiges, bis jetzt kann im Feld 1 nur die 1 stehen, im Feld 2 nur die 2, etc.
	for(int i=1; i<=n; i++) {
		for(int j=1; j<=n; j++) {
			printf("(");
			if(i == j) {
				printf(" (%d, %d) ∧ ", j,i);
			} else {
				printf("¬(%d, %d) ∧ ", j,i);
			}
		}
		//remove last ∧ 
		printf(") ∨ \n");
	}

	printf("\n");

	//schritt 2: Jedes Feld hat einen Nachbarn mit einer kleineren Zahl:
	for(int i=1; i<=n; i++) {
		for(int j=1; j<=n; j++) {
			printf("(%d, %d) → (", i, j);

			//test mit größer null klappt nur für den linken rand, für den rechten muss was geschickteres her!
			if(i-size-1 > 0)
				printf("(%d, %d) ∨ ");
			if(i-size>0)
				printf("(%d, %d) ∨ ");
			if(i-size+1>0)
				printf("(%d, %d) ∨ ");
			if(i-1>0)
			if(i+1>0)
				printf("hi");
		}
	}

	printf("\n");

	//schritt 3: Nur 1 Zahl pro Feld
	for(int i=1; i<=n; i++) {
		printf("( (%d, %d) ∧ ", i, i);
		for(int j=1; j<=n; j++) {
			if(i == j)
				continue;
			printf("¬(%d, %d) ∧ ", i, j);
		}
		//remove last ∧
		printf(") ∨ \n");
	}


}

int main() {
	//	parseHidoku("easy/hidoku-3-6-1.txt");
	computeClauses(3);
	return 10;
}


