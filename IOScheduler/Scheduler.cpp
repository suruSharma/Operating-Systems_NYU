/*
 * Scheduler.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: Suruchi
 */
#include "Scheduler.h"
#include "Instruction.h"

using namespace std;

bool Scheduler::refreshReady(vector<Instruction>& ready) {
	return false;
}

Instruction Scheduler::addToQueue(vector<Instruction>& instructionVector,
		vector<Instruction>& ready, bool locating) {
	Instruction frontInstr = instructionVector.front();
	instructionVector.erase(instructionVector.begin());
	ready.push_back(frontInstr);
	return frontInstr;
}

void Scheduler::schedule(vector<Instruction> instructionVector) {
	int count = 0;
	int instrCount = instructionVector.size();
	int tot_movement = 0;
	int tot_turnAround = 0;
	int tot_waitTime = 0;
	int max_waitTime = 0;
	int current = 0;
	int previous = 0;
	int total_time = 0;
	vector<Instruction> readyVector;
	bool found = false;
	Instruction instr(0, 0, 0);
	while (count != instrCount) {
		if (!found && !readyVector.empty()) {
			bool diff = current - previous >= 0 ? true : false;
			instr = this->getInstruction(readyVector, current, diff);
			found = true;
			tot_movement += abs(current - instr.locationOnDisk);
			int curr_waitTime = total_time - instr.tStamp;
			tot_waitTime += curr_waitTime;
			if (curr_waitTime > max_waitTime) { //keep checking for maximum wait time
				max_waitTime = curr_waitTime;
			}
		}
		if (!found) {
			Instruction returnedInstr = this->addToQueue(instructionVector,
					readyVector, found);
			total_time = returnedInstr.tStamp;
		} else {
			int arrivalTime = total_time + abs(current - instr.locationOnDisk);
			while (!instructionVector.empty()
					&& instructionVector[0].tStamp <= arrivalTime) {
				Instruction returnedInstr = this->addToQueue(instructionVector,
						readyVector, found);
				total_time = returnedInstr.tStamp;
			}
			count++;
			if (instr.locationOnDisk != current) {
				previous = current;
			}
			current = instr.locationOnDisk;
			total_time = arrivalTime;
			found = false;
			int tt = total_time - instr.tStamp;
			tot_turnAround += tt;
		}
		if (this->refreshReady(readyVector)) {
			previous = current;
		}
	}
	double avg_turnAround = (double) tot_turnAround / count;
	double avg_waitTime = (double) tot_waitTime / count;
	printf("SUM: %d %d %.2lf %.2lf %d\n", total_time, tot_movement,
			avg_turnAround, avg_waitTime, max_waitTime);
}

Instruction FIFO::getInstruction(vector<Instruction>& readyVector, int& current,
		bool right) {
	//return the instruction on the front
	Instruction returnedInstr = readyVector.front();
	readyVector.erase(readyVector.begin());
	return returnedInstr;
}

Instruction SSTF::getInstruction(vector<Instruction>& readyVector, int& current,
		bool right) {
	unsigned int location = 2147483647; //0xffffffff;
	int index = -1;
	for (int i = 0; i < readyVector.size(); ++i) {
		int tempLocation = abs(current - readyVector[i].locationOnDisk);
		if (tempLocation < location) {
			location = tempLocation;
			index = i;
		}
	}
	Instruction returnedInstr = readyVector[index];
	readyVector.erase(readyVector.begin() + index);
	return returnedInstr;
}
bool Scheduler::sortRight(Instruction a, Instruction b) {
	return a.locationOnDisk == b.locationOnDisk ?
			a.instructionID < b.instructionID :
			a.locationOnDisk < b.locationOnDisk;
}
bool Scheduler::sortLeft(Instruction a, Instruction b) {
	return a.locationOnDisk == b.locationOnDisk ?
			a.instructionID > b.instructionID :
			a.locationOnDisk < b.locationOnDisk;
}

Instruction SCAN::getInstruction(vector<Instruction>& readyVector, int& current,
		bool right) {
	int index;
	if (right) {
		sort(readyVector.begin(), readyVector.end(), sortRight);
	} else {
		sort(readyVector.begin(), readyVector.end(), sortLeft);
	}
	if (!right) {
		index = 0;
		for (int i = readyVector.size() - 1; i >= 0; --i) {
			if (readyVector[i].locationOnDisk <= current) {
				index = i;
				break;
			}
		}
	} else {
		index = readyVector.size() - 1;
		for (int i = 0; i < readyVector.size(); ++i) {
			if (readyVector[i].locationOnDisk >= current) {
				index = i;
				break;
			}
		}
	}
	Instruction returnedInstr = readyVector[index];
	readyVector.erase(readyVector.begin() + index);
	return returnedInstr;
}

Instruction CSCAN::getInstruction(vector<Instruction>& readyVector,
		int& current, bool right) {
	int index = 0;
	sort(readyVector.begin(), readyVector.end(), sortRight);
	for (int i = 0; i < readyVector.size(); ++i) {
		if (readyVector[i].locationOnDisk >= current) {
			index = i;
			break;
		}
	}
	Instruction returnedInstr = readyVector[index];
	readyVector.erase(readyVector.begin() + index);
	return returnedInstr;
}

bool FSCAN::refreshReady(vector<Instruction>& readyVector) {
	if (readyVector.empty()) {
		readyVector = instructionVectorBuf;
		instructionVectorBuf.clear();
		return true;
	} else {
		return false;
	}
}

Instruction FSCAN::addToQueue(vector<Instruction>& instructionVector,
		vector<Instruction>& readyQueue, bool found) {
	Instruction returnedInstr = instructionVector.front();
	instructionVector.erase(instructionVector.begin());
	if (!found) {
		readyQueue.push_back(returnedInstr);
	} else {
		instructionVectorBuf.push_back(returnedInstr);
	}
	return returnedInstr;
}

Instruction FSCAN::getInstruction(vector<Instruction>& readyQueue, int& current,
		bool right) {
	int index;
	if (right) {
		sort(readyQueue.begin(), readyQueue.end(), sortRight);
	} else {
		sort(readyQueue.begin(), readyQueue.end(), sortLeft);
	}
	if (!right) {
		index = 0;
		for (int i = readyQueue.size() - 1; i >= 0; --i) {
			if (readyQueue[i].locationOnDisk <= current) {
				index = i;
				break;
			}
		}
	} else if (right) {
		index = readyQueue.size() - 1;
		for (int i = 0; i < readyQueue.size(); ++i) {
			if (readyQueue[i].locationOnDisk >= current) {
				index = i;
				break;
			}
		}
	}
	Instruction returnedVector = readyQueue[index];
	readyQueue.erase(readyQueue.begin() + index);
	return returnedVector;
}
