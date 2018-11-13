#include <stdio.h>
#include <stdlib.h>

#define wildcard '$'


struct CommandStruct {
	int commandIndex;
	int paramNumber;
	char ** paramList;
	bool success;	
};

typedef struct CommandStruct Command;

char ** commandTable;
uint8_t commandNumber = 0;
// std::vector <std::string> commandTable;

bool commandTableINIT () {
	// command string length for auto malloc
	// terminator char excluded
	int sizes[] = { 20, 8, 19, 9, 15, 14, 9, 19, 15 };
	commandNumber = 9;

	// // test commands sizes
	// int sizes[] = { 8, 11, 13, 17, 18, 20, 21, 25 };
	// commandNumber = 8;

	commandTable = (char **) malloc (sizeof(char*) * commandNumber);

	if (!commandTable) {
		return false;
	} 

	// allocate space for every string
	for (int i=0; i<commandNumber; i++) {
		commandTable[i] = (char *) malloc (sizeof(char) * (sizes[i]+1));
		// allocate string length + 1 char for terminator

		if (!commandTable[i]) {
			// allocation error

			// deallocate the previously allocated strings
			for (int j=i-1; j>=0; j--) {
				free (commandTable[i]);
			}

			// free the first pointer
			free (commandTable);
			return false;
		}
	}

	// // test commands
	// commandTable[0] = "no param\0";
	// commandTable[1] = "one param $\0";
	// commandTable[2] = "two param $ $\0";
	// commandTable[3] = "three param $ $ $\0";
	// commandTable[4] = "four param $ $ $ $\0";
	// commandTable[5] = "five param $ $ $ $ $\0";
	// commandTable[6] = "six param $ $ $ $ $ $\0";
	// commandTable[7] = "seven param $ $ $ $ $ $ $\0";


	commandTable[0] = "time set $/$/$ $:$:$\0";
	commandTable[1] = "time get\0";

	commandTable[2] = "light set $:$ $ $:$\0";
	commandTable[3] = "light get\0";

	commandTable[4] = "water set $:$ $\0";
	commandTable[5] = "water repeat $\0";
	commandTable[6] = "water get\0";

	commandTable[7] = "temperature set $ $\0";
	commandTable[8] = "temperature get\0";

	return true;
}

char * parseParameter (char * userInputPointer, char * parameterEndCharPointer) {

	int paramSize = 8;
	// max 7 chars + terminator

	char * result = (char *) malloc (sizeof(char) * paramSize);

	if (!result) { // allocation error
		return NULL;
	}

	int i = 0; // will contain the parameter size

	for (i=0; *userInputPointer != 0 && i<paramSize-1; userInputPointer++, i++) {
		// iterate through all user input charachters (needed because parameter may end the string)

		if (*userInputPointer == *parameterEndCharPointer) {
			// if the parameter end char is found, stop copying and return
			break;
		}

		result[i] = *userInputPointer;
	}

	char * newBlock = (char *) realloc (result, sizeof(char) * (i+1));
	// in every case i == result size
	// i + 1 because we need the terminator char

	if (!newBlock) {
		// bad reallocation
		free (result);
		return NULL;
	}

	result = newBlock;

	result[i] = 0;

	return result;
}

Command * parse (char * userInput) {
	bool paramFound = false;

	Command * result = (Command *) malloc (sizeof(Command));

	if (!result) {
		return NULL;
	}

	result->commandIndex = -1;
	result->paramNumber = 0;
	result->paramList = NULL;
	result->success = false;

	// iterate through reference commands
	for (int i=0; i<commandNumber; i++) {
		// printf (">> testing \"%s\"\n", commandTable[i]);

		// iterate through the user input
		for (char *userCharPointer = userInput, *referenceCharPointer = commandTable[i];
				true;
				userCharPointer++, referenceCharPointer++) {

			// successful match
			if (*userCharPointer == 0 && *referenceCharPointer == 0) {
				result->success = true;
				result->commandIndex = i;
				return result;
			}

			// unsuccessful match
			// one ends before the other
			if (*userCharPointer == 0 || *referenceCharPointer == 0) {
				break;
			}

			if (*referenceCharPointer == wildcard) {

				// parameter found, parse it
				char * parameter = parseParameter (userCharPointer, referenceCharPointer+1);

				// if parsed successfully
				if (!parameter) {
					return NULL;
				}

				// calculate the parameter length
				int size = 0;
				for (size=0; parameter[size] != 0; size++) {}

				// shift pointer forward
				// magic number -1 accounts for the 'for' incrementation 
				// can't shift if the parameter length is zero, I would loop forever
				if (size != 0) {
					userCharPointer += size - 1;
				}

				if (result->paramNumber == 0) {
					// if there is no param list allocated
					// allocate it
					result->paramList = (char **) malloc (sizeof(char*));

					if (!result->paramList) {
						// bad allocation
						for (int i=0; i<result->paramNumber; i++) {
							free (result->paramList[i]);	
						}

						free (result->paramList);
						free (result);

						return NULL;
					}

				} else {

					// reallocate parameters+1 for the new one
					char ** newParamList = (char **) realloc (result->paramList, sizeof(char*) * result->paramNumber+1);

					// deallocate struct
					if (!newParamList) {
						for (int i=0; i<result->paramNumber; i++) {
							free (result->paramList[i]);	
						}

						free (result->paramList);
						free (result);

						return NULL;
					}

					result->paramList = newParamList;
				}

				// reallocation successful
				// increase parameter count
				result->paramNumber += 1;

				result->paramList [result->paramNumber-1] = parameter;

			} else if (*userCharPointer != *referenceCharPointer) {
				// difference found
				break;
			}
		}	
	}



	// free memory
	for (int i=0; i<result->paramNumber; i++) {
		free (result->paramList[i]);	
	}

	free (result->paramList);
	free (result);

	return NULL;

	// return result;
}

int main () {
	char input[64];

	commandTableINIT();

	while (fgets (input, 64, stdin)) {	

		for (int i=0; input[i]!=0; i++) {
			if (input[i] == '\n' && input[i+1] == 0) {
				input[i] = 0;
			}
		}

		printf ("got from console:%s.\n", input);

		Command * command = parse (input);

		if (!command) {
			printf ("NULL\n");
			continue;
		}

		printf ("Parsed command: %d\n- %d [commandIndex = %s]\n", command->success, command->commandIndex, commandTable [command->commandIndex]);
		printf ("- %d [paramNumber]\n", command->paramNumber);

		for (int i=0; i<command->paramNumber; i++) {
			printf ("> %s\n", command->paramList[i]);
		}

		// for (int i=0; i<command->paramNumber; i++) {
		// 	free (command->paramList[i]);	
		// }

		// free (command->paramList);
		// free (command);

		printf ("---------\n");
	}
}
