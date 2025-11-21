#ifndef GLOBALS_H
#define GLOBALS_H

typedef enum{
        PROGRAM, IDENTIFIER, SEMICOLON, DOT, VAR, COMMA, COLON, CHAR, INTEGER, BOOLEAN, BEGIN, END, ASSIGNMENT, READ,
        OPEN_BRACKETS, CLOSE_BRACKETS, WRITE, IF, THEN, ELSE, WHILE, DO, DIFFERENT, LESS_THAN, LESS_OR_EQUAL_THAN,
        GREATER_THAN, GREATER_OR_EQUAL_THAN, EQUAL_TO, OR, AND, PLUS, MINUS, MULTIPLY, DIV, CONSTINT, CONSTCHAR,
        NOT, TRUE, FALSE, OPEN_COMMENT, CLOSE_COMMENT, ERROR, EOS
}TAtomo;

typedef struct{
        TAtomo atomo;
        int linha;
        union{
                int numero;
                char id[16];
                char ch;
        }atributo;
}TInfoAtomo;

extern const char* atom_outputs[];
extern const char* atom_outputs_symbols[];
extern char *buffer;
extern char lexema[100000];
extern TAtomo lookahead;
extern TInfoAtomo infoAtomo;
extern int nLinha;
extern int variable_counter;
extern const int IDENTIFIER_MAX_LENGTH;

#endif