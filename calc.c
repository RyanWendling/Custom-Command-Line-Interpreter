/* Ryan Wendling
 * calc.c
 * July 27th, 2016
 * this program performs simple maths, given a line of user input.
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"



/* (1) The purpose of this function is to display the absolute path of CLI's current working directory
 * (2) there are no input parameters
 * (3) there are no output values, however the current path will be displayed to stdout. */
void myExit() {
    
    exit(0);
    return;
}  



/* (1) The purpose of this function is to find the two values we will be using to operate on.
 * Basically just calls the tokenize function and uses the operators as delimiters.
 * (2) the parameter is the line given from stdin
 * (3) the return value will be an array of two char arrays, each hosting one of the numerical values. */
char **find_numbers(char *line) {
    
    char** inputCommandTokens;
    inputCommandTokens = tokenize(line, " /-*+");
    return inputCommandTokens;
}  


/* (1) The purpose of this function is to find the operator type. It does this by looping through the characters
 * in the line given and returning the point in the line where the matched operator is found. otherwise returns 0.
 * (2) the parameter is the line given from stdin
 * (3) the return value will be a char array of the operator type we found. */
char *find_operator(char *line) {
    
    int i;
    for (i = 0; i < strlen(line); i++) {
        
        if (line[i] == 42 || line[i] == 47 || line[i] == 43 || line[i] == 45 ) {
            
            char *foundOperator = (char *) malloc(sizeof(char) * 1);
            sprintf(foundOperator, "%i", line[i]);
            return foundOperator;
        }
    }
    char *noOperator = (char *) malloc(sizeof(char) * 1);
    sprintf(noOperator, "%i", '0');
    return noOperator;
}  


/* (1) the purpose of this function is to get the two values and operator to perform and print out some simple mathematics.
 * this is done by calling the find_operator and find_numbers functions. After that we have a branching if statement to cover all math types.
 * (2) there are no arguements, the calc process is called from the cli file.
 * (3) there are no specified return values, the output of the math will be printed to stdin, however. */
int main(int argc, char *argv[]) {
    
    int active = 1;
    while(active == 1) {
        
        char inputCommand[1024];
        fgets(inputCommand, 1024 ,stdin);

        // Control-D end of file capture
        if (feof(stdin)) {
            
            myExit();
        } 
        
        // Gets values ready for some math.
        char* operator = find_operator(inputCommand);
        char** numbers = find_numbers(inputCommand);
        int operatorVal = atoi(operator);

        if (operatorVal == 42) {
            
            printf("%i", atoi(numbers[0]));
            printf(" * ");
            printf("%i", atoi(numbers[1]));
            printf(" = ");
            printf("%i\n", (atoi(numbers[0]) * atoi(numbers[1])));
        }
        else if (operatorVal == 47) {
            
            printf("%i", atoi(numbers[0]));
            printf(" / ");
            printf("%i", atoi(numbers[1]));
            printf(" = ");
            printf("%i\n", (atoi(numbers[0]) / atoi(numbers[1])));
        }
        else if (operatorVal == 43) {
            
            printf("%i", atoi(numbers[0]));
            printf(" + ");
            printf("%i", atoi(numbers[1]));
            printf(" = ");
            printf("%i\n", (atoi(numbers[0]) + atoi(numbers[1])));
        }
        else if (operatorVal == 45) {
            printf("%i", atoi(numbers[0]));
            printf(" - ");
            printf("%i", atoi(numbers[1]));
            printf(" = ");
            printf("%i\n", (atoi(numbers[0]) - atoi(numbers[1])));
        }
        else {
            printf("Does not compute\n");
        }    
    }    
}