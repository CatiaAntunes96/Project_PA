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

#define TIME_SIZE 20

int G_filenumber = 0;
char G_time[TIME_SIZE] = "";
char *G_filename = NULL;

// filename for stdout redirect
char *G_output_filename = "out.txt";

int i, mismatch = 0, ok = 0, errors = 0, not_sup = 0;

void f_option(char **args, int n_args)
{
    // Number of files passed by prompt
    int n_files = n_args;

    char **files_list = NULL;
    files_list = list_files(args, n_files);

    char *filename = NULL;

    // command for exec call
    char *comm = "file";

    // options for the command called on exec
    char *opt = "-b";

    for (i = 0; i < n_files; ++i)
    {
        // indiviual filename from filenames array
        filename = files_list[i];

        if (!file_check(filename, &errors))
        {
            continue;
        }

        // string containing the extension portion of the filename
        char *extension = get_ext_from_filename(filename);
        if (extension == NULL)
        {
            continue;
        }

        // performs a system call on the command and options passed by argument and redirects stdout to an output file passed by argument as well
        exec_call(G_output_filename, filename, comm, opt);

        // reads from output file of command called by exec and retrives the extension to string
        char *file_check_out = get_str_from_out_file(G_output_filename);
        char *file_ext = get_ext_from_out_str(file_check_out);

        // checks if the file type is supported by the app, runs the string against an array of predefined extensions
        if (!type_check(file_ext, filename, file_check_out, &not_sup))
        {
            continue;
        }

        // basic comparison between the extension on the filename passed by the user and the file type obtained from the file command
        cmp_ext_type(extension, file_ext, filename, &mismatch, &ok);

        FREE(filename);
    }

    FREE(files_list);
}
void b_option(char *arg)
{
    char *batch_filename = arg;
    file_check(batch_filename, &errors);
    char this_option = 'b';

    char *comm = "file";
    char *opt = "-bE";

    exec_call(G_output_filename, batch_filename, comm, opt);

    char *batch_check_out = get_str_from_out_file(G_output_filename);
    char *batch_ext = get_ext_from_out_str(batch_check_out);

    cmp_ext_type(batch_ext, "ascii", batch_filename, &mismatch, &ok);

    char **files_list = NULL;
    char *filename = NULL;
    int n_files = 0;
    int i;

    files_list = read_lines(batch_filename, &n_files, this_option);

    printf("[INFO] analyzing files listed in '%s'\n", batch_filename);

    for (i = 0; i < n_files; ++i)
    {
        sleep(2);
        DEBUG("PID: %d", getpid());
        filename = files_list[i];
        G_filename = filename;
        G_filenumber = i + 1;

        if (!file_check(filename, &errors))
        {
            continue;
        }
        char *extension = get_ext_from_filename(filename);
        if (extension == NULL)
        {
            continue;
        }

        exec_call(G_output_filename, filename, comm, opt);

        char *file_check_out = get_str_from_out_file(G_output_filename);
        char *file_ext = get_ext_from_out_str(file_check_out);

        if (!type_check(file_ext, filename, file_check_out, &not_sup))
        {
            continue;
        }

        cmp_ext_type(extension, file_ext, filename, &mismatch, &ok);

        FREE(filename);
    }

    FREE(files_list);
    printf("[SUMMARY] files analyzed: %d; files OK: %d; files MISMATCH: %d; files NOT SUPPORTED: %d; errors: %d\n", n_files, ok, mismatch, not_sup, errors);
}

void d_option(char *arg)
{
    char *dirname = arg;
    // char *dir_out = "dirout.txt";
    char *comm = "find";

    if (!dir_check(dirname, &errors))
    {
        exit(1);
    }
    printf("[INFO] analyzing files of directory '%s'\n", dirname);

    // exec_call_dir(dir_out, dirname, comm);
    exec_call_dir(G_output_filename, dirname, comm);

    char **files_list = NULL;
    int n_files = 0;
    char this_option = 'd';

    // files_list = read_lines(dir_out, &n_files, this_option);
    files_list = read_lines(G_output_filename, &n_files, this_option);
    char *filename = NULL;

    comm = "file";
    char *opt = "-bE";

    for (i = 0; i < n_files; ++i)
    {
        DEBUG("n files %d", n_files);
        filename = files_list[i];

        if (!file_check(filename, &errors))
        {
            continue;
        }
        char *extension = get_ext_from_filename(filename);
        if (extension == NULL)
        {
            continue;
        }

        exec_call(G_output_filename, filename, comm, opt);

        char *file_check_out = get_str_from_out_file(G_output_filename);
        char *file_ext = get_ext_from_out_str(file_check_out);

        if (!type_check(file_ext, filename, file_check_out, &not_sup))
        {
            continue;
        }

        cmp_ext_type(extension, file_ext, filename, &mismatch, &ok);

        FREE(filename);
    }
    FREE(files_list);
    printf("[SUMMARY] files analyzed:%d; files OK: %d; files MISMATCH: %d; files NOT SUPPORTED: %d; errors: %d\n", n_files, ok, mismatch, not_sup, errors);
}