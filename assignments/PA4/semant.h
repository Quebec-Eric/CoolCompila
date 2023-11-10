#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"
#include <map>
#include <vector>

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;
typedef SymbolTable<Symbol, Symbol> ObjectEnvironment;
typedef std::map<Class_, std::vector<method_class *>> MethodTable;


class ClassTable {
private:


public:
 int semant_errors;
  void install_basic_classes();
  ostream &error_stream;
  ObjectEnvironment objectEnv;
  MethodTable methodTable;
  std::map<Symbol, Class_> classTable;
  void check_main();
  void check_methods();
  void install_methods();
  bool conform(Symbol name1, Symbol name2);
  Class_ LCA(Symbol name1, Symbol name2);
 ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream &semant_error();
  ostream &semant_error(Class_ c);
  ostream &semant_error(Symbol filename, tree_node *t);
};

#endif
