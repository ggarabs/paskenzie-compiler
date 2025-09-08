#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lexer.h"
#include "globals.h"
#include "utils.h"

TInfoAtomo obter_atomo(){
        TInfoAtomo infoAtomo;

        infoAtomo.atomo = ERROR;

        while(*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r'){
                if(*buffer == '\n') nLinha++;
                buffer++;
        }

        if(isdigit(*buffer)) recognizes_constint(&infoAtomo);
        else if(isalpha(*buffer) || *buffer == '_') recognizes_keyword_or_identifier(&infoAtomo);
        else if(is_operator_or_delimiter(*buffer)) recognizes_operator_or_delimiter(&infoAtomo);
        else if(*buffer == '\'') recognizes_constchar(&infoAtomo);
        else if(*buffer == '\0') infoAtomo.atomo = EOS;
        else report_lexical_error(buffer); // mexer aqui

        return infoAtomo;
}

void recognizes_constint(TInfoAtomo *info){
        char *ini_lexema = buffer;
q1:
        if(isdigit(*buffer)){
                buffer++;
                goto q1;
        }else if('d' == *buffer){
                buffer++;
                goto q2;
        }

        goto q4;
q2:
        if(isdigit(*buffer)){
                buffer++;
                goto q4;
        }else if('+' == *buffer){
                buffer++;
                goto q3;
        }
        report_lexical_error(ini_lexema); // return talvez
q3:
        if(isdigit(*buffer)){
                buffer++;
                goto q4;
        }
        report_lexical_error(ini_lexema); // return talvez
q4:
        if(isdigit(*buffer)){
                buffer++;
                goto q4;
        }

        if(!is_token_delimiter()){
                info->atomo = ERROR;
                report_lexical_error(ini_lexema); // return talvez
        }

        strncpy(lexema, ini_lexema, buffer-ini_lexema);
        lexema[buffer-ini_lexema] = '\0';

        // usar atoi?

        int number = 0, partial_result = 0, index = 0;
        for(; index < buffer-ini_lexema; index++){
                if(lexema[index] == 'd'){
                        index++;
                        break;
                }
                partial_result *= 10;
                partial_result += lexema[index]-'0';
        }

        number = partial_result;
        partial_result = 0;

        if(lexema[index] == '+') index++;

        for(; index < buffer-ini_lexema; index++){
                partial_result *= 10;
                partial_result += lexema[index]-'0';
        }

        number *= pow(10, partial_result);

        info->atomo = CONSTINT;
        info->atributo.numero = number;

        return;
}

// verificar regras de validação do identificador
void recognizes_keyword_or_identifier(TInfoAtomo *info){
        char *ini_lexema = buffer;
q1:
        if(isdigit(*buffer) || isalpha(*buffer) ||'_' == *buffer){
                buffer++;
                goto q1;
        }

        if(buffer-ini_lexema > IDENTIFIER_MAX_LENGTH){
                printf("Erro léxico! Identificador com mais de 15 letras");
                exit(1);
        }

        strncpy(info->atributo.id, ini_lexema, buffer-ini_lexema);
        info->atributo.id[buffer-ini_lexema] = '\0';

        const char* keywords[] = {"program", "var", "char", "integer", "boolean", "begin", "end", "read", "write", 
                                 "if", "then", "else", "while", "do", "or", "and", "div", "not", "true", "false"};

        const TAtomo atoms[] = {PROGRAM, VAR, CHAR, INTEGER, BOOLEAN, BEGIN, END, READ, WRITE, IF, THEN, ELSE, 
                                WHILE, DO, OR, AND, DIV, NOT, TRUE, FALSE};

        int keywords_length = sizeof(keywords)/sizeof(keywords[0]);
        bool is_keyword = false;

        for(int index = 0; index < keywords_length; index++){
                if(0 == strcmp(info->atributo.id, keywords[index])){
                        info->atomo = atoms[index];
                        is_keyword = true;
                        break;
                }
        }

        if(!is_keyword) info->atomo = IDENTIFIER;

        return;
}

void recognizes_operator_or_delimiter(TInfoAtomo *info){
        char* ini_lexema = buffer;

        if(*buffer == ':'){
                buffer++;
                goto q1;
        }else if(*buffer == '<'){
                buffer++;
                goto q2;
        }else if(*buffer == '>'){
                buffer++;
                goto q3;
        }else if(*buffer == '('){
                buffer++;
                goto q4;
        }else if(*buffer == '*'){
                buffer++;
                goto q5;
        }else if(*buffer == '+' || *buffer == '-' || *buffer == ')' ||
                 *buffer == ';' || *buffer == ',' || *buffer == '.'){
                buffer++;
                goto q6;
        }
        return;
q1:
        if(*buffer == '=') buffer++;
        goto q6;
q2:
        if(*buffer == '=' || *buffer == '>') buffer++;
        goto q6;
q3:
        if(*buffer == '=') buffer++;
        goto q6;
q4:
        if(*buffer == '*') buffer++;
        goto q6;
q5:
        if(*buffer == ')') buffer++;
        goto q6;
q6:

        strncpy(info->atributo.id, ini_lexema, buffer-ini_lexema);
        info->atributo.id[buffer-ini_lexema] = '\0';

        if(strcmp(info->atributo.id, "(*") == 0){
                info->atomo = OPEN_COMMENT;
                return;
        }else if(strcmp(info->atributo.id, "*)") == 0){
                info->atomo = CLOSE_COMMENT;
                return;
        }

        const char* operator[] = {":=", "<>", "<", "<=", ">", ">=", "+", "-", "*"};
        const TAtomo operators_atoms[] = {ASSIGNMENT, DIFFERENT, LESS_THAN, LESS_OR_EQUAL_THAN, GREATER_THAN,
                                          GREATER_OR_EQUAL_THAN, PLUS, MINUS, MULTIPLY};

        const int operator_length = sizeof(operator)/sizeof(operator[0]);
        bool is_operator = false;

        for(int index = 0; index < operator_length; index++){
                if(strcmp(info->atributo.id, operator[index]) == 0){
                        info->atomo = operators_atoms[index];
                        is_operator = true;
                        break;
                }
        }

        if(is_operator) return;

        const char* delimiters[] = {";", ".", ":", ",", "(", ")"};
        const TAtomo delimiters_atoms[] = {SEMICOLON, DOT, COLON, COMMA, OPEN_BRACKETS, CLOSE_BRACKETS};

        int delimiters_length = sizeof(delimiters);

        for(int index = 0; index < delimiters_length; index++){
                if(strcmp(info->atributo.id, delimiters[index]) == 0){
                        info->atomo = delimiters_atoms[index];
                        break;
                }
        }

        return;
}

// modificar
void recognizes_constchar(TInfoAtomo *info){
        char* ini_lexema = buffer;
        
        buffer += 2;
        ini_lexema++;

        if(*buffer != '\'') return;

        buffer++;

        info->atributo.ch = *ini_lexema;
        info->atomo = CONSTCHAR;

        return;
}