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
#include <dirent.h>

#include "checkers.h"
#include "debug.h"
#include "memory.h"

// number of file types supported by the app
#define NUM_TYPES 7
#define C_ERR_OPEN_FILE 1
#define C_ERR_EMPTY_FILE 2
#define C_ERR_OPEN_DIR 3
#define C_ERR_EMPTY_DIR 4

// array of file types supported by the app
char *G_types[NUM_TYPES] = {"pdf",
                            "gif",
                            "jpg",
                            "png",
                            "mp4",
                            "zip",
                            "html"};

int file_check(char *filename, char option, int *n_errors)
{
    FILE *fptr = NULL;
    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        fprintf(stderr, "[ERROR] cannot open file '%s' -- %s\n",
                filename, strerror(errno));

        // error treatment for file option -- halt the execution
        if (option == 'f')
        {
            exit(C_ERR_OPEN_FILE);
        }

        (*n_errors)++;
        return 0;
    }

    // empty file validation
    fseek(fptr, 0, SEEK_END);
    if (ftell(fptr) == 0)
    {
        printf("[ERROR] '%s' file is empty\n", filename);
        // error treatment for file option -- halt the execution
        if (option == 'f')
        {
            exit(C_ERR_EMPTY_FILE);
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

    // open dir validation
    if (dptr == NULL)
    {
        fprintf(stderr, "[ERROR] cannot open dir '%s' -- %s\n",
                dirname, strerror(errno));
        exit(C_ERR_OPEN_DIR);
    }

    int i = 0;

    while (readdir(dptr) != NULL && i < 3)
    {
        i++;
    }

    // empty dir validation
    // if the third consecutive readdir's result is NULL it means
    // dir is empty
    if (i <= 2)
    {
        printf("[ERROR] '%s' directory is empty\n", dirname);
        exit(C_ERR_EMPTY_DIR);
    }

    if (closedir(dptr) == -1)
    {
        WARNING("Closing dir fault");
    }
}

// validates if the file's type matches the types supported array
int type_check(char *str_type, char *filename, char *filetype, int *n_not_supported)
{

    int comp;
    int j = 0;

    do
    {
        comp = strcmp(str_type, G_types[j]);

        j++;
    } while (comp != 0 && j < NUM_TYPES);

    if (comp != 0)
    {
        printf("[INFO] '%s': type '%s' is not supported by checkFile\n",
               filename, filetype);
        (*n_not_supported)++;
        return 0;
    }

    return 1;
}

// simple string comparison between the filename's extension and the file's type
// execution messages set accordingly
void cmp_ext_type(char *ext, char *type, char *filename, int *n_mism, int *n_ok)
{
    if (strcmp(ext, type) == 0)
    {
        printf("[OK] '%s': extension '%s' matches file type '%s'\n",
               filename, ext, type);
        (*n_ok)++;
    }
    else
    {
        printf("[MISMATCH] '%s': extension is '%s', file type is '%s'\n",
               filename, ext, type);
        (*n_mism)++;
    }
}

// used in -h option
// lists the supported file types
void show_types()
{
    for (int i = 0; i < NUM_TYPES; i++)
    {
        printf(". %s\n", G_types[i]);
    }
}