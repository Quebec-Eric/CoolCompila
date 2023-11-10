#include "semant.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>

#include "utilities.h"

extern int semant_debug;
extern char *curr_filename;
Class_ cls;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void) {
    arg = idtable.add_string("arg");
    arg2 = idtable.add_string("arg2");
    Bool = idtable.add_string("Bool");
    concat = idtable.add_string("concat");
    cool_abort = idtable.add_string("abort");
    copy = idtable.add_string("copy");
    Int = idtable.add_string("Int");
    in_int = idtable.add_string("in_int");
    in_string = idtable.add_string("in_string");
    IO = idtable.add_string("IO");
    length = idtable.add_string("length");
    Main = idtable.add_string("Main");
    main_meth = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any
    //   user-defined class.
    No_class = idtable.add_string("_no_class");
    No_type = idtable.add_string("_no_type");
    Object = idtable.add_string("Object");
    out_int = idtable.add_string("out_int");
    out_string = idtable.add_string("out_string");
    prim_slot = idtable.add_string("_prim_slot");
    self = idtable.add_string("self");
    SELF_TYPE = idtable.add_string("SELF_TYPE");
    Str = idtable.add_string("String");
    str_field = idtable.add_string("_str_field");
    substr = idtable.add_string("substr");
    type_name = idtable.add_string("type_name");
    val = idtable.add_string("_val");
}

ClassTable::ClassTable(Classes classes) {
     /* Fill this in */
}

void ClassTable::install_basic_classes() {
    // The tree package uses these globals to annotate the classes built below.
    // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.

    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    //
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
        class_(Object,
               No_class,
               append_Features(
                   append_Features(
                       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                   single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
               filename);

    //
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class =
        class_(IO,
               Object,
               append_Features(
                   append_Features(
                       append_Features(
                           single_Features(method(out_string, single_Formals(formal(arg, Str)),
                                                  SELF_TYPE, no_expr())),
                           single_Features(method(out_int, single_Formals(formal(arg, Int)),
                                                  SELF_TYPE, no_expr()))),
                       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
                   single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
               filename);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer.
    //
    Class_ Int_class =
        class_(Int,
               Object,
               single_Features(attr(val, prim_slot, no_expr())),
               filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
        class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())), filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //
    Class_ Str_class =
        class_(Str,
               Object,
               append_Features(
                   append_Features(
                       append_Features(
                           append_Features(
                               single_Features(attr(val, Int, no_expr())),
                               single_Features(attr(str_field, prim_slot, no_expr()))),
                           single_Features(method(length, nil_Formals(), Int, no_expr()))),
                       single_Features(method(concat,
                                              single_Formals(formal(arg, Str)),
                                              Str,
                                              no_expr()))),
                   single_Features(method(substr,
                                          append_Formals(single_Formals(formal(arg, Int)),
                                                         single_Formals(formal(arg2, Int))),
                                          Str,
                                          no_expr()))),
               filename);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

void ClassTable::install_classes(Classes classes, ClassTable *classtable) {
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ cls = classes->nth(i);
        Symbol name = cls->get_name();
        if (classtable->classTable.count(name)) {
            semant_error(cls) << "Class " << name << " ja foi definida.\n";
        } else {
            classtable->classTable[name] = cls;
        }
    }
}

void ClassTable::check_inheritance(ClassTable *classtable) {
    for (auto it = classtable->classTable.begin(); it != classtable->classTable.end(); ++it) {
        cls = it->second;
        Symbol parent_name = cls->get_parent_name();
        if (!classtable->classTable.count(parent_name)) {
            semant_error(cls) << "Class " << cls->get_name() << " Esta herdando uma classe nao definida " << parent_name << ".\n";
        } else {
            Symbol current_name = parent_name;
            while (current_name != No_class) {
                if (current_name == cls->get_name()) {
                    semant_error(cls) << "Clico de geranca detectado usando classes" << cls->get_name() << ".\n";
                    break;
                }
                current_name = classtable->classTable[current_name]->get_parent_name();
            }
        }
    }
}

void ClassTable::check_main(ClassTable *classtable) {
    if (classtable->classTable.count(Main) == 0) {
        semant_error() << "Classe principal nao foi definida.\n";
    } else {
        Class_ main_class = classtable->classTable[Main];
        bool main_method_found = false;
        Features features = main_class->getFeatures();
        for (int i = features->first(); features->more(i); i = features->next(i)) {
            Feature feature = features->nth(i);
            if (feature->is_method() && static_cast<method_class *>(feature)->get_name() == main_meth) {
                Formals formals = static_cast<method_class *>(feature)->get_formals();
                if (formals->len() == 0) {
                    main_method_found = true;
                    break;
                } else {
                    semant_error(main_class) << "os metodos da main nao tem parametro\n";
                }
            }
        }
        if (!main_method_found) {
            semant_error(main_class) << "Nao tem o metodo mai\n";
        }
    }
}

std::vector<Class_> getInheritanceChain(Class_ c, std::map<Symbol, Class_> *classTable) {
    std::vector<Class_> chain;
    while (c->get_name() != Object) {
        chain.push_back(c);
        if (classTable->find(c->get_parent_name()) == classTable->end())
            throw "invalid inheritance chain.\n";
        else
            c = (*classTable)[c->get_parent_name()];
    }
    chain.push_back((*classTable)[Object]);
    return chain;
}

std::vector<Class_> getInheritanceChain(Symbol name, std::map<Symbol, Class_> *classTable) {
    if (name == SELF_TYPE)
        name = cls->get_name();
    if (classTable->find(name) == classTable->end())
        throw "not found in class table.\n";
    return getInheritanceChain((*classTable)[name], classTable);
}

method_class *getMethod(Class_ c, Symbol method_name, MethodTable methodTable) {
    std::vector<method_class *> methods = methodTable[c];
    for (size_t i = 0; i < methods.size(); i++)
        if (methods[i]->get_name() == method_name)
            return methods[i];
    return 0;
}

static bool conform(Symbol name1, Symbol name2, std::map<Symbol, Class_> *classTable) {
    if (name1 == SELF_TYPE && name2 == SELF_TYPE)
        return true;
    if (name1 != SELF_TYPE && name2 == SELF_TYPE)
        return false;
    if (name1 == SELF_TYPE)
        name1 = cls->get_name();

    if (classTable->find(name1) == classTable->end()) {
        std::string serror = "not found in class table.\n";
        throw serror;
    }

    if (classTable->find(name2) == classTable->end()) {
        std::string serror = "not found in class table.\n";
        throw serror;
    }

    Class_ c1 = (*classTable)[name1];
    Class_ c2 = (*classTable)[name2];
    std::vector<Class_> chain = getInheritanceChain(c1, classTable);

    for (size_t i = 0; i < chain.size(); i++)
        if (chain[i] == c2)
            return true;

    return false;
}

Class_ ClassTable::LCA(Symbol name1, Symbol name2, std::map<Symbol, Class_> *classTable) {
    std::vector<Class_> chain1 = getInheritanceChain(name1, classTable);
    std::vector<Class_> chain2 = getInheritanceChain(name2, classTable);

    std::reverse(chain1.begin(), chain1.end());
    std::reverse(chain2.begin(), chain2.end());

    size_t i;
    for (i = 1; i < std::min(chain1.size(), chain2.size()); i++)
        if (chain1[i] != chain2[i])
            return chain1[i - 1];

    return chain1[i - 1];
}

void ClassTable::install_methods(std::map<Symbol, Class_> *classtable) {
    for (std::map<Symbol, Class_>::iterator it = classTable.begin(); it != classTable.end(); it++) {
        Features features = it->second->getFeatures();
        std::vector<method_class *> methods;
        for (int i = features->first(); features->more(i); i = features->next(i))
            if (features->nth(i)->is_method()) {
                method_class *method = static_cast<method_class *>(features->nth(i));

                bool existed = false;
                for (size_t j = 0; j < methods.size(); j++)
                    if (methods[j]->get_name() == method->get_name())
                        existed = true;

                if (existed) {
                    std::string error_s = "Method is multiply defined.\n";
                    throw error_s;
                } else
                    methods.push_back(static_cast<method_class *>(features->nth(i)));
            }
        methodTable[it->second] = methods;
    }
}

void ClassTable::check_methods(std::map<Symbol, Class_> *classtable, MethodTable *methodTable) {
    for (std::map<Symbol, Class_>::iterator it = classTable.begin(); it != classTable.end(); it++) {
        if (it->first == Object || it->first == IO || it->first == Int || it->first == Bool || it->first == Str) continue;
        Symbol class_name = it->first;
        cls = it->second;

        std::vector<Class_> chain = getInheritanceChain(cls, &classTable);
        chain.push_back(cls);
        for (size_t k = 1; k < chain.size(); k++) {
            Class_ ancestor_class = chain[k];
            Features ancestor_features = ancestor_class->getFeatures();
            objectEnv.enterscope();
            for (int i = ancestor_features->first(); ancestor_features->more(i); i = ancestor_features->next(i)) {
                if (!ancestor_features->nth(i)->isAttr()) continue;
                attr_class *attr = static_cast<attr_class *>(ancestor_features->nth(i));
                objectEnv.addid(attr->getName(), new Symbol(attr->getType()));
            }
        }

        Features features = cls->getFeatures();
        for (int i = features->first(); features->more(i); i = features->next(i))
            if (features->nth(i)->is_method()) {
                method_class *curr_method = static_cast<method_class *>(features->nth(i));
                // curr_method->checkType();
                for (size_t k = 1; k < chain.size(); k++) {
                    method_class *ancestor_method = getMethod(chain[k], curr_method->get_name(), *methodTable);
                    if (!ancestor_method) continue;

                    if (curr_method->getReturnType() != ancestor_method->getReturnType()) {
                        std::string error_s = "In redefined method, return type is different from original return type.\n";
                        throw error_s;
                    }

                    Formals curr_formals = curr_method->get_formals();
                    Formals ancestor_formals = ancestor_method->get_formals();

                    int k1 = curr_formals->first(), k2 = ancestor_formals->first();
                    while (curr_formals->more(k1) && ancestor_formals->more(k2)) {
                        if (curr_formals->nth(k1)->getType() != ancestor_formals->nth(k2)->getType()) {
                            std::string error_s = "In redefined method, parameter type is different from original type.\n";
                            throw error_s;
                        }
                        k1 = curr_formals->next(k1);
                        k2 = ancestor_formals->next(k2);
                        if (curr_formals->more(k1) xor ancestor_formals->more(k2)) {
                            std::string error_s = "Incompatible number of formal parameters in redefined method.\n";
                            throw error_s;
                        }
                    }
                }
            } else {  // isAttr
                attr_class *curr_attr = static_cast<attr_class *>(features->nth(i));
                // Symbol expr_type = curr_attr->getInitExpr()->checkType();
                if (classTable.find(curr_attr->getType()) == classTable.end()) {
                    std::string error_s = "Class of attribute is undefined.\n";
                    throw error_s;
                    // } else if (classTable.find(expr_type) != classTable.end() && !conform(expr_type, curr_attr->getType(), &classTable)) {
                    //     std::string error_s = "Inferred type of initialization of attribute does not conform to declared type.\n";
                    //     throw error_s;
                }
            }

        for (size_t k = 1; k < chain.size(); k++)
            objectEnv.exitscope();
    }
}

ostream &semant_error(Class_ c) {
    return semant_error(c->get_filename(), c);
}

ostream &semant_error(Formal_class *f) {
    return semant_error();
}

ostream &semant_error(Symbol filename, tree_node *t) {
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream &semant_error() {
    semant_errors++;
    return error_stream;
}

// void method_class::checkType() {
//     objectEnv.enterscope();
//     for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
//         if (formals->nth(i)->getName() == self)
//             semant_error(formals->nth(i)) << "'self' cannot be the name of a formal parameter.\n";
//         else if (objectEnv.lookup(formals->nth(i)->getName()))
//             semant_error(formals->nth(i)) << "Formal parameter " << formals->nth(i)->getName() << " is multiply defined.\n";
//         else if (classTable.find(formals->nth(i)->getType()) == classTable.end())
//             semant_error(formals->nth(i)) << "Class " << formals->nth(i)->getType() << " of formal parameter " << formals->nth(i)->getName() << " is undefined.\n";
//         else
//             objectEnv.addid(formals->nth(i)->getName(), new Symbol(formals->nth(i)->getType()));
//     }
//     Symbol expr_type = expr->checkType();
//     if (return_type != SELF_TYPE && classTable.find(return_type) == classTable.end())
//         semant_error(this) << "Undefined return type " << return_type << " in method " << name << ".\n";
//     else if (!conform(expr_type, return_type))
//         semant_error(this) << "Inferred return type " << expr_type << " of method " << name << " does not conform to declared return type " << return_type << ".\n";
//     objectEnv.exitscope();
// }

// Symbol assign_class::checkType() {
//     Symbol rtype = expr->checkType();

//     if (objectEnv.lookup(name) == 0) {
//         semant_error(this) << "Assignment to undeclared variable " << name << ".\n";
//         type = rtype;
//         return type;
//     }

//     Symbol ltype = *objectEnv.lookup(name);

//     if (!conform(rtype, ltype)) {
//         semant_error(this) << "Type " << rtype << " of assigned expression does not conform to declared type " << ltype << " of identifier " << name << ".\n";
//         type = ltype;
//         return type;
//     }

//     type = rtype;
//     return type;
// }

// Symbol static_dispatch_class::checkType() {
//     bool error = false;
//     Symbol expr_type = expr->checkType();

//     if (this->type_name != SELF_TYPE && classTable.find(this->type_name) == classTable.end()) {
//         semant_error(this) << "Static dispatch to undefined class " << this->type_name << ".\n";
//         type = Object;
//         return type;
//     }

//     if (expr_type != SELF_TYPE && classTable.find(expr_type) == classTable.end()) {
//         type = Object;
//         return type;
//     }

//     if (!conform(expr_type, this->type_name)) {
//         error = true;
//         semant_error(this) << "Expression type " << expr_type << " does not conform to declared static dispatch type " << this->type_name << ".\n";
//     }

//     method_class *method = 0;
//     std::vector<Class_> chain = getInheritanceChain(this->type_name);
//     for (size_t i = 0; i < chain.size(); i++) {
//         Methods methods = methodTable[chain[i]];
//         for (size_t i = 0; i < methods.size(); i++)
//             if (methods[i]->getName() == name) {
//                 method = methods[i];
//                 break;
//             }
//     }

//     if (method == 0) {
//         error = true;
//         semant_error(this) << "Static dispatch to undefined method " << name << ".\n";
//     } else {
//         Formals formals = method->getFormals();
//         int k1 = actual->first(), k2 = formals->first();
//         while (actual->more(k1) && formals->more(k2)) {
//             Symbol actual_type = actual->nth(k1)->checkType();
//             Symbol formal_type = formals->nth(k2)->getType();
//             if (!conform(actual_type, formal_type)) {
//                 error = true;
//                 semant_error(this) << "In call of method " << name << ", type " << actual_type << " of parameter " << formals->nth(k2)->getName() << " does not conform to declared type " << formal_type << ".\n";
//             }
//             k1 = actual->next(k1);
//             k2 = formals->next(k2);
//             if (actual->more(k1) xor formals->more(k2)) {
//                 error = true;
//                 semant_error(this) << "Method " << name << " called with wrong number of arguments.\n";
//             }
//         }
//     }

//     if (error) {
//         type = Object;
//     } else {
//         type = method->getReturnType();
//         if (type == SELF_TYPE)
//             type = this->type_name;
//     }

//     return type;
// }

// Symbol dispatch_class::checkType() {
//     bool error = false;
//     Symbol expr_type = expr->checkType();

//     if (expr_type != SELF_TYPE && classTable.find(expr_type) == classTable.end()) {
//         semant_error(this) << "Dispatch on undefined class " << expr_type << ".\n";
//         type = Object;
//         return type;
//     }

//     method_class *method = 0;
//     std::vector<Class_> chain = getInheritanceChain(expr_type);
//     for (size_t i = 0; i < chain.size(); i++) {
//         Methods methods = methodTable[chain[i]];
//         for (size_t i = 0; i < methods.size(); i++)
//             if (methods[i]->getName() == name) {
//                 method = methods[i];
//                 break;
//             }
//     }

//     if (method == 0) {
//         error = true;
//         semant_error(this) << "Dispatch to undefined method " << name << ".\n";
//     } else {
//         Formals formals = method->getFormals();
//         int k1 = actual->first(), k2 = formals->first();
//         while (actual->more(k1) && formals->more(k2)) {
//             Symbol actual_type = actual->nth(k1)->checkType();
//             Symbol formal_type = formals->nth(k2)->getType();
//             if (!conform(actual_type, formal_type)) {
//                 error = true;
//                 semant_error(this) << "In call of method " << name << ", type " << actual_type << " of parameter " << formals->nth(k2)->getName() << " does not conform to declared type " << formal_type << ".\n";
//             }
//             k1 = actual->next(k1);
//             k2 = formals->next(k2);
//             if (actual->more(k1) xor formals->more(k2)) {
//                 error = true;
//                 semant_error(this) << "Method " << name << " called with wrong number of arguments.\n";
//             }
//         }
//     }

//     if (error) {
//         type = Object;
//     } else {
//         type = method->getReturnType();
//         if (type == SELF_TYPE)
//             type = expr_type;
//     }

//     return type;
// }

// Symbol cond_class::checkType() {
//     if (pred->checkType() != Bool)
//         semant_error(this) << "Predicate of 'if' does not have type Bool.\n";

//     Symbol then_type = then_exp->checkType();
//     Symbol else_type = else_exp->checkType();

//     if (then_type == SELF_TYPE && else_type == SELF_TYPE)
//         type = SELF_TYPE;
//     else
//         type = LCA(then_type, else_type)->getName();
//     return type;
// }

// Symbol loop_class::checkType() {
//     if (pred->checkType() != Bool)
//         semant_error(this) << "Loop condition does not have type Bool.\n";
//     body->checkType();
//     type = Object;
//     return type;
// }

// Symbol typcase_class::checkType() {
//     Symbol expr_type = expr->checkType();

//     std::set<Symbol> branch_type_decls;

//     for (int i = cases->first(); cases->more(i); i = cases->next(i)) {
//         branch_class *branch = static_cast<branch_class *>(cases->nth(i));
//         if (branch_type_decls.find(branch->get_type_decl()) != branch_type_decls.end())
//             semant_error(branch) << "Duplicate branch " << branch->get_type_decl() << " in case statement.\n";
//         else
//             branch_type_decls.insert(branch->get_type_decl());
//         Symbol branch_type = branch->checkType();
//         if (i == cases->first())
//             type = branch_type;
//         else if (type != SELF_TYPE || branch_type != SELF_TYPE)
//             type = LCA(type, branch_type)->getName();
//     }

//     return type;
// }

// Symbol branch_class::checkType() {
//     objectEnv.enterscope();
//     objectEnv.addid(name, new Symbol(type_decl));
//     type = expr->checkType();
//     objectEnv.exitscope();
//     return type;
// }

// Symbol block_class::checkType() {
//     for (int i = body->first(); body->more(i); i = body->next(i))
//         type = body->nth(i)->checkType();
//     return type;
// }

// Symbol let_class::checkType() {
//     objectEnv.enterscope();
//     objectEnv.addid(identifier, new Symbol(type_decl));

//     Symbol init_type = init->checkType();
//     if (classTable.find(type_decl) == classTable.end())
//         semant_error(this) << "Class " << type_decl << " of let-bound identifier " << identifier << " is undefined.\n";
//     else if (init_type != No_type && !conform(init_type, type_decl))
//         semant_error(this) << "Inferred type " << init_type << " of initialization of " << identifier << " does not conform to identifier's declared type " << type_decl << ".\n";

//     type = body->checkType();
//     objectEnv.exitscope();
//     return type;
// }

// Symbol plus_class::checkType() {
//     Symbol type1 = e1->checkType();
//     Symbol type2 = e2->checkType();
//     if (type1 != Int || type2 != Int)
//         semant_error(this) << "non-Int arguments: " << type1 << " + " << type2 << endl;
//     type = Int;
//     return type;
// }

// Symbol sub_class::checkType() {
//     Symbol type1 = e1->checkType();
//     Symbol type2 = e2->checkType();
//     if (type1 != Int || type2 != Int)
//         semant_error(this) << "non-Int arguments: " << type1 << " - " << type2 << endl;
//     type = Int;
//     return type;
// }

// Symbol mul_class::checkType() {
//     Symbol type1 = e1->checkType();
//     Symbol type2 = e2->checkType();
//     if (type1 != Int || type2 != Int)
//         semant_error(this) << "non-Int arguments: " << type1 << " * " << type2 << endl;
//     type = Int;
//     return type;
// }

// Symbol divide_class::checkType() {
//     Symbol type1 = e1->checkType();
//     Symbol type2 = e2->checkType();
//     if (type1 != Int || type2 != Int)
//         semant_error(this) << "non-Int arguments: " << type1 << " / " << type2 << endl;
//     type = Int;
//     return type;
// }

// Symbol neg_class::checkType() {
//     type = e1->checkType();
//     if (type != Int)
//         semant_error(this) << "Argument of '~' has type " << type << " instead of Int.\n";
//     type = Int;
//     return type;
// }

// Symbol lt_class::checkType() {
//     Symbol type1 = e1->checkType();
//     Symbol type2 = e2->checkType();
//     if (type1 != Int || type2 != Int)
//         semant_error(this) << "non-Int arguments: " << type1 << " < " << type2 << endl;
//     type = Bool;
//     return type;
// }

// Symbol eq_class::checkType() {
//     Symbol t1 = e1->checkType(), t2 = e2->checkType();
//     if ((t1 == Int || t1 == Bool || t1 == Str || t2 == Int || t2 == Bool || t2 == Str) && t1 != t2)
//         semant_error(this) << "Illegal comparison with a basic type.\n";
//     type = Bool;
//     return type;
// }

// Symbol leq_class::checkType() {
//     Symbol type1 = e1->checkType();
//     Symbol type2 = e2->checkType();
//     if (type1 != Int || type2 != Int)
//         semant_error(this) << "non-Int arguments: " << type1 << " <= " << type2 << endl;
//     type = Bool;
//     return type;
// }

// Symbol comp_class::checkType() {
//     type = e1->checkType();
//     if (type != Bool)
//         semant_error(this) << "Argument of 'not' has type " << type << " instead of Bool.\n";
//     type = Bool;
//     return type;
// }

// Symbol int_const_class::checkType() {
//     type = Int;
//     return type;
// }

// Symbol bool_const_class::checkType() {
//     type = Bool;
//     return type;
// }

// Symbol string_const_class::checkType() {
//     type = Str;
//     return type;
// }

// Symbol new__class::checkType() {
//     if (this->type_name != SELF_TYPE && classTable.find(this->type_name) == classTable.end()) {
//         semant_error(this) << "'new' used with undefined class " << this->type_name << ".\n";
//         this->type_name = Object;
//     }
//     type = this->type_name;
//     return type;
// }

// Symbol isvoid_class::checkType() {
//     e1->checkType();
//     type = Bool;
//     return type;
// }

// Symbol no_expr_class::checkType() {
//     type = No_type;
//     return type;
// }

// Symbol object_class::checkType() {
//     if (name == self) {
//         type = SELF_TYPE;
//     } else if (objectEnv.lookup(name)) {
//         type = *objectEnv.lookup(name);
//     } else {
//         semant_error(this) << "Undeclared identifier " << name << ".\n";
//         type = Object;
//     }
//     return type;
// }

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant() {
    initialize_constants();
    ClassTable *classtable = new ClassTable(classes);
    classtable->install_classes(classes, classtable);
    classtable->check_inheritance(classtable);
    classtable->check_main(classtable);
    classtable->install_methods(&(classtable->classTable));
    classtable->check_methods(&(classtable->classTable), &(classtable->methodTable));

    /* ClassTable constructor may do some semantic analysis */

    /* some semantic analysis code may go here */

    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}
