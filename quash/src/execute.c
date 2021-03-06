
/**
 * Austin & Kari
 * @file execute.c
 *
 * @brief Implements interface functions between Quash and the environment and
 * functions that interpret an execute commands.
 *
 * @note As you add things to this file you may want to change the method signature
 */


#include "execute.h"
#include <stdio.h>
#include "deque.h"
#include "quash.h"
#include <sys/wait.h>
// Remove this and all expansion calls to it
/**
 * @brief Note calls to any function that requires implementation
 */
//#define IMPLEMENT_ME()
  //fprintf(stderr, "IMPLEMENT ME: %s(line %d): %s()\n", __FILE__, __LINE__, __FUNCTION__)

IMPLEMENT_DEQUE_STRUCT(pid_deque, pid_t);
IMPLEMENT_DEQUE(pid_deque, pid_t);

typedef struct JOB {
	int num;
	int job_id;
	int pfd[2][2];
	char* command;
	pid_deque pid_queue;
} JOB;

IMPLEMENT_DEQUE_STRUCT(job_deque, JOB);
IMPLEMENT_DEQUE(job_deque, JOB);

job_deque Jobs; //creates queue of Jobs
bool initial = true;

static void remove_job(JOB* job){
		free(job->command);
		destroy_pid_deque(&job->pid_queue);
}

static void init_job(JOB* job){
	assert(job != NULL);
	job->job_id = -0;
	job->command = get_command_string();
	job->pid_queue = new_pid_deque(1);
	job->num = 0;
}
/***************************************************************************
 * Interface Functions
 ***************************************************************************/

// Return a string containing the current working directory.
char* get_current_directory(bool* should_free) {
  // TODO: Get the current working directory. This will fix the prompt path.
  // HINT: This should be pretty simple
  //IMPLEMENT_ME();
  // Change this to true if necessary
  *should_free = true;
    return (getcwd(NULL,0));
}

// Returns the value of an environment variable env_var
const char* lookup_env(const char* env_var) {
  // TODO: Lookup environment variables. This is required for parser to be able
  // to interpret variables from the command line and display the prompt
  // correctly
  // HINT: This should be pretty simple

  //IMPLEMENT_ME();

  // TODO: Remove warning silencers
//  (void) env_var; // Silence unused variable warning

  return getenv(env_var);
}

// Check the status of background jobs
void check_jobs_bg_status() {
  // TODO: Check on the statuses of all processes belonging to all background
  // jobs. This function should remove jobs from the jobs queue once all
  // processes belonging to a job have completed.
  //IMPLEMENT_ME();

  // TODO: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd); ^^^ this is above.
	int status;
	pid_t pid;
	pid_t pfront;
	pid_t pstate;
	JOB job_current;
	size_t num_of_processes;
	size_t num_of_jobs = length_job_deque(&Jobs);

	for(size_t i=0; i<num_of_jobs; i++){
		job_current = pop_front_job_deque(&Jobs);
		num_of_processes = length_pid_deque(&job_current.pid_queue);
		pfront = peek_front_pid_deque(&job_current.pid_queue);
		for(size_t j=0; j<num_of_processes; j++){
			pid = pop_front_pid_deque(&job_current.pid_queue);
			pstate = waitpid(pid, &status, WNOHANG);
			if (pstate == 0){
				push_back_pid_deque(&job_current.pid_queue, pid);
			}
			else {
				assert(pstate == pid);
			}
		}
	 if(!is_empty_pid_deque(&job_current.pid_queue)){
			push_back_job_deque(&Jobs, job_current);
	 }
	 else {
		 print_job_bg_complete(job_current.job_id, pfront, job_current.command);
		 destroy_pid_deque(&job_current.pid_queue);
		 free(job_current.command);
	 }
	}
}


// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

static void print_jobs_helper(JOB j){
  print_job(j.job_id, peek_front_pid_deque(&j.pid_queue), j.command);
}
/***************************************************************************
 * Functions to process commands
 ***************************************************************************/
// Run a program reachable by the path environment variable, relative path, or
// absolute path
void run_generic(GenericCommand cmd) {
  // Execute a program with a list of arguments. The `args` array is a NULL
  // terminated (last string is always NULL) list of strings. The first element
  // in the array is the executable
  char* exec = cmd.args[0];
  char** args = cmd.args;

  // TODO: Remove warning silencers
  //(void) exec; // Silence unused variable warning
  //(void) args; // Silence unused variable warning

  // TODO: Implement run generic
  //IMPLEMENT_ME();
execvp(exec, args);
  perror("ERROR: Failed to execute program");
}

// Print strings
void run_echo(EchoCommand cmd) {
  // Print an array of strings. The args array is a NULL terminated (last
  // string is always NULL) list of strings.
  //char** str = cmd.args;
  // TODO: Remove warning silencers
  //(void) str; // Silence unused variable warning

  // TODO: Implement echo DONE!!
  //IMPLEMENT_ME();
  int i=0;
  while(cmd.args[i] != NULL){
      printf("%s ",cmd.args[i]);
      i++;
    }
  printf("\n");

  // Flush the buffer before returning
  fflush(stdout);
}

// Sets an environment variable
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  // TODO: Remove warning silencers
  //(void) env_var; // Silence unused variable warning
  //(void) val;     // Silence unused variable warning

  // TODO: Implement export.DONE!!
  // HINT: This should be quite simple.
  //IMPLEMENT_ME();
  setenv(env_var,val,1);
}
// Changes the current working directory
void run_cd(CDCommand cmd) {
  // Get the directory name
  const char* dir = cmd.dir;
	char* pwd = realpath(dir, NULL);
  // Check if the directory is valid
  if (dir == NULL) {
    perror("ERROR: Failed to resolve path");
  }
	  chdir(pwd);
		setenv("OLD_PWD", lookup_env("PWD"), 1);
		setenv("PWD",pwd, 1);

  // TODO: Change directory DONE!!

  // TODO: Update the PWD environment variable to be the new current working
  // directory and optionally update OLD_PWD environment variable to be the old
  // working directory.
  //IMPLEMENT_ME();

  free(pwd);
}

// Sends a signal to all processes contained in a job
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;
	size_t queue_length = length_job_deque(&Jobs);
	size_t pid_length;
	pid_t pid;
	JOB job_current;

	for (size_t i=0; i<queue_length; i++)
	{
		job_current = pop_front_job_deque(&Jobs);
		pid_length = length_pid_deque(&job_current.pid_queue);
		if (job_current.job_id==job_id)
		{
			for (size_t j=0; j<pid_length; j++)
			{
				pid = pop_front_pid_deque(&job_current.pid_queue);
				kill(pid,signal);
				push_back_pid_deque(&job_current.pid_queue, pid);
			}
		}
		push_back_job_deque(&Jobs, job_current);
	}

  // TODO: Remove warning silencers
  //(void) signal; // Silence unused variable warning
  //(void) job_id; // Silence unused variable warning

  // TODO: Kill all processes associated with a background job
  //IMPLEMENT_ME();

}


// Prints the current working directory to stdout
void run_pwd() {
  // TODO: Print the current working directory
  //IMPLEMENT_ME();
   bool should_free = true;
   char* pwd = get_current_directory(&should_free);
   printf("%s\n", pwd);
   free(pwd);
  // Flush the buffer before returning
  fflush(stdout);
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // TODO: Print background jobs
  //IMPLEMENT_ME();
	apply_job_deque(&Jobs,print_jobs_helper);
  // Flush the buffer before returning
  fflush(stdout);
}

/***************************************************************************
 * Functions for command resolution and process setup
 ***************************************************************************/

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for child processes.
 *
 * This version of the function is tailored to commands that should be run in
 * the child process of a fork.
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void child_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);
  switch (type) {
  case GENERIC:
	  run_generic(cmd.generic);
    break;

  case ECHO:
    run_echo(cmd.echo);
    break;

  case PWD:
    run_pwd();
    break;

  case JOBS:
    run_jobs();
    break;

  case EXPORT:
  case CD:
  case KILL:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for the quash process.
 *
 * This version of the function is tailored to commands that should be run in
 * the parent process (quash).
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);
  switch (type) {
  case EXPORT:
    run_export(cmd.export);
    break;

  case CD:
    run_cd(cmd.cd);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case GENERIC:
  case ECHO:
  case PWD:
  case JOBS:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief Creates one new process centered around the @a Command in the @a
 * CommandHolder setting up redirects and pipes where needed
 *
 * @note Processes are not the same as jobs. A single job can have multiple
 * processes running under it. This function creates a process that is part of a
 * larger job.
 *
 * @note Not all commands should be run in the child process. A few need to
 * change the quash process in some way
 *
 * @param holder The CommandHolder to try to run
 *
 * @sa Command CommandHolder
 */
void create_process(CommandHolder holder, JOB* job) {
  // Read the flags field from the parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND; // This can only be true if r_out
                                               // is true

  // TODO: Remove warning silencers
  //(void) p_in;  // Silence unused variable warning
  //(void) p_out; // Silence unused variable warning
  //(void) r_in;  // Silence unused variable warning
  //(void) r_out; // Silence unused variable warning
  //(void) r_app; // Silence unused variable warning

  // TODO: Setup pipes, redirects, and new process
  //IMPLEMENT_ME();

  // parent_run_command(holder.cmd); // This should be done in the parent branch of
  //                                 // a fork
  // child_run_command(holder.cmd); // This should be done in the child branch of a fork
int fileNum;
int write = job->num % 2;
int read = (job->num-1) % 2;
pipe(job->pfd[write]);
pid_t pid;
pid = fork();

if(pid == 0){

	if (r_in) {
		fileNum = fileno(fopen(holder.redirect_in,"r"));
		dup2(fileNum, STDIN_FILENO);
		close(fileNum);
	}
	if (r_out&&r_app){
		fileNum = fileno(fopen(holder.redirect_out, "a"));
		dup2(fileNum, STDOUT_FILENO);
		close(fileNum);
		}
	if (r_out&&!r_app){
		fileNum = fileno(fopen(holder.redirect_out, "w"));
		dup2(fileNum, STDOUT_FILENO);
		close(fileNum);
	}

	if(p_in){
		dup2(job->pfd[read][0], STDIN_FILENO);
		close(job->pfd[read][0]);
	} else if(0 <= read-1){
		close(job->pfd[read][0]);
	}

	if (p_out){
		dup2(job->pfd[write][1], STDOUT_FILENO);
		close(job->pfd[write][1]);
	}
	else {
		close(job->pfd[write][1]);
	}
	child_run_command(holder.cmd);
  remove_job(job);
	exit(EXIT_SUCCESS);
}
else {

	if(p_out){
		close(job->pfd[write][1]);
	}
	if(p_in){
		close(job->pfd[read][0]);
	}
	push_back_pid_deque(&job->pid_queue, pid);
	parent_run_command(holder.cmd);
}
++job->num;
}

// Run a list of commands
void run_script(CommandHolder* holders) {
  if (holders == NULL){
    return;
	}
	if (initial) {
    Jobs = new_job_deque(1);
    initial = false;
  }
  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT &&
      get_command_holder_type(holders[1]) == EOC) {
      end_main_loop();
      return;
  }
  CommandType type;
  JOB job;
  init_job(&job);
  // Run all commands in the `holder` array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i)
    create_process(holders[i], &job);
  if (!(holders[0].flags & BACKGROUND)) {
		int stat=0;
    // Not a background Job
    // TODO: Wait for all processes under the job to complete
    //IMPLEMENT_ME();
		if(!is_empty_pid_deque(&job.pid_queue)){
			waitpid(pop_front_pid_deque(&job.pid_queue),&stat,0);
		}
		remove_job(&job);
  }
  else {
	  if(is_empty_job_deque(&Jobs)){
		  job.job_id =1;
	  }
	  else {
		  job.job_id = peek_back_job_deque(&Jobs).job_id +1;
	  }
    // A background job.
    // TODO: Push the new job to the job queue
    //IMPLEMENT_ME();
	  push_back_job_deque(&Jobs, job);
    // TODO: Once jobs are implemented, uncomment and fill the following line
    print_job_bg_start(job.job_id, peek_front_pid_deque(&job.pid_queue), job.command);
	}
}
