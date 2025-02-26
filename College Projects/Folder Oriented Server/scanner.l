%{
#include "parser.tab.h"
#include <string.h>

%}

%%

#.*                     { /* Comentários: ignorar */ }
^[A-Z][A-Z]+[ ]\/.*     { yylval.str = strdup(yytext); return REQ;}
^Host.*                 { yylval.str = strdup(yytext); return HOST;}
:                       { return COLON; }
,                       { return COMMA; }
[^,:\n\r]+              { yylval.str = strdup(yytext);return FIELD;}
^[\r]                   { /* Final da requisição: ignorar */ }
\n                      { return EOL; }

%%

int yywrap() {
    return 1;
}
