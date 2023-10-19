/*
 *  cool.y
 *              Parser definition for the COOL language.
 *
 */
%{
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"
#include "utilities.h"

extern char *curr_filename;

#define YYLTYPE int
#define cool_yylloc curr_lineno
extern int node_lineno;
#define YYLLOC_DEFAULT(Current, Rhs, N) Current = Rhs[1]; node_lineno = Current;
#define SET_NODELOC(Current) node_lineno = Current;


void yyerror(char *s);        /*  defined below; called for each parse error */
extern int yylex();           /*  the entry point to the lexer  */


/************************************************************************/
/*                DONT CHANGE ANYTHING IN THIS SECTION                  */

Program ast_root;	      /* the result of the parse  */
Classes parse_results;        /* for use in semantic analysis */
int omerrs = 0;               /* number of errors in lexing and parsing */
%}

/* A union of all the types that can be the result of parsing actions. */
%union {
  Boolean boolean;
  Symbol symbol;
  Program program;
  Class_ class_;
  Classes classes;
  Feature feature;
  Features features;
  Formal formal;
  Formals formals;
  Case case_;
  Cases cases;
  Expression expression;
  Expressions expressions;
  char *error_msg;
}

/* 
   Declare the terminals; a few have types for associated lexemes.
   The token ERROR is never used in the parser; thus, it is a parse
   error when the lexer returns it.

   The integer following token declaration is the numeric constant used
   to represent that token internally.  Typically, Bison generates these
   on its own, but we give explicit numbers to prevent version parity
   problems (bison 1.25 and earlier start at 258, later versions -- at
   257)
*/
%token CLASS 258 ELSE 259 FI 260 IF 261 IN 262 
%token INHERITS 263 LET 264 LOOP 265 POOL 266 THEN 267 WHILE 268
%token CASE 269 ESAC 270 OF 271 DARROW 272 NEW 273 ISVOID 274
%token <symbol>  STR_CONST 275 INT_CONST 276 
%token <boolean> BOOL_CONST 277
%token <symbol>  TYPEID 278 OBJECTID 279 
%token ASSIGN 280 NOT 281 LE 282 ERROR 283

/*  DON'T CHANGE ANYTHING ABOVE THIS LINE, OR YOUR PARSER WONT WORK       */
/**************************************************************************/
 
   /* Complete the nonterminal list below, giving a type for the semantic
      value of each non terminal. (See section 3.6 in the bison 
      documentation for details). */

/* Declare types for the grammar's non-terminals. */
%type <program> program
%type <classes> class_list
%type <class_> class

/* You will want to change the following line. */

%type <features> dummy_feature_list
%type <feature> feature
%type <formals> formal_list
%type <formal> formal
%type <cases> case_list
%type <case_> case
%type <expressions> expression_list1
%type <expressions> expression_list2
%type <expression> expression
%type <expression> let
%type <expression> optional_assign
%type <error_msg> error_msg


/* Precedence declarations go here. */
%nonassoc '<' '=' '>' LE 
%left '+' '-' 
%left '*' '/'  
%right '~'  
%right '!' 
%left '.'
%right FLAG
%right ASSIGN
%right NOT
%left ISVOID

/*
    Oq colocar no expression

    OBJECTID ASSIGN expression

    expression '.' OBJECTID '('   expression_list1  ')'  

    OBJECTID '(' expression_list1  ')'  ==vai ter que tem um self

    IF expression THEN expression ELSE expression FI == cund cond 
        
    WHILE expression LOOP expression POOL  == loop

    LET let 

    CASE expression OF case_list ESAC

    '{' expression_list2 '}' 


    NEW TYPEID

    ISVOID  expression

    expression " linha das operacoes'  expression

    '~' '!'   expression
    NOT expression

      '(' expression ')'  

    OBJECTID

    INT_CONST

    BOOL_CONST

    STR_CONST   



   */

%%
/* 
   Save the root of the abstract syntax tree in a global variable.
*/

//Inicio da ASA
program	              : class_list	{ 
                                        @$ = @1; 
                                        ast_root = program($1); // aki salva na arqvore
                                    }
                                    
                                    ;



// Lista de classes
class_list           	: class	/* single class */
                                    { 
                                        $$ = single_Classes($1);
                                        parse_results = $$;
                                    }
	                                  | class_list class	/* several classes */
		                                { 
                                      $$ = append_Classes($1,single_Classes($2)); 
                                      parse_results = $$;
                                    }
                                    | error_msg {}
	                                  ;




          
/* If no parent is specified, the class inherits from the Object class. */

// Definicao de classe
class	                : CLASS TYPEID '{' dummy_feature_list '}' ';'
		                                        {
                                                  $$ = class_($2,idtable.add_string("Object"),$4,
			                                            stringtable.add_string(curr_filename));
                                            }
	                                          | CLASS TYPEID INHERITS TYPEID '{' dummy_feature_list '}' ';'
		                                        { 
                                              $$ = class_($2,$4,$6,stringtable.add_string(curr_filename));
                                            }
                                            | error_msg {}
	                                          ;



/* Feature list may be empty, but no empty features in list. */

// listica das features
dummy_feature_list                    :/* empty */
                                      { 
                                          $$ = nil_Features();
                                      }
                                      | dummy_feature_list feature 
                                        {
					                                $$ = append_Features($1, single_Features($2));
			                                  }
			                                	;



// lista de argumentos
formal_list                           : {
					                                $$ = nil_Formals();
			                                	}
			                                	| formal {
				                                	$$ = single_Formals($1);
				                                }
			                                 	| formal_list ',' formal {
				                                	$$ = append_Formals($1, single_Formals($3));
				                                }
                                         | error_msg {}
			                                	;


// features das classes e metodos       
feature		                            : OBJECTID '(' formal_list ')' ':' TYPEID '{' expression '}' ';' {
                                        $$ = method($1, $3, $6, $8);
                                      }
                                      | OBJECTID ':' TYPEID optional_assign ';' {
                                        $$ = attr($1, $3, $4);
                                      }
                                      |error_msg {}
                                      ;



//argumentos
formal		                            : OBJECTID ':' TYPEID 
                                        {
					                                $$ = formal($1, $3);
				                                }
			                                	;


      expression_list1 : { /* empty */
                $$ = nil_Expressions();
              }
              | expression { /* single expression */
                $$ = single_Expressions($1);
              } 
              | expression_list1 ',' expression { /* several expressions */
                $$ = append_Expressions($1, single_Expressions($3));
              }
              ;


        expression_list2 : expression ';' { /* single expression */
                $$ = single_Expressions($1);
              } 
              | expression_list2 expression ';' { /* several expressions */
                $$ = append_Expressions($1, single_Expressions($2));
              }
              | error ';' { yyerrok; }
              ;


// falar o problema e error com base na derivacao da entrada
error_msg                             : ';' 
                                        {
                                         $$ = strdup("';' Erro");
                                        }
                                      | ',' 
                                          {
                                           $$ = strdup("',' Erro");
                                          }
                                      ;




//tratando o let
let	                                  : OBJECTID ':' TYPEID optional_assign IN expression
                                          {           
                                            $$ = let($1, $3, $4, $6);

                                          }
                                        | OBJECTID ':' TYPEID optional_assign ',' let
                                          {
			                                       $$ = let($1, $3, $4, $6);
		                                      }
                                        | error_msg {}
                                        ;

case_list	                             :  case
                                        {
				                                	$$ = single_Cases($1);
			                                	} 
				                                | case_list case
                                          {
				                                    $$ = append_Cases($1, single_Cases($2));
				                                  }
				                                ;


case		                              : OBJECTID ':' TYPEID DARROW expression ';'
                                        {
					                                $$ = branch($1, $3, $5);
				                                }
                                        ;



                                        
optional_assign                       : 
                                        {
                                          $$ = NULL;
                                        }
                                        | ASSIGN expression
                                          {
                                           $$ = $2;
                                          }
                                          ;


/* end of grammar */
%%

/* This function is called automatically when Bison detects a parse error. */
void yyerror(char *s)
{
  extern int curr_lineno;

  cerr << "\"" << curr_filename << "\", line " << curr_lineno << ": " \
    << s << " at or near ";
  print_cool_token(yychar);
  cerr << endl;
  omerrs++;

  if(omerrs>50) {fprintf(stdout, "More than 50 errors\n"); exit(1);}
}

