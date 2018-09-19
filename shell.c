#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

    // Infinite loop until user requests to exit (Requirement #2)
    while(1) {
        char *input = NULL;
        size_t size = 0;
        ssize_t chars;
        int num_args = 0;
        int max_args = 64;
        char *arg_vector[max_args];
        // pid_t pid;
        // int status;

        // Print user prompt
        printf("%s", user_prompt);
        // read from user command line
        if((chars = getline(&input, &size, stdin)) == -1) {
            perror("Could not read from user command line");
            exit(EXIT_FAILURE);
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
	/*int i;
        for(i=0;i<num_args;i++) {
            printf("Arg %d is \"%s\"\n", i, arg_vector[i]);
        }
        printf("Number of args: %d\n", num_args);
*/
        // Execute user command and print result
        execute_cmd(arg_vector, num_args);
        // free buffer allocated for input
        free(input);
    }
    return 0;
}
