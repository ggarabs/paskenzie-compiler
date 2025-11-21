// Gustavo Garabetti - RA: 10409258

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "errors.h"
#define PRIME_NUMBER 211

int busca_tabela_simbolos(char* s);

typedef struct _TNo{
        char ID[16];
        int endereco;
        struct _TNo* prox;
}TNo;

typedef struct {
        TNo *entradas[PRIME_NUMBER];
}TTabelaSimbolos;

TTabelaSimbolos page_table;
int hashMack(char* s);

void aloca_variavel(){
        if(lookahead != IDENTIFIER) return;

        int hash_bucket = hashMack(infoAtomo.atributo.id);
        int address = busca_tabela_simbolos(infoAtomo.atributo.id);

        if(address != -1) report_semantic_error(infoAtomo.atributo.id, NOT_UNIQUE);

        TNo* variable_iterator = page_table.entradas[hash_bucket];
        while(variable_iterator != NULL && variable_iterator->prox != NULL) variable_iterator = variable_iterator->prox;

        TNo* new_node = (TNo*)malloc(sizeof(TNo));
        new_node->endereco = variable_counter++;
        new_node->prox = NULL;
        strncpy(new_node->ID, infoAtomo.atributo.id, strlen(infoAtomo.atributo.id));

        if(variable_iterator == NULL) page_table.entradas[hash_bucket] = new_node;
        else variable_iterator->prox = new_node;
}

int busca_tabela_simbolos(char *s){
        if(lookahead != IDENTIFIER) return 0;

        int address = hashMack(s);
        TNo* variable_iterator = page_table.entradas[address];
        while(variable_iterator != NULL){
                if(strcmp(variable_iterator->ID, s) == 0){
                        return variable_iterator->endereco;
                }
                variable_iterator = variable_iterator->prox;
        }
        return -1;
}

void imprime_tabela_simbolos(){
        printf("\nTABELA DE SIMBOLOS\n");
        for(int i = 0; i < PRIME_NUMBER; i++){
                TNo* variable_iterator = page_table.entradas[i];
                while(variable_iterator != NULL){
                        printf("Entrada Tabela Simbolos: [%d] => %s | Endereco: %d\n", i, variable_iterator->ID, variable_iterator->endereco);
                        variable_iterator = variable_iterator->prox;
                }
        }
}