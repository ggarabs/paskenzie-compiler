#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"
#include "globals.h"

int main(int argc, char** argv){
        if(argc == 1){
                printf("Nenhum código fonte fornecido.\n");
                return 1;
        }
        if(argc > 2){
                printf("Mais do que um código fonte fornecido.\n");
                return 1;
        }

        char* source_code_filename = argv[1];

        FILE* file_pointer;

        file_pointer = fopen(source_code_filename, "rb");

        if(NULL == file_pointer){
                printf("Arquivo fonte não encontrado na pasta atual.\n");
                return 1;
        }

        fseek(file_pointer, 0, SEEK_END);
        long file_size = ftell(file_pointer);

        file_pointer = fopen(source_code_filename, "r");

        if(NULL == file_pointer){
                printf("Arquivo fonte não encontrado na pasta atual.\n");
                return 1;
        }

        char temporary_buffer[file_size];

        buffer = calloc(file_size, sizeof(char));

        while(fgets(temporary_buffer, sizeof(temporary_buffer), file_pointer) != NULL) strcat(buffer, temporary_buffer);

        nLinha = 1;

        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;

        program();
        consome(EOS);

        printf("\n%d linha(s) analisada(s), programa sintaticamente correto\n", nLinha);

        fclose(file_pointer);

        return 0;
}