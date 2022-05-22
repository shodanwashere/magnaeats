#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include <stdio.h>

// Estabilish the opening of the logfile
FILE * open_logfile(char *name);

void generate_log_line(char* action, char* output);

// write a line to the log file
int write_log(FILE *logfile, char* line);

// close the logfile
int close_logfile(FILE *logfile);

// Log the latest action
void log(FILE *logfile, char* action, int action_size, int args, ...);

// Write a log explaining an error
void logerr(FILE *logfile, char* err_msg);

#endif