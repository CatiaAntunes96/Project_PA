#ifndef GETTERS_H
#define GETTERS_H

// max size for an extension supported by the app
#define SIZE_EXT 5

char **list_files(char **args, int n_args);
char **read_batch(char *filename, int *total);
char *extract(char *filename);
char *get_file_type(char *out_filename);

#endif