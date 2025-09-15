#include "globals.h"
#ifndef ERRORS_H
#define ERRORS_H

void report_lexical_error();
void report_syntax_error(TAtomo atomo, TInfoAtomo infoAtomo);
void report_comment_error(char* incorrect_token);

#endif