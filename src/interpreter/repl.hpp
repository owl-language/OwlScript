#ifndef repl_hpp
#define repl_hpp
#include <iostream>
#include "interpreter.hpp"
#include "../ast/astbuilder.hpp"
using std::string;
using std::cout;
using std::endl;


class REPL {
    private:
        ASTBuilder astBuilder;
        Interpreter interpreter;
    public:
        REPL() {

        }
        void repl() {
            cout<<"[OwlScript 0.1]"<<endl;
            string input;
            interpreter.setLoud(false);
            while (input != ".exit" && input != "quit") {
                cout<<"Owlscript> ";
                getline(cin, input);
                cout<<"   '-> "<<input<<endl;
                if (input == ".trace") {
                    interpreter.setLoud(true);
                    continue;
                }
                if (input != ".exit" && input != "quit") {
                    auto ast = astBuilder.build(input);
                    ASTTracer trace;
                    trace.traverse(ast);
                    interpreter.run(ast);
                }
            }
        }
};

#endif