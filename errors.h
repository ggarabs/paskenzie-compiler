#include "globals.h"
#ifndef ERRORS_H
#define ERRORS_H

typedef enum{
        NOT_UNIQUE, NOT_DECLARED
}TSemanticErrorType;

void report_lexical_error();
void report_syntax_error(TAtomo atomo, TInfoAtomo infoAtomo);
void report_comment_error(char* incorrect_token);
void report_semantic_error(char* incorrect_token, TSemanticErrorType type);

#endif