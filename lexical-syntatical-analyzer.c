#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum{
        PROGRAM, IDENTIFIER, SEMICOLON, DOT, VAR, COMMA, COLON, CHAR, INTEGER, BOOLEAN, BEGIN, END, ASSIGNMENT, READ,
        OPEN_BRACKETS, CLOSE_BRACKETS, WRITE, IF, THEN, ELSE, WHILE, DO, DIFFERENT, LESS_THAN, LESS_OR_EQUAL_THAN,
        GREATER_THAN, GREATER_OR_EQUAL_THAN, EQUAL_TO, OR, AND, PLUS, MINUS, MULTIPLY, DIV, CONSTINT, CONSTCHAR,
        NOT, TRUE, FALSE, ERROR, EOS
}TAtomo;

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

        buffer = calloc(10000, sizeof(char));

        while(fgets(temporary_buffer, sizeof(temporary_buffer), file_pointer) != NULL) strcat(buffer, temporary_buffer);

        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;

        program();
        consome(EOS);

        fclose(file_pointer);

        return 0;
}

TInfoAtomo obter_atomo(){
        TInfoAtomo infoAtomo;

        infoAtomo.atomo = ERROR;

        while(*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') buffer++;

        if(isdigit(*buffer)) recognizes_constint(&infoAtomo);
        else if(isalpha(*buffer) || *buffer == '_') recognizes_keyword_or_identifier(&infoAtomo);
        else if(is_operator_or_delimiter(buffer)) recognizes_operator_or_delimiter(&infoAtomo);
        else if(*buffer == '\'') recognizes_constchar(&infoAtomo);
        else if(*buffer == '\0') infoAtomo.atomo = EOS;

        return infoAtomo;
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
        lexema[buffer-ini_lexema] = '\0';

        info->atomo = CONSTINT;
        info->atributo.numero = atof(lexema);

        return;
}

void recognizes_keyword_or_identifier(TInfoAtomo *info){
        char *ini_lexema = buffer;
q1:
        if(isdigit(*buffer) || isalpha(*buffer) || *buffer == '_'){
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
        }else if(*buffer == '+' || *buffer == '-' || *buffer == '*' ||
                 *buffer == ';' || *buffer == ':' || *buffer == ',' ||
                 *buffer == '.' || *buffer == '(' || *buffer == ')'){
                buffer++;
                goto q4;
        }
        return;
q1:
        if(*buffer == '='){
                buffer++;
                goto q4;
        }
        return;
q2:
        if(*buffer == '=' || *buffer == '>') buffer++;
        goto q4;

q3:
        if(*buffer == '=') buffer++;
        goto q4;

q4:

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
        info->atomo = CONSTCHAR;

        return;
}

void consome(TAtomo atomo){
        if(lookahead == atomo){
                infoAtomo = obter_atomo();
                lookahead = infoAtomo.atomo;
        }
        else{
                printf("erro sintático: esperado [%c] encontrado [%c]\n", atomo, lookahead);
                exit(1);
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
                        consome(' ');
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
        if (lookahead == READ) {
                read_statement();
        } else if (lookahead == WRITE) {
                write_statement();
        } else if (lookahead == IF) {
                if_statement();
        } else if (lookahead == WHILE) {
                while_statement();
        } else if (lookahead == BEGIN) {
                statement_part();
        } else {
                assignment_statement();
        }
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
        while(lookahead == ELSE){
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
        while(lookahead == DIFFERENT || lookahead == LESS_THAN || lookahead == LESS_OR_EQUAL_THAN || 
              lookahead == GREATER_OR_EQUAL_THAN ||  lookahead == GREATER_THAN || 
              lookahead == EQUAL_TO || lookahead == OR || lookahead == AND){
                relational_operator();
                simple_expression();
        }
}

void relational_operator(){
        if(lookahead == DIFFERENT){
                consome(DIFFERENT);
        }else if(lookahead == LESS_THAN){
                consome(LESS_THAN);
        }else if(lookahead == LESS_OR_EQUAL_THAN){
                consome(LESS_OR_EQUAL_THAN);
        }else if(lookahead == GREATER_OR_EQUAL_THAN){
                consome(GREATER_OR_EQUAL_THAN);
        }else if(lookahead == GREATER_THAN){
                consome(GREATER_THAN);
        }else if(lookahead == ASSIGNMENT){
                consome(ASSIGNMENT);
        }else if(lookahead == OR){
                consome(OR);
        }else if(lookahead == AND){
                consome(AND);
        }else consome(OR);
}

void simple_expression(){
        term();
        while(lookahead == PLUS || lookahead == MINUS){
                adding_operator();
                term();
        }
}

void adding_operator(){
        if(lookahead == PLUS){
                consome(PLUS);
        }else if(lookahead == MINUS){
                consome(MINUS);
        }else{
                consome(PLUS);
        }
}

void term(){
        factor();
        while(lookahead == MULTIPLY || lookahead == DIV){
                multiplying_operator();
                factor();
        }
}

void multiplying_operator(){
        if(lookahead == MULTIPLY){
                consome(MULTIPLY);
        }else if(lookahead == DIV){
                consome(DIV);
        }else{
                consome(MULTIPLY);
        }
}

void factor(){
        switch(lookahead){
                case IDENTIFIER:
                        consome(IDENTIFIER);
                        break;
                case CONSTINT:
                        consome(CONSTINT);
                        break;
                case CONSTCHAR:
                        consome(CONSTCHAR);
                        break;
                case OPEN_BRACKETS:
                        consome(OPEN_BRACKETS);
                        expression();
                        consome(CLOSE_BRACKETS);
                        break;
                case NOT:
                        consome(NOT);
                        factor();
                        break;
                case TRUE:
                        consome(TRUE);
                        break;
                case FALSE:
                        consome(FALSE);
                        break;
                default:
                        consome(IDENTIFIER);
        }
}
