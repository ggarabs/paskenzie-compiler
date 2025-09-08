#include "globals.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void report_lexical_error(char* incorrect_token){
        while(*buffer != EOS && *buffer != '\n' && *buffer != ' ' && !is_token_delimiter()) buffer++;
        int incorrect_token_length = buffer-incorrect_token;
        if(incorrect_token_length > 0){
                strncpy(lexema, incorrect_token, buffer-incorrect_token);
                printf("#  %d:erro léxico, token [%s] não reconhecido.\n", nLinha, lexema);
        }
        exit(1);
}

bool is_operator_or_delimiter(char symbol){
        return symbol == ',' || symbol == '.' || 
                symbol == ';' || symbol == ':' || 
                symbol == '(' || symbol == ')' ||
                symbol == '<' || symbol == '>' || 
                symbol == '+' || symbol == '-' ||
                symbol == '*';
}

bool is_token_delimiter(){
        return *buffer == '\n' || *buffer == ' ' || 
               *buffer == ')' || *buffer == '*' || 
               *buffer == '+' || *buffer == '-' || 
               *buffer == '<' || *buffer == '>' || 
               *buffer == '=' || *buffer == ';' || *buffer == ':';
}