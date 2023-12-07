//
// The following include files must come first.

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

#include "cool-io.h"
#include "tree.h"
#include "cool.h"
#include "stringtab.h"
#include <vector>
#include <map>
#define yylineno curr_lineno;
extern int yylineno;

inline Boolean copy_Boolean(Boolean b) {return b; }
inline void assert_Boolean(Boolean) {}
inline void dump_Boolean(ostream& stream, int padding, Boolean b)
	{ stream << pad(padding) << (int) b << "\n"; }

void dump_Symbol(ostream& stream, int padding, Symbol b);
void assert_Symbol(Symbol b);
Symbol copy_Symbol(Symbol b);

class Program_class;
typedef Program_class *Program;
class Class__class;
typedef Class__class *Class_;
class Feature_class;
typedef Feature_class *Feature;
class Formal_class;
typedef Formal_class *Formal;
class Expression_class;
typedef Expression_class *Expression;
class Case_class;
typedef Case_class *Case;

typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;
typedef list_node<Feature> Features_class;
typedef Features_class *Features;
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;

//--------
// Estrutura para facilitar o controle de contexto de cada 
// pedaço da geração de código.
typedef struct cgen_context
{
	Symbol	class_name;
	Symbol 	method_name;
	Class_ 	self_class_def;
	std::vector<Symbol> symbols_scope;
	std::map<Symbol, int> class_attr_offsets;
	std::map<Symbol, int> method_attr_offsets;
	std::map<Symbol, int> classtag;
	std::map<Symbol, std::map<Symbol, int>> dispatch_offsets;

	void push_scope_id(Symbol id){
		symbols_scope.push_back(id);
	}

	void pop_scope_id(){
		symbols_scope.pop_back();
	}

	int get_scope_id_offset(Symbol id){
		if(!symbols_scope.size()) return -1;

		int offset = symbols_scope.size() -1;
		while(offset >= 0 && symbols_scope[offset] != id)
			offset--;
		
		int offset_scope_start = symbols_scope.size() - 1 - offset;
		int scope_id_offset = symbols_scope[offset] == id ? offset_scope_start : -1;
		return scope_id_offset;
	}

	int get_method_attr_offset(Symbol id){
		return (
			method_attr_offsets.find(id) != method_attr_offsets.end() ? 
				method_attr_offsets[id] : -1
		);
	}

	int get_class_attr_id_offset(Symbol id){
		return (
			class_attr_offsets.find(id) != class_attr_offsets.end() ? 
				class_attr_offsets[id] : -1
		);
	}

	int get_class_method_dispatch_offset(Symbol class_name, Symbol method_name){
		return dispatch_offsets[class_name][method_name];
	}

} cgen_context;

#define Program_EXTRAS                          \
virtual void cgen(ostream&) = 0;		\
virtual void dump_with_types(ostream&, int) = 0; 



#define program_EXTRAS                          \
void cgen(ostream&);     			\
void dump_with_types(ostream&, int);            

#define Class__EXTRAS                   \
virtual Symbol get_name() = 0;  	\
virtual Symbol get_parent() = 0;    	\
virtual Symbol get_filename() = 0;      \
virtual Features get_features() = 0; \
virtual void dump_with_types(ostream&,int) = 0; 


#define class__EXTRAS                                  \
Symbol get_name()   { return name; }		       \
Symbol get_parent() { return parent; }     	       \
Symbol get_filename() { return filename; }             \
Features get_features() { return features; } \
void dump_with_types(ostream&,int);                    


#define Feature_EXTRAS                                        \
virtual bool is_method() = 0; \
virtual bool is_attr() = 0;   \
virtual void dump_with_types(ostream&,int) = 0; 


#define Feature_SHARED_EXTRAS                                       \
void dump_with_types(ostream&,int);    

#define method_EXTRAS                                \
	bool is_method() { return true; }                \
	bool is_attr() { return false; }                 \
	Symbol get_name() { return name; }               \
	Formals get_formals() { return formals; }        \
	Symbol get_return_type() { return return_type; } \
	Expression get_body_expr() { return expr; }

#define attr_EXTRAS                         \
	bool is_method() { return false; }      \
	bool is_attr() { return true; }         \
	Symbol get_name() { return name; }      \
	Symbol get_type() { return type_decl; } \
	Expression get_init_expr() { return init; }


#define Formal_EXTRAS                              \
virtual void dump_with_types(ostream&,int) = 0;		\
virtual Symbol get_name() = 0;

#define formal_EXTRAS                           \
void dump_with_types(ostream&,int);				\
Symbol get_name() { return name; }
 
#define Case_EXTRAS                             \
virtual void dump_with_types(ostream& ,int) = 0;


#define branch_EXTRAS                                   \
void dump_with_types(ostream& ,int);				\
Symbol type;                          \
Symbol get_name() { return name; }    \
Symbol get_type() { return type; }    \
void set_type(Symbol s) { type = s; } \
Symbol get_declaration_type() { return type_decl; }

#define Expression_EXTRAS                    \
Symbol type;                                 \
Symbol get_type() { return type; }           \
Expression set_type(Symbol s) { type = s; return this; } \
virtual void code(ostream&, cgen_context) = 0; \
virtual void dump_with_types(ostream&,int) = 0;  \
void dump_type(ostream&, int);               \
Expression_class() { type = (Symbol) NULL; }

#define Expression_SHARED_EXTRAS           \
void code(ostream&, cgen_context); 			   \
void dump_with_types(ostream&,int); 


#endif
