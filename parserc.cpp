#include <stdio.h>
#include <stdlib.h>

#define wildcard '$'
// using '$' wildcard;

// struct Command {
// 	std::string command;
// 	std::vector <std::string> paramList;
// 	bool success;

// 	Command () : success(false) {};
// };

// std::vector <std::string> commandTable;

// void commandTableINIT () {
// 	commandTable.push_back ("time set $/$/$ $:$:$");
// 	commandTable.push_back ("time get");

// 	commandTable.push_back ("light set $:$ $ $:$");
// 	commandTable.push_back ("light get");

// 	commandTable.push_back ("water set $:$ $");
// 	commandTable.push_back ("water repeat $");
// 	commandTable.push_back ("water get");

// 	commandTable.push_back ("temperature set $ $");
// 	commandTable.push_back ("temperature get");
// }

char * parseParameter ( char * userInputPointer, 
						char * userInputEndPointer,
						char * parameterEndCharPointer) {

	char * result = (char *) malloc (sizeof(char) * 16); // max 15 chars + terminator

	if (!result) { // allocation error
		return NULL;
	}

	int i = 0; // will contain the parameter size

	for (i=0; userInputPointer != userInputEndPointer && i<15; userInputPointer++, i++) {
		// iterate through all user input charachters (needed because parameter may end the string)

		if (*userInputPointer == *parameterEndCharPointer) {
			// if the parameter end char is found, stop copying and return
			break;
		}

		result[i] = *userInputPointer;
	}

	result = (char *) realloc (result, sizeof(char) * (i+1));
	// in every case i == result size
	// i + 1 because we need the terminator char

	if (!result) {
		// bad reallocation
		return NULL;
	}

	result[i] = 0;

	return result;
}

// std::string parseParameter (std::string::iterator userInputIterator,
// 							std::string::iterator userInputEndIterator,
// 							std::string::iterator parameterEndCharIterator) {

// 	std::string result;

// 	for (; userInputIterator != userInputEndIterator; userInputIterator++) {	
// 		// iterate through all user input charachters (needed because parameter may end the string)

// 		if (*userInputIterator == *parameterEndCharIterator){
// 			// if the parameter end char is found, stop copying and return
// 			return result;
// 		}

// 		// add the character from the command to the parameter string
// 		result.push_back (*userInputIterator);
// 	}

// 	// command ending with parameter case
// 	return result;
// }

// Command parse (std::string userInput) {
// 	bool paramFound = false;

// 	Command result;

// 	for (std::vector<std::string>::iterator commandPrototypeIterator = commandTable.begin();
// 		commandPrototypeIterator != commandTable.end();
// 		commandPrototypeIterator++) {

// 		for (std::string::iterator userChar = userInput.begin(), referenceChar = commandPrototypeIterator->begin();	
// 				true;
// 				userChar++, referenceChar++) {


// 			if ( userChar == userInput.end() && referenceChar == commandPrototypeIterator->end() ) {
// 				// successful match
// 				result.success = true;
// 				result.command = *commandPrototypeIterator;
// 				return result;
// 			}	

// 			if (userChar == userInput.end()) {
// 				// unsuccessful match
// 				// one ends before the other

// 				break;
// 			}

// 			if (referenceChar == commandPrototypeIterator->end()) {
// 				break;
// 			}

// 			if (*referenceChar == wildcard) {	
// 				std::string parameter = parseParameter (userChar, userInput.end(), std::next(referenceChar, 1));	

// 				userChar += parameter.size()-1;
// 				// -1 -> accounts for the 'for' incrementation

// 				if (parameter.empty()) {
// 					result.success = false;
// 					return result;
// 				}

// 				result.paramList.push_back (parameter);

// 			} else if (*userChar != *referenceChar) {
// 				// difference found
// 				break;
// 			}
// 		}
// 	}

// 	return result;
// }


int main () {
	char input[] = "superca";
	char endChar = 'f';

	char * parsedParameterPointer = parseParameter (input, input+7, &endChar);

	printf ("%s\n", parsedParameterPointer);

	free (parsedParameterPointer);

	return 0;
}

// int main () {
// 	std::string buffer; 
// 	commandTableINIT();

// 	for (;;) {
// 		std::getline (std::cin, buffer);

// 		Command command = parse (buffer);
// 		// execute (command);

// 		std::cout << command.success << std::endl;

// 		std::cout << ">> " << command.command << '\n';

// 		for (int i=0; i<command.paramList.size(); i++) {
// 			std::cout << ">> " << command.paramList[i] << '\n';
// 		}

// 		std::cout << "---------------------------------\n";

// 	}
// }