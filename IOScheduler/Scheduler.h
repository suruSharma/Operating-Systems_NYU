/*
 * Scheduler.h
 *
 *  Created on: Apr 27, 2015
 *      Author: Suruchi
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "Instruction.h"

class Scheduler {

public:
	void schedule(std::vector<Instruction>);

	static bool sortRight(Instruction, Instruction);
	static bool sortLeft(Instruction, Instruction);

	virtual bool refreshReady(std::vector<Instruction>&);

	virtual Instruction addToQueue(std::vector<Instruction>&,
			std::vector<Instruction>&, bool);

	virtual Instruction getInstruction(std::vector<Instruction>&, int&,
			bool) = 0;
};

class FIFO: public Scheduler {
public:
	Instruction getInstruction(std::vector<Instruction>&, int&, bool);
};

class SSTF: public Scheduler {
public:
	Instruction getInstruction(std::vector<Instruction>&, int&, bool);
};

class SCAN: public Scheduler {
public:
	Instruction getInstruction(std::vector<Instruction>&, int&, bool);
};

class CSCAN: public Scheduler {
public:
	Instruction getInstruction(std::vector<Instruction>&, int&, bool);
};

class FSCAN: public Scheduler {
private:
	std::vector<Instruction> instructionVectorBuf;
public:
	bool refreshReady(std::vector<Instruction>&);
	Instruction addToQueue(std::vector<Instruction>&, std::vector<Instruction>&,
			bool);
	Instruction getInstruction(std::vector<Instruction>&, int&, bool);
};

#endif /* SCHEDULER_H_ */
