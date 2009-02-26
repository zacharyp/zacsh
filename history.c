/* CS201: Assignment #3
 * Zachary Pitts
 * history.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "zacsh.h"
#include "eveccommand.h"
#include "history.h"

static hnode * tail = NULL;	//Head pointer for linear linked list
static int history_size = 0;	//Size of history


/* history_free
 * Function to be called at the end of the program to free all memory used by
 * linked list.
 */
void history_free()
{
	if(tail)
	{
		hnode * orig_tail = tail;

		while(tail)
		{
			hnode * temp = tail;

			//loop until back at the original tail location, then stop
			if(tail->next != orig_tail)
				tail = tail->next;
			else 
				tail = NULL;

			if(temp->command_text)
				free (temp->command_text);
			free (temp);
		}
	}
	return;
}


/* history_exec
 * Function will execute the command in the history indicated by the passed 
 * integer, that should be between 1 and HISTORYMAX.
 */
void history_exec(int history_number)
{
	int j, bg, length;
   char buffer[LINEMAX] = "";
   char * newargv[MAXARGV];   //Command to parse and execute
	hnode * temp = tail;

	if(tail == NULL)
		printf("No history\n");
	else if (history_number < 1 || history_number > history_size)
		printf("Invalid history number.\n");
	else
	{
		//loop through list until correct node
		for(j= 0; temp && j < history_number; ++j)
			temp = temp->next;

		length = strlen(temp->command_text);
		strncpy(buffer, temp->command_text, length);
		buffer[length] = '\0';

		bg = parseLine(buffer, newargv);

		//Run historical command
		if(!builtin_check(newargv))
			eveccommand(newargv, bg);
	}
}


/* history_add
 * Function will add the passed char array to the history list.  If there are 
 * already HISTORYMAX historical items in the list, then the last item in the
 * history will be removed before the new item is added.
 */
void history_add(char * newCommand)
{
	hnode * newnode = (hnode *) malloc (sizeof(hnode));

	newnode->command_text = (char *) calloc ((strlen(newCommand) +1) * sizeof(char), 1);
	strncpy(newnode->command_text, newCommand, strlen(newCommand));
	newnode->command_text[strlen(newnode->command_text)] = '\0';

	//if history at max, then remove head, i.e. oldest item, to make room for new
	if(history_size == HISTORYMAX)
	{
		hnode * tofree = tail->next;
		tail->next = tofree->next;

		if(tofree->command_text)
			free (tofree->command_text);
		free (tofree);
	}

	if(history_size < HISTORYMAX)	
		++history_size;

	if(!tail)
	{
		tail = newnode;
		tail->next = tail;
	}
	else
	{
		newnode->next = tail->next;
		tail->next = newnode;
		tail = newnode;
	}
}


/* history_list
 * Function will print out a list of the history with numerical values for each
 * item in the history.
 */
void history_list()
{
	int j;
	hnode * temp = tail;

	for(j= 0; j < history_size; ++j)
	{
		temp = temp->next;
		printf("%d - %s", j+1, temp->command_text);
	}
}
