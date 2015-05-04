/*
 * Instruction.cpp
 *
 *  Created on: Apr 29, 2015
 *      Author: Suruchi
 */

#include "Instruction.h"

Instruction::Instruction(int timestamp, int location, int instrID) {
	tStamp = timestamp;
	locationOnDisk = location;
	instructionID = instrID;
}

