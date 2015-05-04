#ifndef BIT_H
#define BIT_H

void setFrame(unsigned int&, int);
unsigned int getFrame(unsigned int);

void setCurrentBit(unsigned int&);
void setModBit(unsigned int&);
void setRefBit(unsigned int&);
void setOutBit(unsigned int&);

void resetCurrnetBit(unsigned int&);
void resetModBit(unsigned int&);
void resetRefBit(unsigned int&);
void resetOutBit(unsigned int&);

unsigned int getCurrentBit(unsigned int);
unsigned int getModBit(unsigned int);
unsigned int getRefBit(unsigned int);
unsigned int getOutBit(unsigned int);

#endif
