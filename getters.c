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
        DEBUG("Original: %d", (int)strlen(args[i]));
        strcpy(files_array[i], args[i]);
        DEBUG("Cópia: %d", (int)strlen(files_array[i]));
    }

    return files_array;
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
                files_list[i] = (char *)MALLOC(sizeof(char) * ((int)nread));
                strncpy(files_list[i], line, nread - 1);

                (*total)++;
            }
        }

        FREE(line);

        if (fclose(f) == -1)
        {
            WARNING("Failed to close batch file");
        }
    }
    return files_list;
}

char *extract(char *filename)
{
    char *chr = strrchr(filename, 46);
    DEBUG("%s", chr);
    int i;
    char *str_ext = NULL;

    if (chr == NULL)
    {
        WARNING("[WARNING] '%s' filename does not have an extension", filename);
    }
    else
    {
        int n_bytes = (int)strlen(chr);
        str_ext = MALLOC(n_bytes);

        for (i = 0; i < n_bytes; ++i)
        {
            str_ext[i] = chr[i + 1];
            DEBUG("cópia: %c | original:%c | i: %d", str_ext[i], chr[i + 1], i);
        }
        chr = NULL;
    }
    DEBUG("string retorno: %s", str_ext);
    return str_ext;
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

        str_type[i] = tolower(buffer);
        DEBUG("cópia %c | lido %c", str_type[i], buffer);
    }
    DEBUG("Após cópia: %s", str_type);

    if (close(fd) == -1)
    {
        ERROR(1, "Failed to close output file");
    }

    DEBUG("str retorno: %s", str_type);

    return str_type;
}