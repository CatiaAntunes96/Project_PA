/**
 * @file main.c
 * @brief CLI checkFile
 * @date 2021-11-06
 * @author Cátia Antunes & Inês Machado
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
#include <dirent.h>

#include "args.h"
#include "debug.h"
#include "memory.h"
#include "getters.h"
#include "checkers.h"
#include "sys.h"

// filename for stdout redirect
char *G_output_filename = "./out.txt";

int main(int argc, char *argv[])
{
    struct gengetopt_args_info args_info;

    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        exit(1);
    }

    if (args_info.file_given > 0)
    {

        // Number of files passed by prompt
        int n_files = args_info.file_given;
        int i;

        char **files_list = NULL;
        files_list = list_files(args_info.file_arg, n_files);

        char *filename = NULL;

        // command for exec call
        char *comm = "file";

        // options for the command called on exec
        char *opt = "-b";

        for (i = 0; i < n_files; ++i)
        {
            // indiviual filename from filenames array
            filename = files_list[i];

            if (!file_check(filename))
            {
                continue;
            }

            // string containing the extension portion of the filename
            char *extension = extract(filename);
            if (extension == NULL)
            {
                continue;
            }

            // performs a system call on the command and options passed by argument and redirects stdout to an output file passed by argument as well
            exec_call(G_output_filename, filename, comm, opt);

            // reads from output file of command called by exec and retrives the extension to string
            char *file_check_out = get_file_out(G_output_filename);
            char *file_ext = get_extension(file_check_out);

            // checks if the file type is supported by the app, runs the string against an array of predefined extensions
            if (!type_check(file_ext, filename))
            {
                continue;
            }

            // basic comparison between the extension on the filename passed by the user and the file type obtained from the file command
            cmp_ext_type(extension, file_ext, filename);

            FREE(filename);
        }

        FREE(files_list);
    }
    else if (args_info.batch_given > 0)
    {
        char *batch_filename = args_info.batch_arg;
        file_check(batch_filename);

        char *btype = "./btype.txt";
        char *comm = "file";
        char *opt = "-bE";

        exec_call(btype, batch_filename, comm, opt);

        char *batch_check_out = get_file_out(G_output_filename);
        char *batch_ext = get_extension(batch_check_out);

        cmp_ext_type(batch_ext, "ascii", batch_filename);

        char **files_list = NULL;
        char *filename = NULL;
        int n_files = 0;
        char this_option = 'b';
        int i;

        files_list = read_lines(batch_filename, &n_files, this_option);

        DEBUG("Valor por ref: %d", n_files);
        printf("[INFO] analyzing files listed in '%s'\n", batch_filename);

        for (i = 0; i < n_files; ++i)
        {
            filename = files_list[i];

            if (!file_check(filename))
            {
                continue;
            }
            char *extension = extract(filename);
            DEBUG("extension recebida pela função: %s", extension);
            if (extension == NULL)
            {
                continue;
            }

            exec_call(G_output_filename, filename, comm, opt);

            char *file_check_out = get_file_out(G_output_filename);
            char *file_ext = get_extension(file_check_out);
            DEBUG("FILE EXT %s", file_ext);

            if (!type_check(file_ext, filename))
            {
                continue;
            }

            cmp_ext_type(extension, file_ext, filename);

            FREE(filename);
        }

        FREE(files_list);
        printf("[SUMMARY] files analyzed:%d; files OK: ; files MISMATCH: ; errors: \n", n_files);
    }
    else
    {
        char *dirname = args_info.directory_arg;

        char this_option = 'd';

        char *dir_out = "./dirout.txt";
        char *comm = "find";
        char *opt = "";

        if (!dir_check(dirname))
        {
            exit(1);
        }
        printf("[INFO] analyzing files of directory '%s'\n", dirname);
        DEBUG("antes do call");

        exec_call(dir_out, dirname, comm, opt);

        DEBUG("depois do call");

        char **files_list = NULL;
        int n_files = 0;
        int i;

        files_list = read_lines(dir_out, &n_files, this_option);
        char *filename = NULL;

        comm = "file";
        opt = "-bE";

        for (i = 0; i < n_files; ++i)
        {
            filename = files_list[i];
            DEBUG("file list %s", files_list[i]);

            if (!file_check(filename))
            {
                DEBUG("%d", file_check(filename));
                continue;
            }
            char *extension = extract(filename);
            if (extension == NULL)
            {
                continue;
            }

            exec_call(G_output_filename, filename, comm, opt);

            char *file_check_out = get_file_out(G_output_filename);
            char *file_ext = get_extension(file_check_out);

            if (!type_check(file_ext, filename))
            {
                continue;
            }

            cmp_ext_type(extension, file_ext, filename);

            FREE(filename);
        }
        FREE(files_list);
        printf("[SUMMARY] files analyzed:%d; files OK: ; files MISMATCH: ; errors: \n", n_files);
    }

    cmdline_parser_free(&args_info);

    return 0;
}

// to be determined
// int path_check(char *filename)
// {
//     if (strchr(filename, 47) == NULL)
//     {
//         return 1;
//     }
//     else
//     {
//         return 0;
//     }
// }
