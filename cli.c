/* Ryan Wendling
 * assignment3, cli.c
 * August 19th, 2016
 * this program emulates a command line interpreter and attempts to perform various internal and external functions.
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
#include "tokenizer.h"
#include "ttymode.h"
#include "charGetter.h"
#include "myCommandHistory.h"


int exteriorCall = 0;   // This determines if we need to make a call to a default system function.
int pipeCall = 0;


/* (1) The purpose of this function is to soundly exit the current program.
 * (2) there are no input parameters, this will either be called by typing "exit" or control-D
 * (3) there is no output, the program is gone */
static void myExit() {
    
    exit(0);
    return;
}  



/* (1) The purpose of this function is to get the real path of the program we want to run.
 * (2) the input parameters are the name of the program we want to run and the original path of the cli program
 * (3) we get the absolute pathname of the program we want to run as our return string */
char *getRealPath(char *programString, char *originalPath) {
    
    char filePath[PATH_MAX + 1];
    char pwd[1000];
    
    char *currentPath = getcwd(pwd, sizeof(pwd));
    chdir(originalPath);
    char *res = realpath(programString, filePath);
    chdir(currentPath);
    return res;
}  



/* (1) The purpose of this function is to take the two process command token arrays, and run the two processes,
 * with one being the input/output of the other. Here we use the command tokens we created in the main cli function. 
 * (2) the two input parameters are two seperated command line tokens for the two processes that will run.
 * (3) there is output depending on what the two called programs do. */
static void piping(char **givenCommandTokens1, char **givenCommandTokens2, char *originalPath) {

    pipeCall = 1;                       // Used to make sure the program doesn't try to call the original token array as one program.                            
    int oldStdin2 = dup(STDIN_FILENO);  // This will backup our original file descriptors
    int oldStdout2 = dup(STDOUT_FILENO);
    pid_t status;
    pid_t status2;
    int childExit;
    int childExit2;
                
    status = fork();
    if (status < 0) {
        
        printf("fork failed\n");
    }

    // child process
    if (status == 0) {
        
        int pipeEnds[2];
        pipe(pipeEnds);
        status2 = fork();
        if (status2 < 0) {
                
            printf("fork failed\n");
 
        // grandchild process
        } else if (status2 == 0) {
    
            // Here we close the original stdout and replace it with a pipe end.
            close(1);
            close(pipeEnds[0]);
            dup2(pipeEnds[1], STDOUT_FILENO);  
            
            if (!strncmp("pwd", givenCommandTokens1[0], 3) || !strncmp("cd", givenCommandTokens1[0], 2) ||
                !strncmp("exit", givenCommandTokens1[0], 4) || !strncmp("fref", givenCommandTokens1[0], 4) ||
                !strncmp("listf", givenCommandTokens1[0], 5) || !strncmp("calc", givenCommandTokens1[0], 4)) {
                
                char *truePathProgram1 = getRealPath(givenCommandTokens1[0], originalPath);
                if (execvp(truePathProgram1, givenCommandTokens1) < 0) { 
                    
                    printf("%s",givenCommandTokens1[0]);
                    printf(": command not found\n");
                }    
            } else {
                
                if (execvp(givenCommandTokens1[0], givenCommandTokens1) < 0) { 
                    
                    printf("%s",givenCommandTokens1[0]);
                    printf(": command not found\n");
                }    
            }
            
        // child's parent
        } else {
            
            // here we want to wait for the first process to finish, then we can take in it's output and such.
            wait(&childExit2);
            
            // Here we close the original stdin and replace it with a pipe end.
            close(0);
            close(pipeEnds[1]);
            dup2(pipeEnds[0], STDIN_FILENO); 
            
            if (!strncmp("pwd", givenCommandTokens2[0], 3) || !strncmp("cd", givenCommandTokens2[0], 2) ||
                !strncmp("exit", givenCommandTokens2[0], 4) || !strncmp("fref", givenCommandTokens2[0], 4) ||
                !strncmp("listf", givenCommandTokens2[0], 5) || !strncmp("calc", givenCommandTokens2[0], 4)) {
                
                char *truePathProgram2 = getRealPath(givenCommandTokens2[0], originalPath);
                if (execvp(truePathProgram2, givenCommandTokens2) < 0) { 
                    
                    printf("%s",givenCommandTokens2[0]);
                    printf(": command not found\n");
                }    
            } else {
                
                if (execvp(givenCommandTokens2[0], givenCommandTokens2) < 0) { 
                    
                    printf("%s",givenCommandTokens2[0]);
                    printf(": command not found\n");
                }    
            }
            //wait(&childExit2);
        }

    // parent process, going to keep the CLI open after the two processes finish.
    } else {
        
        wait(&childExit);
        dup2(oldStdin2, 0);
        dup2(oldStdout2, 1);
    }      
    return;
} 



/* (1) The purpose of this function is redirect the stdin/stdout depending on the input string given.
 * This is done by splicing the original command line input to get a file we will open for either writing or reading.
 * we will also potentially replace the stdin and stdout file descriptors with these newly opened files.
 * (2) the parameter is the command line string we get from the user
 * (3) there are no output values, but dup/dup2 are used to change how stdin and stdout behave. */
void redirect(char *inputCommand) {
    
    char redirectFile [100];
    int redirectFileFD = 69;
    char** commandTokens = tokenizeDefault(inputCommand, " ");
    
    // removes newline char from last token
    int tokens = token_count(commandTokens);
    commandTokens[tokens - 1][strlen(commandTokens[tokens - 1])] = '\0';
    
    for (int size = 0; commandTokens[size] != NULL; size++) {

        char* temp = commandTokens[size];
        
        //new output, will open file for writing, after grabbing it from the commandTokens array
        if (temp[0] == '>') {  
        
            if (strlen(commandTokens[size]) == 1) {
                
                strncpy (redirectFile, commandTokens[size + 1], strlen (commandTokens[size + 1]) + 1);
            } else {

                strncpy( redirectFile, temp + 1, strlen(temp) + 1);
            }
         redirectFile [strlen(redirectFile)] = '\0'; 
         redirectFileFD = open(redirectFile, O_WRONLY | O_CREAT, 0666); 
         if (redirectFileFD == -1) {
             
             printf("File didn't open\n");
         }    
         dup2(redirectFileFD, 1); 
        }
        
        //new output, will open file for reading, after grabbing it from the commandTokens array
        if (temp[0] == '<') {
        
            if (strlen(commandTokens[size]) == 1) {
                
                strncpy (redirectFile, commandTokens[size + 1], strlen (commandTokens[size + 1]) + 1);
            } else {

                strncpy( redirectFile, temp + 1, strlen(temp) + 1);
            }  
        
        redirectFile [strlen(redirectFile)] = '\0'; 
        redirectFileFD = open(redirectFile, O_RDONLY | O_CREAT, 0666);  
        if (redirectFileFD == -1) {
            
             printf("File didn't open\n");
        }  
        dup2(redirectFileFD, 0); 
        }    
    }            
    return;
}  



/* (1) The purpose of this function is to display the absolute path of CLI's current working directory
 * (2) there are no input parameters
 * (3) there are no output values, however the current path will be displayed to stdout. */
void myPwd() {
    
    char pwd[1000];
    if (getcwd(pwd, sizeof(pwd)) != NULL) {
        
        printf("%s", pwd);
    } else {
        
        printf("couldn't get path");
    }    
    return;
}  



/* (1) The purpose of this function is to change the current working directory to the absolute or relative path as specified by "path"
 * (2) the parameter is the path that we probably tokenized from the initial fgets in main.
 * (3) there is no return value, although we will print out the change in the current directory. */
void myCd(char *path) {
    
    int success = chdir(path);
    if (success == -1) {
        
        printf("no such directory: ");
        printf("%s\n", path);
    } else {
        
        printf("cwd changed to: ");
        printf("%s\n", path);
    }    
    return;
}  



/* (1) The purpose of this function is to take the arguementList and call the correct fuction from that list via forking and exec.
 * (2) the parameter is the list of arguements which hold the function call and additional arguements to send.
 * (3) there is no return value, although we will print out the change in the current directory. */
void forkAndStart(char* originalPath, char* pathFixer, char **arguementList) {
    
    char *res = getRealPath(pathFixer, originalPath); 
     
    pid_t status;
    int childExit;
    
    status = fork();
    
    if (status < 0) {
        
        printf("fork failed\n");
    }
    
    // child process
    if (status == 0) {
        
        if (exteriorCall == 1) {
            if (execvp(arguementList[0], arguementList) < 0) {
                
                printf("%s",arguementList[0]);
                printf(": command not found\n");
            }  
            
        } else {
            if (execvp(res, arguementList) < 0) {
                
                printf("%s",arguementList[0]);
                printf(": command not found\n");
            }    
        }
    
    // parent process
    } else {
        
        wait(&childExit);
    }    
    exteriorCall = 0;
    return;
}  



/* (1) The purpose of this function is to take in a commmand line from the user, tokenize it, and eventually exec it
 * depending on what was given. I/O Redirection may also be called here. This will repeat until a manual exit.
 * (2) the parameter is the list of arguements which hold the function call and additional arguements to send.
 * (3) there is no return value, although we something will happen depending on which of a multitude of functions are called.. */
int main(int argc, char *argv[]) {
    

    atexit(tty_atexit);                      //ASSIGNMENT3
    
    int oldStdin = dup(STDIN_FILENO);       // This will backup our original file descriptors
    int oldStdout = dup(STDOUT_FILENO);
    int active = 1;                         // Just a placeholder value to make sure our loop is still running.
    
    // This gets a copy of our original path.
    char absDir [1000];
    getcwd(absDir, sizeof(absDir));
    char *res = getcwd(absDir, sizeof(absDir));
    
    while(active == 1) {
        
        pipeCall = 0;
        printf("$> ");
        
        char *inputCommand;
        inputCommand = getInput();

        // Control-D end of file capture
        if (feof(stdin)) {
            myExit();
        }    
        
            // This gets a copy of our original command line.
            char * inputCommandCopy = malloc(strlen(inputCommand) + 1); 
            strcpy(inputCommandCopy, inputCommand);
            
            // Gets our arguement line into tokens.
            char** inputCommandTokens;
            inputCommandTokens = tokenize(inputCommand, " ");
            int numTokens = token_count(inputCommandTokens);

            // Does I/O redirection if "><" symbols are found.
            if (((strchr (inputCommandCopy, '<')) || (strchr (inputCommandCopy, '>')))) {
                
                redirect(inputCommandCopy);
            }  
            
            // Does pipe and fork if "|" symbol is found.
            if (strchr (inputCommandCopy, '|')) {
                
                printf("doing pipe\n");
                char** firstProcessTokens;
                char** secondProcessTokens;
                
                // call tokenPipes function twice to get both processes with arguements
                firstProcessTokens = tokenPipes(inputCommandTokens, 1);
                secondProcessTokens = tokenPipes(inputCommandTokens, 2);

                // Calls piping function with two process command token arrays and the original file path of cli.
                piping(firstProcessTokens, secondProcessTokens, res);
            } 
            
            // Here is where functions will be called depending on first token given
            if ((numTokens > 0) && (pipeCall == 0)) {
                
                if (!strncmp("pwd", inputCommandTokens[0], 3)) {
                    
                    myPwd();
                    
                } else if (!strncmp("cd", inputCommandTokens[0], 2)) {
                    
                    myCd(inputCommandTokens[1]);
                    
                } else if (!strncmp("exit", inputCommandTokens[0], 4)) {  
                    
                    myExit();
                    
                } else if (!strncmp("fref", inputCommandTokens[0], 4)) { 
                    
                    forkAndStart(res, "fref", inputCommandTokens);
                    
                } else if (!strncmp("listf", inputCommandTokens[0], 5)) { 
                    
                    forkAndStart(res, "listf", inputCommandTokens);
                    
                } else if (!strncmp("calc", inputCommandTokens[0], 4)) { 
                    
                    forkAndStart(res, "calc", inputCommandTokens);
                    
                // this where we attempt to call other functions, a flag is set to call a different exec function later on
                // these "other" functions include normal unix calls, local files and nonexistant functions.
                } else { 
                    
                    exteriorCall = 1;
                    forkAndStart(res, inputCommandTokens[0], inputCommandTokens);
                }
            }    
            
            // This resets our standard file descriptors for the next go around of the cli
            dup2(oldStdin, 0);
            dup2(oldStdout, 1);
    }
}