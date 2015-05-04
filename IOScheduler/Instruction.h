/*
 * Instruction.h
 *
 *  Created on: Apr 29, 2015
 *      Author: Suruchi
 */

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_


class Instruction {
    public:
        int tStamp;
        int locationOnDisk;
        int instructionID;

        Instruction(int, int, int);
};


#endif /* INSTRUCTION_H_ */
