#ifndef SYS_H
#define SYS_H

extern char G_time[20];
extern char *G_filename;
extern int G_filenumber;

void exec_call(char *out_filename, char *filename);
void exec_call_dir(char *out_filename, char *dirname);
void signal_handler(int signal, siginfo_t *siginfo, void *context);

#endif