#ifndef astbuilder_hpp
#define astbuilder_hpp
#include "ast.hpp"
#include "../parser/parser.hpp"
#include "../lexer/lexer.hpp"
#include <iostream>
using std::string;

class ASTBuilder {
    private:
    
    public:
        ASTBuilder() {
            
        }
        ASTNode* build(string text) {
            Lexer lexer;
            Parser parser;
            return parser.parse(lexer.lexString(text));
        }
        ASTNode* fromFile(string filename) {
            Lexer lexer;
            Parser parser;
            return parser.parse(lexer.lex(filename));
        }
};

#endif