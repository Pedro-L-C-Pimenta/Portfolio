%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

void yyerror(const char *s);
extern int yylex();
//Definicao de variaveis 
extern int yylineno; // Para rastrear o número da linha
char* host;
%}

%debug

%union {
    char* str;
}

%token <str> FIELD REQ HOST
%token COLON COMMA EOL

%%

input:
    /* Entrada vazia */
    | input line
    ;

line:
    FIELD COLON values EOL          { Cria_e_add_no_campo($1);yylineno+=1;}
    | FIELD COLON EOL               { Cria_e_add_no_campo($1);yylineno+=1;}   
    | COLON values EOL              {   yyerror("Erro: Nome do campo vazio."); 
                                        zera_inicioValor(); 
                                        yylineno+=1;
                                    }
    | REQ EOL                       {   get_requisicao($1);
                                        yylineno+=1;
                                    }
    |HOST EOL                       {   host = $1;
                                        yylineno+=1;
                                    }
    | EOL                           { yylineno+=1;/* Linha vazia */ }
    ;

values:
    /* Campo de valor vazio */
    | value_list
    ;

value_list:
    FIELD                            { Cria_e_add_no_valor($1); }
    | value_list COMMA FIELD         { Cria_e_add_no_valor($3); }
    | value_list COLON FIELD         { Cria_e_add_no_valor($3); }
    ;

%%
/* Função para retornar erro */
void yyerror(const char *s) {
    fprintf(stderr, "Erro na linha %d: %s\n", yylineno, s);
}