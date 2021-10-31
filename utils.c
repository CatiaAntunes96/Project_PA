#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "sys.h"
#include "debug.h"
#include "memory.h"
#include "getters.h"
#include "checkers.h"
#include "sys.h"
#include "utils.h"

// string's size for displaying date and time in the
// requested format
#define TIME_SIZE 20

#define C_ERR_OPEN_BATCH 1
#define C_ERR_BATCH_TYPE 2

int G_filenumber = 0;
char G_time[TIME_SIZE] = "";
char *G_filename = NULL;

// filename to which the stdout is redirected when
// an exec system call is performed
char *G_output_filename = "out.txt";

void f_option(char **args, int n_args)
{
    char this_option = 'f';

    // number of files passed by prompt
    int n_files = n_args;

    // array of filenames
    char **files_list = list_files(args, n_files);

    process_file(files_list, n_files, this_option);
}

void b_option(char *arg)
{
    // this debug is placed here for SIGUSR1 testing purposes only
    DEBUG("PID: %d", getpid());

    char this_option = 'b';

    char *batch_filename = arg;

    if ((fopen(batch_filename, "r") == NULL))
    {
        fprintf(stderr, "[ERROR] cannot open file '%s' -- %s\n", batch_filename, strerror(errno));
        exit(C_ERR_OPEN_BATCH);
    }

    exec_call(G_output_filename, batch_filename);

    char *out_file_info = get_str_from_out_file(G_output_filename);
    char *out_file_ext = get_ext_from_out_str(out_file_info);

    if (strcmp(out_file_ext, "ascii") != 0)
    {
        printf("[ERROR] invalid batch file type -- batch must be text file\n");
        exit(C_ERR_BATCH_TYPE);
    }

    printf("[INFO] analyzing files listed in '%s'\n", batch_filename);

    int n_files = 0;
    char **files_list = read_lines(batch_filename, &n_files);

    process_file(files_list, n_files, this_option);
}

void d_option(char *arg)
{
    char this_option = 'd';
    char *dirname = arg;
    int n_files = 0;

    // atempts to open dir and assess if it is ok to proceed
    dir_check(dirname);

    printf("[INFO] analyzing files of directory '%s'\n", dirname);

    // exec system call to obtain the directory's file entries
    exec_call_dir(G_output_filename, dirname);

    // string array of the previously written output file's lines
    char **files_list = read_lines(G_output_filename, &n_files);

    process_file(files_list, n_files, this_option);
}

void process_file(char **files_list, int n_files, char option)
{
    char *filename = NULL;
    int i;
    int mismatch = 0, ok = 0, errors = 0, not_sup = 0;

    // iteration over filenames' array to process each file
    // at a time
    for (i = 0; i < n_files; ++i)
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
        char *out_file_ext = get_ext_from_out_str(out_file_info);

        // checks if the file type is supported by the app by comparing
        // the out_file_ext string with an array of globally defined
        // extensions
        if (!type_check(out_file_ext, filename, out_file_info, &not_sup))
        {
            continue;
        }

        // comparison between the two extensions retrieved from the filename
        // passed by the user and the file type obtained from the exec
        // system call
        cmp_ext_type(extension, out_file_ext, filename, &mismatch, &ok);

        FREE(filename);
    }

    if (option == 'b' || option == 'd')
    {
        printf("[SUMMARY] files analyzed:%d; files OK: %d; files MISMATCH: %d; files NOT SUPPORTED: %d; errors: %d\n", n_files, ok, mismatch, not_sup, errors);
    }
}