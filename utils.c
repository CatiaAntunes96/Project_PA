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
// making an exec system call
char *G_output_filename = "out.txt";

int i;
int n_files = 0;
int mismatch = 0, ok = 0, errors = 0, not_sup = 0;

void f_option(char **args, int n_args)
{
    char this_option = 'f';

    // number of files passed by prompt
    int n_files = n_args;

    char **files_list = NULL;

    // array of filenames
    files_list = list_files(args, n_files);

    char *filename = NULL;

    // command for exec call
    char *comm = "file";

    // options for the command passed to exec
    char *opt = "-bE";

    // iteration over filenames' array to process each
    // file at a time
    for (i = 0; i < n_files; ++i)
    {
        filename = files_list[i];

        // validate if file exists
        file_check(filename, this_option, &errors);

        // pointer to the first character of the filename's
        // extension portion
        char *extension = get_ext_from_filename(filename);
        if (extension == NULL)
        {
            continue;
        }

        // performs an exec system call on the command and options
        // passed by parameter and redirects stdout to an output file
        // globally defined
        exec_call(G_output_filename, filename, comm, opt);

        // reads the exec system call's output from a txt file and returns
        // a string of that data
        char *out_file_info = get_str_from_out_file(G_output_filename);

        // recives the data string and returns a string of the first
        // characters lowered for comparison
        char *out_file_ext = get_ext_from_out_str(out_file_info);

        // checks if the file type is supported by the app by comparing
        // the string with an array of globally defined extensions
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

    FREE(files_list);
}

void b_option(char *arg)
{
    char this_option = 'b';

    char *batch_filename = arg;

    if (!file_check(batch_filename, this_option, &errors))
    {
        exit(C_ERR_OPEN_BATCH);
    }

    char *batch_ext = get_ext_from_filename(batch_filename);

    char *comm = "file";
    char *opt = "-bE";

    exec_call(G_output_filename, batch_filename, comm, opt);

    char *out_file_info = get_str_from_out_file(G_output_filename);
    char *out_file_ext = get_ext_from_out_str(out_file_info);

    if (!cmp_ext_type(batch_ext, out_file_ext, batch_filename, &mismatch, &ok))
    {
        ERROR(C_ERR_BATCH_TYPE, "Invalid batch file type: batch must be text file");
    }

    printf("[INFO] analyzing files listed in '%s'\n", batch_filename);

    char **files_list = NULL;
    char *filename = NULL;

    files_list = read_lines(batch_filename, &n_files, this_option);

    for (i = 0; i < n_files; ++i)
    {
        // this debug is placed here for SIGUSR1 testing purposes only
        DEBUG("PID: %d", getpid());

        filename = files_list[i];
        G_filename = filename;
        G_filenumber = i + 1;

        if (!file_check(filename, this_option, &errors))
        {
            continue;
        }

        char *extension = get_ext_from_filename(filename);

        if (extension == NULL)
        {
            continue;
        }

        exec_call(G_output_filename, filename, comm, opt);

        char *out_file_info = get_str_from_out_file(G_output_filename);
        char *out_file_ext = get_ext_from_out_str(out_file_info);

        if (!type_check(out_file_ext, filename, out_file_info, &not_sup))
        {
            continue;
        }

        cmp_ext_type(extension, out_file_ext, filename, &mismatch, &ok);

        FREE(filename);
    }

    FREE(files_list);

    printf("[SUMMARY] files analyzed: %d; files OK: %d; files MISMATCH: %d; files NOT SUPPORTED: %d; errors: %d\n", n_files, ok, mismatch, not_sup, errors);
}

void d_option(char *arg)
{
    char this_option = 'd';

    char *dirname = arg;

    dir_check(dirname);

    printf("[INFO] analyzing files of directory '%s'\n", dirname);

    char *comm = "find";

    exec_call_dir(G_output_filename, dirname, comm);

    char **files_list = NULL;
    files_list = read_lines(G_output_filename, &n_files, this_option);

    DEBUG("%d", n_files);
    for (i = 0; i < n_files; ++i)
    {
        DEBUG("%s", files_list[i]);
    }

    char *filename = NULL;

    comm = "file";
    char *opt = "-bE";

    for (i = 0; i < n_files; ++i)
    {
        filename = files_list[i];

        if (!file_check(filename, this_option, &errors))
        {
            continue;
        }

        char *extension = get_ext_from_filename(filename);
        if (extension == NULL)
        {
            continue;
        }

        exec_call(G_output_filename, filename, comm, opt);

        char *out_file_info = get_str_from_out_file(G_output_filename);
        char *out_file_ext = get_ext_from_out_str(out_file_info);

        DEBUG("aqui");
        if (!type_check(out_file_ext, filename, out_file_info, &not_sup))
        {
            continue;
        }
        DEBUG("ali");
        cmp_ext_type(extension, out_file_ext, filename, &mismatch, &ok);
        DEBUG("chegou");
        FREE(filename);
    }

    FREE(files_list);

    printf("[SUMMARY] files analyzed:%d; files OK: %d; files MISMATCH: %d; files NOT SUPPORTED: %d; errors: %d\n", n_files, ok, mismatch, not_sup, errors);
}