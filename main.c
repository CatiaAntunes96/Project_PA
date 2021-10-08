/**
 * @file main.c
 * @brief Description
 * @date 2018-1-1
 * @author name of author
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "args.h"
#include "debug.h"
#include "memory.h"

int main(int argc, char *argv[])
{
    struct gengetopt_args_info args_info;

    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        exit(1);
    }

    char extensions[7][6] = {".pdf", ".gif", ".jpg", ".png", ".mp4", ".zip", ".html"};

    int num_files = args_info.file_given;

    char *filename = (char *)malloc(sizeof(char) * 2);

    strcpy(filename, args_info.file_arg);
    
    printf("%s\n", filename);

    printf("%d\n", num_files);

    /*for (int i = 0; i < num_files; i++)
    {
        // falta ver como percorrer os argumentos da opção
        char *filename = args_info.file_arg;
        FILE *fptr = NULL;
        fptr = fopen(filename, "r");

        if (fptr == NULL)
        {
            ERROR(1, "cannot open file '%s'", filename);
        }

        char *str_ext = strrchr(filename, 46);

        /*int comp;
        int j = 0;
        do
        {
            comp = strcmp(str_ext, extensions[j]);
            j++;
        } while (comp != 0 && j < 7);

        if(comp != 0) {
            ERROR(2, "[INFO] '%s': typ")
        }

        for (int i = 0; i < 7; i++)
        {
            if (strcmp(str_ext, extensions[i]) == 0)
            {
                break;
            }
        }

        pid_t pid = fork();
        switch (pid)
        {
        case -1:
            ERROR(5, "fork failed\n");
            break;
        case 0:
            int fd_out = open("./out.txt", O_CREAT | O_EXCL);
            if (fd_out == -1)
            {
                ERROR(3, "Failed to create output file\n");
            }

            int fd_err = open("./err.txt", O_CREAT | O_EXCL);
            if (fd_err == -1)
            {
                ERROR(3, "Failed to create error file\n");
            }

            dup2(fd_out, STDOUT_FILENO);
            dup2(fd_err, STDERR_FILENO);

            if (close(fd_out) == -1)
            {
                ERROR(4, "Failed to close file descriptor %d\n", fd_out);
            }

            if (close(fd_err) == -1)
            {
                ERROR(4, "Failed to close file descriptor %d\n", fd_err);
            }

            execlp("file", "file", "-b", NULL);

        default:
            waitpid(pid, NULL, 0);
            break;
        }

        fclose(fptr);
    }*/

    free(filename);

    cmdline_parser_free(&args_info);

    return 0;
}
