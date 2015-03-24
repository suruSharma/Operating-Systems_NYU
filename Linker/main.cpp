#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;

string symbolsDefined[512];
int symbolsAddress[512];
string symbolErrors[512];
int symbolsIndex = 0;
int symbolsUsed[512];
int symbolModule[512];

int srNo = 0;

void checkForRule4() {
	cout << endl;
	for (int i = 0; i < symbolsIndex; i++) {
		string symbol = symbolsDefined[i];
		int isSymUsed = symbolsUsed[i];
		int module = symbolModule[i];

		if (isSymUsed == 0) {
			cout << "Warning: Module " << module << ": " << symbol
					<< " was defined but never used" << endl;
		}
	}
}
int getSymbolIndex(string symbolName) {
	for (int i = 0; i < symbolsIndex; i++) {
		if (symbolName == symbolsDefined[i])
			return i;
	}
	return -1;
}
void printSymbolTable() {
	cout << "Symbol Table" << endl;
	for (int i = 0; i < symbolsIndex; i++) {
		cout << symbolsDefined[i] << "=" << symbolsAddress[i]; //<<errors[i]<<endl;
		if (symbolErrors[i] != "")
			cout << " " << symbolErrors[i];
		cout << endl;
	}
	cout << endl;
	cout << "Memory Map" << endl;
}

void printMemoryTable(int value, string error) {
	cout << setw(3) << setfill('0') << srNo << ": " << setw(4) << setfill('0')
			<< value;
	if (error != " ")
		cout << " " << error;
	cout << endl;

	srNo++;
}

void checkForRule7(int module, string variablesInUse[], int varUsed[],
		int index) {
	for (int i = 0; i < index; i++) {
		string var = variablesInUse[i];
		if (varUsed[i] == 0) {
			cout << "Warning: Module " << module << ": " << var
					<< " appeared in the uselist but was not actually used"
					<< endl;
		}
	}
}

void checkForRule5(int maxValue, int module, int startIndex, int endIndex) {
	for (int i = startIndex; i < endIndex; i++) {
		int val = symbolsAddress[i];
		string symbol = symbolsDefined[i];
		if (val > maxValue) {
			cout << "Warning: Module " << module << ": " << symbol << " to big "
					<< val << " (max=" << maxValue << ") assume zero relative"
					<< endl;
			symbolsAddress[i] = 0;
		}
	}
}

void __parseerror(int errcode, int linenum, int lineoffset) {
    static char* errstr[] = {
    "NUM_EXPECTED", // Number expect
    "SYM_EXPECTED", // Symbol Expected
    "ADDR_EXPECTED", // Addressing Expected
    "SYM_TOLONG", // Symbol Name is to long
    "TO_MANY_DEF_IN_MODULE", // > 16
    "TO_MANY_USE_IN_MODULE", // > 16
    "TO_MANY_INSTR" // total num_instr exceeds memory size (512)
    };
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}

bool is_number(string& s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void secondPass(char * fileName) {
	ifstream fin(fileName);

	char c;

	int number_of_lines = 0;

	int module = 0;

	int offset = 0;

	while (!fin.eof()) {
		//SYMBOL COLLECTION BEGINS(first line)
		fin.get(c);

		if (c == ' ' || c == '\n' || c == '\t') {
			// if first character itself is a space then continue till you find the next non-space
			while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
				fin.get(c);
			}

		}

		if (fin.eof()) {
			break;
		}

		module++;
		string symCountStr = ""; //number of symbols that are declared in the first line

		while (c != ' ' && c != '\n') {
			symCountStr += c;
			fin.get(c);

			if (c == ' ') {
				break;
			}
			if (c == '\n') {
				number_of_lines++;
				break;
			}
		}

		int symCountInt = atoi(symCountStr.c_str());

		int varCount = 1; //to track the input

		while (varCount <= symCountInt) {

			//gets the symbol name
			fin.get(c);
			if (c == ' ') {
				// if first character itself is a space then continue till you find the next non-space
				while (c == ' ')
					fin.get(c);
			}

			string symbolName = "";
			while (c != ' ' && c != '\n') {
				symbolName += c;
				fin.get(c);

				if (c == ' ') {
					break;
				}

				if (c == '\n') {
					number_of_lines++;
					break;
				}
			}

			//gets the symbol address value
			fin.get(c);
			if (c == ' ') {
				// if first character itself is a space then continue till you find the next non-space
				while (c == ' ')
					fin.get(c);
			}

			string symbolAddressStr = "";
			while (c != ' ' && c != '\n') {
				symbolAddressStr += c;
				fin.get(c);

				if (c == ' ') {
					break;
				}

				if (c == '\n') {
					number_of_lines++;
					break;
				}
			}

			int symbolAddressInt = atoi(symbolAddressStr.c_str());

			int index = getSymbolIndex(symbolName);

			if (index != -1) {
				symbolErrors[index] =
						"Error: This variable is multiple times defined; first value used";
			} else {
				symbolsDefined[symbolsIndex] = symbolName;
				symbolsAddress[symbolsIndex] = symbolAddressInt + offset;
				symbolsIndex++;
			}
			varCount++;
		}

		//SYMBOL COLLECTION ENDS

		//USE LIST COLLECTION BEGINS
		fin.get(c);

		if (c == ' ') {
			// if first character itself is a space then continue till you find the next non-space
			while (c == ' ')
				fin.get(c);
		}

		string useCaseCountStr = ""; //number of symbols that are being used in the module

		while (c != ' ' && c != '\n') {
			useCaseCountStr += c;
			fin.get(c);

			if (c == ' ') {
				break;
			}
			if (c == '\n') {
				number_of_lines++;
				break;
			}
		}

		int useCaseCountInt = atoi(useCaseCountStr.c_str());

		string variablesInUse[useCaseCountInt];

		int varUsed[useCaseCountInt]; //for RULE 7

		int useCount = 0; //to track the input

		while (useCount < useCaseCountInt) {
			//gets the use case variables
			fin.get(c);
			if (c == ' ') {
				// if first character itself is a space then continue till you find the next non-space
				while (c == ' ')
					fin.get(c);
			}

			string useCaseVarName = "";
			while (c != ' ' && c != '\n') {
				useCaseVarName += c;
				fin.get(c);

				if (c == ' ') {
					break;
				}

				if (c == '\n') {
					number_of_lines++;
					break;
				}
			}

			variablesInUse[useCount] = useCaseVarName;
			varUsed[useCount] = 0;
			useCount++;
		}
		//USE LIST COLLECTION ENDS

		//INSTRUCTION COLLECTION BEGINS

		fin.get(c);

		if (c == ' ' || c == '\n' || c == '\t') {
			fin.get(c);
			while (c == ' ' || c == '\n' || c == '\t') {
				fin.get(c);
			}
		}

		string instrCountStr = "";

		while (c != ' ' && c != '\n') {
			instrCountStr += c;
			fin.get(c);

			if (c == ' ') {
				break;
			}
			if (c == '\n') {
				number_of_lines++;
				break;
			}
		}

		int instrCountInt = atoi(instrCountStr.c_str());

		int instrCtr = 1;

		while (instrCtr <= instrCountInt) {
			fin.get(c);

			if (c == ' ' || c == '\n' || c == '\t') {
				if (c == '\n') {
					number_of_lines++;
				}
				fin.get(c);
				while (c == ' ' || c == '\n' || c == '\t') {
					fin.get(c);
				}
			}

			string instrType = "";

			while (c != ' ' && c != '\n' && c != '\t') {
				instrType += c;
				fin.get(c);

				if (c == ' ') {
					break;
				}
				if (c == '\n') {
					number_of_lines++;
					break;
				}
			}

			fin.get(c);

			if (c == ' ' || c == '\n' || c == '\t') {
				fin.get(c);
				while (c == ' ' || c == '\n' || c == '\t') {
					fin.get(c);
				}
			}

			string instrCodeStr = "";

			while (c != ' ' && c != '\n' && c != '\t') {
				instrCodeStr += c;
				fin.get(c);

				if (c == ' ') {
					break;
				}
				if (c == '\n') {
					number_of_lines++;
					break;
				}
			}

			int instrCodeInt = atoi(instrCodeStr.c_str());

			if (instrType == "R") {
				if (instrCodeInt > 9999) {
					printMemoryTable(9999,
							"Error: Illegal opcode; treated as 9999"); //third parameter is the error
				} else if (instrCodeInt % 1000 > instrCountInt) {
					printMemoryTable((((instrCodeInt / 1000) * 1000) + offset),
							"Error: Relative address exceeds module size; zero used"); //third parameter is the error
				} else {
					printMemoryTable((instrCodeInt + offset), " "); //third parameter is the error
				}

			} else if (instrType == "I") {
				if (instrCodeInt > 9999) {
					printMemoryTable(9999,
							"Error: Illegal immediate value; treated as 9999"); //third parameter is the error
				} else {
					printMemoryTable(instrCodeInt, " "); //third parameter is the error
				}

			} else if (instrType == "A") {
				if (instrCodeInt > 9999) {
					printMemoryTable(9999,
							"Error: Illegal opcode; treated as 9999"); //third parameter is the error
				} else if (instrCodeInt % 1000 > 512) {
					printMemoryTable(((instrCodeInt / 1000) * 1000),
							"Error: Absolute address exceeds machine size; zero used"); //third parameter is the error
				} else {
					printMemoryTable(instrCodeInt, " "); //third parameter is the error
				}

			} else if (instrType == "E") {
				if (instrCodeInt > 9999) {
					printMemoryTable(9999,
							"Error: Illegal opcode; treated as 9999"); //third parameter is the error
				} else {
					int variableIndex = instrCodeInt % 1000;

					if (variableIndex >= useCaseCountInt) {
						printMemoryTable(instrCodeInt,
								"Error: External address exceeds length of uselist; treated as immediate"); //third parameter is the error
					} else {
						string symbolToBeUsed = variablesInUse[variableIndex];
						int index = getSymbolIndex(symbolToBeUsed); //if it is -1, means that the variable is not yet defined
						if (index == -1) {
							//means that the variable is not defined, RULE 3 invoked
							printMemoryTable((instrCodeInt - variableIndex),
									"Error: " + symbolToBeUsed
											+ " is not defined; zero used"); //third parameter is the error
							varUsed[variableIndex] = 1; //for RULE 7
						} else {
							//variable is defined
							varUsed[variableIndex] = 1; //for RULE 7
							symbolsUsed[index] = 1;
							int memValue = symbolsAddress[index];
							printMemoryTable(
									(instrCodeInt + memValue - variableIndex),
									" "); //third parameter is the error
						}

					}
				}

			}
			instrCtr++;

		}
		offset += instrCountInt;

		//RULE 7 STARTS
		checkForRule7(module, variablesInUse, varUsed, useCount);
		//RULE 7 ENDS
		continue;//change to break to check code module by module
	}

	checkForRule4();

}

void firstPass(char* fileName) {
	ifstream fin(fileName);

	int number_of_lines = 1;

	int lineOffSet = 1;

	int module = 0;



	int offset = 0;

	int startIndex = symbolsIndex;

	char c;
	fin.get(c);

	while (!fin.eof()) {
		module++;

		//SYMBOL COLLECTION BEGINS(first line)

		//fetching a word STARTS : number of variables defined
		while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
			if (c == '\n') {
				number_of_lines++;
				lineOffSet = 1;
				fin.get(c);
			} else {
				lineOffSet++;
				fin.get(c);
			}
		}

		string symCountStr = ""; //number of symbols that are declared in the first line

        int symLength = 0;

		while (c != ' ' && c != '\n' && c != '\t') {
			symCountStr += c;
			lineOffSet++;
			symLength++;
			fin.get(c);
		}

		int symCountInt = atoi(symCountStr.c_str());

		if(symCountInt > 16) //TO_MANY_DEF_IN_MODULE
        {
            __parseerror(4, number_of_lines, (lineOffSet-symLength));
            exit(EXIT_FAILURE);
        }

		int varCount = 1;       //to track the input

		while (varCount <= symCountInt) {

			//fetching a word STARTS : variable name
			int varPreviousOffset = 0;
			while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
				if (c == '\n') {
					number_of_lines++;
					varPreviousOffset = lineOffSet;
					lineOffSet = 1;
					fin.get(c);
				} else {
					lineOffSet++;
					fin.get(c);
				}
			}

			if(fin.eof())
            {
                __parseerror(1, number_of_lines-1, varPreviousOffset);
                exit(EXIT_FAILURE);
            }

			string symbolName = ""; //name of the symbol
            int symbolNameLen = 0;
			while (c != ' ' && c != '\n' && c != '\t') {
				symbolName += c;
				lineOffSet++;
				symbolNameLen++;
				fin.get(c);
			}

			if(symbolNameLen > 16)
            {
                __parseerror(3, number_of_lines, (lineOffSet-symbolNameLen));
                exit(EXIT_FAILURE);
            }
            else if(is_number(symbolName) == 1)
            {
                __parseerror(1, number_of_lines, (lineOffSet-symbolNameLen));
                exit(EXIT_FAILURE);
            }

			//fetching a word ENDS:variable name

			//fetching a word STARTS : variable address
			while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
				if (c == '\n') {
					number_of_lines++;
					lineOffSet = 1;
					fin.get(c);
				} else {
					lineOffSet++;
					fin.get(c);
				}

			}

			string symbolAddressStr = ""; //address of symbol

            int symbolAddrLen = 0;
			while (c != ' ' && c != '\n' && c != '\t') {
				symbolAddressStr += c;
				lineOffSet++;
				symbolAddrLen++;
				fin.get(c);
			}

			//fetching a word ENDS:variable address

			int isNum = is_number(symbolAddressStr);

            if(isNum == 0) //it is not a number, we were expecting a number
            {
                __parseerror(0, number_of_lines, (lineOffSet-symbolAddrLen));
                exit(EXIT_FAILURE);
            }

			int symbolAddressInt = atoi(symbolAddressStr.c_str());



			int index = getSymbolIndex(symbolName);

			if (index != -1) {
				symbolErrors[index] =
						"Error: This variable is multiple times defined; first value used";
			} else {
				symbolsDefined[symbolsIndex] = symbolName;
				symbolsAddress[symbolsIndex] = symbolAddressInt + offset;
				symbolsUsed[symbolsIndex] = 0;
				symbolModule[symbolsIndex] = module;
				symbolsIndex++;
			}

			varCount++;
		}

		//SYMBOL COLLECTION ENDS

		//USE LIST COLLECTION BEGINS

		//fetching a word STARTS : count of use list
		while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
			if (c == '\n') {
				number_of_lines++;
				lineOffSet = 1;
				fin.get(c);
			} else {
				lineOffSet++;
				fin.get(c);
			}

		}

		string useCaseCountStr = ""; //number of symbols that are declared in the first line

        int useCtrLen = 0;
		while (c != ' ' && c != '\n' && c != '\t') {
			useCaseCountStr += c;
			lineOffSet++;
			useCtrLen++;
			fin.get(c);
		}

		//fetching a word ENDS: count of use list

		int useCaseCountInt = atoi(useCaseCountStr.c_str());

		if(useCaseCountInt > 16) // TO_MANY_USE_IN_MODULE
        {
            __parseerror(5, number_of_lines, (lineOffSet-useCtrLen));
            exit(EXIT_FAILURE);
        }

		int useCount = 1;           //to track the input

		while (useCount <= useCaseCountInt) {

			//fetching a word STARTS : count of use list
			while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
				if (c == '\n') {
					number_of_lines++;
					lineOffSet = 1;
					fin.get(c);
				} else {
					lineOffSet++;
					fin.get(c);
				}

			}

			string useCaseVarName = ""; //number of symbols that are declared in the first line
			// string word = "";

            int useCaseVarNameLen = 0;
			while (c != ' ' && c != '\n' && c != '\t') {
				useCaseVarName += c;
				lineOffSet++;
				useCaseVarNameLen++;
				fin.get(c);
			}

			//check if the useCaseVarName is a number, SYM_EXPECTED error

            int isNum = is_number(useCaseVarName);

            if(isNum == 1) //it is a number, we were expecting a symbol
            {
                __parseerror(1, number_of_lines, (lineOffSet-useCaseVarNameLen));
                exit(EXIT_FAILURE);
            }
			//fetching a word ENDS: count of use list
			useCount++;
		}
		//USE LIST COLLECTION ENDS

		//INSTRUCTION COLLECTION BEGINS

		//fetching a word STARTS : number of instructions in module
		while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
			if (c == '\n') {
				number_of_lines++;
				lineOffSet = 1;
				fin.get(c);
			} else {
				lineOffSet++;
				fin.get(c);
			}

		}

		string instrCountStr = ""; //number of symbols that are declared in the first line
		// string word = "";

        int instrCountLen = 0;
		while (c != ' ' && c != '\n' && c != '\t') {
			instrCountStr += c;
			lineOffSet++;
			instrCountLen++;
			fin.get(c);
		}

		//fetching a word ENDS: number of instructions in module

		int instrCountInt = atoi(instrCountStr.c_str());
		offset += instrCountInt;

        if(offset > 512)//TO_MANY_INSTR
        {
            __parseerror(6, number_of_lines, (lineOffSet-instrCountLen));
            exit(EXIT_FAILURE);
        }

		int instrCtr = 1;

		while (instrCtr <= instrCountInt) {

			//fetching a word STARTS : Instruction type
			int instrPreviousOffset = 0;
			while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
				if (c == '\n') {
					number_of_lines++;
					instrPreviousOffset = lineOffSet;
					lineOffSet = 1;
					fin.get(c);
				} else {
					lineOffSet++;
					fin.get(c);
				}

			}

			string instrType = ""; //number of symbols that are declared in the first line
			int instrTypeLen= 0;


			while (c != ' ' && c != '\n' && c != '\t') {
				instrType += c;

				lineOffSet++;
				instrTypeLen++;
				fin.get(c);
			}

			if(fin.eof())
            {
                __parseerror(2, number_of_lines-1, instrPreviousOffset);
                exit(EXIT_FAILURE);
            }

			//fetching a word ENDS: Instruction type

			//fetching a word STARTS : Instruction type
			while (!fin.eof() && (c == ' ' || c == '\n' || c == '\t')) {
				if (c == '\n') {
					number_of_lines++;
					lineOffSet = 1;
					fin.get(c);
				} else {
					lineOffSet++;
					fin.get(c);
				}

			}

			string instrCodeStr = ""; //number of symbols that are declared in the first line
			// string word = "";

			while (c != ' ' && c != '\n' && c != '\t') {
				instrCodeStr += c;
				lineOffSet++;
				fin.get(c);
			}

			//fetching a word ENDS: Instruction type
			instrCtr++;
		}

		//RULE 5 STARTS
		checkForRule5(offset - 1, module, startIndex, symbolsIndex);
		//RULE 5 ENDS

		continue;//change to break to check code module by module
		// continue;
	}

	printSymbolTable();
	secondPass(fileName);
}

int main(int argc, char *argv[]) {

	char *fileName = argv[1];

	//cout<<fileName<<endl;

	firstPass(fileName);

	return 0;
}
