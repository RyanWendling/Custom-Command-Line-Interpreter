/* Ryan Wendling
 * listf.c
 * July 27th, 2016
 * this program is a replacement of the UNIX ls command. The purpose is to list the contents of one or more directories,
 * or, the names and details of individual files as specified by the command line arguements.
 */

#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include "tokenizer.h"


//make the flag values global, this way the functions can use them
int lFlag = 0;
int aFlag = 0;
int cFlag = 0;
int mFlag = 0;
int numDirectories = 0;
int longestLength= 0;



/* (1) Simple function to get the padding length for the printing output format
 * (2) the parameter is a positive number.
 * (3) the return value will be an int value. */
int lengthHelper(int x) {
    
    if(x>=1000000000) return 10;
    if(x>=100000000) return 9;
    if(x>=10000000) return 8;
    if(x>=1000000) return 7;
    if(x>=100000) return 6;
    if(x>=10000) return 5;
    if(x>=1000) return 4;
    if(x>=100) return 3;
    if(x>=10) return 2;
    
    return 1;
}



/* (1) The purpose of this function is to find the number of file entries in the section we are looking through.
 * This is done by looping through the directories and incrementing size for each file found in the stream.
 * (2) the parameter is the list of directories we have gotten from the command line arguements
 * (3) the return value will be an int value. */
int numEntries(char** directoryList) {

    int entries = 0;    
    int size = 1;
    do {
        
        DIR *thisDirectory;
        struct dirent *dirStruct;

        if (numDirectories == 1) {
            
            thisDirectory = opendir(".");
        } else {
            
            thisDirectory = opendir(directoryList[size]);  
        }

        if (thisDirectory) {
            while ((dirStruct = readdir(thisDirectory)) != NULL) {
                entries++;
            }

        closedir(thisDirectory);
        }
     size++;   
    } while (directoryList[size] != NULL);
    return entries;
}   



/* (1) The purpose of this function is to find the spacing required to correctly format the sizes of the files
 * This is done by looping through the directories and keeping a tally of the largest file, in bytes.
 * (2) the parameter is the list of directories we have gotten from the command line arguements
 * (3) there is no return value, but the global "longestLength" variable is changed */
void findLongestLength(char** directoryList) {

    int size = 1;
    do {
        
        DIR *thisDirectory;
        struct dirent *dirStruct;
        
        // Hinders on whether the call to listf was given a valid directory or not
        if (numDirectories == 1) {
            
            thisDirectory = opendir(".");
        } else {
            
            thisDirectory = opendir(directoryList[size]);   
        }
        if (thisDirectory) {
            
            while ((dirStruct = readdir(thisDirectory)) != NULL) {
                
                struct stat fileStats;
                stat(dirStruct->d_name, &fileStats);
                
                int thisLength = lengthHelper(fileStats.st_size);
                if (thisLength > longestLength) {
                    
                    longestLength = thisLength;
                }    
            }

        closedir(thisDirectory);
        }  
        size++;      
    } while (directoryList[size] != NULL);
    
    return;
} 



/* (1) The purpose of this function is to print the files from specified directories. Also prints additional
 * information depending on flags set from the command line arguements.
 * This is done by looping through the directories and incrementing size for each file found in the stream, alphabetically.
 * (2) the parameter is the list of directories we have gotten from the command line arguements
 * (3) there is no return value, just some output to stdin */
void printList(char** directoryList) {
    
    int size = 1;
    int current = 0;
    char* excludeName1 = ".";
    char* excludeName2 = "..";
    do {
        
        struct dirent **nameList;
        int numDirs;

        // Hinders on whether the call to listf was given a valid directory or not
        if (numDirectories == 1) {
            
            numDirs = scandir(".", &nameList, NULL, alphasort);
        } else {    
            
            numDirs = scandir(directoryList[size], &nameList, NULL, alphasort);
        }    
        if (numDirs < 0) {
            
            printf("error with scandir\n");
        } else {
            
            while (current < numDirs) {
        
                struct stat fileStats;
                stat(nameList[current]->d_name, &fileStats);
                
                if (lFlag == 1) {
                        
                    if ( strncmp(nameList[current]->d_name, excludeName1, 1) && strncmp(nameList[current]->d_name, excludeName2, 2)) {
                        printf("\n");
                        
                        // Prints out the type of the file.
                        switch (fileStats.st_mode & S_IFMT) {
                        case S_IFBLK:  printf(" o ");        break;
                        case S_IFCHR:  printf(" o ");        break;
                        case S_IFDIR:  printf(" d ");        break;
                        case S_IFIFO:  printf(" o ");        break;
                        case S_IFLNK:  printf(" l ");        break;
                        case S_IFREG:  printf(" - ");        break;
                        case S_IFSOCK: printf(" o ");        break;
                        default:       printf(" o ");        break;
                        }
                        
                        // Prints out the permissions of the file
                        printf( (fileStats.st_mode & S_IRUSR) ? "r" : "-");
                        printf( (fileStats.st_mode & S_IWUSR) ? "w" : "-");
                        printf( (fileStats.st_mode & S_IXUSR) ? "x" : "-");
                        printf( (fileStats.st_mode & S_IRGRP) ? "r" : "-");
                        printf( (fileStats.st_mode & S_IWGRP) ? "w" : "-");
                        printf( (fileStats.st_mode & S_IXGRP) ? "x" : "-");
                        printf( (fileStats.st_mode & S_IROTH) ? "r" : "-");
                        printf( (fileStats.st_mode & S_IWOTH) ? "w" : "-");
                        printf( (fileStats.st_mode & S_IXOTH) ? "x" : "-");
                        printf(" ");
                        
                        // Prints out the number of links connected to the file.
                        printf("%ld", fileStats.st_nlink);
                        
                        // Pets the correct format to print out the user and group names.
                        struct group *grp;
                        struct passwd *pwd;
                        pwd = getpwuid(fileStats.st_uid);
                        printf(" %s", pwd->pw_name);
                        grp = getgrgid(fileStats.st_gid);
                        printf(" %s", grp->gr_name);
                        
                        // Prints out the size of the file in Bytes, with spacing.
                        int currentSize = lengthHelper(fileStats.st_size);
                        int offset = longestLength - currentSize;
                        printf( "%*s", offset, "");
                        printf(" %lld", (long long) fileStats.st_size);
                        
                        //specialized flag options, wont always print
                        char s[1000];
                        if (lFlag == 1 && aFlag == 0 && cFlag == 0 && mFlag == 0) {
                            
                            struct tm * p = localtime(&fileStats.st_mtime);
                            printf(" ");
                            strftime(s, 1000, "M%m/%d/%Y-%R", p);
                            printf("%s", s);
                        }
                        
                        if (mFlag == 1 && lFlag == 1) {
                            
                            struct tm * p = localtime(&fileStats.st_mtime);
                            printf(" ");
                            strftime(s, 1000, "M%m/%d/%Y-%R", p);
                            printf("%s", s);
                        }
                        if (aFlag == 1 && lFlag == 1) {
                            
                            struct tm * p = localtime(&fileStats.st_atime);
                            printf(" ");
                            strftime(s, 1000, "A%m/%d/%Y-%R", p);
                            printf("%s", s);
                        }
                        if (cFlag == 1 && lFlag == 1) {

                            struct tm * p = localtime(&fileStats.st_ctime);
                            printf(" ");
                            strftime(s, 1000, "C%m/%d/%Y-%R", p);
                            printf("%s", s);
                        }
                        
                        // Always print names, regardless of flags set.
                        printf(" %s ", nameList[current]->d_name);
                        free(nameList[current]);
                    }      
                }
                // Always print names, regardless of flags set.
                if (lFlag == 0) {
                    
                    if ( strncmp(nameList[current]->d_name, excludeName1, 1) && strncmp(nameList[current]->d_name, excludeName2, 2)) {
                        
                        printf(" %s ", nameList[current]->d_name);
                        free(nameList[current]);
                    }    
                } 
            current++;
            }
        free(nameList);
        }
    size++;   
    } while (directoryList[size] != NULL);
    
    printf("\n");
}    
    

    
/* (1) the purpose of this function is to display the contents of a directory, we can give a directory name as an input parameter,
 * otherwise, we use the current directory.
 * (2) there are various optional arguements... -l gives additional information, -l gives last access time, -l gives creation time,
 * -m changes nothing. You can combine the arguement letters for even more info, provided atleast -l is used.
 * (3) there are no specified return values, the related files/directories will be printed to stdin, however. */
int main(int argc, char *argv[]) {
    
    char** directoryList = malloc(sizeof *directoryList * 100);

    /*get the optional arguements by looking for a token that starts with "-", after that everything is a potential directory/file,
     * if no directories / no valid directories, use current directory. */
    for (int i = 0; i < argc; ++i) {
        
        char* anArguement = argv[i];
        if (anArguement[0] == '-') {
            
            for (int j = 1; j < strlen(anArguement); j++){
                
                if (anArguement[j] == 'l') {
                    
                    lFlag = 1;
                } else if (anArguement[j] == 'a') {
                    
                    aFlag = 1;
                } else if (anArguement[j] == 'c') {
                    
                    cFlag = 1;
                } else if (anArguement[j] == 'm') {
                    
                    mFlag = 1;
                } else {
                    
                }    
            }
        } else {
            
            directoryList[numDirectories] = malloc(strlen (anArguement));
            strcpy(directoryList[numDirectories], anArguement);
            numDirectories++;
        }
    }  
    
    int numOfEntries = numEntries(directoryList) - 2;
    printf("%i ", numOfEntries);
    printf("entries found\n");
    findLongestLength(directoryList);

    printList(directoryList);
    
    return 0;
}