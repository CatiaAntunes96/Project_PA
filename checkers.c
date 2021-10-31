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
#define NUM_EXT 11
#define C_ERR_OPEN_FILE 1
#define C_ERR_OPEN_DIR 2

// array of extensions supported by the app
char *G_extensions[NUM_EXT] = {"pdf", "gif", "jpg", "jpeg", "png", "mp4", "zip", "7z", "gz", "tgz", "html"};

int file_check(char *filename, char option, int *n_errors)
{
    FILE *fptr = NULL;
    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        fprintf(stderr, "[ERROR] cannot open file '%s' -- %s\n", filename, strerror(errno));
        if (option == 'f')
        {
            exit(C_ERR_OPEN_FILE);
        }
        (*n_errors)++;
        return 0;
    }

    if (fclose(fptr) == -1)
    {
        WARNING("Closing file fault");
    }

    return 1;
}

void dir_check(char *dirname)
{
    DIR *dptr = NULL;

    dptr = opendir(dirname);

    if (dptr == NULL)
    {
        fprintf(stderr, "[ERROR] cannot open dir '%s' -- %s\n", dirname, strerror(errno));
        exit(C_ERR_OPEN_DIR);
    }

    if (closedir(dptr) == -1)
    {
        WARNING("Closing dir fault");
    }
}

int type_check(char *str_type, char *filename, char *filetype, int *n_not_supported)
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
        printf("[INFO] '%s': type '%s' is not supported by checkFile\n", filename, filetype);
        (*n_not_supported)++;
        return 0;
    }
    return 1;
}

int cmp_ext_type(char *ext, char *type, char *filename, int *n_mism, int *n_ok)
{
    // this condition's purpose is to acess if the batch file recieved from the prompt
    // is a text file
    if (strcmp("ascii", type) == 0)
    {
        return 1;
    }
    if (strcmp(ext, type) != 0)
    {
        printf("[MISMATCH] '%s': extension is '%s', file type is '%s'\n", filename, ext, type);
        (*n_mism)++;
        return 0;
    }
    printf("[OK] '%s': extension '%s' matches file type '%s'\n", filename, ext, type);
    (*n_ok)++;
    return 1;
}

void show_extensions()
{
    for (int i = 0; i < NUM_EXT - 2; i++)
    {
        printf("%s, ", G_extensions[i]);
    }
    printf("%s\n", G_extensions[NUM_EXT - 1]);
}