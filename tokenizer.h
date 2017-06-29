/*
 *	tokenizer.h
 * 
 * 	header file for a string tokenizer
 * 
 * 	Ryan Wendling, August 19th
 */
 
#ifndef TOKENIZER_DEF
#define TOKENIZER_DEF 1

// break up the line into individual tokens, based on delimiters
// returns a null-terminated array of strings
char **tokenize(char *line, char *delimiters);

// break up the line into individual tokens, based on delimiters
// returns a null-terminated array of strings
char **tokenizeDefault(char *line, char *delimiters);

// returns a count of the number of strings in the null-terminated
// array (excluding the null element)
int token_count(char **tokens);

// this further enhances our token array by splitting into two parts,
// for the two processes that will run via fork and pipe
char **tokenPipes(char ** initialTokens, int section);

#endif