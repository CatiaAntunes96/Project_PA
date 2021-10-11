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

char EXTENSIONS[7][6] = {"pdf", "gif", "jpg", "png", "mp4", "zip", "html"};

int main(int argc, char *argv[])
{
    struct gengetopt_args_info args_info;

    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        exit(1);
    }

    // int num_files = args_info.file_given;

    // char **files_list = NULL;
    // files_list = MALLOC(sizeof(char *) * num_files);

    // for (int i = 0; i < num_files; i++)
    // {
    //     char *filename = MALLOC(sizeof(char) * (strlen(args_info.file_arg[i]) + 1));
    //     strcpy(filename, args_info.file_arg[i]);
    //     FREE(filename);
    // }

    // FREE(files_list);

    char *filename = args_info.file_arg[0];
    FILE *fptr = NULL;
    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        ERROR(1, "cannot open file '%s'", filename);
    }

    fclose(fptr);

    char *str_ext = strrchr(filename, 46);
    char new_str_ext[(int)strlen(str_ext)];

    if (str_ext == NULL)
    {
        str_ext = "";
        printf("[WARNING] '%s' filename does not have an extension\n", filename);
    }
    else
    {
        for (int i = 0; i < (int)strlen(str_ext); i++)
        {
            new_str_ext[i] = str_ext[i + 1];
        }
    }

    int fd_out;
    int fd_err;

    char *out_filename = "./out.txt";
    char *err_filename = "./err.txt";

    pid_t pid = fork();

    switch (pid)
    {
    case -1:
        ERROR(5, "fork failed\n");
        break;
    case 0:

        fd_out = open(out_filename, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0777);

        if (fd_out == -1)
        {
            ERROR(3, "Failed to create output file\n");
        }

        fd_err = open(err_filename, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0777);
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

        execlp("file", "file", "-b", filename, NULL);
        break;

    default:
        wait(NULL);
    }

    fd_out = open(out_filename, O_RDONLY);

    if (fd_out == -1)
    {
        ERROR(7, "Failed to open outuput file");
    }

    char str_type[5] = "";
    char buffer;

    for (int i = 0; i < 5; ++i)
    {
        if (read(fd_out, &buffer, 1) == -1)
        {
            ERROR(6, "Failed to read from output file");
        }
        if (buffer == 32)
        {
            break;
        } 

        str_type[i] += tolower(buffer);
    }

    printf("%s\n", str_type);

    if (close(fd_out) == -1)
    {
        ERROR(7, "Failed to close output file");
    }

    int comp;
    int j = 0;
    do
    {
        comp = strcmp(str_type, EXTENSIONS[j]);
        j++;
    } while (comp != 0 && j < 7);

    if (comp != 0)
    {
        printf("[INFO] '%s': type '%s' is not supported by checkFile\n", filename, str_type);
        exit(1);
    }

    if (strcmp(str_type, new_str_ext) != 0)
    {
        printf("[MISMATCH] '%s': extension is '%s', file type is '%s'\n", filename, new_str_ext, str_type);
    }
    else
    {
        printf("[OK] '%s': extension '%s' matches file type '%s'\n", filename, new_str_ext, str_type);
    }

    // ler o out.txt para uma string e fazer a comparação com o filename e com o array de extensoes suportadas

    cmdline_parser_free(&args_info);

    return 0;
}
