#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef enum{
        PROGRAM,
        SEMICOLON,
        DOT,
        VAR,
        IDENTIFIER,
        COMMA,
        COLON,
        CHAR,
        INTEGER,
        BOOLEAN,
        BEGIN,
        END,
        ASSIGNMENT,
        READ,
        OPEN_BRACKETS,
        CLOSE_BRACKETS,
        WRITE,
        IF,
        THEN,
        ELSE,
        WHILE,
        DO,
        DIFFERENT,
        LESS_THAN,
        LESS_OR_EQUAL_THEN,
        GREATER_THAN,
        GREATER_OR_EQUAL_THEN,
        EQUAL_TO,
        OR,
        AND,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        CONSTINT,
        CONSTCHAR,
        NOT,
        TRUE,
        FALSE,
        ERRO,
        EOS
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
char lexema[20];
int nLinha;
char lookahead;
TInfoAtomo infoAtomo;

TInfoAtomo obter_atomo();
void recognizes_constint(TInfoAtomo *info);
void recognizes_identifier(TInfoAtomo *info);
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

        infoAtomo.atomo = ERRO;

        while(*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') buffer++;

        if(isdigit(*buffer)) recognizes_constint(&infoAtomo);
        else if(isalpha(*buffer) || *buffer == '_') recognizes_identifier(&infoAtomo);
        else if(*buffer == '\0') infoAtomo.atomo = EOS;
        else recognizes_constchar(&infoAtomo);

        return infoAtomo;
}

void recognizes_constint(TInfoAtomo *info){
        char *ini_lexema = buffer;
q1:
        if(*buffer == 'd'){
                buffer++;
                goto q2;
        }
        if(isdigit(*buffer)){
                buffer++;
                goto q1;
        }
        info->atributo.numero = CONSTINT;
        return;
q2:
        if(*buffer == '+'){
                buffer++;
                goto q3;
        }
        if(isdigit(*buffer)){
                buffer++;
                goto q4;
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

void recognizes_identifier(TInfoAtomo *info){
        char *ini_lexema = buffer;
q1:
        if(isdigit(*buffer) || isalpha(*buffer) || *buffer == '_'){
                buffer++;
                goto q1;
        }

        strncpy(info->atributo.id, ini_lexema, buffer-ini_lexema);
        info->atributo.id[buffer-ini_lexema] = '\0';

        info->atomo = IDENTIFIER;

        return;
}

void recognizes_constchar(TInfoAtomo *info){
        info->atomo = CONSTCHAR;
        info->atributo.ch = *buffer;
        buffer++;
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
        if(lookahead.atomo == VAR){
                consome(VAR);
                variable_declaration();
                consome(SEMICOLON);
                while(lookahead.atomo == IDENTIFIER){
                        variable_declaration();
                        consome(SEMICOLON);
                }
        }
}

void variable_declaration(){
        consome(IDENTIFIER);
        while(lookahead.atomo == COMMA){
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
                        consome('');
                        break;
        }
}

void statement_part(){
        consome(BEGIN);
        statement();
        while(lookahead.atomo == SEMICOLON){
                consome(SEMICOLON);
                statement();
        }
        consome(END);
}

void statement(){
        switch(lookahead){
                case "read":
                        read_statement();
                        break;
                case "write":
                        write_statement();
                        break;
                case  "if":
                        if_statement();
                        break;
                case "while":
                        while_statement();
                        break;
                case "begin":
                        statement_part();
                        break;
                default:
                        assignment_statement();
        }
}

void assignment_statement(){
        consome(VARIABLE);
        consome(ASSINGMENT);
        expression();
}

void read_statement(){
        consome(READ);
        consome(OPEN_BRACKETS);
        consome(VARIABLE);
        while(lookahead.atomo == COLON){
                consome(colon);
                consome(variable);
        }
        consome(CLOSE_BRACKETS);
}

void write_statement(){
        consome(WRITE);
        consome(OPEN_BRACKETS);
        consome(VARIABLE);
        while(lookahead.atomo == COLON){
                consome(colon);
                consome(variable);
        }
        consome(CLOSE_BRACKETS);
}

void if_statement(){
        consome(IF);
        expression();
        consome(THEN);
        statement();
        while(lookahead.atomo == ELSE){
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
        while(lookahead.atomo == DIFFERENT){
                relational_operator();
                simple_expression();
        }
}

void relational_operator(){
        switch(lookahead){
                case "<>":
                        consome(DIFFERENT);
                        break;
                case "<":
                        consome(LESS_THAN);
                        break;
                case  "<=":
                        consome(LESS_OR_EQUAL_THAN);
                        break;
                case ">=":
                        consome(GREATER_OR_EQUAL_THAN);
                        break;
                case ">":
                        consome(GREATER_THAN);
                        break;
                case "=":
                        consome(ASSIGNEMENT);
                        break;
                case "or":
                        consome(OR);
                        break;
                case "and":
                        consome(AND);
                        break;
                default:
                        consome(AND);
                        break;
        }
}

void simple_expression(){
        term();
        while(lookahead.atomo == PLUS || lookahead.atomo == MINUS){
                adding_operator();
                term();
        }
}

void adding_operator(){
        switch(lookahead){
                case '+':
                        consome(PLUS);
                        break;
                case '-':
                        consome(MINUS);
                        break;
                default:
                        consome(PLUS);
        }
}

void term(){
        factor();
        while(lookahead.atomo == MULTIPLY || lookahead.atomo == DIVISION){
                multiplying_operator();
                factor();
        }
}

void multiplying_operator(){
        switch(lookahead){
                case '*':
                        consome(MULTIPLY);
                        break;
                case "div":
                        consome(DIVISION);
                        break;
                default:
                        consome(MULTIPLY);
        }
}

void factor(){
        switch(lookahead.atomo){
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
