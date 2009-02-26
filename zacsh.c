#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "zacsh.h"
#include "eveccommand.h"
#include "history.h"

int main(int argc, char **argv)
{
	int length = 0;
	int bg;			//background process or not
	char newCommand[LINEMAX] = "";
	char buffer[LINEMAX] = "";
	char * newargv[MAXARGV];	//New command to parse and execute

	Signal(SIGCHLD, child_reaper);	//SIGCHLD handler
	initjobs();	//Initialize job list for processing jobs

	while(1)
	{
		strcpy(newCommand, "");
		printf("zacsh$ ");	//output prompt
		fflush(stdout);

		fgets(newCommand, LINEMAX, stdin);	//Get the new string

		if (feof(stdin))	//If input is EOF, then break from loop
		{
			fflush(stdout);
			break;
		}

		//copy new command to a buffer to be modified
		length = strlen(newCommand);
		strncpy(buffer, newCommand, length);
		buffer[length] = '\0';

		bg = parseLine(buffer, newargv); //Parse the new command	

		if (!newargv[0]) //empty command, so do nothing
			;
		else if (newargv[0][0] == '!' && newargv[0][1] != '\0') //history call
	   {
			int i;
			char hist_num[LINEMAX] = "";

			//Remove ! from first token
			for(i = 0; newargv[0][i+1] ; ++i)
				hist_num[i] = newargv[0][i+1];

			history_exec(atoi(hist_num));
		}
		else //record command to history, and then run it
		{
			//copy command to history linked list
			history_add(newCommand);

			if(!builtin_check(newargv))
				eveccommand(newargv, bg);	
		}
	}

	printf("\n");

	//destroy all allocated memory in history LLL
	history_free();

	return 0;
}

/* builtin_check
 * Check to see if the line contains any builtin commands, and if it does then
 * run them, and return true.  Otherwise, return false.
 */
int builtin_check(char * newargv[])
{
	if (!strcmp(newargv[0], "chdir"))
	{
		if (chdir(newargv[1]) < 0)
			fprintf(stderr, "Failed to run command '%s': %s\n",
			newargv[0], strerror(errno));
	}
	else if (!strcmp(newargv[0], "myshell"))
		myshell(newargv);
	else if (!strcmp(newargv[0], "history"))
		history_list();
	else
		return 0;

	return 1;
}

/* parseLine
 * Function will parse a character array into an array of character arrays,
 * with each string token of the original array stored in successive index
 * of the new array of char arrays.
 */
int parseLine(char * buffer, char ** argv)
{
	char * delimiter;	//space delimiter
	int argc;		//number of arguments
	int bg;			//background process or not

	buffer[strlen(buffer)-1] = ' ';	//remove trailing newline
	while(*buffer && (*buffer == ' '))
		++buffer;

	//Build the argument list
	argc = 0;
	while ((delimiter = strchr(buffer, ' ')))
	{
		argv[argc++] = buffer;
		*delimiter = '\0';
		buffer = delimiter + 1;
		while (*buffer && (*buffer == ' '))  
			buffer++;
	}
	argv[argc] = NULL;

	if (argc == 0)
		return 1;

	if ((bg = (*argv[argc-1] == '&')) != 0)
		argv[--argc] = NULL;

	return bg;
}

/* myshell
 * Simple function to call the system call to directly run a command.
 */
void myshell(char ** argv)
{
	char temp[LINEMAX];	//will hold the modified command
	int j;

	strcpy(temp,"");
	for (j = 1; argv[j] != '\0'; ++j)
	{
		strcat(temp,argv[j]);
		strcat(temp," ");
	}
	system(temp);
}
