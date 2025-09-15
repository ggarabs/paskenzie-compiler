// Gustavo Garabetti - RA: 10409258

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
        // se há um erro léxico no primeiro caractere do buffer, então eu limpo o contéudo do lexema anterior para capturar o token errado no report
        else lexema[0] = '\0'; 

        return infoAtomo;
}

// CONSTINT <= 2*10^9
void recognizes_constint(TInfoAtomo *info){
        char *ini_lexema = buffer;
        goto q1;

qL:
        strncpy(lexema, ini_lexema, buffer-ini_lexema);
        lexema[buffer-ini_lexema] = '\0';
        return;

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

        goto qL;
        
q3:
        if(isdigit(*buffer)){
                buffer++;
                goto q4;
        }

        goto qL;
q4:
        if(isdigit(*buffer)){
                buffer++;
                goto q4;
        }

        if(!is_token_delimiter()) goto qL;

        strncpy(lexema, ini_lexema, buffer-ini_lexema);
        lexema[buffer-ini_lexema] = '\0';

        int number = 0, index = 0;
        char* mantissa = (char*)calloc(IDENTIFIER_MAX_LENGTH, sizeof(char));
        for(; index < buffer-ini_lexema; index++) if(lexema[index] == 'd') break;

        strncpy(mantissa, lexema, index);
        mantissa[index] = '\0';

        if(lexema[index] == 'd') index++;

        // verificação de limites na mantissa.
        if(strlen(mantissa) > 10) return;
        if(strlen(mantissa) == 10 && mantissa[0] > '2') return;

        number = atoi(mantissa);

        if(lexema[index] == '+') index++;

        char* pot10 = (char*)calloc(IDENTIFIER_MAX_LENGTH, sizeof(char));
        strncpy(pot10, lexema+index, buffer-ini_lexema-index);
        pot10[index] = '\0';

        int partial_result = atoi(pot10);

        // verificação de limites do número.
        if((strlen(mantissa) + partial_result) > 10) return;
        if((strlen(mantissa) + partial_result == 10) && mantissa[0] > '2') return;

        number *= pow(10, partial_result);

        info->atomo = CONSTINT;
        info->atributo.numero = number;

        return;
}

void recognizes_keyword_or_identifier(TInfoAtomo *info){
        char *ini_lexema = buffer;
q1:
        if(isdigit(*buffer) || isalpha(*buffer) ||'_' == *buffer){
                buffer++;
                goto q1;
        }

        if(buffer-ini_lexema > IDENTIFIER_MAX_LENGTH){
                strncpy(lexema, ini_lexema, buffer-ini_lexema);
                lexema[buffer-ini_lexema] = '\0';
                return;
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

        if(0 == strcmp(info->atributo.id, "(*")){
                info->atomo = OPEN_COMMENT;
                return;
        }else if(0 == strcmp(info->atributo.id, "*)")){
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

void recognizes_constchar(TInfoAtomo *info){
        char* ini_lexema = buffer;
        
        buffer += 2;

        if(*buffer != '\''){
                strncpy(lexema, ini_lexema, buffer-ini_lexema);
                lexema[buffer-ini_lexema] = '\0';
                return;
        }

        ini_lexema++;
        buffer++;

        info->atributo.ch = *ini_lexema;
        info->atomo = CONSTCHAR;

        return;
}