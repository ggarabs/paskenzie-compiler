// Gustavo Garabetti - RA: 10409258

#include "globals.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

long get_file_size(FILE* file_pointer, char* source_code_filename){
        fseek(file_pointer, 0, SEEK_END);

        return ftell(file_pointer);
}

bool is_operator_or_delimiter(char symbol){
        return symbol == ',' || symbol == '.' || 
                symbol == ';' || symbol == ':' || 
                symbol == '(' || symbol == ')' ||
                symbol == '<' || symbol == '>' || 
                symbol == '+' || symbol == '-' ||
                symbol == '*' || symbol == '=';
}

bool is_token_delimiter(){
        return *buffer == '\n' || *buffer == ' ' || 
               *buffer == '\t' || *buffer == '\r' ||
               *buffer == ')' || *buffer == '*' || 
               *buffer == '+' || *buffer == '-' || 
               *buffer == '<' || *buffer == '>' || 
               *buffer == '=' || *buffer == ';' || 
               *buffer == ':' || *buffer == '(';
}
