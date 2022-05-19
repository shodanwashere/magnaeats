#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "log.h"
#include "metime.h"

FILE * open_logfile(char *name){
    return fopen(name, "a");
}

void generate_log_line(char* action, char* output){
    char* time;
    get_curr_time_string(time);
    sprintf(output, "%s %s\n", time, action);
}

int write_log(FILE *logfile, char* line){
    return fputs(line, logfile);
}

int close_logfile(FILE *logfile){
    fclose(logfile);
}

void log(FILE *logfile, char* action, int action_size, int args, ...){ // increases modularity
    va_list ap;                                                        // type for the list of arguments
    char* complete_action = malloc(sizeof(char) * (action_size + 30)); // generates a string for the complete action
    strcpy(complete_action, action);                                   // put the basic action first
    

    va_start(ap, args);                                                // start up the argument list with a macro
    for(int i = 0;  i < args; i++){                                    // cycle that concatenates all the arguments into the action line
        to_cat = va_arg(ap, char*)
        strcat(complete_action, " ");
        strcat(complete_action, to_cat);
    }

    char* logline = malloc(sizeof(char) * (strlen(complete_action) + 30)); 
    generate_log_line(complete_action, logline);                       // concatenate the current time with the action

    write_log(logfile, logline);                                       // write to logfile
    free(complete_action);                                             // free all allocated memory
    free(logline);

    va_end(ap);                                                        // end all actions with the argument list
}

void logerr(char* err_msg){
    char *logline = malloc(sizeof(char) * (strlen(err_msg) + 30));     // generate a logline ready for the concatenation of the 
    generate_log_line(err_msg, logline);                               // concatenation of the error message with the current time

    write_log(logfile, logline);                                       // write to logfile

    free(logline);                                                     // free reserved memory
}