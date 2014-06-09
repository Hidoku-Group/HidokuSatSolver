#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

/**
 * returns the number of the field that is "steps" steps in the left direction of "from"
 * if there is no such field in this direction simply 0 will be returned
 */
int left(int size, int from, int steps) {
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
	int right = from+steps, d = from / size;

	if(from%size == 0) {
		return 0;
	} else if(right <= (d+1)*size){
		return right;
	}
	return 0;
}

int top(int size, int from, int steps) {
	int top = from-steps*size;
	if(top > 0) {
		return top;
	}
	return 0;
}

int bottom(int size, int from, int steps) {
	int bottom = from + steps * size;
	if(bottom <= size*size) {
		return bottom;
	}
	return 0;
}


int computeClauses(int size) {
	int n = size*size;

	printf("schritt 1: jede zahl kommt im Spielfeld genau einmal vor\n");
	for(int k=1; k<=n; k++) {
		for(int i=1; i<=n; i++) {
			for(int j=1; j<=n; j++) {
				printf("(");
				if(k == j) {
					printf(" (%d, %d) ∧ ", j,i);
				} else {
					printf("¬(%d, %d) ∧ ", j,i);
				}
			}
			//remove last ∧ 
			printf(") ∨ \n");
		}
		printf("\n");
	}

	printf("\n");

	printf("schritt 2: Jedes Feld hat einen Nachbarn mit einer kleineren Zahl:\n");
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

	printf("schritt 3: Nur 1 Zahl pro Feld\n");
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
	printf("%d", bottom(3, 9, 1));
	//computeClauses(3);
	return 10;
}
