/*
 * main.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: Suruchi
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <queue>
#include <vector>
#include <stack>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <unistd.h>

#include <iostream>
#include <string>

#include "Scheduler.h"
#include "Instruction.h"

using namespace std;

Scheduler* sched = new FIFO();
vector<Instruction> instrVector;

//Maintain data structure for instructions
void insert_instruction(Instruction instr) {
	instrVector.push_back(instr);
}

//Read and store instructions in a vector
void readInstructions(char *instrFile) {
	FILE* iFile = fopen(instrFile, "r");
	int id = 0;
	while (!feof(iFile)) {
		char buf[5000];
		fgets(buf, 5000, iFile);
		if (feof(iFile) || buf[0] == '#') {
			continue;
		} else {
			int t, l;
			sscanf(buf, "%d%d", &t, &l);
			Instruction instr(t, l, id++);
			insert_instruction(instr);
		}
	}
}
int main(int argc, char* argv[]) {
	int c;
	while ((c = getopt(argc, argv, "s:v")) != -1) {
		switch (c) {
		case 's':
			if (optarg[0] == 'i') {
				sched = new FIFO();
			} else if (optarg[0] == 'j') {
				sched = new SSTF();
			} else if (optarg[0] == 's') {
				sched = new SCAN();
			} else if (optarg[0] == 'c') {
				sched = new CSCAN();
			} else if (optarg[0] == 'f') {
				sched = new FSCAN();
			}
			break;
		}
	}

	readInstructions(argv[argc - 1]);

	sched->schedule(instrVector);

	return 0;
}
