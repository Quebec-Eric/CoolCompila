/*
 *  The scanner definition for COOL.
 */
%option noyywrap
/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

 extern  FILE *fin;/* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;


/*
*=================================== DECLARATION =====================================
*/

char string_const[MAX_STR_CONST];


int str_counter = 0;
int string_const_len;
bool  str_contain_null_char;


bool is_invalid(const char * yytext){
  int val = atoi(yytext);
  return(val<0);
}
%}





/*
*=================================== DEFINITIONS =====================================
*/

%x STRING
%x BLOCK_COMMENT

ANYTHING            (.|"\n")*
LETTER              [a-zA-Z]+
INT_CONST           [0-9]+
BOOL_CONST          ((t)(?i:rue)|(f)(?i:alse))
STR_CONST            \"{ANYTHING}\"
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




%%

\n      {curr_lineno++;}

"(*"               { BEGIN(BLOCK_COMMENT); }
<BLOCK_COMMENT>"*)"  { BEGIN(INITIAL); }  


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




\"	{
	memset(string_const, 0, sizeof string_const);
	string_const_len = 0; str_contain_null_char = false;
	BEGIN STRING;
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




<STRING>\\\n {curr_lineno++;}
<STRING>\n {
	curr_lineno++;
	strcpy(cool_yylval.error_msg, "String Not finished");
	BEGIN 0; return (ERROR);
}

<STRING>\"		{ 
	if (string_const_len > 1 && str_contain_null_char) {
		strcpy(cool_yylval.error_msg, "String contains null character");
		BEGIN 0; return (ERROR);
	}
	cool_yylval.symbol = stringtable.add_string(string_const);
	BEGIN 0; return (STR_CONST);
}


<STRING><<EOF>> {
	strcpy(cool_yylval.error_msg, "EOF in strin");
	BEGIN 0; return (ERROR);
}


<STRING>.		{ 
	if (string_const_len >= MAX_STR_CONST) {
		strcpy(cool_yylval.error_msg, "String constant too long");
		BEGIN 0; return (ERROR);
	} 
	string_const[string_const_len++] = yytext[0]; 
}


<BLOCK_COMMENT><<EOF>> { 
    strcpy(cool_yylval.error_msg, "Comentario não fechado");
    BEGIN(INITIAL); 
    return (ERROR); 
}


<BLOCK_COMMENT>.  {}


.                   { cool_yylval.error_msg = yytext;
                      return (ERROR);
                    }


%%

