#include "command.h"

/**
 * Check for built-in commands that are not to be treated as programs to be launched. 
 * No child process should be spawned when these are entered.
 * Return:
 *      0 : cmd is not a built-in command
 *      1 : cmd is a build-in command
 */
int builtin_cmd(char *cmd) {
    if(strcmp(&cmd[0], "exit") == 0) {
        // exit the shell
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(&cmd[0], "pid") == 0) {
        // Print process ID
        printf("%d\n", getpid());
    }
    else if (strcmp(&cmd[0], "ppid") == 0) {
        // Print parent process ID
        printf("%d\n", getppid());
    }
    else if (strcmp(&cmd[0], "cd") == 0) {
        // Change directory
        char* directory;
        if(sizeof(cmd) != 1) {
            directory = &cmd[1];
        }
        else{
            // change to home directory if not args
            directory = getenv("HOME");
        }
        chdir(directory);
    }
    else if (strcmp(&cmd[0], "pwd") == 0) {
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
    else if (strcmp(&cmd[0], "set") == 0) {
        // Set environment variable
        setenv(&cmd[1], &cmd[2], 1);
    }
    else if (strcmp(&cmd[0], "get") == 0) {
        // Get environment variable and print
        printf("%s\n", getenv(&cmd[1]));
    }
    else {
        // Not a built-in command!
        return 0;
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
void execute_shell_cmd(char *cmd) {
    char last_char = cmd[strlen(cmd)-1];
    int result = 0;
    // Check for ampersand, indicating to run in background
    if(last_char == '&') {
        // Run process in background
        result = execvp(&cmd[0], &cmd);
    }
    else {
        // Run process in foreground
        result = execvp(&cmd[0], &cmd);
    }
    // Notify the user, requested command is not found and cannot be run.
    // Requirement #5.
    if(result == -1) {
        printf("Requested command \"%s\" was not found and cannot be run.\n", &cmd[0]);
    }
}

/**
 * Execute a command. First check if it is a built-in command.
 * If not, attempt to execute a shell command.
 */
void execute_cmd(char *cmd) {
    if(!builtin_cmd(cmd)){
        execute_shell_cmd(cmd);
    }
}