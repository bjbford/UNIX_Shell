#include "command.h"

/**
 * Check for built-in commands that are not to be treated as programs to be launched. 
 * No child process should be spawned when these are entered.
 * Return:
 *      0 : cmd is not a built-in command
 *      1 : cmd is a build-in command
 */
int builtin_cmd(char **cmd, int num_args, struct job *joblist, int num_jobs) {
    int result = 0;
    if(strcmp(cmd[0], "exit") == 0) {
        // exit the shell
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(cmd[0], "pid") == 0) {
        // Print process ID
        printf("%d\n", getpid());
    }
    else if (strcmp(cmd[0], "ppid") == 0) {
        // Print parent process ID
        printf("%d\n", getppid());
    }
    else if (strcmp(cmd[0], "cd") == 0) {
        // Change directory, NOTE: '~' doesn't work with chdir()
        char* directory;
        if(num_args >= 2) {
            directory = cmd[1];
        }
        else{
            // change to home directory if not args
            directory = getenv("HOME");
        }
        result = chdir(directory);
    }
    else if (strcmp(cmd[0], "pwd") == 0) {
        // Print the working directory
        long size;
        char* buffer;
        char* ptr;
        
        size = pathconf(".", _PC_PATH_MAX);
        // allocate buffer to hold absolute path
        buffer = (char *)malloc((size_t) size);

        // Check if pathname exceeded buffer size
        if(buffer != NULL) {
            // ptr points to buffer that holds pathname
            ptr = getcwd(buffer, (size_t) size);
            printf("%s\n", ptr);
        }
        else {
            // Print error that name exceeded buffer size
            perror(NULL);
        }
        free(buffer);
    }
    else if (strcmp(cmd[0], "set") == 0) {
        // Check for correct number of arguments
        if (num_args == 3) {
            // Set environment variable
            result = setenv(cmd[1], cmd[2], 1);
        }
        else if (num_args < 3) {
            printf("Not enough arguments for \"set\" command. Format should be \"set <var> <value>\".\n");
        }
        else {
            printf("Too many arguments for \"set\" command. Format should be \"set <var> <value>\".\n");
        }
    }
    else if (strcmp(cmd[0], "get") == 0) {
        // Check for correct number of arguments
        if (num_args == 2) {
            // Get environment variable and print if found
            char *env_var = getenv(cmd[1]);
            if(env_var == NULL) {
                printf("There is no environment variable \"%s\"\n", cmd[1]);
            }
            else {
                printf("%s\n", getenv(cmd[1]));
            }
        }
        else if (num_args < 2) {
            printf("Not enough arguments for \"get\" command. Format should be \"get <var>\".\n");
        }
        else {
            printf("Too many arguments for \"get\" command. Format should be \"get <var>\".\n");
        }
    }
    else if(strcmp(cmd[0], "jobs") == 0) {
        // Jobs command
        int i;
        printf("MADE IT HERE... num_jobs: %d\n", num_jobs);
        for(i = 0;i<num_jobs;i++) {
            printf("PID: [%d] Command: \"%s\" running.\n", joblist[i].pid, joblist[i].name);
        }
    }
    else {
        // Not a built-in command!
        return 0;
    }
    // Print errno to user.
    if(result == -1) {
        perror(NULL);
    }
    return 1;
}

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
void execute_shell_cmd(char **cmd, int num_args, struct job *joblist, int num_jobs) {
    int result = 0;
    // Check for ampersand, indicating to run in background
    if(strcmp(cmd[num_args-1], "&") == 0) {
        // Add background process to joblist
        strcpy(joblist[num_jobs].name, cmd[0]);
        joblist[num_jobs].pid = fork();

        if(joblist[num_jobs].pid == 0) {
            // Child process
            // Replace '&' with NULL pointer
            cmd[num_args - 1] = (char *) NULL;
            // Remove last arg, ensure it ends with NULL pointer
            cmd[num_args] = (char *) NULL;
            // Execute command
            result = execvp(cmd[0], cmd);
        }
        else if(joblist[num_jobs].pid > 0){
            //Parent process
            printf("[%d] %s\n", joblist[num_jobs].pid, joblist[num_jobs].name);
            // Iterate 
            num_jobs++;
        }
        else {
            // Catch fork() error
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    else {
        // Foreground process
        int pid = fork();
        if(pid == 0) {
            // Child process
            printf("[%d] %s\n", getpid(), cmd[0]);
            // Ensure cmd ends with NULL pointer
            cmd[num_args] = (char *) NULL;
            // Execute command
            result = execvp(cmd[0], cmd);
        }
        else if (pid > 0){
            // Parent process
            int status;
            while(waitpid(pid, &status, WNOHANG) == 0)
            {
                // Waiting for the process to complete since foreground
            }
            // Obtain status info from child's processes
            if(WIFSIGNALED(status)){
                printf("[%d] %s Killed (%d)\n", pid, cmd[0], WTERMSIG(status));
            }
            else if(WIFEXITED(status)) {
                printf("[%d] %s Exit %d\n", pid, cmd[0], WEXITSTATUS(status));
            }
        }
        else {
            // Catch fork() error
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    // Notify the user, requested command is not found and cannot be run.
    // Requirement #5.
    if(result == -1) {
        char *error_string = strerror(errno);
        printf("Cannot execute \"%s\": %s\n", cmd[0], error_string);
        exit(EXIT_FAILURE);
    }
}

/**
 * Execute a command. First check if it is a built-in command.
 * If not, attempt to execute a shell command.
 */
void execute_cmd(char **cmd, int num_args, struct job *joblist, int num_jobs) {
    if(!builtin_cmd(cmd, num_args, joblist, num_jobs)){
        execute_shell_cmd(cmd, num_args, joblist, num_jobs);
    }
}