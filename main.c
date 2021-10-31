/**
 * @file main.c
 * @brief CLI checkFile
 * @date 2021-11-06
 * @author Cátia Antunes & Inês Machado
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>

#include "args.h"
#include "debug.h"
#include "memory.h"
#include "sys.h"
#include "utils.h"
#include "checkers.h"
#include "getters.h"

#define C_ERR_SIGQUIT 1
#define C_ERR_CMDL_PARSER 2
#define C_ERR_SIGUSR1 3

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_sigaction = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGQUIT, &act, NULL) < 0)
    {
        ERROR(C_ERR_SIGQUIT, "sigaction - SIGQUIT");
    }

    struct gengetopt_args_info args_info;

    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        exit(C_ERR_CMDL_PARSER);
    }

    if (args_info.file_given > 0)
    {
        f_option(args_info.file_arg, args_info.file_given);
    }
    else if (args_info.batch_given > 0)
    {
        if (sigaction(SIGUSR1, &act, NULL) < 0)
        {
            ERROR(C_ERR_SIGUSR1, "sigaction - SIGUSR1");
        }

        time_t start_time = time(NULL);
        struct tm *time_data = localtime(&start_time);
        strftime(G_time, sizeof(G_time), "%Y.%m.%d_%Hh%M:%S", time_data);

        b_option(args_info.batch_arg);
    }
    else if (args_info.directory_given > 0)
    {
        d_option(args_info.directory_arg);
    }
    else
    {
        char *student1 = "Ines Machado - 2200723";
        char *student2 = "Catia Antunes - 2212492";
        printf("Authors:\n%s\n%s\nFile types supported by checkFile:\n", student1, student2);
        show_extensions();
    }

    cmdline_parser_free(&args_info);

    return 0;
}
