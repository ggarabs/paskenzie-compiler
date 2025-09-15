#include <stdio.h>
#include "globals.h"
#include "errors.h"
#ifndef PARSER_H
#define PARSER_H

void consome(TAtomo atomo);
void consome_comment();

void program();
void block();
void variable_declaration_part();
void variable_declaration();
void type();
void statement_part();
void statement();
void assignment_statement();
void read_statement();
void write_statement();
void if_statement();
void while_statement();
void expression();
void relational_operator();
void simple_expression();
void adding_operator();
void term();
void multiplying_operator();
void factor();

#endif