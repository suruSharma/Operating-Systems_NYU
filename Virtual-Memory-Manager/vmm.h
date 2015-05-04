#ifndef VMM_H
#define VMM_H

#include <iostream>
#include <vector>

class VMM {

public:
	int ofs;
	int numOfFrames;
	std::vector<int> randomValues;
	VMM();

	int getRandomNumber(int);

	virtual void updatePage(std::vector<unsigned int>&, unsigned int);

	virtual int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&) = 0;
};

class NRU: public VMM {
private:
	int count;
public:
	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

class LRU: public VMM {
public:
	void updatePage(std::vector<unsigned int>&, unsigned int);

	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

class Randoom: public VMM {
public:
	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

class FIFO: public VMM {
public:
	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

class SecChance: public VMM {
public:
	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

class PhysicalClock: public VMM {
private:
	int count;
public:
	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

class VirtualClock: public VMM {
private:
	int count;
public:
	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

class PhysicalAging: public VMM {
private:
	std::vector<unsigned int> ageVector;
public:
	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

class VirtualAging: public VMM {
private:
	std::vector<unsigned int> ageVector;
public:
	int getPage(std::vector<unsigned int>&, std::vector<unsigned int>&,
			std::vector<unsigned int>&);
};

#endif
