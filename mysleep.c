/* CS201: Assignment #3
 * Zachary Pitts
 * mysleep.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	srand ( time(NULL) );
	sleep((rand() % 10) + 1);

	return 0;
}
