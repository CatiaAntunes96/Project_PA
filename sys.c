/**
 * Ines Alexandra Ribeiro Machado   2200723
 * Catia Sofia Ribeiro Antunes      2212492
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "sys.h"
#include "debug.h"
#include "memory.h"

// performs an exec system call to file over a given file and
// writes the stdout to a file
void exec_call(char *out_filename, char *filename)
{

    pid_t pid = fork();

    FILE *fout = NULL;

    switch (pid)
    {
    case -1:
        WARNING("fork() failed");
        break;
    case 0:
        fout = freopen(out_filename, "w+", stdout);

        if (fout == NULL)
        {
            WARNING("Redirecting fault");
        }

        execlp("file", "file", "-bE", filename, NULL);

        break;

    default:
        wait(NULL);
    }
}

// performs an exec system call to find the files in a given directory
// and writes the stdout to a file
void exec_call_dir(char *out_filename, char *dirname)
{

    pid_t pid = fork();

    FILE *fout = NULL;

    switch (pid)
    {
    case -1:
        WARNING("fork() failed");
        break;
    case 0:

        fout = freopen(out_filename, "w+", stdout);

        if (fout == NULL)
        {
            WARNING("Redirecting fault");
        }

        execlp("find", "find", dirname, "-maxdepth", "1", "-type", "f", NULL);

        break;

    default:
        wait(NULL);
    }
}

// same function as the one presented on class' signals sheet example 2
void signal_handler(int signal, siginfo_t *siginfo, void *context)
{
    (void)context;

    int aux = errno;

    if (signal == SIGQUIT)
    {
        printf("Captured SIGQUIT signal (sent by PID: %ld). Use SIGINT to terminate application\n",
               (long)siginfo->si_pid);
        pause();
    }

    if (signal == SIGUSR1)
    {
        printf("Batch process started at %s\nProcessing file no. %d '%s'\n", G_time, G_filenumber,
               G_filename);
    }

    errno = aux;
}