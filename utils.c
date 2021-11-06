
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
#include <signal.h>

#include "utils.h"
#include "debug.h"
#include "memory.h"
#include "getters.h"
#include "checkers.h"
#include "sys.h"

// string's size for displaying date and time in the
// requested format
#define TIME_SIZE 20

#define C_ERR_OPEN_BATCH 1
#define C_ERR_EMPTY_BATCH 2
#define C_ERR_BATCH_TYPE 3

int G_filenumber = 0;
char G_time[TIME_SIZE] = "";
char *G_filename = NULL;

// filename to which the stdout is redirected when an exec
// system call is performed
char *G_output_filename = "out.txt";

void f_option(char **args, int n_args)
{
    char this_option = 'f';

    process_file(args, n_args, this_option);
}

void b_option(char *arg)
{
    // this debug is placed here for SIGUSR1 testing purposes only
    DEBUG("PID: %d", getpid());

    char this_option = 'b';
    char *batch_filename = arg;

    FILE *f = NULL;

    // open batch file validation
    if ((f = fopen(batch_filename, "r")) == NULL)
    {
        fprintf(stderr, "[ERROR] cannot open file '%s' -- %s\n",
                batch_filename, strerror(errno));
        exit(C_ERR_OPEN_BATCH);
    }

    // empty batch file validation
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0)
    {
        printf("[ERROR] batch file is empty\n");
        exit(C_ERR_EMPTY_BATCH);
    }

    if (fclose(f) == -1)
    {
        WARNING("Closing file fault");
    }

    // exec system call to obtain the batch file's type
    exec_call(G_output_filename, batch_filename);

    // string of the output file's content written after the exec
    // system call
    char *out_file_info = get_str_from_out_file(G_output_filename);

    // string containing the first characters in a row of the string
    // above, is this case, the delimiter used is Space
    char *out_file_type = get_type_from_out_str(out_file_info);

    // validates if the batch file's type is text
    if (out_file_type == NULL || (strcmp(out_file_type, "ascii") != 0 && strcmp(out_file_type, "utf-8")) != 0)
    {
        printf("[ERROR] invalid batch file type -- batch must be text file\n");
        exit(C_ERR_BATCH_TYPE);
    }

    printf("[INFO] analyzing files listed in '%s'\n", batch_filename);

    int n_files = 0;
    // string array of the batch file's lines
    char **files_list = read_lines(batch_filename, &n_files);

    process_file(files_list, n_files, this_option);
}

void d_option(char *arg)
{
    char this_option = 'd';
    char *dirname = arg;

    // atempts to open dir and assess if it is ok to proceed
    dir_check(dirname);

    printf("[INFO] analyzing files of directory '%s'\n", dirname);

    // exec system call to obtain the directory's file entries
    exec_call_dir(G_output_filename, dirname);

    int n_files = 0;
    // string array of the previously written output file's lines
    char **files_list = read_lines(G_output_filename, &n_files);

    process_file(files_list, n_files, this_option);
}

void process_file(char **files_list, int n_files, char option)
{
    char *filename = NULL;
    int i;
    int ok = 0, mismatch = 0, errors = 0, not_sup = 0;

    // iteration over filenames' array to process each file
    // at a time
    for (i = 0; i < n_files; i++)
    {
        filename = files_list[i];

        if (option == 'b')
        {
            G_filename = filename;
            G_filenumber = i + 1;
        }

        // atempts to open file and assess if it is ok to proceed
        if (!file_check(filename, option, &errors))
        {
            continue;
        }

        // string containing the filename's extension portion
        char *extension = get_ext_from_filename(filename);

        // get_ext_from_filename sets a warning no extension was
        // found in the file's name -- execution is resumed
        if (extension == NULL)
        {
            continue;
        }

        // exec system call to obtain the file's type
        exec_call(G_output_filename, filename);

        // string of the output file's content written after the exec
        // system call
        char *out_file_info = get_str_from_out_file(G_output_filename);

        // string containing the first characters in a row of the string
        // above, is this case, the delimiter used is Space
        char *out_file_type = get_type_from_out_str(out_file_info);

        // checks if the file type is supported by the app by comparing
        // the out_file_type string with an array of globally defined
        // types
        if (!type_check(out_file_type, filename, out_file_info, &not_sup))
        {
            continue;
        }

        // comparison between the two strings retrieved from the filename's
        // extension passed by the user and the file type's extension obtained
        // from the exec system call
        cmp_ext_type(extension, out_file_type, filename, &mismatch, &ok);

        //FREE(filename);
    }

    if (option == 'b' || option == 'd')
    {
        printf("[SUMMARY] files analyzed:%d; files OK: %d; files MISMATCH: %d; files NOT SUPPORTED: %d; errors: %d\n",
               n_files, ok, mismatch, not_sup, errors);
    }
}