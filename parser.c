#include <stdbool.h>
#include <stdlib.h>
#include "parser.h"
#include "globals.h"
#include "lexer.h"

void consome(TAtomo atomo){
       if(lookahead == OPEN_COMMENT) consome_comment();
       if(lookahead == EOS) return;

       if(lookahead == atomo){
                printf("#  %d:%s", nLinha, atom_outputs[atomo]);
                if(atomo == IDENTIFIER) printf(" : %s", infoAtomo.atributo.id);
                else if(atomo == CONSTCHAR) printf(" : %c", infoAtomo.atributo.ch);
                else if(atomo == CONSTINT) printf(" : %d", infoAtomo.atributo.numero);
                printf("\n");
                infoAtomo = obter_atomo();
                lookahead = infoAtomo.atomo;
                if(lookahead == OPEN_COMMENT) consome_comment();
        }else{
                printf("#  %d:erro sintático, esperado [%s] encontrado [%s]\n", nLinha, atom_outputs_symbols[atomo], atom_outputs_symbols[lookahead]);
                exit(1);
        }
}

void consome_comment(){
        while(lookahead == OPEN_COMMENT){
                printf("#  %d:%s\n", nLinha, atom_outputs[lookahead]);
                while(lookahead != CLOSE_COMMENT){
                        infoAtomo = obter_atomo();
                        lookahead = infoAtomo.atomo;
                }
                infoAtomo = obter_atomo();
                lookahead = infoAtomo.atomo;
        } 
}

void program(){
        consome(PROGRAM);
        consome(IDENTIFIER);
        consome(SEMICOLON);
        block();
        consome(DOT);
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
}

void variable_declaration(){
        consome(IDENTIFIER);
        while(lookahead == COMMA){
                consome(COMMA);
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
                case  INTEGER:
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
        consome(IDENTIFIER);
        consome(ASSIGNMENT);
        expression();
}

void read_statement(){
        consome(READ);
        consome(OPEN_BRACKETS);
        consome(IDENTIFIER);
        while(lookahead == COMMA){
                consome(COMMA);
                consome(IDENTIFIER);
        }
        consome(CLOSE_BRACKETS);
}

void write_statement(){
        consome(WRITE);
        consome(OPEN_BRACKETS);
        consome(IDENTIFIER);
        while(lookahead == COLON){
                consome(COLON);
                consome(IDENTIFIER);
        }
        consome(CLOSE_BRACKETS);
}

void if_statement(){
        consome(IF);
        expression();
        consome(THEN);
        statement();
        if(lookahead == ELSE){
                consome(ELSE);
                statement();
        }
}

void while_statement(){
        consome(WHILE);
        expression();
        consome(DO);
        statement();
}

void expression(){
        simple_expression();
        if(lookahead == DIFFERENT || lookahead == LESS_THAN || lookahead == LESS_OR_EQUAL_THAN || 
              lookahead == GREATER_OR_EQUAL_THAN ||  lookahead == GREATER_THAN || 
              lookahead == EQUAL_TO || lookahead == OR || lookahead == AND){
                relational_operator();
                simple_expression();
        }
}

void relational_operator(){
        const int operator_atom[] = {DIFFERENT, LESS_THAN, LESS_OR_EQUAL_THAN, GREATER_OR_EQUAL_THAN, 
                                        GREATER_THAN, ASSIGNMENT, OR, AND};

        const int operator_atom_length = sizeof(operator_atom)/sizeof(int);

        bool is_relational_operator = false;

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
                adding_operator();
                term();
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
                multiplying_operator();
                factor();
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
                        }else consome(factor_atoms[index]);
                        is_factor = true;
                        break;
                }
        }

        if(!is_factor) consome(IDENTIFIER);
}