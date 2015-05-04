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

#include "vmm.h"
#include "bit.h"

using namespace std;
VMM::VMM() {
	ofs = 0;
	numOfFrames = 0;
}

//myRandom implementation as in PDF
int VMM::getRandomNumber(int burst) {
	if (ofs == randomValues.size() - 1)
		ofs = 0;
	int returnValue = randomValues[ofs] % burst;
	ofs++;
	return returnValue;
}

void VMM::updatePage(std::vector<unsigned int>& list, unsigned int i) {
}

int NRU::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {

	vector<vector<int> > priority(4, vector<int>());

	//put page in vector as per priority
	for (int i = 0; i < pagesVector.size(); i++) {
		unsigned int p = pagesVector[i];
		if (getCurrentBit(pagesVector[i]) == 1) {
			unsigned int ref = getRefBit(p);
			unsigned int mod = getModBit(p);

			if (ref == 0 && mod == 0) {
				priority[0].push_back(getFrame(p));
			} else if (ref == 0 && mod == 1) {
				priority[1].push_back(getFrame(p));
			} else if (ref == 1 && mod == 0) {
				priority[2].push_back(getFrame(p));
			} else if (ref == 1 && mod == 1) {
				priority[3].push_back(getFrame(p));
			}
		}
	}

	int returnPage = -1;
	for (int i = 0; i < 4; i++) {
		if (priority[i].size() > 0) {
			int rand = getRandomNumber(priority[i].size());
			returnPage = priority[i][rand];
			break;
		}
	}

	count++;
	if (count == 10) {
		count = 0;
		for (int i = 0; i < pagesVector.size(); i++) {
			if (getCurrentBit(pagesVector[i]) == 1) {
				resetRefBit(pagesVector[i]);
			}
		}
	}

	return returnPage;
}

void LRU::updatePage(std::vector<unsigned int>& frameVector, unsigned int num) {
	for (int i = 0; i < frameVector.size(); i++) {
		if (frameVector[i] == num) {
			frameVector.erase(frameVector.begin() + i);
			break;
		}
	}
	frameVector.push_back(num);
}

int LRU::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {
	unsigned int frameNumber = frames.front();
	frames.erase(frames.begin());

	frames.push_back(frameNumber);
	return frames.back();
}

int Randoom::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {
	int rand = getRandomNumber(frames.size());
	return frames[rand];
}

int FIFO::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {
	unsigned int frameNumber = frames.front();
	frames.erase(frames.begin());
	frames.push_back(frameNumber);
	return frames.back();
}

int SecChance::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {

	int flag = 0;
	unsigned int frameNum, pg;

	while (!flag) {
		frameNum = frames.front();
		pg = rev_frames[frameNum];
		if (getRefBit(pagesVector[pg]) == 1) {
			resetRefBit(pagesVector[pg]);
			frames.erase(frames.begin());
			frames.push_back(frameNum);
		} else {
			flag = 1;
			frames.erase(frames.begin());
			frames.push_back(frameNum);
		}
	}
	return frames.back();
}

int PhysicalClock::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {
	int flag = 0;
	unsigned int frameNum, pg;

	while (!flag) {
		frameNum = frames[count];
		pg = rev_frames[frameNum];

		if (getRefBit(pagesVector[pg]) == 1) {
			resetRefBit(pagesVector[pg]);
		} else {
			flag = 1;
		}
		count = (count + 1) % frames.size();
	}
	return frameNum;
}

int VirtualClock::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {
	int flag = 0;
	unsigned int ret;

	while (!flag) {
		if (getCurrentBit(pagesVector[count]) == 1) {
			if (getRefBit(pagesVector[count]) == 1) {
				resetRefBit(pagesVector[count]);
			} else {
				flag = 1;
				ret = getFrame(pagesVector[count]);
			}
		}
		count = ((count + 1) % pagesVector.size());
	}

	return ret;
}

int PhysicalAging::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {

	unsigned int mAge = 0xffffffff;
	unsigned int mFrame = -1;
	unsigned int mIndex = -1;

	//initialize if it is not initialized yet
	if (ageVector.size() == 0) {
		ageVector = std::vector<unsigned int>(numOfFrames, 0);
	}

	for (int i = 0; i < frames.size(); i++) {

		ageVector[i] = (ageVector[i] >> 1)
				| ((getRefBit(pagesVector[rev_frames[frames[i]]])) << 31);

		if (ageVector[i] < mAge) {
			mAge = ageVector[i];
			mFrame = frames[i];
			mIndex = i;
		}

		resetRefBit(pagesVector[rev_frames[frames[i]]]);

	}
	ageVector[mIndex] = 0;//reset the min age counter

	return mFrame;
}

int VirtualAging::getPage(vector<unsigned int>& pagesVector,
		vector<unsigned int>& frames, vector<unsigned int>& rev_frames) {

	unsigned int mAge = 0xffffffff;
	unsigned int mIndex = -1;

	//initialize if it is not initialized yet
	if (ageVector.size() == 0) {
		ageVector = std::vector<unsigned int>(64, 0);
	}

	for (int i = 0; i < pagesVector.size(); i++) {
		unsigned int pg = pagesVector[i];
		ageVector[i] = (ageVector[i] >> 1)
				| ((getRefBit(pg)) << 31);

		if (getCurrentBit(pg) == 1 && ageVector[i] < mAge) {
			mAge = ageVector[i];
			mIndex = i;
		}

		if (getCurrentBit(pg) == 1) {
			resetRefBit(pagesVector[i]);
		}
	}
	ageVector[mIndex] = 0;

	//cout<<"**"<<mIndex<<endl;

	return getFrame(pagesVector[mIndex]);
}
