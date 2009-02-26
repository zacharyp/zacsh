/* CS201: Assignment #3
 * Zachary Pitts
 * history.h
 */

#define HISTORYMAX 10

//Struct
typedef struct history_node
{
	char * command_text;
	struct history_node * next;
} hnode;

//Prototype
void history_add(char * new_command);
void history_exec(int history_number);
void history_free();
void history_list();
