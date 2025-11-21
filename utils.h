#include <stdio.h>
#include <stdbool.h>
#ifndef UTILS_H
#define UTILS_H

long get_file_size(FILE* file_pointer, char* source_code_filename);

bool is_operator_or_delimiter(char symbol);
bool is_token_delimiter();
int proximo_rotulo();

#endif