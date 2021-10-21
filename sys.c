
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "sys.h"
#include "debug.h"
#include "memory.h"

void exec_call(char *out_filename, char *filename, char *command, char *options)
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

        execlp(command, command, options, filename, NULL);

        break;

    default:
        wait(NULL);
    }
}

void exec_call_dir(char *out_filename, char *filename, char *command)
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

        execlp(command, command, filename, NULL);

        break;

    default:
        wait(NULL);
    }
}