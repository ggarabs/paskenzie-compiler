// Gustavo Garabetti - RA: 10409258

#include "globals.h"

// vetores para orientar melhor a impressão de símbolos a partir do lookahead na saída do código
const char* atom_outputs[] = {"program", "identifier", "ponto_vigula", "ponto", "var", "virgula", "dois_pontos", "char",
                              "integer", "boolean", "begin", "end", "atribuicao", "read", "abre_par", 
                              "fecha_par", "write", "if", "then", "else", "while", "do", "diferente", "menor", "menor ou igual", 
                              "maior", "maior ou igual", "igual", "or", "and", "mais", "menos", "vezes", "dividido", "constint", "constchar",
                              "not", "true", "false", "comentario", "", "", "fim de arquivo"};

const char* atom_outputs_symbols[] = {"program", "identifier or variable", ";", ".", "var", ",", ":", "type",
                              "type", "type", "begin", "end", ":=", "read", "(", 
                              ")", "write", "if", "then", "else", "while", "do", "<>", "<", "<=", 
                              ">", ">=", "=", "or", "and", "adding operator", "adding operator", "multiplying_operator", "multiplying_operator", "constint", "constchar",
                              "not", "true", "false", "(*", "*)", "", "\0"};

TAtomo lookahead;
TInfoAtomo infoAtomo;
char *buffer;
char lexema[100000];    // lexema definido com valor acima do id máximo para exibir tokens incorretos mesmo sem armazenar no id do átomo
int nLinha = 1;
const int IDENTIFIER_MAX_LENGTH = 15;
