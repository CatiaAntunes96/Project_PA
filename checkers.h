#ifndef CHECKERS_H
#define CHECKERS_H

int file_check(char *filename, int *n_errors);
int dir_check(char *dirname, int *n_errors);
int type_check(char *str_type, char *filename, char *filetype, int *n_not_supported);
void cmp_ext_type(char *ext, char *type, char *filename, int *n_mism, int *n_ok);
void show_extensions();

#endif