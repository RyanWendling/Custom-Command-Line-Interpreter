/* Ryan Wendling
 * fref.c
 * July 27th, 2016
 * this program immitates the unix grep command. it looks through given directories and files and goes through every line of code
 * looking for certain given characters. If it is a match for the character, the line is printed out to stdin with consideration
 * to the regular expressions logic.
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "regexp.h"


int inputGiven = 0;  // Flag to see if we were given commands originally, or the user entered them via promps.



/* (1) The purpose of this function is to loop through all the file lines, printing out the line if a character is matched from
 * the regular expressions list. the match function is called from the regexp.c file
 * (2) the parameter is the list files and the regular expressions string.
 * (3) no return value, but lines are printed out to stdin. */
void readAndPrint(char* expressions, char** filesList) {
    
    int spot = 0;
    while (filesList[spot] != NULL) {
        
        FILE* currentFile;

        // make sure we have a string ending in a NULL and not a newline
        if (expressions[strlen(expressions) - 1] == 10) {
            
            expressions[strlen(expressions) - 1] = '\0';
        }
        currentFile = fopen(filesList[spot], "r");
        if (currentFile == NULL) {
            
            printf("\n");
            printf("File ");
            printf("%s",filesList[spot]);
            printf(" not found\n");
        } else {
            
            char * currentLine = NULL;
            size_t lengthOfLine = 0;
            ssize_t read;
            int lineNum = 1;
            while ((read = getline(&currentLine, &lengthOfLine, currentFile)) != -1) {
                
                int positiveMatch = match(expressions, currentLine);
                if (positiveMatch == 1) {
                    printf("%s", filesList[spot]);
                    printf(":%i ", lineNum);
                    printf("%s", currentLine);
                    
                }
                lineNum++;
            }
            fclose(currentFile);
            free(currentLine);
        }
        spot++;
    }
    return;
}  



/* (1) the purpose of this function is to get the names of the files to search through and a list of some regular expressions.
 * from there, we will call a function to print out lines of the file if certain characters are found there.
 * (2) there are optional arugements that include the expressions list and the files. otherwise they are gotten from user input later.
 * (3) there are no specified return values, the output from the files will be printed to stdin, however. */
int main(int argc, char *argv[]) {
    
    /*get the optional arguements by looking for a token that starts with "-", after that everything is a potential directory/file,
     * if no directories / no valid directories, use current directory. */
    char** filesList = malloc(sizeof *filesList * 100);
    char* expressionsString;
    char inputExpressions[100];
    char inputFiles[1000];
        
    if (argc < 3) {

        printf("no arguements, enter regular expression identifiers, excluding \" : \n");
        fgets(inputExpressions, 1024 ,stdin);
        
        printf("now enter the list of files: \n");
        fgets(inputFiles, 1024 ,stdin);
        filesList = tokenize(inputFiles, " ");
        
        inputGiven = 1;
        readAndPrint(inputExpressions, filesList);
        
    } else {
    
        int location = 0;
        expressionsString = argv[1];
        for (int i = 2; i < argc; ++i) {
            
            char* anArguement = argv[i];
            filesList[location] = malloc(strlen (anArguement));
            strcpy(filesList[location], anArguement);
            location++;
        }   
        readAndPrint(expressionsString, filesList);
    } 
}