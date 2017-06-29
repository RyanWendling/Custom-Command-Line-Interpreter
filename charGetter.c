/* Ryan Wendling
 * charGetter.c
 * August 19th, 2016
 * Basically a replacement for fgets. charGetter turns input into cbreak mode where we define functions for 
 * the enter key, backspace Control-D, etc.
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
#include "ttymode.h"
#include "myCommandHistory.h"


char *array[1000];
int potentialArrow = 0;



/* (1) The purpose of this function is to soundly exit the current program.
 * (2) there are no input parameters, this will either be called by typing "exit" or control-D
 * (3) there is no output, the program is gone */
static void myExit() {
    
    exit(0);
    return;
} 



/* (1) The purpose of this function is continually loop until a newline or control-D is found,
 * from there we return a concatenated string or exit the program
 * (2) There are no parameters, the input is gotten from the user after the function is activated.
 * (3) A string is returned as our output, to be used with the remainder of the main cli program */
char *getInput() {
        
        int currentSpot = 0;
        
        //add characters to this, newline finalizes, control-D empties
        char *concatenatedInput = malloc(sizeof(char*) * 1024);    
        int notNewline = 0;       // This flag will remain 0 unitl the user presses "enter"
        tty_cbreak(STDIN_FILENO);
        while (notNewline == 0) {

            int charVal = getchar();
            char thisChar = charVal;
            if (charVal == '\n') {
                
                notNewline = 1;
                
                //add string as long as it isnt null
                if (strcmp(concatenatedInput, "")) {
                    addCommandString(concatenatedInput);
                }
                resetLocale();
            }   

            
            
            // This logic captures our up and down arrow keys, the section here looks for esc[A or B, in sequence.
            if (potentialArrow == 2) {  
                
                // Call up arrow memory function
                if (thisChar == 65) {

                    char *recievedString = upArrowMemory(strlen(concatenatedInput));
                    strncpy(concatenatedInput, recievedString, 100); 
                    printf("%s",concatenatedInput);
                   
                // Call down arrow memory function    
                } else if (thisChar == 66) {  

                    char *recievedString = downArrowMemory(strlen(concatenatedInput));
                    strncpy(concatenatedInput, recievedString, 100); 
                    printf("%s",concatenatedInput);
                } else {
                    
                    potentialArrow = 0;
                }    
            }
            if (potentialArrow == 1) {
                
                if (thisChar == 91) {

                    potentialArrow = 2;
                } else {
                    potentialArrow = 0;
                }
            }    
            if (thisChar == 27) {

                potentialArrow = 1;
            } 
            
            
        
            // Captures standard input that displays easily from a keyboard
            if (thisChar > 31 && thisChar < 127 && potentialArrow == 0) {
                
                putchar(thisChar);
                strncat(concatenatedInput, &thisChar, 1);
                currentSpot++;
            }  
            
            // Deletes the most recent character.
            if (thisChar == 8 || thisChar == 127) {
                
                if (strlen(concatenatedInput) > 0) {
                    
                    putchar('\b');
                    putchar(' ');
                    putchar('\b');
                    currentSpot--;
                    concatenatedInput [strlen(concatenatedInput) - 1] = '\0'; 
                }    
            }
            
            // Control-D command that clears the current string and exits the program
            if (thisChar == 4) {
                
                for (int i = 0; i <= strlen(concatenatedInput); i++) {
                    putchar('\b');
                    putchar(' ');
                    putchar('\b');
                }    
                memset(concatenatedInput, '\0', 1024);
                notNewline = 1;
                myExit();
            }
        }  
        tty_reset(STDIN_FILENO);
        
        //Makes the string the right size to be used correctly with tokenizer later.
        concatenatedInput [strlen(concatenatedInput)] = 32; 
        concatenatedInput [strlen(concatenatedInput) + 1] = '\0'; 
        printf("\n");
    return concatenatedInput;
}

  