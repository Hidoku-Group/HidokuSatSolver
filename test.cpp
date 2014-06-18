#include <stdio.h>
#include <ctime>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>


using namespace std;
//100 millionen: zuerst string +=, dann ins file
int main() {
	clock_t begin = clock();

	ofstream outfile;
	outfile.open("out.txt");

	//string res = "";
    for(int i=0; i<100000000; i++) {
		outfile << "hello";
	}
//outfile << res;
outfile.close();
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	
	cout << "verbraucht: " << elapsed_secs;
	return 1;
}
