/*
 *  The scanner definition for COOL.
 */
%option noyywrap
//TESTE
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
 *  Add Your own definitions here
 */

%}

/*
 * Define names for regular expressions here.
 */

DARROW          =>

%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */
{DARROW}		{ return (DARROW); }



"{"			{printf("Abre chave");return '{'; }
"}"			{printf("fecha chave"); return '}'; }
"("			{printf("Abre parenteses"); return '('; }
")"			{printf("fecha parenteses"); return ')'; }
"~"			{printf("til"); return '~'; }
","			{printf("virgula"); return ','; }
"-"			{printf("ifem"); return '-'; }
"*"			{printf("mutiplicacao"); return '*'; }
"/"			{printf("barra"); return '/'; }
"%"			{printf("procentagem"); return '%'; }
"."			{printf("ponto"); return '.'; }
"<"			{printf("Menor"); return '<'; }
">"			{printf("Mior"); return '<'; }
"="			{printf("igual"); return '='; }
"!"			{printf("esclamacao"); return '!'; }
"@"			{printf("f"); return '@'; }
"#"			{printf("c"); return '#'; }
"$"			{printf("dinhero"); return '$'; }
";"			{printf("Ponto e virgula"); return ';'; }
":"			{printf("Dois pontos"); return ':'; }
"+"			{printf("Somar"); return '+'; }



 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */


(?i:POOL)		{ return (POOL); }
(?i:THEN)		{ return (THEN); }
(?i:WHILE)		{ return (WHILE); }
(?i:CLASS)  {return(CLASS); }
(?i:ELSE)  { return (ELSE); }
(?i:FI)     { return (FI); }
(?i:IF)     { return (IF); }
(?i:LET)    { return (LET); }
(?i:ISVOID) { return (ISVOID); }
(?i:INHERITS)	{ return (INHERITS); }
(?i:NOT)		{ return (NOT); }


 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */


%%

