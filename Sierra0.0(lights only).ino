//Sierra0.0(lights only)
#include "Timer.h" //custom lib micros()-based
#include "DS3231.h"
#include <EEPROM.h> //using eeprom to save value and states

// Parser wildcard
#define wildcard '$'

// Parser Struct
struct CommandStruct {
	int commandIndex;
	int paramNumber;
	char **paramList;
	bool success;	
};
typedef struct CommandStruct Command;

#define statLedPin 3 //blinking led indicating loop running
#define fanLowPower 90
#define fanHighPower 255

#define lightPin 9
#define EE_light_addigitalRead 00 //available 00 to 09

// Parser command table
char **commandTable;
uint8_t commandNumber = 0;

Timer timer;
DS3231 rtc (A4, A5);
Time rtcTime;

int l_OnH = 18;
int l_OnM = 00;
int l_OffH = 10;
int l_OffM = 00;

void setup() {
	Serial.begin (9600); // 38400 ?
	Serial.print ("boot...");
	rtc.begin ();
	timer.init (1000);
	timer.ckSet (1000,20);

	commandTableINIT();

	pinMode (lightPin, OUTPUT);
	pinMode (statLedPin, OUTPUT);
	
	digitalWrite (lightPin, EEPROM.read (EE_light_addigitalRead) );

}

void loop() {
	char input[64];

	// read until newline
	if (Serial.available() > 0) {
		int i = 0;

		for (i=0; i<64; i++) {
			input[i] = Serial.read();	

			if (input[i] == '\n') {
				input[i] = 0; // add terminator
				break; // stop reading
			}
		}

		if (input[i] != 0) {
			// incorrect command reading
			Serial.println ("Incorrect command reading.");
			return;
		}

		Command *command = parse (input); // parse command string

		if (!command) {
			// wrong command
			Serial.println ("Incorrect command.");
			return ;
		}

		if (command->success == false) {
			// unsuccessful parsing
			Serial.println ("Unsuccessful parsing.");
			freeCommand (command);
			return ;
		}

		// linux kernel style switch
		// double indented case
		bool outcome = false;
		switch (command->commandIndex) {
			case 0:
					outcome = timeSet (command->paramList, command->paramNumber);

					if (!outcome) {
						Serial.println ("Fail.");
					} else {
						Serial.println ("Success.");
					}

					freeCommand (command);
					break;
			case 1: 
					timeGet ();
					break;
			case 2:
					outcome = lightSet (command->paramList, command->paramNumber);

					if (!outcome) {
						Serial.println ("Fail.");
					} else {
						Serial.println ("Success.");
					}

					freeCommand (command);
					break;
			case 3:
					lightGet();
					break;
		}
	}

	rtcTime = rtc.getTime();

	digitalWrite (statLedPin, timer.Clock());	

	lights_Core (l_OnH,l_OnM,l_OffH,l_OffM);
}

bool timeSet (char **parameterList, int parameterNumber) {
	return false;	
}

void timeGet () {
	Time now = rtc.getTime();

	Serial.print ("Time now: ");
	Serial.print (now.date);
	Serial.print ("/");
	Serial.print (now.mon);
	Serial.print ("/");
	Serial.print (now.year);
	Serial.print (" ");
	Serial.print (now.hour);
	Serial.print (" ");
	Serial.print (now.min);
	Serial.print (" ");
	Serial.println (now.sec);
}

bool lightSet (char ** parameterList, int parameterNumber) {
	return false;
}

void lightGet () {
	Serial.println ("Hello Sir");
}

void lights_Core (int OnHour, int OnMinute, int OffHour, int OffMinute) {

    if (rtcTime.min == OnMinute & rtcTime.hour == OnHour) {
     	digitalWrite (lightPin, 1);
     	EEPROM.write (EE_light_addigitalRead, 1);
	}
  
    if (rtcTime.min == OffMinute & rtcTime.hour == OffHour) {
     	digitalWrite (lightPin, 0); 
     	EEPROM.write (EE_light_addigitalRead, 0);
	}
}

void freeCommand (Command *command) {
	if (!command){
		// not allocated!
		return ;
	}

	for (int i=0; i<command->paramNumber; i++) {
		free (command->paramList[i]);
	}

	free (command->paramList);
	free (command);
}

bool commandTableINIT () {
	// command string length for auto malloc
	// terminator char excluded

	// // time, light, water and temperature enabled
	// int sizes[] = { 20, 8, 19, 9, 15, 14, 9, 19, 15 };
	// commandNumber = 9;

	// time and lights only
	int sizes[] = { 20, 8, 19, 9 };
	commandNumber = 4;

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

	// // WATER AND TEMPERATURE ARE DISABLED
	// // ASK @sugo TO ENABLE
	// commandTable[4] = "water set $:$ $\0";
	// commandTable[5] = "water repeat $\0";
	// commandTable[6] = "water get\0";

	// commandTable[7] = "temperature set $ $\0";
	// commandTable[8] = "temperature get\0";

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
					char **newParamList = (char **) realloc (result->paramList, sizeof(char*) * result->paramNumber+1);

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
