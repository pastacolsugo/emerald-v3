#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iterator>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

#define wildcard '$'
// using '$' wildcard;

struct Command {
	std::string command;
	std::vector <std::string> paramList;
	bool success;

	Command () : success(false) {};
};

std::vector <std::string> commandTable;

void commandTableINIT () {
	commandTable.push_back ("time set $/$/$ $:$:$");
	commandTable.push_back ("time get");

	commandTable.push_back ("light set $:$ $ $:$");
	commandTable.push_back ("light get");

	commandTable.push_back ("water set $:$ $");
	commandTable.push_back ("water repeat $");
	commandTable.push_back ("water get");

	commandTable.push_back ("temperature set $ $");
	commandTable.push_back ("temperature get");
}

void error () {
	std::cout << "Error!\n";
}

std::string parseParameter (std::string::iterator userInputIterator,
							std::string::iterator userInputEndIterator,
							std::string::iterator parameterEndCharIterator) {

	std::string result;

	// std::cout << "parsing parameter:\n";

	for (; userInputIterator != userInputEndIterator; userInputIterator++) {
		// std::cout << *userInputIterator;

		if (*userInputIterator == *parameterEndCharIterator){
			// std::cout << "param End - " << result << '\n';
			return result;
		}


		result.push_back (*userInputIterator);
	}

	std::cout << '\n';
	return result;
}

Command parse (std::string userInput) {
	bool paramFound = false;

	Command result;

	for (std::vector<std::string>::iterator commandPrototypeIterator = commandTable.begin();
		commandPrototypeIterator != commandTable.end();
		commandPrototypeIterator++) {

		for (std::string::iterator userChar = userInput.begin(), referenceChar = commandPrototypeIterator->begin();	
				// userChar != userInput.end() && referenceChar != commandPrototypeIterator->end(); 
				true;
				userChar++, referenceChar++) {

			// std::cout << "userChar to userInput: " << std::distance (userChar, userInput.end()) << '\n';
			// std::cout << "referenceChar to commandPrototypeIterator->end(): " << std::distance (referenceChar, commandPrototypeIterator->end()) << '\n';

			if ( userChar == userInput.end() && referenceChar == commandPrototypeIterator->end() ) {
				// successful match
				result.success = true;
				result.command = *commandPrototypeIterator;
				return result;
			}	

			if (userChar == userInput.end()) {
				// unsuccessful match
				// one ends before the other

				// std::cout << "giampaolo\n";
				break;
			}

			if (referenceChar == commandPrototypeIterator->end()) {
				// std::cout << "sincostan\n";
				break;
			}

			// std::cout << (int)*userChar << " == " << (int)*referenceChar << " ?\n";

			if (*referenceChar == wildcard) {

				std::string parameter = parseParameter (userChar, userInput.end(), std::next(referenceChar, 1));

				userChar += parameter.size()-1;
				// -1 -> accounts for the 'for' incrementation

				if (parameter.empty()) {
					result.success = false;
					return result;
				}

				result.paramList.push_back (parameter);

			} else if (*userChar != *referenceChar) {
				// difference found
				break;
			}
		}

	}
	// std::cout << "shwifty\n";
	return result;
}

int main () {
	std::string buffer; 
	commandTableINIT();

	for (;;) {
		std::getline (std::cin, buffer);

		Command command = parse (buffer);
		// execute (command);

		std::cout << command.success << std::endl;

		std::cout << ">> " << command.command << '\n';

		for (int i=0; i<command.paramList.size(); i++) {
			std::cout << ">> " << command.paramList[i] << '\n';
		}

		std::cout << "---------------------------------\n";

	}
}