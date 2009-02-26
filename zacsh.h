/* CS201: Assignment #3
 * Zachary Pitts
 * zacsh.h
 */

//Constants
#define LINEMAX 1024
#define MAXARGV 256

//Function prototypes
int parseLine(char * buffer, char ** argv);
void myshell(char ** argv);
int builtin_check(char * newargv[]);
