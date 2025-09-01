#include <stdio.h>
#include <string.h>

char buffer[10000];

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
        file_pointer = fopen(source_code_filename, "r");

        if(NULL == file_pointer){
                printf("Arquivo fonte não encontrado na pasta atual.\n");
                return 1;
        }

        char temporary_buffer[10000];

        while(fgets(temporary_buffer, sizeof(temporary_buffer), file_pointer) != NULL) strcat(buffer, temporary_buffer);

        printf("%s", buffer);

        fclose(file_pointer);

        return 0;
}
