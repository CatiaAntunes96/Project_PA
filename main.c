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

// void file_opt(char *filename);

int file_check(char *filename);

char *extract(char *filename);

//char **file_list(char **args, int n_args);

int main(int argc, char *argv[])
{
    struct gengetopt_args_info args_info;

    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        exit(1);
    }

    int num_files = args_info.file_given;

    char **files_list = NULL;
    files_list = (char **)MALLOC(sizeof(char *) * num_files);

    int i;

    for (i = 0; i < num_files; i++)
    {
        files_list[i] = (char *)MALLOC(sizeof(char) * (strlen(args_info.file_arg[i]) + 1));
        strcpy(files_list[i], args_info.file_arg[i]);
    }

    for (i = 0; i < num_files; ++i)
    {
        char *filename = files_list[i];
        if (file_check(filename) > 0)
        {
            continue;
        }
        char *extension = extract(filename);
        DEBUG("%s", extension);
    }

    FREE(files_list);

    // int fd_out;

    // char *out_filename = "./out.txt";

    // pid_t pid = fork();

    // switch (pid)
    // {
    // case -1:
    //     ERROR(5, "fork failed\n");
    //     break;
    // case 0:

    //     fd_out = open(out_filename, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0777);

    //     if (fd_out == -1)
    //     {
    //         ERROR(3, "Failed to create output file\n");
    //     }

    //     dup2(fd_out, STDOUT_FILENO);

    //     if (close(fd_out) == -1)
    //     {
    //         ERROR(4, "Failed to close file descriptor %d\n", fd_out);
    //     }

    //     execlp("file", "file", "-b", "E", filename, NULL);
    //     break;

    // default:
    //     wait(NULL);
    // }

    // fd_out = open(out_filename, O_RDONLY);

    // if (fd_out == -1)
    // {
    //     ERROR(7, "Failed to open outuput file");
    // }

    // char str_type[5] = "";
    // char buffer;

    // for (int i = 0; i < 5; ++i)
    // {
    //     if (read(fd_out, &buffer, 1) == -1)
    //     {
    //         ERROR(6, "Failed to read from output file");
    //     }
    //     if (buffer == 32)
    //     {
    //         break;
    //     }

    //     str_type[i] += tolower(buffer);
    // }

    // printf("%s\n", str_type);

    // if (close(fd_out) == -1)
    // {
    //     ERROR(7, "Failed to close output file");
    // }

    // int comp;
    // int j = 0;
    // do
    // {
    //     comp = strcmp(str_type, EXTENSIONS[j]);
    //     j++;
    // } while (comp != 0 && j < 7);

    // if (comp != 0)
    // {
    //     printf("[INFO] '%s': type '%s' is not supported by checkFile\n", filename, str_type);
    //     exit(1);
    // }

    // if (strcmp(str_type, new_str_ext) != 0)
    // {
    //     printf("[MISMATCH] '%s': extension is '%s', file type is '%s'\n", filename, new_str_ext, str_type);
    // }
    // else
    // {
    //     printf("[OK] '%s': extension '%s' matches file type '%s'\n", filename, new_str_ext, str_type);
    // }

    // ler o out.txt para uma string e fazer a comparação com o filename e com o array de extensoes suportadas

    cmdline_parser_free(&args_info);

    return 0;
}

int file_check(char *filename)
{
    FILE *fptr = NULL;
    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        fprintf(stderr, "[ERROR] cannot open file '%s' -- %s\n", filename, strerror(errno));
        return 1;
    }

    fclose(fptr);
    return 0;
}

// char **file_list(char **args, int n_args)
// {
//     int i;

//     char **file_list = (char **)MALLOC((sizeof(char *)) * n_args);

//     for (i = 0; i < n_args; ++i)
//     {
//         char *filename = (char *)MALLOC((sizeof(char)) * 100);
//         strcpy(filename, args[i]);
//         strcpy(file_list[i], filename);
//         FREE(filename);
//     }

//     FREE(file_list);
// }

char *extract(char *filename)
{
    char *chr = strrchr(filename, 46);
    int i;
    char *str_ext = NULL;

    if (chr == NULL)
    {
        printf("[WARNING] '%s' filename does not have an extension\n", filename);
    }
    else
    {
        int n_bytes = (int)strlen(chr) + 1;
        str_ext = MALLOC(n_bytes);

        for (i = 0; i < n_bytes; ++i)
        {
            str_ext[i] += chr[i + 1];
        }
    }

    return str_ext;
}