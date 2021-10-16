#ifndef CHECKERS_H
#define CHECKERS_H

int file_check(char *filename);
int dir_check(char *dirname);
int type_check(char *str_type, char *filename);
void cmp_ext_type(char *ext, char *type, char *filename);

#endif