//Types
typedef void handler_t(int);

struct job_t {              // The job struct
   pid_t pid;              // job PID 
};

//Prototypes
void eveccommand(char ** commandString, int bg);
pid_t Fork(void);
void unix_error(char *);
void child_reaper(int sig);
handler_t * Signal(int signum, handler_t *handler);
void initjobs();
int jobadd(pid_t pid);
int jobdelete(pid_t pid);
int jobfind(pid_t pid);
void jobclear(struct job_t *job);
