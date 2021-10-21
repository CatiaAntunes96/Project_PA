#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#include "getters.h"
#include "debug.h"
#include "memory.h"

char **list_files(char **args, int n_args)
{
    int i;

    char **files_array = NULL;
    files_array = (char **)MALLOC(sizeof(char *) * n_args);

    for (i = 0; i < n_args; ++i)
    {
        files_array[i] = (char *)MALLOC(sizeof(char) * ((int)strlen(args[i]) + 1));
        strcpy(files_array[i], args[i]);
    }

    return files_array;
}

char **read_lines(char *filename, int *total, char option)
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
        ssize_t j;

        for (i = 0; i < n_lines; ++i)
        {
            if ((nread = getline(&line, &len, f)) > 0)
            {
                files_list[i] = (char *)MALLOC(sizeof(char) * ((int)nread));
                for (j = 0; j < nread && line[j] != '\n'; ++j)
                {
                    {
                        files_list[i][j] = line[j];
                    }
                }
                for (; j < nread; ++j)
                {
                    files_list[i][j] = '\0';
                }
                (*total)++;
            }
        }

        FREE(line);

        if (fclose(f) == -1)
        {
            WARNING("Failed to close file");
        }
    }

    if (option == 'b')
    {
        return files_list;
    }
    else
    {
        for (i = 0; i < n_lines; ++i)
        {
            if (i + 1 >= n_lines)
            {
                files_list[i] = NULL;
            }
            files_list[i] = files_list[i + 1];
        }
        (*total)--;
        return files_list;
    }
}

char *extract(char *filename)
{
    char *chr = strrchr(filename, 46);
    int i;
    char *str_ext = NULL;

    if (chr == NULL)
    {
        WARNING("'%s' filename does not have an extension", filename);
    }
    else
    {
        int n_bytes = (int)strlen(chr);
        str_ext = MALLOC(n_bytes);

        for (i = 0; i < n_bytes; ++i)
        {
            str_ext[i] = chr[i + 1];
        }
        chr = NULL;
    }
    return str_ext;
}

char *get_file_out(char *out_filename)
{
    char *str_type = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    ssize_t j;

    FILE *f = fopen(out_filename, "r");

    if (f == NULL)
    {
        WARNING("Could not open file '%s' for reading", out_filename);
    }

    if ((nread = getline(&line, &len, f)) > 0)
    {
        str_type = (char *)MALLOC(sizeof(char) * ((int)nread));
        for (j = 0; j < nread && line[j] != '\n'; ++j)
        {
            str_type[j] = line[j];
        }
        for (; j < nread; ++j)
        {
            str_type[j] = '\0';
        }
    }
    FREE(line);

    if (fclose(f) == -1)
    {
        WARNING("Failed to close batch file");
    }

    return str_type;
}

char *get_extension(char *str)
{
    char *chr = strchr(str, 32);

    int i;
    char *file_ext = NULL;

    if (chr == NULL)
    {
        return NULL;
    }
    else
    {
        int n_bytes = ((int)strlen(str) - (int)strlen(chr));
        file_ext = MALLOC(n_bytes + 1);

        for (i = 0; i < n_bytes + 1; ++i)
        {
            file_ext[i] = tolower(str[i]);
            if (str[i] == 32)
            {
                file_ext[i] = '\0';
            }
        }
        chr = NULL;
    }
    return file_ext;
}