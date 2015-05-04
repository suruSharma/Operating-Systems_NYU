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

#include "bit.h"

void setFrame(unsigned int& pg, int fr) {
	pg = (pg & 0xf0000000) | fr;
}

unsigned int getFrame(unsigned int pg) {
	return ((pg << 4) >> 4);
}

void setCurrentBit(unsigned int& pg) {
	unsigned int mask = 0x80000000;
	pg |= mask;
}

void setModBit(unsigned int& pg) {
	unsigned int mask = 0x40000000;
	pg |= mask;
}

void setRefBit(unsigned int& pg) {
	unsigned int mask = 0x20000000;
	pg |= mask;
}

void setOutBit(unsigned int& pg) {
	unsigned int mask = 0x10000000;
	pg |= mask;
}
void resetCurrnetBit(unsigned int& pg) {
	unsigned int mask = 0x7fffffff;
	pg &= mask;
}
void resetModBit(unsigned int& pg) {
	unsigned int mask = 0xbfffffff;
	pg &= mask;
}
void resetRefBit(unsigned int& pg) {
	unsigned int mask = 0xdfffffff;
	pg &= mask;
}
void resetOutBit(unsigned int& pg) {
	unsigned int mask = 0xefffffff;
	pg &= mask;
}

unsigned int getCurrentBit(unsigned int pg) {
	return ((pg >> 31) | 0);
}

unsigned int getModBit(unsigned int pg) {
	return ((pg >> 30) & 1) | 0;
}

unsigned int getRefBit(unsigned int pg) {
	return ((pg >> 29) & 1) | 0;
}

unsigned int getOutBit(unsigned int pg) {
	return ((pg >> 28) & 1) | 0;
}
