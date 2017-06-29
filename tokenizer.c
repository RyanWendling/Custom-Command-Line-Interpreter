/* Ryan Wendling
 * tokenizer.c
 * August 19th, 2016
 * Most Advanced tokenizer program to be used with assignment3, the sleek, new and stylish
 * tokenize program now accounts for piping symbols!
 */


#include <stdio.h>
#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>


char *array[1000];



/* (1) The purpose of this function is seperate a char** into seperate tokens depending on 
 * the delimiters given. This is done through the standard strtok function.
 * (2) The parameters are a line of characters to seperate, and a list of characters with which to seperate the tokens
 * (3) An array of strings is returned, each string holding a token */
char **tokenizeDefault(char *line, char *delimiters) {
    
    int i = 0;
    array[i] = strtok(line, delimiters);

    while(array[i] != NULL) {
        
        array[++i] = strtok(NULL, delimiters);
    }    
    return array;
}

    
    
/* (1) The purpose of this function is seperate a char** into seperate tokens depending on 
 * the delimiters given. This is done by going through the line and finding the points delimiters are found at
 * and how long it has been since the last delimiter has been found. We also make special cases for quotations and redirection symbols.
 * (2) The parameters are a line of characters to seperate, and a list of characters with which to seperate the tokens
 * (3) An array of strings is returned, each string holding a token */    
char **tokenize(char *line, char *delimiters) {

    int j;                              // for the loops
    int i;                              // for the loops
    int N = 1;                          // number of delimiters found
    int charArraySizes[1024];           // sizes of each word
    int charArraystart[1024];           // starting position of each word
    charArraySizes[0] = 0;
    int t = 1;                          // used for indexing of charArraySizes;
    int s = 1;                          // used for indexing of charArraystart;
    charArraystart[0] = 0;
    int activeQuotes = 0;               // used to capture multiple tokens between quotes
    
   
   
    /* Loop through the line we recieved, at each character, we compare it to the list of delimiters.
     if a match is found, we add the point in the line we found this at, and find the size of this new token
     by subtracting the position of the previous token. Note special case for the first token found. */
    for (i = 0; i < strlen(line); i++) {
        
        // This makes it so a string isn't seperated if the first quotes set is found.
        if (line[i] == '\"') {
            
            if (activeQuotes == 0) {
                
                activeQuotes = 1;
            } else {
                
                activeQuotes = 0;
            }
        }    
        for (j = 0; j < strlen(delimiters); j++) {
            
            if (line[i] == delimiters[j] && activeQuotes == 0) {
                
                N++;
                charArraystart[s] = i;
                if (N > 2) {
                            
                    charArraySizes[t] = i - (charArraystart[s - 1] + 1);
                } else {
                            
                    charArraySizes[t] = i - charArraystart[s - 1];
                }    
                t++;
                s++;
            }
        }
        
        // Special case to recognize the last token, if there is one.
        if (i == strlen(line) - 1) {
            
            charArraystart[s] = i;
            if (N > 1) {
                
                charArraySizes[t] = i - (charArraystart[s - 1] + 1);
            } else {
                
                charArraySizes[t] = i - charArraystart[s - 1];
            } 
        }    
    }
    
    // We can finally preserve some memory for our tokenArray, now that we know how many tokens there will be.
    char** tokenArr = malloc(sizeof *tokenArr * (N + 1));
    
    // edit to compensate for quotes, assignment2
    char** tokenArrFinal = malloc(sizeof *tokenArr * (N + 1));
    
    // edit to compensate for redirection, assignment2
    char** tokenArrActualFinal = malloc(sizeof *tokenArr * (N + 1));
    
    // edit to compensate for piping, assignment3
    //char** tokenArrLegitActualFinal = malloc(sizeof *tokenArr * (N + 1));    
    int m = 1;    // goes through all previously made tokens
    int l = 0;    // orders the tokens into the tokenArr
    
    // Here we will add the tokens to the tokenArray by using the data from our two arrays.
    while (m <= N) {
        
        // We only add the tokens if they are real tokens, meaning they were found in the delimiter check from earlier,
        // so the arraysize will be more than zero for this index.
        if (charArraySizes[m] > 0) {
            
            // we use this for our current starting point in splicing our string
            int startingpoint = charArraystart[m - 1];
            
            // declare enough room to hold a string
            tokenArr[l] = malloc(100);
            
            // special case needed for the first token found
            if (startingpoint > 0) {
                
                strncpy(tokenArr[l], line + (startingpoint + 1), charArraySizes[m]+1);
                tokenArr[l][charArraySizes[m]] = '\0';
            } else {
                
                strncpy(tokenArr[l], line + startingpoint, charArraySizes[m]+1);
                tokenArr[l][charArraySizes[m]] = '\0';
            }    
            l++;
        }
        m++;
    }
    tokenArr[N + 1] = '\0';
    

    
    /* From here we copy most of the tokenArr content into the new tokenArrFinal.  we also strip off these quotations
     * from the combined strings. the tracker is used to append a null to the tokenArrFinal when we are done filling it */
    int tracker1 = 0;
    for (int size = 0; tokenArr[size] != NULL; size++) {
        
        tokenArrFinal[size] = malloc(100);
        char* temp = tokenArr[size];
        char newTemp [strlen(tokenArr[size]) - 1];
        if (strstr (tokenArr[size], "\"")) {
            
            strncpy( newTemp, temp + 1, strlen(temp));
            newTemp[strlen(temp) - 2]='\0';
            strncpy( tokenArrFinal[size], newTemp, strlen(newTemp) + 1);
        } else {
            
            strncpy( newTemp, temp, strlen(temp));
            newTemp[strlen(temp)]='\0';
            strncpy( tokenArrFinal[size], newTemp, strlen(newTemp)+1);            
        }   
        tracker1++;
    }  
    tokenArrFinal[tracker1] = '\0';

    
    
    /* From here we copy most of the tokenArrFinal content into the new tokenArrActualFinal. Here we will forego adding a string if it 
     immediatly follows the string "<" or ">". we also wont add any string that contains a "<" or ">". */
    int newArrayDesignator = 0;
    int tracker2 = 0;
    for (int size2 = 0; tokenArrFinal[size2] != NULL; size2++) {
        
        char* temp2 = tokenArrFinal[size2];
        char newTemp2 [strlen(tokenArrFinal[size2])];
        if ((temp2[0] == '>') || (temp2[0] == '<')) {
           
            if (strlen(tokenArrFinal[size2]) == 1) {
                
                if (tokenArrFinal[size2 + 1] != NULL) {
                    
                    // Skips the next string, if it exists.
                    size2++;
                }
            } else {
                
                // Do nothing, we dont want this redirection string, it is not an actual function arguement.
            }
        } else {
            
            tokenArrActualFinal[newArrayDesignator] = malloc(100);
            strncpy( newTemp2, temp2, strlen(temp2));
            newTemp2[strlen(temp2)]='\0';
            strncpy( tokenArrActualFinal[newArrayDesignator], newTemp2, strlen(newTemp2)+1);
            newArrayDesignator++;
        } 
        tracker2++;
    }
    tokenArrActualFinal[tracker2] = '\0';


    return tokenArrActualFinal;
}  



/* (1) The purpose of this function is seperate the initial tokenized command string into two new command token arrays
 * in preparation of forking. This is done by going through the tokens and adding the tokens to a new array if they are
 * before the '|' symbol and adding the other tokens to another array if they are after the '|' symbol.
 * (2) The parameters the tokenized command array and a flag to determine if we are working before or after the '|' symbol.
 * (3) An array of strings is returned, about half the size of the initial tokenized command array. */    
char **tokenPipes(char ** initialTokens, int section) {
    
    char** pipeTokenFirst = malloc(sizeof *pipeTokenFirst * 10);
    char** pipeTokenSecond = malloc(sizeof *pipeTokenSecond * 10);
    int changingPoint = 0;          // Determines when we start adding the tokens to the new array
    int newArrayDesignator2 = 0;
    int tracker3 = 0;
    if (section == 1) {

        for (int size3 = 0; initialTokens[size3] != NULL; size3++) {
            
            if (changingPoint == 0) {
                
                char* temp3 = initialTokens[size3];
                char newTemp3 [strlen(initialTokens[size3])];
                
                // Keep adding tokens until the '|' character is found.
                if (!strchr (temp3, '|')) {
                
                    pipeTokenFirst[newArrayDesignator2] = malloc(100);
                    strncpy( newTemp3, temp3, strlen(temp3));
                    newTemp3[strlen(temp3)]='\0';
                    strncpy( pipeTokenFirst[newArrayDesignator2], newTemp3, strlen(newTemp3)+1);
                    newArrayDesignator2++;
                    tracker3++;
                    
                } else {
                    
                    changingPoint = 1;
                }    
            }
        }
        pipeTokenFirst[tracker3] = '\0';
        
        return pipeTokenFirst;     
    } else {
        
        for (int size3 = 0; initialTokens[size3] != NULL; size3++) {
            
            char* temp3 = initialTokens[size3];
            char newTemp3 [strlen(initialTokens[size3])];
            
            // Go through the token array, doing nothing until the '|' symbol is found.
            if (strchr (temp3, '|')) {
            
                changingPoint = 1;
            }
            if (changingPoint == 1) {
                
                if ((strlen(initialTokens[size3]) == 1) && (temp3[0] == '|')) {
                
                    // Do nothing, we dont want this piping string, it is not an actual function arguement.
                    
                // Remove the '|' character from the beginning of this token.    
                } else if ((strlen(initialTokens[size3]) > 1) && (temp3[0] == '|')) {
                    
                    pipeTokenSecond[newArrayDesignator2] = malloc(100);
                    strncpy( newTemp3, temp3, strlen(temp3));
                    newTemp3[strlen(temp3)]='\0';
                    strncpy( pipeTokenSecond[newArrayDesignator2], newTemp3+1, strlen(newTemp3));
                    newArrayDesignator2++;
                    tracker3++;
                } else {
                    
                    pipeTokenSecond[newArrayDesignator2] = malloc(100);
                    strncpy( newTemp3, temp3, strlen(temp3));
                    newTemp3[strlen(temp3)]='\0';
                    strncpy( pipeTokenSecond[newArrayDesignator2], newTemp3, strlen(newTemp3)+1);
                    newArrayDesignator2++;
                    tracker3++;
                }
            } else {
                
            }    
        }
        pipeTokenSecond[tracker3] = '\0';
        
        return pipeTokenSecond; 
    } 
}    



/* (1) The purpose of this function is give us a count of how many tokens there are
 * (2) The parameter is an array of strings, most likely created in a earlier tokenize function.
 * (3) An int is returned of the number of tokens found */
int token_count(char **tokens) {
    
    int size;
    for (size = 0; tokens[size] != NULL; size++);
    return size; 
}    