#ifndef GETTERS_H
#define GETTERS_H

char **read_lines(char *filename, int *total);
char *get_ext_from_filename(char *filename);
char *get_str_from_out_file(char *out_filename);
char *get_type_from_out_str(char *str);

#endif