/* CS201: Assignment #3
 * Zachary Pitts
 * eveccommand.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "zacsh.h"
#include "eveccommand.h"

#define MAXJOBS      32

//scoped variables
static struct job_t jobs[MAXJOBS]; // job list


/* eveccommand
 * Function takes a parsed character array, and exec's it through a
 * child process.
 */
void eveccommand(char * argv[MAXARGV], int bg)
{
	pid_t pid;		//new process id
   sigset_t mask;
	int argc = 0;
	int saved_stdout = dup(1);
	FILE * o_file = NULL;

	if (argv[0] == NULL)	//No command, so return without exec
		return;

	//Block child signals
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	while(argv[argc])
		++argc;

	//If >, then open output file, and change file descriptor for
	//stdout to this file
	if(argc > 2 && !strcmp(argv[argc - 2], ">"))
	{
		o_file = freopen(argv[argc-1], "w", stdout);

		if(!o_file)
			fprintf(stderr, "Failed to run command '%s': %s\n",
			argv[0], strerror(errno));

		argv[argc-2] = NULL; //stop exec from getting using ">"
	}

	//Child process
	if ((pid = Fork()) == 0)
	{
		//unblock SIGCHLD
		sigprocmask(SIG_UNBLOCK, &mask, NULL);

		if (execvp(argv[0], argv) < 0)
		{
			fprintf(stderr, "Failed to run command '%s': %s\n",
			argv[0], strerror(errno));

			exit(1);
		}
	}

	//Parent waits if not a background process
	if(!bg)
	{
		jobadd(pid);

		//Now that child pid is in the job list, unblock SIGCHLD signals
		sigprocmask(SIG_UNBLOCK, &mask, NULL);

		//Until child process is not on the job list, sleep 100 microseconds
		while(jobfind(pid))
			usleep(100);

		//Put output back to stdout, if needed
		dup2(saved_stdout, 1);
	}
	else
	{
		jobadd(pid);

		//Put output back to stdout, if needed
		dup2(saved_stdout, 1);

		printf("Background: (%d) %s\n", pid, argv[0]);
	}

	return;
}

	
/* Fork
 * Function creates a forked child process, and returns the pid of this new
 * process.
 */
pid_t Fork(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
	{
		fprintf(stderr, "%s: %s\n", "Fork error", strerror(errno));
		exit(0);
	}

	return pid;
}


/* unix_error
 * function for unix style errors 
 */
void unix_error(char *msg)
{
   fprintf(stdout, "%s: %s\n", msg, strerror(errno));
   exit(1);
}


/* Signal 
 * Wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler)
{
   struct sigaction action, old_action;

   action.sa_handler = handler;
   sigemptyset(&action.sa_mask); // block sigs of type being handled
   action.sa_flags = SA_RESTART; // restart syscalls if possible

   if (sigaction(signum, &action, &old_action) < 0)
      unix_error("Signal error");
   return (old_action.sa_handler);
}


/* child_reaper
 * This function will reap all zombies.  It will be used as the SIGCHLD
 * handler function.
 */
void child_reaper(int sig)
{
	pid_t pid;

	while (( pid = waitpid(-1, NULL, 0)) > 0)
		jobdelete(pid);
	if (errno != ECHILD)
			;		

	return;
}


/* initjobs
 * Simple function to clear out any pid information in the array of jobs
 */
void initjobs()
{
   int i;

   for (i = 0; i < MAXJOBS; ++i)
      jobclear(&jobs[i]);
}


/* jobclear 
 * Clear the job struct data
 */
void jobclear(struct job_t *job)
{
   job->pid = 0;
}


/* jobadd
 * Adds a job to the array by pid.
 */
int jobadd(pid_t pid)
{
   int i;

   if (pid < 1)
      return 0;

   for (i = 0; i < MAXJOBS; i++)
	{
		if (jobs[i].pid == 0)
		{
         jobs[i].pid = pid;
         return 1;
      }
   }
	//too many jobs
   return 0;

}


/* jobdelete
 * Removes a job by pid from the job array.
 */
int jobdelete(pid_t pid)
{
   int i;

   if (pid < 1)
      return 0;

   for (i = 0; i < MAXJOBS; ++i)
	{
		if (jobs[i].pid == pid)
		{
			jobclear(&jobs[i]);
			return 1;
		}
   }
   return 0;
}

/* jobfind
 * Finds a job on the job list, return 1 if it exists.
 */
int jobfind(pid_t pid)
{
	int i;

   for (i = 0; i < MAXJOBS; ++i)
      if (jobs[i].pid == pid)
         return 1;

	return 0;
}
