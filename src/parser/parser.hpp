#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include "../lexer/lexer.hpp"
#include "../ast/ast.hpp"
using std::string;



class Parser {
        bool loud;
        int recDepth;
        vector<Lexeme> lexemes;
        int lexPos;
        Lexeme current;
        TOKENS lookahead();
        void nexttoken();
        bool match(TOKENS token);
        void enter(string s);
        void leave(string s);
        void leave();
        void say(string s);
    public:
        Parser(bool trace = false);
        ASTNode* parse(vector<Lexeme>& tokens);
    private:
        ASTNode* program();
        ASTNode* expression();
        ASTNode* statement();
        ASTNode* statementList();
        ASTNode* ifStatement();
        ASTNode* loopStatement();
        ASTNode* listStatement();
        ASTNode* printStatement();
        ASTNode* defStatement();
        ASTNode* idStatement();
        ASTNode* returnStatement();
        ASTNode* exprStatement();
        ASTNode* term();
        ASTNode* factor();
        ASTNode* var();
        ASTNode* simpleExpr();
        ASTNode* listExpr();
        ASTNode* lambdaExpr();
        ASTNode* paramList();
        ASTNode* argsList();
};

#endif