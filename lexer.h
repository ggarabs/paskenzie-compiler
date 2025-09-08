#include <stdio.h>
#include "globals.h"
#ifndef LEXER_H
#define LEXER_H

TInfoAtomo obter_atomo();
void recognizes_constint(TInfoAtomo *info);
void recognizes_keyword_or_identifier(TInfoAtomo *info);
void recognizes_operator_or_delimiter(TInfoAtomo *info);
void recognizes_constchar(TInfoAtomo *info);

#endif