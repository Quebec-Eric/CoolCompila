/*
  * --------------------------------------------
  * Arquivo bruno.txt :
  * --------------------------------------------
 */

/*
=================================== DEFINIÇÕES =====================================
*/
int str_counter = 0;

/*
=================================== DECLARAÇÕES =====================================
*/

ANYTHING            (.|"\n")*
LETTER              [a-zA-Z]+
INT_CONST           [0-9]+
BOOL_CONST          ((t)(?i:rue)|(f)(?i:alse))
STR_CONST            \"{ANYTHING}[^"]\"
OPERATOR            ("+"|"-"|"*"|"/")
COMPARISON_OP       ("<"|"=")
SPECIAL_CHAR        ({COMPARISON_OP}|"("|")"|"{"|"}"|";"|":"|"."|","|"~"|"@")
WHITE_SPACE         (" "|\f|\r|\t|\v)
TYPEID              ("SELF_TYPE"|[A-Z]({LETTER}|{INT_CONST}|"_")*)
OBJECTID            ("self"|{LETTER}({LETTER}|{INT_CONST}|"_")*)
DARROW              "=>"
LE                  "<="
ASSIGN              "<-"

CLASS               (?i:class)
INHERITS            (?i:inherits)
NEW                 (?i:new)
ELSE                (?i:else)
FI                  (?i:fi)
IF                  (?i:if)
IN                  (?i:in)
LET                 (?i:let)
THEN                (?i:then)
WHILE               (?i:while)
LOOP                (?i:loop)
POOL                (?i:pool)
CASE                (?i:case)
ESAC                (?i:esac)
OF                  (?i:of)
ISVOID              (?i:isvoid)
NOT                 (?i:not)

MULTILINE_COMMENT   \(\*{ANYTHING}\*\)
SINGLELINE_COMMENT  "--"{ANYTHING}"--"


/*
====================== REGRAS ===========================
*/
{CLASS}             return (CLASS);
{INHERITS}          return (INHERITS);  
{NEW}               return (NEW);
{ELSE}              return (ELSE);
{FI}                return (FI);
{IF}                return (IF);
{NOT}               return (NOT);
{IN}                return (IN);
{LET}               return (LET);
{THEN}              return (THEN);
{WHILE}             return (WHILE);
{LOOP}              return (LOOP);
{POOL}              return (POOL);
{CASE}              return (CASE);
{ESAC}              return (ESAC);
{OF}                return (OF);
{ISVOID}            return (ISVOID);
{DARROW}            return (DARROW);
{ASSIGN}	          return (ASSIGN);
{LE}		            return (LE);

{STR_CONST}         { string_buf_ptr = (char*) &string_buf;
                      cool_yylval.symbol = idtable.add_string(string_buf_ptr, str_counter);
                      str_counter = 0;
                      BEGIN(INITIAL);
                      return (STR_CONST);
                    }

{INT_CONST}         { cool_yylval.symbol = idtable.add_string(yytext);
                      return (INT_CONST);
                    }
                    
{SPECIAL_CHAR}      { return (int)(yytext[0]); }

{OPERATOR}          { return (int)(yytext[0]); }

{BOOL_CONST}        { cool_yylval.boolean = (yytext[0]);
                      return (BOOL_CONST);
                    }

{TYPEID}            { cool_yylval.symbol = idtable.add_string(yytext);
                      return (TYPEID);
                    }

{OBJECTID}          { cool_yylval.symbol = idtable.add_string(yytext);
                      return (OBJECTID);
                    }

{WHITE_SPACE}         {}
{MULTILINE_COMMENT}   {}
{SINGLELINE_COMMENT}  {}

.                   { cool_yylval.error_msg = yytext;
                      return (ERROR);
                    }

/*
  * --------------------------------------------
  * Arquivo arquivoEric.txt :
  * --------------------------------------------
 */

/*
 Verificar string nula
*/
if (string_const_len > 1 && str_contain_null_char) {
	strcpy(cool_yylval.error_msg, "String nula");
	BEGIN 0; return (ERROR);
}




/*
 Verificar se o comentario foi fechado
*/
"\*)" {
	strcpy(cool_yylval.error_msg, "Comentario nao fechado");
	return (ERROR);
}



/*
 Verificar se uma string estiver rodando ao fim do arquivo
*/

<STRING><<EOF>> {
	strcpy(cool_yylval.error_msg, "EOF in strin");
	BEGIN 0; return (ERROR);
}




/*
 Verificar se uma string e maior que o len esperado
*/

if (string_const_len >= MAX_STR_CONST) {
	strcpy(cool_yylval.error_msg, "String muiro grande");
	BEGIN 0; return (ERROR);
}


/*
 Verificar se uma sting foi terminada corretamente
*/

<STRING>\n {
	curr_lineno++;
	strcpy(cool_yylval.error_msg, "String nao terminada");
	BEGIN 0; return (ERROR);
}



/*
 verificar se o valor inteiro e valido
*/

[0-9]+ {
    if(is_invalid(yytext)) {
        strcpy(cool_yylval.error_msg, "Valor invalido");
        return (ERROR);
    }
}





%%
