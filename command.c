#include "command.h"

/**
 * Check for built-in commands that are not to be treated as programs to be launched. 
 * No child process should be spawned when these are entered.
 * Return:
 *      0 : cmd is not a built-in command
 *      1 : cmd is a build-in command
 */
int builtin_cmd(char **cmd, int num_args) {
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
void execute_shell_cmd(char **cmd, int num_args) {
    char *last_arg = cmd[num_args-1];
    int result = 0;
    // Create a child process
    pid_t pid = fork();
    // Catch fork() error
    if(pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    // Check for ampersand, indicating to run in background
    if(strcmp(last_arg, "&") == 0) {
        // Remove last arg, ensure it ends with NULL pointer
        cmd[num_args - 1] = (char *) NULL;
        // Run process in background
        if (pid == 0){
            // Child process: Print pid to user
            printf("[%d] %s\n", getpid(), cmd[0]);
            result = execvp(cmd[0], cmd);
        }
    }
    else {
        // Ensure cmd ends with NULL pointer
        cmd[num_args] = (char *) NULL;
        // Run process in foreground
        if(pid) {
            // Parent process
            int status;
            pid_t wpid = waitpid(pid, &status, 0);
            // Catch waitpid() error
            if(wpid == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
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
            // Child process: Print pid to user
            printf("[%d] %s\n", getpid(), cmd[0]);
            result = execvp(cmd[0], cmd);
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
void execute_cmd(char **cmd, int num_args) {
    if(!builtin_cmd(cmd, num_args)){
        execute_shell_cmd(cmd, num_args);
    }
}