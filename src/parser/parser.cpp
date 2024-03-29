#include "parser.hpp"

TOKENS Parser::lookahead() {
    return current.tokenVal;
}

void Parser::nexttoken() {
    if (lexPos+1 == lexemes.size()) {
        current = Lexeme(EOFTOKEN, "<fin>", lexPos);
    } else {
        current = lexemes[++lexPos];
    }
}

bool Parser::match(TOKENS token) {
    if (token == current.tokenVal) {
        nexttoken();
        return true;
    }
    if (current.tokenVal != EOFTOKEN) {
        cout<<"mismatched token on line "<<current.lineNumber<<": "<<tokenString[current.tokenVal]<<",expected: "<<tokenString[token]<<endl;
    }
    return false;
}

Parser::Parser(bool trace) {
    loud = trace;
    recDepth = 0;
}

ASTNode* Parser::parse(vector<Lexeme>& tokens) {
    lexemes = tokens;
    current = tokens[0];
    lexPos = 0;
    return program();
}

ASTNode* Parser::program() {
    return statementList();
}

ASTNode* Parser::statementList() {
    enter("statement list");
    ASTNode* node = statement();
    ASTNode* m = node;
    while (lookahead() != RCURLY && lookahead() != EOFTOKEN) {
        ASTNode* t = statement();
        if (m == nullptr) {
            node = m = t;
            node->next = m;
        } else {
            m->next = t;
            m = t;
        }
    }
    leave();
    return node;
}

ASTNode* Parser::paramList() {
    return argsList();
}

ASTNode* Parser::argsList() {
    enter("args list");
    ASTNode* node;
    if (lookahead() == RPAREN) {
        leave("no arguments.");
        return node;
    } 
    ASTNode d;
    ASTNode* c = &d;
    int argCount = 0;
    do {
        c->left = expression();
        c = c->left;
        argCount++;
        if (lookahead() == COMA)
            match(COMA);
        else break;
    } while(lookahead() != RPAREN);
    node = d.left;
    leave(to_string(argCount) + " arguments.");
    return node;
}

ASTNode* Parser::ifStatement() {
    enter("if statement");
    ASTNode* node = makeStmtNode(IF_STMT, lookahead(), current.stringVal);
    match(IF);
    match(LPAREN);
    node->left = simpleExpr();
    match(RPAREN);
    match(LCURLY);
    node->mid = statementList();
    if (lookahead() == RCURLY)
        match(RCURLY);
    if (lookahead() == ELSE) {
        match(ELSE);
        node->right = statementList();
        if (lookahead() == RCURLY)
            match(RCURLY);
    } else cout<<"Hey, who ate my closing brace?"<<endl;
    leave();
    return node;
}

ASTNode* Parser::loopStatement() {
    enter("loop");
    ASTNode* node = makeStmtNode(LOOP_STMT, lookahead(), current.stringVal);
    match (LOOP);
    match(LPAREN);
    node->left = simpleExpr();
    match(RPAREN);
    match(LCURLY);
    node->right = program();
    match(RCURLY);
    leave();
    return node;
}

ASTNode* Parser::listStatement() {
    enter("list statement");
    ASTNode* node;
    if (lookahead() == PUSH) {
        node = makeStmtNode(PUSH_STMT, lookahead(), current.stringVal);
        match(PUSH);
        match(LPAREN);
        node->left = simpleExpr();
        match(COMA);
        node->right = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        leave("push");
        return node;
    }
    if (lookahead() == APPEND) {
        node = makeStmtNode(APPEND_STMT, lookahead(), current.stringVal);
        match(APPEND);
        match(LPAREN);
        node->left = simpleExpr();
        match(COMA);
        node->right = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        leave("append");
        return node;
    }
    if (lookahead() == POP) {
        ASTNode* node = makeStmtNode(POP_STMT, lookahead(), current.stringVal);
        match(POP);
        match(LPAREN);
        node->left = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        leave("pop");
        return node;
    }
    leave();
    return nullptr;
}

ASTNode* Parser::printStatement() {
    enter("print");
    ASTNode* node = makeStmtNode(PRINT_STMT, lookahead(), current.stringVal);
    match(PRINT);
    node->left = expression();
    if (lookahead() == SEMI)
        match(SEMI);
    leave();
    return node;
}

ASTNode* Parser::defStatement() {
    enter("procedure definition");
    ASTNode* node = makeStmtNode(DEF_STMT, lookahead(), current.stringVal);
    match(DEF);
    node->data.stringVal = current.stringVal;
    match(ID);
    match(LPAREN);
    node->left = paramList();
    match(RPAREN);
    match(LCURLY);
    node->right = program();
    match(RCURLY);
    leave();
    return node;
}

ASTNode* Parser::idStatement() {
    enter("id statement");
    ASTNode* node = makeExprNode(ID_EXPR, lookahead(), current.stringVal);
    match(ID);
    if (lookahead() == ASSIGN) {
        ASTNode* t = makeStmtNode(ASSIGN_STMT, lookahead(), current.stringVal);
        t->left = node;
        node = t;
        match(ASSIGN);
        node->right = simpleExpr();
        if (lookahead() == SEMI)
            match(SEMI);
        leave();
        return node;
    } else {
        leave();
        return var();
    }
    leave();
    return node;
}

ASTNode* Parser::returnStatement() {
    enter("return statement");
    ASTNode*  node = makeStmtNode(RETURN_STMT, lookahead(), current.stringVal);
    match(RETURN);
    node->left = simpleExpr();
    if (lookahead() == SEMI)
        match(SEMI);
    leave();
    return node;
}

ASTNode* Parser::exprStatement() {
    enter("expr statement");
    ASTNode* node = simpleExpr();
    if (lookahead() == SEMI)
        match(SEMI);
    leave();
    return node;
}

ASTNode* Parser::statement() {
    ASTNode* node;
    if (lookahead() == PRINT) {
        return printStatement();
    }
    if  (lookahead() == IF) {
        return ifStatement();
    }
    if (lookahead() == LOOP) {
        return loopStatement();
    }
    if (lookahead() == PUSH || lookahead() == APPEND || lookahead() == POP) {
        return listStatement();
    }
    if (lookahead() == DEF) {
        return defStatement();
    }
    if (lookahead() == ID) {
        return idStatement();
    }
    if (lookahead() == LPAREN) {
        return exprStatement();
    }
    if (lookahead() == RETURN) {
        return returnStatement();
    }
    cout<<"Unknown Token on Line: "<< current.lineNumber<<": "<<current.stringVal<<endl;
    nexttoken();
    return node;
}

ASTNode* Parser::simpleExpr() {
    enter("simple expr");
    ASTNode* node = expression();
    if (lookahead() == EQUAL || lookahead() == LESS || lookahead() == NOTEQUAL || lookahead() == GREATER) {
        ASTNode* t = makeExprNode(OP_EXPR, lookahead(), current.stringVal);
        t->left = node;
        match(lookahead());
        node = t;
        node->right = expression();
    }
    leave();
    return node;
}

ASTNode* Parser::expression() {
    enter("expr");
    ASTNode* node = term();
    while (lookahead() == PLUS || lookahead() == MINUS) {
        ASTNode* expNode = makeExprNode(OP_EXPR, lookahead(), current.stringVal);
        expNode->left = node;
        node = expNode;
        match(lookahead());
        node->right = term();
    }
    leave();
    return node;
}

ASTNode* Parser::term() {
    enter("term");
    ASTNode* node = factor();
    while (lookahead() == MULTIPLY || lookahead() == DIVIDE) {
        ASTNode* expNode = makeExprNode(OP_EXPR, lookahead(), current.stringVal);
        expNode->left = node;
        node = expNode;
        match(lookahead());
        node->right = factor();
    }
    leave();
    return node;
}

ASTNode* Parser::factor() {
    ASTNode* node;
    enter("factor");
    if (lookahead() == NUMBER) {
        node = makeExprNode(CONST_EXPR, lookahead(), current.stringVal);
        match(NUMBER);
        leave("number");
        return node;
    }
    if (lookahead() == QUOTE) {
        match(QUOTE);
        node = makeExprNode(STRINGLIT_EXPR, lookahead(), current.stringVal);
        match(STRING);
        match(QUOTE);
        leave("string");
        return node;
    }
    if (lookahead() == ID) {
        leave("id");
        return var();
    }
    if (lookahead() == LPAREN) {
        match(LPAREN);
        node = simpleExpr();
        match(RPAREN);
    }
    if (lookahead() == LAMBDA) {
        leave("lambda");
        return lambdaExpr();
    }
    if (lookahead() == LSQ || lookahead() == LENGTH || lookahead() == SORT || lookahead() == POP)
        return listExpr();
    leave();
    return node;
}

ASTNode* Parser::var() {
    enter("var");
    ASTNode* node = nullptr;
    if (lookahead() == ID) {
        node = makeExprNode(ID_EXPR, lookahead(), current.stringVal);
        match(ID);
        if (lookahead() == LSQ) {
            match(LSQ);
            node->left = simpleExpr();
            match(RSQ);
            leave("list access");
            return node;
        }
        if (lookahead() == LPAREN) {
            match(LPAREN);
            node->type.expr = FUNC_EXPR;
            node->left = argsList();
            match(RPAREN);
            leave("procedure call");
            return node;
        }
    }
    return node;
}

ASTNode* Parser::listExpr() {
    if (lookahead() == LSQ) {
        ASTNode* node = makeExprNode(LIST_EXPR, lookahead(), current.stringVal);
        match(LSQ);
        if (lookahead() == RSQ) {
            match(RSQ);
            return node;
        } else {
            ASTNode d;
            ASTNode* c = &d;
            do {
                c->left = expression();
                c = c->left;
                if (lookahead() == COMA)
                    match(COMA);
            } while(lookahead() != RSQ);
            match(RSQ);
            node->left = d.left;
            return node;
        }
    }
    if (lookahead() == LENGTH) {
        ASTNode* node = makeExprNode(LISTLEN_EXPR, lookahead(), current.stringVal);
        match(LENGTH);
        match(LPAREN);
        node->left = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        return node;
    }
    if (lookahead() == SORT) {
        ASTNode* node = makeExprNode(SORT_EXPR, lookahead(), current.stringVal);
        match(SORT);
        match(LPAREN);
        node->left = simpleExpr();
        match(RPAREN);
        if (lookahead() == SEMI)
            match(SEMI);
        return node;
    }
    return nullptr;
}

ASTNode* Parser::lambdaExpr() {
    enter("lambda");
    ASTNode* node = makeExprNode(LAMBDA_EXPR, lookahead(), current.stringVal);
    match(LAMBDA);
    match(LPAREN);
    if (lookahead() != RPAREN)
        node->left = simpleExpr();
    match(RPAREN);
    match(LCURLY);
    node->right = statementList();
    if (lookahead() == RCURLY)
        match(RCURLY);
    leave();
    return node;
}

void Parser::enter(string s) {
    recDepth++;
    say(s);
}

void Parser::say(string s) {
    if (loud) {
        for (int i = 0; i < recDepth; i++)
            cout<<"  ";
        cout<<"("<<recDepth<<") "<<s<<endl;
    }
}

void Parser::leave(string s) {
    say(s);
    recDepth--;
}

void Parser::leave() {
    --recDepth;
}