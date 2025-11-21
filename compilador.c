// Gustavo Garabetti - RA: 10409258

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"
#include "globals.h"
#include "utils.h"
#include "semantic-analyzer.h"

int main(int argc, char** argv){
        // obtenção do nome do código-fonte em paskenzie a partir dos argumentos fornecidos por linha de comando
        if(argc == 1){
                printf("Nenhum código fonte fornecido.\n");
                return 1;
        }
        if(argc > 2){
                printf("Mais do que um código fonte fornecido.\n");
                return 1;
        }

        char* source_code_filename = argv[1];
        FILE* file_pointer = fopen(source_code_filename, "rb"); // abertura do arquivo para leitura em binário

        if(NULL == file_pointer){
                printf("Arquivo fonte não encontrado na pasta atual.\n");
                return 1;
        }

        long file_size = get_file_size(file_pointer, source_code_filename);

        file_pointer = fopen(source_code_filename, "r");

        char* temporary_buffer = (char*) calloc(file_size, sizeof(char));
        buffer = (char*) calloc(file_size+1, sizeof(char));

        // gravar conteúdo do arquivo no buffer
        while(fgets(temporary_buffer, file_size, file_pointer) != NULL) strcat(buffer, temporary_buffer);

        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;

        program();
        consome(EOS);

        imprime_tabela_simbolos();

        printf("\n%d linha(s) analisada(s), programa sintaticamente correto\n", nLinha);

        fclose(file_pointer);

        return 0;
}
