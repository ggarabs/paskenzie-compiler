#include "globals.h"

const char* atom_outputs[] = {"program", "identifier", "ponto_vigula", "ponto", "var", "virgula", "dois_pontos", "char",
                              "integer", "boolean", "begin", "end", "atribuicao", "read", "abre_par", 
                              "fecha_par", "write", "if", "then", "else", "while", "do", "diferente", "menor", "menor ou igual", 
                              "maior", "maior ou igual", "igual", "or", "and", "mais", "menos", "vezes", "dividido", "constint", "constchar",
                              "not", "true", "false", "comentario", "", "", "fim de arquivo"};

const char* atom_outputs_symbols[] = {"program", "identifier", ";", ".", "var", ",", ":", "char",
                              "integer", "boolean", "begin", "end", ":=", "read", "(", 
                              ")", "write", "if", "then", "else", "while", "do", "<>", "<", "<=", 
                              ">", ">=", "=", "or", "and", "+", "-", "*", "div", "constint", "constchar",
                              "not", "true", "false", "(*", "*)", "", "\0"};

char *buffer;
char lexema[16];
TAtomo lookahead;
TInfoAtomo infoAtomo;
int nLinha;
const int IDENTIFIER_MAX_LENGTH = 15;