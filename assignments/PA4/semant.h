#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>

#include <iostream>
#include <map>
#include <vector>

#include "cool-tree.h"
#include "list.h"
#include "stringtab.h"
#include "symtab.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;
typedef SymbolTable<Symbol, Symbol> ObjectEnvironment;
typedef std::map<Class_, std::vector<method_class *>> MethodTable;
int semant_errors = 0;
ostream &error_stream = cerr;

class ClassTable {
   private:
   public:
    void install_basic_classes();
    ObjectEnvironment objectEnv;
    MethodTable methodTable;
    std::map<Symbol, Class_> classTable;

    void install_classes(Classes, ClassTable *);
    void check_inheritance(ClassTable *);
    void check_main(ClassTable *);
    Class_ LCA(Symbol, Symbol, std::map<Symbol, Class_> *);
    void install_methods(std::map<Symbol, Class_> *);
    void check_methods(std::map<Symbol, Class_> *, MethodTable *);

    ClassTable(Classes);
    int errors() { return semant_errors; }
};

ostream &semant_error();
ostream &semant_error(Class_ c);
ostream &semant_error(Symbol filename, tree_node *t);

#endif
