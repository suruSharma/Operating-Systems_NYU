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

int frameCount;

bool O, P, F, S;

VMM* algo;

std::vector<int> randomValues;
int totalRandomValues = 0;
int instructions = 0;
int unmapped = 0;
int mapped = 0;
int in = 0;
int out = 0;
int zero = 0;
std::vector<unsigned int> pageVector = std::vector<unsigned int>(64, 0);
std::vector<unsigned int> frames;
std::vector<unsigned int> reverseFrames;
unsigned int limit;

using namespace std;
class Instructions {
public:
	unsigned int op;
	unsigned int page;
	Instructions(int, int);
};

Instructions::Instructions(int o, int p) {
	op = o;
	page = p;
}
std::vector<Instructions> instrVector;

//Maintain data structure for instructions
void insert_instruction(Instructions instr) {
	instrVector.push_back(instr);
}

//Read and store instructions in a vector
void readInstructions(char *instrFile) {
	FILE* iFile = fopen(instrFile, "r");
	while (!feof(iFile)) {
		char buf[5000];
		fgets(buf, 5000, iFile);
		if (feof(iFile) || buf[0] == '#') {
			continue;
		} else {
			int op;
			int page;
			sscanf(buf, "%d%d", &op, &page);
			Instructions instr(op, page);
			insert_instruction(instr);
		}
	}
}

//Read and store the ransom variables in the vmm class for algos to use
void readRandomNumbers(char *rFile) {
	ifstream fin(rFile);
	string word;

	fin >> word;
	fin >> word;
	while (!fin.eof()) {
		int num = atoi(word.c_str());
		algo->randomValues.insert(algo->randomValues.begin() + algo->ofs, num);
		algo->ofs++;
		fin >> word;
	}
	fin.close();
	algo->ofs = 0;
}

void parseCommand(int argc, char* argv[]) {

	//getops implemented as given
	int c;
	int optLen;
	while ((c = getopt(argc, argv, "a:o:f:")) != -1) {
		switch (c) {
		case 'o':
			optLen = strlen(optarg);
			for (int i = 0; i < optLen; ++i) {
				switch (optarg[i]) {
				case 'O':
					O = true;
					break;
				case 'P':
					P = true;
					break;
				case 'F':
					F = true;
					break;
				case 'S':
					S = true;
					break;
				}
			}
			break;
		case 'a':
					switch (optarg[0]) {
					case 'N':
						algo = new NRU();
						break;
					case 'l':
						algo = new LRU();
						break;
					case 'r':
						algo = new Randoom();
						break;
					case 'f':
						algo = new FIFO();
						break;
					case 's':
						algo = new SecChance();
						break;
					case 'c':
						algo = new PhysicalClock();
						break;
					case 'X':
						algo = new VirtualClock();
						break;
					case 'a':
						algo = new PhysicalAging();
						break;
					case 'Y':
						algo = new VirtualAging();
						break;
					default:
						break;
					}
					break;
		case 'f':
			sscanf(optarg, "%d", &frameCount);
			break;
		}
	}

	readRandomNumbers(argv[argc - 1]);//store the random numbers in the vmm class for algos to use

	readInstructions(argv[argc - 2]);//read the instructions file and store them in the instructions vector

	algo->numOfFrames = frameCount;

	reverseFrames = std::vector<unsigned int>(frameCount, 0);//cannot be initialized before the framecount is available

}

void mapPageFrame(unsigned int op, unsigned int pg) {
	unsigned int& page = pageVector[pg];
	//cout<<"Page = "<<pageVector[pg]<<endl;
	if (O) {
		printf("==> inst: %d %d\n", op, pg);
	}

	if (getCurrentBit(page) == 0) {

		unsigned int num;
		if (frames.size() < frameCount) {
			num = frames.size();
			setFrame(page, num);
			if (O) {
				printf("%d: ZERO     %4d\n", instructions, num);
				printf("%d: MAP  %4d%4d\n", instructions, pg, num);
			}
			zero++;//Check
			mapped++;//Check

			frames.push_back(num);
			reverseFrames[num] = pg;

		} else {
			num = algo->getPage(pageVector, frames, reverseFrames);//get replacement page from algo class
			//cout<<"FrameNumber = "<<num<<endl;
			unsigned int previousPageIndex = reverseFrames[num];


			unsigned int& previousPage = pageVector[previousPageIndex];

			if (O) {

				printf("%d: UNMAP%4d%4d\n", instructions, previousPageIndex,
						num);
			}
			unmapped++;//check

			resetCurrnetBit(previousPage);
			resetRefBit(previousPage);

			if (getModBit(previousPage) == 1) {
				//Check if the previous page is dirty
				resetModBit(previousPage);
				setOutBit(previousPage);

				if (O) {
					printf("%d: OUT  %4d%4d\n", instructions, previousPageIndex,
							num);
				}
				out++;//check
			}

			if (getOutBit(page) == 1) {

				if (O) {
					printf("%d: IN   %4d%4d\n", instructions, pg, num);
				}
				in++;//check

			} else {

				if (O) {
					printf("%d: ZERO     %4d\n", instructions, num);
				}
				zero++;//check

			}

			if (O) {
				printf("%d: MAP  %4d%4d\n", instructions, pg, num);
			}
			mapped++;//check

			setFrame(page, num);
			reverseFrames[num] = pg;
		}

		setCurrentBit(page);
		if (op == 0) {
			setRefBit(page);
		} else {
			setRefBit(page);
			setModBit(page);
		}

	} else {
		algo->updatePage(frames, getFrame(page));
		if (op == 0) {
			setRefBit(page);
		} else {
			setRefBit(page);
			setModBit(page);
		}
	}
	instructions++;//check
}

void mapFrame() {
	for (int i = 0; i < instrVector.size(); i++) {
		Instructions inst = instrVector[i];
		mapPageFrame(inst.op, inst.page);
	}
}

void results() {
    if (P) {
        for (int i = 0; i < pageVector.size(); ++i) {
            if (getCurrentBit(pageVector[i])) {
                printf("%d:", i);
                if (getRefBit(pageVector[i])) {
                    printf("R");
                } else {
                    printf("-");
                }

                if (getModBit(pageVector[i])) {
                    printf("M");
                } else {
                    printf("-");
                }

                if (getOutBit(pageVector[i])) {
                    printf("S ");
                } else {
                    printf("- ");
                }

            } else {
                if (getOutBit(pageVector[i])) {
                    printf("# ");
                } else {
                    printf("* ");
                }
            }
        }
        printf("\n");
    }
    if (F) {
        for (int i = 0; i < reverseFrames.size(); ++i) {
            if (reverseFrames[i] == -1) {
                printf("* ");
            } else {
                printf("%d ", reverseFrames[i]);
            }
        }
        printf("\n");
    }
    if (S) {
        unsigned long long cost;

        cost = (long long)instructions + 400 * (long long)(mapped
                + unmapped) + 3000 * (long long)(in + out)
                + 150 * (long long)zero;


        printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
                instructions, unmapped, mapped, in,
                out, zero, cost);
    }
}

int main(int argc, char* argv[]) {

	parseCommand(argc, argv);

	mapFrame();

	results();

	return 0;
}
