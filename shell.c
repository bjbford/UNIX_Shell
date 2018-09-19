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
        // Execute user command and print result
        // TODO: delimit input by whitespace and make a vector of args
        execute_cmd(input);
        // free buffer allocated for input
        free(input);
    }
    return 0;
}