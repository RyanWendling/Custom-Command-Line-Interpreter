/* Ryan Wendling
 * charGetter.c
 * August 19th, 2016
 * myCommandHistory is used to grab and modify the strings
 * that we had used at some point in our terminal, cli.c.
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>


int size = 0;
int currentLocale = 0;
char oldCommandsList [20][100];



/* a simple function that would reset our current location in the oldCommandsList array back to 0,
 * for whatever reason. takes no parameters nor does it return any. */
void resetLocale() {
    currentLocale = 0;
}   



/* (1) The purpose of this function is to add the given string to our array of older commands. 
 * this is done by adding the new string to the 0th position of the array and pushing the rest back.
 * (2) The input is given as a string, gotten from the user after they press the enter button
 * (3) Nothing is returned, although the oldCommandsList array is changed. */
void addCommandString(char *aCommandString) {
    
    if (size == 0) {
        
        strncpy( oldCommandsList[0], aCommandString, strlen(aCommandString) + 1); 
        size++;
        
    } else {
        
        for (int k = (size); k > 0; k--){   

            strncpy(oldCommandsList[k], oldCommandsList[k - 1], 100); 
        }
        strncpy( oldCommandsList[0], aCommandString, strlen(aCommandString) + 1); 
        size++;
    }    
    return;
}



/* (1) The purpose of this function is to clear the current string and bring up an earlier called string.
 * this is done by passing in the length of current string that we should clear. From there, we use a global variable,
 * currentLocale to find the index of the string we should be pulling from oldCommandsList.
 * (2) The input parameter is the currentStringSize, which is the length of the string currently on the terminal.
 * (3) A string is returned as our output, it is now the current string on the terminal and can be modified by the user.*/
char *upArrowMemory(int currentStringSize) {
    
    for (int j = 0; j < currentStringSize; j++) {
        
        putchar('\b');
        putchar(' ');
        putchar('\b');
    }
    if (currentLocale <= (size - 1)) {
        
        currentLocale++;
    } 
    
    return oldCommandsList[currentLocale - 1];
}



/* (1) The purpose of this function is to clear the current string and bring up a newer called string.
 * this is done by passing in the length of current string that we should clear. From there, we use a global variable,
 * currentLocale to find the index of the string we should be pulling from oldCommandsList.
 * (2) The input parameter is the currentStringSize, which is the length of the string currently on the terminal.
 * (3) A string is returned as our output, it is now the current string on the terminal and can be modified by the user.*/
char *downArrowMemory(int currentStringSize) {
    
    for (int j = 0; j < currentStringSize; j++) {
        
        putchar('\b');
        putchar(' ');
        putchar('\b');
    }
    if (currentLocale > 0) {
        
        currentLocale--;
    }
    if (currentLocale == 0) {
        
        return "";
    } else {
        
        return oldCommandsList[currentLocale -1];
    }    
}
  