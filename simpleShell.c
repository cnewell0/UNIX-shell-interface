/**
 Author: Cade Newell
 Project 3
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


#define MAX_LINE		80 /* 80 chars per line, per command, should be enough. */
#define MAX_COMMANDS	9 /* size of history */

char history[MAX_COMMANDS][MAX_LINE];
char display_history [MAX_COMMANDS][MAX_LINE];


int command_count = 0;

/**
 * Add the most recent command to the history.
 */

void addtohistory(char inputBuf[]) {

    int i = 0;

    strcpy(history[command_count % MAX_COMMANDS], inputBuf);

    while (inputBuf[i] != '\n' && inputBuf[i] != '\0') {
        display_history[command_count % MAX_COMMANDS][i] = inputBuf[i];
        i++;
    }
    display_history[command_count % MAX_COMMANDS][i] = '\0';
    ++command_count;


    return;
}

/**
 * The setup function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings.
 */

int setup(char inputBuf[], char *args[],int *bufCheck)
{
    int length,
        i,
        next,
        commandNumber,
        nextIndex;

    next = 0;

    do {
        printf("osh>");
        fflush(stdout);
        length = read(STDIN_FILENO,inputBuf,MAX_LINE);
    }
    while (inputBuf[0] == '\n');

    if (length == 0)
        exit(0);

    if ( (length < 0) ) {
        perror("problem reading command");
        exit(-1);
    }


    if (inputBuf[0] == '!') {
        if (command_count == 0) {
            printf("No history\n");
            return 1;
        } else if (inputBuf[1] == '!') {
            strcpy(inputBuf, history[(command_count - 1) % MAX_COMMANDS]);
            length = strlen(inputBuf) + 1;
        } else if (isdigit(inputBuf[1])) {
            commandNumber = atoi(&inputBuf[1]);
            strcpy(inputBuf, history[commandNumber]);
            length = strlen(inputBuf) + 1;
        }
    }

     addtohistory(inputBuf);

    for (i=0;i<length;i++) {

        switch (inputBuf[i]){
            case ' ':
            case '\t' :
                if (nextIndex != -1) {
                    args[next] = &inputBuf[nextIndex];
                    next++;
                }
                inputBuf[i] = '\0';
                nextIndex = false;
                break;

            // use boolean
            case '\n':
                if (nextIndex != -1) {
                    args[next] = &inputBuf[nextIndex];
                    next++;
                }
                inputBuf[i] = '\0';
                args[next] = NULL;
                break;

            default :
                if (nextIndex == -1)
                    nextIndex = i;
                if (inputBuf[i] == '&') {
                    *bufCheck  = 1;
                    inputBuf[i-1] = '\0';
                }
        }
        args[next] = NULL;
    }

    if (*bufCheck)
        args[--next] = NULL;
    return 1;
}



int main(void)
{
    char inputBuf[MAX_LINE]; 	/* buffer to hold the command entered */
    int bufCheck;             	/* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    pid_t child;            		/* process id of the child process */
    int result;

    int i, upper;

    int process = 1;



    while (process){
        bufCheck = 0;

        process = setup(inputBuf,args,&bufCheck);

        if (strncmp(inputBuf, "exit", 4) == 0)
            return 0;
        else if (strncmp(inputBuf,"history", 7) == 0) {
            if (command_count < MAX_COMMANDS)
                upper = command_count;
            else
                upper = MAX_COMMANDS;

            for (i = 0; i < upper; i++) {
                printf("%d \t %s\n", i, display_history[i]);
            }

            continue;
        }


            if (process) {child = fork();
                switch (child) {
                    case -1:
                        perror("Process could not fork");
                        break;

                    case 0:
                        result = execvp(args[0],args);
                        if (result != 0){
                            perror("error with execvp");
                            exit(-2);
                        }
                        break;

                    default :
                        if (bufCheck == 0)
                            while (child != wait(NULL));


        }
    }
}

return 0;
}