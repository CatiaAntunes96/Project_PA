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
#include <ctype.h>

#include "args.h"
#include "debug.h"
#include "memory.h"

#define SIZE_EXT 5
#define NUM_EXT 7

char *G_extensions[NUM_EXT] = {"pdf", "gif", "jpg", "png", "mp4", "zip", "html"};

// void file_opt(char *filename);

int file_check(char *filename);

int path_check(char *filename);

char *extract(char *filename);

void exec_call(char *out_filename, char *filename, char *command, char *options);

char *get_file_type(char *out_filename);

void type_check(char *str_type, char *filename);

void cmp_ext_type(char *ext, char *type, char *filename);

char **read_batch(char *filename, int *total);

//char **file_list(char **args, int n_args);

int main(int argc, char *argv[])
{
    struct gengetopt_args_info args_info;

    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        exit(1);
    }

    if (args_info.file_given > 0)
    {

        // Number of files passed by prompt
        int num_files = args_info.file_given;

        char **files_list = NULL;
        files_list = (char **)MALLOC(sizeof(char *) * num_files);

        int i;

        for (i = 0; i < num_files; i++)
        {
            // array of filenames recieved from terminal
            files_list[i] = (char *)MALLOC(sizeof(char) * (strlen(args_info.file_arg[i]) + 1));
            strcpy(files_list[i], args_info.file_arg[i]);
        }

        for (i = 0; i < num_files; ++i)
        {
            // indiviual filename from filenames array
            char *filename = files_list[i];

            // filaneme for stdout redirect
            char *output_filename = "./out.txt";

            // command for exec call
            char *comm = "file";

            // options for the command called on exec
            char *opt = "-b";

            if (!file_check(filename))
            {
                continue;
            }

            // string containing the extension portion of the filename
            char *extension = extract(filename);
            if (extension == NULL)
            {
                continue;
            }

            // performs a system call on the command and options passed by argument and redirects stdout to an output file passed by argument as well
            exec_call(output_filename, filename, comm, opt);

            // reads from output file of command called by exec and retrives the extension to string
            char *file_type = get_file_type(output_filename);

            // checks if the file type is supported by the app, runs the string against an array of predefined extensions
            type_check(file_type, filename);

            // basic comparison between the extension on the filename passed by the user and the file type obtained from the file command
            cmp_ext_type(extension, file_type, filename);

            FREE(filename);
        }


        FREE(files_list);
    }
    else if (args_info.batch_given > 0)
    {
        char *batch_filename = args_info.batch_arg;
        file_check(batch_filename);

        char *btype = "./btype.txt";
        char *comm = "file";
        char *opt = "-bE";

        exec_call(btype, batch_filename, comm, opt);

        char *batch_type = get_file_type(btype);

        cmp_ext_type(batch_type, "ascii", batch_filename);

        char **files_list = NULL;
        int n_files = 0;
        int i;

        files_list = read_batch(batch_filename, &n_files);

        for (i = 0; i < n_files; ++i)
        {
            char *filename = files_list[i];
            char *output_filename = "./out.txt";

            if (!file_check(filename))
            {
                continue;
            }
            char *extension = extract(filename);
            if (extension == NULL)
            {
                continue;
            }

            exec_call(output_filename, filename, comm, opt);

            

            char *file_type = get_file_type(output_filename);

        
        
            type_check(file_type, filename);

            cmp_ext_type(extension, file_type, filename);

            FREE(filename);
        }

        FREE(files_list);
    }
    else
    {
    }

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
        return 0;
    }

    fclose(fptr);
    return 1;
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
        WARNING("[WARNING] '%s' filename does not have an extension", filename);
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

char *get_file_type(char *out_filename)
{
    char *str_type = NULL;
    char buffer;
    int fd;

    fd = open(out_filename, O_RDONLY);

    if (fd == -1)
    {
        ERROR(1, "Failed to open output file");
    }

    str_type = MALLOC(SIZE_EXT);

    for (int i = 0; i < SIZE_EXT; ++i)
    {
        if (read(fd, &buffer, 1) == -1)
        {
            ERROR(1, "Failed to read from output file");
        }
        if (buffer == 32)
        {
            break;
        }

        str_type[i] += tolower(buffer);
    }

    if (close(fd) == -1)
    {
        ERROR(1, "Failed to close output file");
    }

    return str_type;
}

void type_check(char *str_type, char *filename)
{
    int comp;
    int j = 0;

    do
    {
        comp = strcmp(str_type, G_extensions[j]);
        j++;
    } while (comp != 0 && j < NUM_EXT);

    if (comp != 0)
    {
        printf("[INFO] '%s': type '%s' is not supported by checkFile\n", filename, str_type);
        exit(1);
    }
}

void cmp_ext_type(char *ext, char *type, char *filename)
{
    if (strcmp(ext, type) != 0)
    {
        printf("[MISMATCH] '%s': extension is '%s', file type is '%s'\n", filename, ext, type);
    }
    else
    {
        printf("[OK] '%s': extension '%s' matches file type '%s'\n", filename, ext, type);
    }
}

char **read_batch(char *filename, int *total)
{
    char **files_list = NULL;
    char ch;
    int n_lines = 0;
    int i;
    *total = 0;

    FILE *f = fopen(filename, "r");

    if (f == NULL)
    {
        WARNING("Could not open file '%s' for reading", filename);
    }
    else
    {
        while ((ch = fgetc(f)) != EOF)
        {
            if (ch == '\n')
            {
                ++n_lines;
            }
        }

        files_list = (char **)MALLOC(sizeof(char *) * n_lines);

        freopen(filename, "r", f);
        char *line = NULL;
        size_t len = 0;
        ssize_t nread;

        for (i = 0; i < n_lines; ++i)
        {
            if ((nread = getline(&line, &len, f)) > 0)
            {
                files_list[i] = (char *)MALLOC(sizeof(char) * ((int)nread + 1));
                strcpy(files_list[i], line);
                (*total)++;
                DEBUG("%s", files_list[i]);
            }
        }

        FREE(line);

        if (fclose(f) == -1)
        {
            WARNING("Failed to close batch file.");
        }
    }
    return files_list;
}

int path_check(char *filename)
{
    if (strchr(filename, 47) == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
