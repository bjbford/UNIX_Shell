#include "command.h"

int main(int argc, char **argv) {
    char default_prompt[] = "308sh> ";
    char user_prompt[100];
    // check for user prompt command line option (Requirement #1)
    if(argc > 1 && !strcmp(argv[1], "-p") && argv[2] != NULL) {
        strcpy(user_prompt, argv[2]);
    }      
    else {
        strcpy(user_prompt, default_prompt);
    }
    printf("User prompt is: \"%s\"\n", user_prompt);

    // Init list of jobs for extra credit
    struct job joblist[20];
    int num_jobs = 0;

    // Infinite loop until user requests to exit (Requirement #2)
    while(1) {
        char *input = NULL;
        size_t size = 0;
        ssize_t chars;
        int num_args = 0;
        int max_args = 64;
        char *arg_vector[max_args];

        // Print user prompt
        printf("%s", user_prompt);
        // read from user command line
        if((chars = getline(&input, &size, stdin)) == -1) {
            perror("Could not read from user command line");
            exit(EXIT_FAILURE);
        }
        // Check if the user just hit enter (Reprompt)
        if(strcmp(input, "\n") == 0 && strlen(input) == 1) {
            continue;
        }
        // Change '\n' at end of input to null terminator
        input[strlen(input) - 1] = '\0';
        // Get first argument delimited by space
        char *argument = strtok(input, " ");	
        // Create array of user arguments from input string delimited by space
        while((argument != NULL) && (num_args < max_args)) {
            arg_vector[num_args++] = argument;
            argument = strtok(NULL, " ");
        }
        // Catch max arg warning and notify user.
        if(num_args >= max_args) {
            printf("Warning: You reached the maximum allowable number of arguments: %d\n", max_args);
        }
        char *file_name = (char *) NULL;
        int redirect = 0;
        // Extra credit output to a file
        if(strcmp(arg_vector[num_args - 2], ">") == 0) {
            // output to file in last_arg
            file_name = arg_vector[num_args - 1];
            // set redirect flag
            redirect = 1;
            // remove > and filename from arg_vector
            arg_vector[num_args - 1] = (char *) NULL;
            arg_vector[num_args - 2] = (char *) NULL;
            num_args = num_args - 2;
        }
        // Print the exit status of a background child process. Requirement #9
        int i;
		for(i = 0;i<=num_jobs;i++) {
            int status;
            pid_t wpid = waitpid(joblist[i].pid, &status, WNOHANG);
            if(wpid > 0) {
                // Obtain status info from child's processes
                if(WIFEXITED(status)) {
                    printf("Background: [%d] \"%s\" Exit %d\n", joblist[i].pid, joblist[i].name, WEXITSTATUS(status));
                }
                else if(WIFSIGNALED(status)){
                    printf("Background: [%d] \"%s\" Killed (%d)\n", joblist[i].pid, joblist[i].name, WTERMSIG(status));
                }
                // Background process finished, shrink list
				int j;
				for(j = 0;j<=num_jobs;j++) {
                    job next_job = joblist[j+1];
					joblist[j] = next_job;
				}
				num_jobs--;
				i--;
            }
		}
        // Execute user command and print result
        execute_cmd(arg_vector, num_args, joblist, num_jobs, file_name, redirect);
        // free buffer allocated for input
        free(input);
    }
    return 0;
}