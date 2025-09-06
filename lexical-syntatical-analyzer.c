#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum{
        PROGRAM, IDENTIFIER, SEMICOLON, DOT, VAR, COMMA, COLON, CHAR, INTEGER, BOOLEAN, BEGIN, END, ASSIGNMENT, READ,
        OPEN_BRACKETS, CLOSE_BRACKETS, WRITE, IF, THEN, ELSE, WHILE, DO, DIFFERENT, LESS_THAN, LESS_OR_EQUAL_THAN,
        GREATER_THAN, GREATER_OR_EQUAL_THAN, EQUAL_TO, OR, AND, PLUS, MINUS, MULTIPLY, DIV, CONSTINT, CONSTCHAR,
        NOT, TRUE, FALSE, OPEN_COMMENT, CLOSE_COMMENT, ERROR, EOS
}TAtomo;

const char* atom_outputs[] = {"program", "identifier", "ponto_vigula", "ponto", "var", "virgula", "dois_pontos", "char",
                              "integer", "boolean", "begin", "end", "atribuicao", "read", "abre_parentesis", 
                              "fecha_parentesis", "write", "if", "then", "else", "while", "do", "diferente", "menor", "menor ou igual", 
                              "maior", "maior ou igual", "igual", "or", "and", "mais", "menos", "vezes", "dividido", "constint", "constchar",
                              "not", "true", "false", "comentario", "", "", "fim de arquivo"};

typedef struct{
        TAtomo atomo;
        int linha;
        union{
                double numero;
                char id[16];
                char ch;
        }atributo;
}TInfoAtomo;

char *buffer;
char lexema[16];
TAtomo lookahead;
TInfoAtomo infoAtomo;
int nLinha;

TInfoAtomo obter_atomo();
void recognizes_keyword_or_identifier(TInfoAtomo *info);
void recognizes_constint(TInfoAtomo *info);
void recognizes_operator_or_delimiter(TInfoAtomo *info);
void recognizes_constchar(TInfoAtomo *info);

void consome(TAtomo atomo);

void program();
void block();
void variable_declaration_part();
void variable_declaration();
void type();
void statement_part();
void statement();
void assignment_statement();
void read_statement();
void write_statement();
void if_statement();
void while_statement();
void expression();
void relational_operator();
void simple_expression();
void adding_operator();
void term();
void multiplying_operator();
void factor();

bool is_operator_or_delimiter(char* symbol){
        return *symbol == ',' || *symbol == '.' || 
                *symbol == ';' || *symbol == ':' || 
                *symbol == '(' || *symbol == ')' ||
                *symbol == '<' || *symbol == '>' || 
                *symbol == '+' || *symbol == '-' ||
                *symbol == '*';
}

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

        buffer = calloc(10000, sizeof(char)); // verificar

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

TInfoAtomo obter_atomo(){
        TInfoAtomo infoAtomo;

        infoAtomo.atomo = ERROR;

        while(*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r'){
                if(*buffer == '\n') nLinha++;
                buffer++;
        }

        if(isdigit(*buffer)) recognizes_constint(&infoAtomo);
        else if(isalpha(*buffer) || *buffer == '_') recognizes_keyword_or_identifier(&infoAtomo);
        else if(is_operator_or_delimiter(buffer)) recognizes_operator_or_delimiter(&infoAtomo);
        else if(*buffer == '\'') recognizes_constchar(&infoAtomo);
        else if(*buffer == '\0') infoAtomo.atomo = EOS;

        return infoAtomo;
}

void consome(TAtomo atomo){
        printf("%d\n", lookahead);
        if(lookahead == OPEN_COMMENT){
                printf("#  %d:%s", nLinha, atom_outputs[atomo]);
                while(lookahead != CLOSE_COMMENT){
                        infoAtomo = obter_atomo();
                        lookahead = infoAtomo.atomo;
                }
                infoAtomo = obter_atomo();
                lookahead = infoAtomo.atomo;
        }
        if(lookahead == atomo){
                printf("#  %d:%s", nLinha, atom_outputs[atomo]);
                if(atomo == CONSTINT || atomo == CONSTCHAR || atomo == IDENTIFIER) printf(" : %s", infoAtomo.atributo.id);
                printf("\n");
                infoAtomo = obter_atomo();
                lookahead = infoAtomo.atomo;
        }else{
                printf("erro sintático: esperado [%c] encontrado [%c]\n", atomo, lookahead);
                exit(1);
        }
}



void recognizes_constint(TInfoAtomo *info){
        char *ini_lexema = buffer;
q1:
        if(isdigit(*buffer)){
                buffer++;
                goto q1;
        }else if('d' == *buffer){
                buffer++;
                goto q2;
        }
              
        info->atributo.numero = CONSTINT;
        return;
q2:
       if(isdigit(*buffer)){
                buffer++;
                goto q4;
        }else if('+' == *buffer){
                buffer++;
                goto q3;
        }

        return;
q3:
        if(isdigit(*buffer)){
                buffer++;
                goto q3;
        }
        return;
q4:
        if(isdigit(*buffer)){
                buffer++;
                goto q4;
        }

        strncpy(lexema, ini_lexema, buffer-ini_lexema);
        info->atributo.id[buffer-ini_lexema] = '\0';

        info->atomo = CONSTINT;
        info->atributo.numero = atof(lexema);

        return;
}

void recognizes_keyword_or_identifier(TInfoAtomo *info){
        char *ini_lexema = buffer;
q1:
        if(isdigit(*buffer) || isalpha(*buffer) ||'_' == *buffer){
                buffer++;
                goto q1;
        }

        strncpy(info->atributo.id, ini_lexema, buffer-ini_lexema);
        info->atributo.id[buffer-ini_lexema] = '\0';

        const char* keywords[] = {"program", "var", "char", "integer", "boolean", "begin", "end", "read", "write", 
                                 "if", "then", "else", "while", "do", "or", "and", "div", "not", "true", "false"};

        const TAtomo atoms[] = {PROGRAM, VAR, CHAR, INTEGER, BOOLEAN, BEGIN, END, READ, WRITE, IF, THEN, ELSE, 
                                WHILE, DO, OR, AND, DIV, NOT, TRUE, FALSE};

        int keywords_length = sizeof(keywords)/sizeof(keywords[0]);
        bool is_keyword = false;

        for(int index = 0; index < keywords_length; index++){
                if(0 == strcmp(info->atributo.id, keywords[index])){
                        info->atomo = atoms[index];
                        is_keyword = true;
                        break;
                }
        }

        if(!is_keyword) info->atomo = IDENTIFIER;

        return;
}

void recognizes_operator_or_delimiter(TInfoAtomo *info){
        char* ini_lexema = buffer;

        if(*buffer == ':'){
                buffer++;
                goto q1;
        }else if(*buffer == '<'){
                buffer++;
                goto q2;
        }else if(*buffer == '>'){
                buffer++;
                goto q3;
        }else if(*buffer == '('){
                buffer++;
                goto q4;
        }else if(*buffer == '*'){
                buffer++;
                goto q5;
        }else if(*buffer == '+' || *buffer == '-' || *buffer == ')' ||
                 *buffer == ';' || *buffer == ',' || *buffer == '.'){
                buffer++;
                goto q6;
        }
        return;
q1:
        if(*buffer == '=') buffer++;
        goto q6;
q2:
        if(*buffer == '=' || *buffer == '>') buffer++;
        goto q6;
q3:
        if(*buffer == '=') buffer++;
        goto q6;
q4:
        if(*buffer == '*') buffer++;
        goto q6;
q5:
        if(*buffer == ')') buffer++;
        goto q6;
q6:

        strncpy(info->atributo.id, ini_lexema, buffer-ini_lexema);
        info->atributo.id[buffer-ini_lexema] = '\0';

        const char* operator[] = {":=", "<>", "<", "<=", ">", ">=", "+", "-", "*"};
        const TAtomo operators_atoms[] = {ASSIGNMENT, DIFFERENT, LESS_THAN, LESS_OR_EQUAL_THAN, GREATER_THAN,
                                          GREATER_OR_EQUAL_THAN, PLUS, MINUS, MULTIPLY};

        const int operator_length = sizeof(operator)/sizeof(operator[0]);
        bool is_operator = false;

        for(int index = 0; index < operator_length; index++){
                if(strcmp(info->atributo.id, operator[index]) == 0){
                        info->atomo = operators_atoms[index];
                        is_operator = true;
                        break;
                }
        }

        if(is_operator) return;

        const char* delimiters[] = {";", ".", ":", ",", "(", ")"};
        const TAtomo delimiters_atoms[] = {SEMICOLON, DOT, COLON, COMMA, OPEN_BRACKETS, CLOSE_BRACKETS};

        int delimiters_length = sizeof(delimiters);

        for(int index = 0; index < delimiters_length; index++){
                if(strcmp(info->atributo.id, delimiters[index]) == 0){
                        info->atomo = delimiters_atoms[index];
                        break;
                }
        }

        return;
}

void recognizes_constchar(TInfoAtomo *info){
        char* ini_lexema = buffer;
        
        buffer += 2;
        ini_lexema++;

        if(*buffer != '\'') return;

        buffer++;

        info->atributo.ch = *ini_lexema;
        strncpy(info->atributo.id, ini_lexema, 1);
        info->atributo.id[1] = '\0';
        info->atomo = CONSTCHAR;

        return;
}

void program(){         // OK
        consome(PROGRAM);
        consome(IDENTIFIER);
        consome(SEMICOLON);
        block();
        consome(DOT);
}

void block(){           // OK
        variable_declaration_part();
        statement_part();
}

void variable_declaration_part(){ // OK
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

void variable_declaration(){ // OK
        consome(IDENTIFIER);
        while(lookahead == COMMA){
                consome(COMMA);
                consome(IDENTIFIER);
        }
        consome(COLON);
        type();
}

void type(){ // Verificar
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

void statement_part(){ // OK
        consome(BEGIN);
        statement();
        while(lookahead == SEMICOLON){
                consome(SEMICOLON);
                statement();
        }
        consome(END);
}

void statement(){ // Verificar
        if (lookahead == READ) read_statement();
        else if (lookahead == WRITE) write_statement();
        else if (lookahead == IF) if_statement();
        else if (lookahead == WHILE) while_statement();
        else if (lookahead == BEGIN) statement_part();
        else assignment_statement();
}

void assignment_statement(){ // OK
        consome(IDENTIFIER);
        consome(ASSIGNMENT);
        expression();
}

void read_statement(){ // OK
        consome(READ);
        consome(OPEN_BRACKETS);
        consome(IDENTIFIER);
        while(lookahead == COMMA){
                consome(COMMA);
                consome(IDENTIFIER);
        }
        consome(CLOSE_BRACKETS);
}

void write_statement(){ // OK
        consome(WRITE);
        consome(OPEN_BRACKETS);
        consome(IDENTIFIER);
        while(lookahead == COLON){
                consome(COLON);
                consome(IDENTIFIER);
        }
        consome(CLOSE_BRACKETS);
}

void if_statement(){ // OK
        consome(IF);
        expression();
        consome(THEN);
        statement();
        if(lookahead == ELSE){
                consome(ELSE);
                statement();
        }
}

void while_statement(){ // OK
        consome(WHILE);
        expression();
        consome(DO);
        statement();
}

void expression(){ // OK
        simple_expression();
        if(lookahead == DIFFERENT || lookahead == LESS_THAN || lookahead == LESS_OR_EQUAL_THAN || 
              lookahead == GREATER_OR_EQUAL_THAN ||  lookahead == GREATER_THAN || 
              lookahead == EQUAL_TO || lookahead == OR || lookahead == AND){
                relational_operator();
                simple_expression();
        }
}

void relational_operator(){ // Verificar
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

        if(!is_relational_operator) consome(OR); // Pensar melhor

}

void simple_expression(){ // OK
        term();
        while(lookahead == PLUS || lookahead == MINUS){
                adding_operator();
                term();
        }
}

void adding_operator(){ // OK
        if(lookahead == PLUS) consome(PLUS);
        else if(lookahead == MINUS) consome(MINUS);
        else consome(PLUS);
}

void term(){ // OK
        factor();
        while(lookahead == MULTIPLY || lookahead == DIV){
                multiplying_operator();
                factor();
        }
}

void multiplying_operator(){ // OK
        if(lookahead == MULTIPLY) consome(MULTIPLY);
        else if(lookahead == DIV) consome(DIV);
        else consome(MULTIPLY);
}

void factor(){ // OK
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
