#include <stdio.h>
#include <stdbool.h>
#ifndef UTILS_H
#define UTILS_H

void report_lexical_error(char* buffer);
bool is_operator_or_delimiter(char symbol);
bool is_token_delimiter();

#endif