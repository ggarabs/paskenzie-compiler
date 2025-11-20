// Gustavo Garabetti - RA: 10409258

#include "errors.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "utils.h"

void report_lexical_error(){
        bool is_buffer_changed = false;

        printf("%s\n", lexema);

        // se o erro foi encontrado no meio de um token, esse trecho de código o recupera
        char *ini_lexema = buffer-strlen(lexema);
        while(*buffer != EOS && *buffer != '\n' && *buffer != ' ' && !is_token_delimiter()){
                buffer++;
                is_buffer_changed = true;
        }
        if(is_buffer_changed){
                int incorrect_token_length = buffer-ini_lexema;
                if(incorrect_token_length > 0){
                        strncpy(lexema, ini_lexema, buffer-ini_lexema);
                        lexema[incorrect_token_length] = '\0';
                }
        }

        if(strlen(lexema) > IDENTIFIER_MAX_LENGTH) printf("#  %d:erro léxico, comprimento do token [%s] maior que o permitido.\n", nLinha, lexema);
        else printf("#  %d:erro léxico, token [%s] não reconhecido.\n", nLinha, lexema);
        
        exit(1);
}

void report_syntax_error(TAtomo atomo, TInfoAtomo infoAtomo){
        if(lookahead == IDENTIFIER) printf("#  %d:erro sintático, esperado [%s] encontrado [identifier:%s]\n", nLinha, atom_outputs_symbols[atomo], infoAtomo.atributo.id);
        else printf("#  %d:erro sintático, esperado [%s] encontrado [%s]\n", nLinha, atom_outputs_symbols[atomo], atom_outputs_symbols[lookahead]);
        exit(1);
}

void report_comment_error(char* incorrect_token){
        printf("#  %d:erro léxico, comentário mal formado.\n", nLinha);
        exit(1);
}
