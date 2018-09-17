#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    char default_prompt[] = "308sh> ";
    char user_prompt[100];
    // check for user prompt command line option
    if(argc > 1 && !strcmp(argv[1], "-p") && argv[2] != NULL) {
        strcpy(user_prompt, argv[2]);
    }      
    else {
        strcpy(user_prompt, default_prompt);
    }
    printf("User prompt is: \"%s\"\n", user_prompt);
    return 0;
}