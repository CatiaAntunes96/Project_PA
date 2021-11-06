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
#include <ctype.h>

#include "getters.h"
#include "debug.h"
#include "memory.h"

// used for options -b and -d
// reads from a given file, copies the content into a string
// array and returns the number of files read by reference
char **read_lines(char *filename, int *total)
{
    char **files_list = NULL;
    char ch;
    int n_lines = 0;
    int i;
    *total = 0; // number of filenames read

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
                n_lines++;
            }
        }

        files_list = (char **)MALLOC(sizeof(char *) * n_lines);

        freopen(filename, "r", f);
        char *line = NULL;
        size_t len = 0;
        ssize_t nread;
        ssize_t j;

        for (i = 0; i < n_lines; i++)
        {
            if ((nread = getline(&line, &len, f)) > 0)
            {
                files_list[i] = (char *)MALLOC(sizeof(char) * ((int)nread));

                // this portion of code is copied from man page STRCPY(3)
                // due to strncpy not working properly for the intended
                // purpose, in this case is to replace the line break with
                // the string terminator
                for (j = 0; j < nread && line[j] != '\n'; j++)
                {
                    {
                        files_list[i][j] = line[j];
                    }
                }
                for (; j < nread; j++)
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

    return files_list;
}

// looks for a '.' char in a filename and returns a string containing
// the characters after the desginated symbol
char *get_ext_from_filename(char *filename)
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

        for (i = 0; i < n_bytes; i++)
        {
            str_ext[i] = chr[i + 1];
        }
        chr = NULL;
    }
    return str_ext;
}

// reads from the file to which the exec's stdout is redirected, copies
// the line read into a string and returns it
char *get_str_from_out_file(char *out_filename)
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
        for (j = 0; j < nread && line[j] != '\n'; j++)
        {
            str_type[j] = line[j];
        }
        for (; j < nread; j++)
        {
            str_type[j] = '\0';
        }
    }
    FREE(line);

    if (fclose(f) == -1)
    {
        WARNING("Failed to close '%s'", out_filename);
    }

    return str_type;
}

// looks for 'Space' in the string passed by parameter and returns
// a string containing the characters found before that delimiter
char *get_type_from_out_str(char *str)
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

        for (i = 0; i < n_bytes + 1; i++)
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