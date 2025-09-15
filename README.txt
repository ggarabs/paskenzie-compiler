Este trabalho foi concluído completamente, abrangendo todas as regras gramaticais especificadas no arquivo fornecido.

Devido à modularização do código, esse programa requer um comando de compilação específico. Para facilitar a execução, criei um Makefile.
Para executar o programa, siga esses passos:

1) Para windows, com Makefile:
    Necessário: MinGW e Make instalados.
    Comandos:

        mingw32-make
        ./compilador.exe

2) Para windows, sem Makefile:
    Necessário: MinGW instalado.
    Comandos:

        gcc -Wall -Wno-unused-result -g -Og compilador.c globals.c lexer.c parser.c utils.c errors.c -o compilador -lm
        ./compilador.exe

3) Para linux, com Makefile:
    Necessário: Make instalado.
    Comandos:

        make
        ./compilador

        Para limpar e gerar outro executável, basta rodar:
        make clean

A arquitetura foi pensada contendo vários módulos, e abaixo eu especifico as suas funções individuais:

- compilador.c:
    Arquivo que contém a função principal, onde há a captura do nome do arquivo vindo do cmd, abre o arquivo, calcula seu comprimento, 
    aloca memória em tempo de execução para ele e dispara a função inicial program().

- errors.c:
    Nesse arquivo, eu reporto todos os possíveis erros gerados pelo compilador, incluindo erros léxicos e sintáticos.
    Os erros léxicos são tratados em duas funções, uma que trata de erros de não reconhecimento do token e outra que 
    reporta desbalanço de comentários.

    Na função report_lexical_error, visei capturar todo o token incorreto para a exibição ao usuário. Isso exigiu processamento extra, 
    onde há retrocesso do ponteiro do buffer até a origem do token.

- globals.c:
    No arquivo globals, foram incluídas as constantes e variáveis globais compartilhadas entre todos os arquivos, nela, há dois vetores 
    de mapeamento que visam traduzir um lookahead global para as suas respectivas strings e símbolos, a fim de exibir o conteúdo de cada 
    token para o usuário no terminal.

- lexer.c:
    Este é o analisador léxico. Nele foi dividido a determinação de cada token em 4 funções:
        - Reconhecimento de constantes inteiras (<= 2*1e9).
        - Reconhecimento de palavras-chave e identificadores, que nesse momento, são indistinguíveis.
        - Reconhecimento de símbolos, como operadores relacionais ou delimitadores de asserções.
        - Reconhecimento de caracteres ASCII.

    Dentro de algumas dessas funções, eu capturo os tokens não-reconhecidos e os insiro no lexema, retorno o atomo como ERRO e lanço o erro 
    léxico dentro analisador sintático, pois todo token deve ser repassado a ele na arquitetura em questão.

- parser.c:
    Este é o analisador sintático. Nele, além das regras definidas na gramática, temos a função obrigatória "consome()", onde dentro dela
    há um redirecionamento especial para a função "consome_comment()", lá eu recebo os lexemas do lexer e descarto um a um, independentemente
    do seu token, até que o fechamento do comentário seja reconhecido. Além disso, se logo depois vir outra abertura de comentário, a função é
    chamada novamente, garantindo que o lookahead prossiga com o processamento do código útil. Em algumas funções, eu substitui os longos switchcases
    e if/elses por mapeamento em vetores e for-loops, para aumentar a manutenabilidade do código e torná-lo mais limpo.

- utils.c:
    Contém funções globais com objetivos específicos cuja implementação pode ser omitida do programador sem perda da compreensão.
    Busca abstrair código repetitivo ou de nível muito baixo.

Além disso, criei uma pasta /tests com 27 testes unitários para testar o comportamento do programa para cada função específica do programa.