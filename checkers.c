#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>

#include "checkers.h"
#include "debug.h"
#include "memory.h"

// number of extensions supported by the app
#define NUM_EXT 7

// array of extensions supported by the app
char *G_extensions[NUM_EXT] = {"pdf", "gif", "jpg", "png", "mp4", "zip", "html"};

int file_check(char *filename)
{
    FILE *fptr = NULL;
    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        fprintf(stderr, "[ERROR] cannot open file '%s' -- %s\n", filename, strerror(errno));
        return 0;
    }

    if (fclose(fptr) == -1)
    {
        WARNING("Closing file fault");
    }

    return 1;
}

int dir_check(char *dirname)
{
    DIR *dptr = NULL;
    dptr = opendir(dirname);

    if (dptr == NULL)
    {
        fprintf(stderr, "[ERROR] cannot open dir '%s' -- %s\n", dirname, strerror(errno));
        return 0;
    }

    if (closedir(dptr) == -1)
    {
        WARNING("Closing dir fault");
    };
    return 1;
}

int type_check(char *str_type, char *filename)
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
        return 0;
    }
    return 1;
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