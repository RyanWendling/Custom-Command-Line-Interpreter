/*
 *	myCommandHistory.c
 * 
 * 	header file for our command line memory
 * 
 * 	Ryan Wendling, June 2016
 */
 

// This function adds a string that the user has just completed to our array of previously sent commands.
void addCommandString(char *aCommandString);

// gets older string that was used previously, returns same string if oldest string
char *upArrowMemory();

// gets newer string from where we are currently. returns blank line if we are at newest string.
char *downArrowMemory();

void resetLocale();