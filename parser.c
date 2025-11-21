// Gustavo Garabetti - RA: 10409258

#include <stdbool.h>
#include <stdlib.h>
#include "parser.h"
#include "globals.h"
#include "lexer.h"
#include "utils.h"
#include "errors.h"
#include "semantic-analyzer.h"

const int operator_atom[] = {DIFFERENT, LESS_THAN, LESS_OR_EQUAL_THAN, GREATER_OR_EQUAL_THAN, 
                                        GREATER_THAN, EQUAL_TO, OR, AND};

const char* operator_instructions[] = {"\tCMDG", "\tCMME", "\tCMEG", "\tCMAG", "\tCMMA", "\tCMIG", "\tDISJ", "\tCONJ"};                                        

void consome(TAtomo atomo){
       if(lookahead == OPEN_COMMENT) consome_comment();
       if(lookahead == EOS) return;
       if(lookahead == atomo){
                // printf("#  %d:%s", nLinha, atom_outputs[atomo]);
                // if(atomo == IDENTIFIER) printf(" : %s", infoAtomo.atributo.id);
                // else if(atomo == CONSTCHAR) printf(" : %c", infoAtomo.atributo.ch);
                // else if(atomo == CONSTINT) printf(" : %d", infoAtomo.atributo.numero);
                // printf("\n");
                infoAtomo = obter_atomo();
                lookahead = infoAtomo.atomo;
                if(infoAtomo.atomo == ERROR) report_lexical_error();
                if(lookahead == OPEN_COMMENT) consome_comment();        // trata caso de comentários consecutivos
        }else
                report_syntax_error(atomo, infoAtomo);
}

void consome_comment(){
        while(lookahead == OPEN_COMMENT){
                if(lookahead == EOS) report_lexical_error();
                // printf("#  %d:%s\n", nLinha, atom_outputs[lookahead]);
                infoAtomo = obter_atomo();
                lookahead = infoAtomo.atomo;
                while(lookahead != CLOSE_COMMENT){
                        if(lookahead == EOS) report_comment_error(buffer);      // trata comentários não fechados
                        infoAtomo = obter_atomo();
                        lookahead = infoAtomo.atomo;
                }
                infoAtomo = obter_atomo();
                lookahead = infoAtomo.atomo;                
        }
}

void program(){
        printf("\tINPP\n");
        consome(PROGRAM);
        consome(IDENTIFIER);
        consome(SEMICOLON);
        block();
        consome(DOT);
        if(variable_counter > 0) printf("\tDMEM %d\n", variable_counter);
        printf("\tPARA\n");
}

void block(){
        variable_declaration_part();
        statement_part();
}

void variable_declaration_part(){
        if(lookahead == VAR){
                consome(VAR);
                variable_declaration();
                consome(SEMICOLON);
                while(lookahead == IDENTIFIER){
                        variable_declaration();
                        consome(SEMICOLON);
                }
        }
        if(variable_counter > 0) printf("\tAMEM %d\n", variable_counter);
}

void variable_declaration(){
        aloca_variavel();
        consome(IDENTIFIER);
        while(lookahead == COMMA){
                consome(COMMA);
                aloca_variavel();
                consome(IDENTIFIER);
        }
        consome(COLON);
        type();
}

void type(){
        switch(lookahead){
                case CHAR:
                        consome(CHAR);
                        break;
                case INTEGER:
                        consome(INTEGER);
                        break;
                case BOOLEAN:
                        consome(BOOLEAN);
                        break;
                default:
                        consome(CHAR);
                        break;
        }
}

void statement_part(){
        consome(BEGIN);
        statement();
        while(lookahead == SEMICOLON){
                consome(SEMICOLON);
                statement();
        }
        consome(END);
}

void statement(){
        if (lookahead == READ) read_statement();
        else if (lookahead == WRITE) write_statement();
        else if (lookahead == IF) if_statement();
        else if (lookahead == WHILE) while_statement();
        else if (lookahead == BEGIN) statement_part();
        else assignment_statement();
}

void assignment_statement(){
        int endereco = busca_tabela_simbolos(infoAtomo.atributo.id);
        if(endereco == -1) report_semantic_error(infoAtomo.atributo.id, NOT_DECLARED);
        consome(IDENTIFIER);
        consome(ASSIGNMENT);
        expression();
        printf("\tARMZ %d\n", endereco);
}

void read_statement(){
        consome(READ);
        consome(OPEN_BRACKETS);
        int endereco = busca_tabela_simbolos(infoAtomo.atributo.id);
        if(endereco == -1) report_semantic_error(infoAtomo.atributo.id, NOT_DECLARED);
        consome(IDENTIFIER);
        printf("\tLEIT\n");
        printf("\tARMZ %d\n", endereco);
        while(lookahead == COMMA){
                consome(COMMA);
                endereco = busca_tabela_simbolos(infoAtomo.atributo.id);
                if(endereco == -1) report_semantic_error(infoAtomo.atributo.id, NOT_DECLARED);
                consome(IDENTIFIER);
                printf("\tLEIT\n");
                printf("\tARMZ %d\n", endereco);
        }
        consome(CLOSE_BRACKETS);
}

void write_statement(){
        consome(WRITE);
        consome(OPEN_BRACKETS);
        int endereco = busca_tabela_simbolos(infoAtomo.atributo.id);
        if(endereco == -1) report_semantic_error(infoAtomo.atributo.id, NOT_DECLARED);
        consome(IDENTIFIER);
        printf("\tCRVL %d\n", endereco);
        printf("\tIMPR\n");
        while(lookahead == COMMA){
                consome(COMMA);
                int endereco = busca_tabela_simbolos(infoAtomo.atributo.id);
                if(endereco == -1) report_semantic_error(infoAtomo.atributo.id, NOT_DECLARED);
                consome(IDENTIFIER);
                printf("\tCRVL %d\n", endereco);
                printf("\tIMPR\n");
        }
        consome(CLOSE_BRACKETS);
}

void if_statement(){
        int L1 = proximo_rotulo();
        int L2 = proximo_rotulo();
        consome(IF);
        expression();
        consome(THEN);
        printf("\tDSVF L%d\n", L1);
        statement();
        printf("\tDSVS L%d\n", L2);
        printf("L%d: NADA\n", L1);
        if(lookahead == ELSE){
                consome(ELSE);
                statement();
        }
        printf("L%d: NADA\n", L2);
}

void while_statement(){
        int L1 = proximo_rotulo();
        int L2 = proximo_rotulo();
        printf("L%d: NADA\n", L1);
        consome(WHILE);
        expression();
        printf("\tDSVF L%d\n", L2);
        consome(DO);
        statement();
        printf("\tDSVS L%d\n", L1);
        printf("L%d: NADA\n", L2);
}

void expression(){
        simple_expression();

        const int operator_atom_length = sizeof(operator_atom)/sizeof(int);

        for(int index = 0; index < operator_atom_length; index++){
                if(lookahead == operator_atom[index]){
                        relational_operator();
                        simple_expression();
                        printf("%s\n", operator_instructions[index]);
                        break;
                }
        }
}

void relational_operator(){
        bool is_relational_operator = false;

        const int operator_atom_length = sizeof(operator_atom)/sizeof(int);

        for(int index = 0; index < operator_atom_length; index++){
                if(lookahead == operator_atom[index]){
                        consome(operator_atom[index]);
                        is_relational_operator = true;
                        break;
                }
        }

        if(!is_relational_operator) consome(OR);

}

void simple_expression(){
        term();
        while(lookahead == PLUS || lookahead == MINUS){
                bool is_sum = lookahead == PLUS;
                adding_operator();
                term();
                if(is_sum) printf("\tSOMA\n");
                else printf("\tSUBT\n");
        }
}

void adding_operator(){
        if(lookahead == PLUS) consome(PLUS);
        else if(lookahead == MINUS) consome(MINUS);
        else consome(PLUS);
}

void term(){
        factor();
        while(lookahead == MULTIPLY || lookahead == DIV){
                bool is_multiplication = lookahead == MULTIPLY;
                multiplying_operator();
                factor();
                if(is_multiplication) printf("\tMULT\n");
                else printf("\tDIVI\n");
        }
}

void multiplying_operator(){
        if(lookahead == MULTIPLY) consome(MULTIPLY);
        else if(lookahead == DIV) consome(DIV);
        else consome(MULTIPLY);
}

void factor(){
        const int factor_atoms[] = {IDENTIFIER, CONSTINT, CONSTCHAR, 
                                   OPEN_BRACKETS, NOT, TRUE, FALSE};

        const int factor_atoms_length = sizeof(factor_atoms)/sizeof(int);

        bool is_factor = false;
        for(int index = 0; index < factor_atoms_length; index++){
                if(lookahead == factor_atoms[index]){
                        if(factor_atoms[index] == OPEN_BRACKETS){
                                consome(OPEN_BRACKETS);
                                expression();
                                consome(CLOSE_BRACKETS);
                        }else if(factor_atoms[index] == NOT){
                                consome(NOT);
                                factor();
                        }else if(factor_atoms[index] == CONSTINT){
                                printf("\tCRCT %d\n", infoAtomo.atributo.numero);
                                consome(CONSTINT);
                        }else if(factor_atoms[index] == IDENTIFIER){
                                int endereco = busca_tabela_simbolos(infoAtomo.atributo.id);
                                if(endereco == -1) report_semantic_error(infoAtomo.atributo.id, NOT_DECLARED);
                                printf("\tCRVL %d\n", endereco);
                                consome(IDENTIFIER);
                        }else consome(factor_atoms[index]);
                        is_factor = true;
                        break;
                }
        }

        if(!is_factor) consome(IDENTIFIER);
}
