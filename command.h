#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//jobs struct for extra credit
typedef struct job {
	int pid;
	char name[100];
} job;

/**
 * Check for built-in commands that are not to be treated as programs to be launched. 
 * No child process should be spawned when these are entered.
 * Return:
 *      0 : cmd is not a built-in command
 *      1 : cmd is a build-in command
 */
int builtin_cmd(char **cmd, int num_args, struct job *joblist, int num_jobs);

/**
 * For each executed command, the shell should spawn a child process to run the command. 
 * The shell should try to run the command exactly as typed. This requires the user to know the full
 * path to the executable, unless the directory is listed in the PATH environment variable.
 * Requirement #4.
 * 
 * If the last character in the user input is an ampersand (&), the child should be run in the background
 * Remove the & when passing the parameters to exec. 
 * Requirement #9.
 */
void execute_shell_cmd(char **cmd, int num_args, struct job *joblist, int num_jobs, char *file_name, int redirect);

/**
 * Execute a command. First check if it is a built-in command.
 * If not, attempt to execute a shell command.
 */
void execute_cmd(char **cmd, int num_args, struct job *joblist, int num_jobs, char *file_name, int redirect);

/**
 * Redirect output from a command to specifed file_name. After completion
 * of command, output will redirect to terminal.
 */
void redirect_output(char *file_name);

#endif 
