#include "LLParser.h"
#include <iostream>
#include <vector>
#include <map>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;


ASTNode LLParser::parseAST(const vector<Token>& inputTokens) {
    tokens = inputTokens;
    current = 0;
    ASTNode root = parseS();
    return root;
}

void LLParser::exportAST(const ASTNode& root, const string& filename) {
    ofstream file(filename);
    file << root.toJson().dump(4);
    file.close();
    //cout << "AST written to " << filename << endl;
}

ASTNode LLParser::makeLeaf(const string& type, const string& value)
{
    return ASTNode{type, value, {}};
}
Token LLParser::peek() {
    if (current >= tokens.size()) return { TokenType::END, "" };
    return tokens[current];
}

void LLParser::advance() {
    if (current < tokens.size()) current++;
}

void LLParser::match(TokenType expected) {
    if (current >= tokens.size()) {
        errors.push_back("Unexpected end of input");
        success=false;
        return;  // ← prevent out-of-bounds access
    }
    if (tokens[current].type != expected) {
        errors.push_back("Unexpected token '" + tokens[current].lexeme + "' (expected: " + tokenTypeToString(expected) + ")");
        success=false;
    }
    advance();
}

bool LLParser::check(TokenType type) {
    if (current >= tokens.size()) return false;
    return tokens[current].type == type;
}
ASTNode LLParser::parseS() {
    ASTNode node{"S", ""};

    node.children.push_back(makeLeaf("MAIN", tokens[current].lexeme)); match(TokenType::MAIN);
    match(TokenType::LPAREN);
    match(TokenType::RPAREN);
    match(TokenType::LBRACE);

    while (!check(TokenType::RBRACE)&& current < tokens.size()) {
        node.children.push_back(parseStatement());
    }

    match(TokenType::RBRACE);
    return node;
}

ASTNode LLParser::parseStatement() {
    ASTNode stmt{"Statement", ""};

    if (check(TokenType::WHILE)) {
        stmt.children.push_back(makeLeaf("WHILE", tokens[current].lexeme)); match(TokenType::WHILE);
        match(TokenType::LPAREN);
        stmt.children.push_back(parseExpr());
        match(TokenType::RPAREN);
        match(TokenType::LBRACE);
        while (!check(TokenType::RBRACE) && current < tokens.size()) {
            stmt.children.push_back(parseStatement());
        }
        match(TokenType::RBRACE);
    }
    else if (check(TokenType::IDENTIFIER)) {
        // Check if this is a variable declaration
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::NAME) {
            stmt.children.push_back(makeLeaf("IDENTIFIER", tokens[current].lexeme)); match(TokenType::IDENTIFIER);
            stmt.children.push_back(makeLeaf("NAME", tokens[current].lexeme)); match(TokenType::NAME);
            match(TokenType::SEMICOLON);
        } else {
            // Could be an expression (e.g., function call), fallback
            stmt.children.push_back(parseExpr());
            match(TokenType::SEMICOLON);
        }
    }
    else if (check(TokenType::RETURN)) {
        stmt.children.push_back(makeLeaf("RETURN", tokens[current].lexeme)); match(TokenType::RETURN);
        match(TokenType::SEMICOLON);
    }
    else if (check(TokenType::BREAK)) {
        stmt.children.push_back(makeLeaf("BREAK", tokens[current].lexeme)); match(TokenType::BREAK);
        match(TokenType::SEMICOLON);
    }
    else if (check(TokenType::CONTINUE)) {
        stmt.children.push_back(makeLeaf("CONTINUE", tokens[current].lexeme)); match(TokenType::CONTINUE);
        match(TokenType::SEMICOLON);
    }
    else if (canStartExpr()) {
        stmt.children.push_back(parseExpr());
        match(TokenType::SEMICOLON);
    }
    else {
        success=false;
        errors.push_back("Unexpected token in statement: '" + tokens[current].lexeme + "'");
    }

    return stmt;
}





bool LLParser::canStartExpr() {
    return check(TokenType::IDENTIFIER) || check(TokenType::NAME);
}

ASTNode LLParser::parseExpr() {
    ASTNode expr{"Expr", ""};

    // Assignment case: a = ...
    if (check(TokenType::NAME)) {
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::REAL_OP &&
            tokens[current + 1].lexeme == "=") {

            ASTNode assign{"Assignment", ""};
            assign.children.push_back(makeLeaf("NAME", tokens[current].lexeme));
            match(TokenType::NAME);

            assign.children.push_back(makeLeaf("REAL_OP", tokens[current].lexeme));
            match(TokenType::REAL_OP);

            assign.children.push_back(parseExpr());  // RHS is another expression
            return assign;
        }
    }

    // Now parse additive/multiplicative expression: a + 3.5, 3 * b, etc.
    return parseSimpleExpr();
}

ASTNode LLParser::parseSimpleExpr() {
    ASTNode node{"SimpleExpr", ""};

    node.children.push_back(parsePrimary());

    while (check(TokenType::ARTH_OP) || check(TokenType::REAL_OP) || check(TokenType::LOGIC_OP)) {
        ASTNode op = makeLeaf("OP", tokens[current].lexeme);
        match(tokens[current].type);
        ASTNode right = parsePrimary();

        ASTNode bin{"BinaryExpr", ""};
        bin.children.push_back(node.children.back());  // Left operand
        bin.children.push_back(op);
        bin.children.push_back(right);

        node.children.pop_back();  // Replace last with binary
        node.children.push_back(bin);
    }

    return node;
}

ASTNode LLParser::parsePrimary() {
    if (check(TokenType::DIGIT)) {
        ASTNode node = makeLeaf("DIGIT", tokens[current].lexeme);
        match(TokenType::DIGIT);
        return node;
    }
    else if (check(TokenType::REAL)) {
        ASTNode node = makeLeaf("REAL", tokens[current].lexeme);
        match(TokenType::REAL);
        return node;
    }
    else if (check(TokenType::NAME)) {
        ASTNode node = makeLeaf("NAME", tokens[current].lexeme);
        match(TokenType::NAME);
        return node;
    }
    else if (check(TokenType::LPAREN)) {
        match(TokenType::LPAREN);
        ASTNode expr = parseExpr();
        match(TokenType::RPAREN);
        return expr;
    }
    
    ASTNode TEMP = makeLeaf("error", tokens[current].lexeme);
    errors.push_back("Expected DIGIT, REAL, NAME, or '(' in primary expression");
    success=false;
    if(current !=tokens.size())
    {
        current++;
    }
    
    return  TEMP;
}




ASTNode LLParser::parseExprArray() {
    ASTNode node{"ExprArray", ""};

    if (check(TokenType::LBRACKET)) {
        node.children.push_back(makeLeaf("LBRACKET", tokens[current].lexeme)); match(TokenType::LBRACKET);
        node.children.push_back(makeLeaf("DIGIT", tokens[current].lexeme)); match(TokenType::DIGIT);
        node.children.push_back(makeLeaf("RBRACKET", tokens[current].lexeme)); match(TokenType::RBRACKET);
        node.children.push_back(parseExpr0());
    }

    return node;  // Even if empty, return node to keep tree structure consistent
}
ASTNode LLParser::parseExpr3() {
    ASTNode node{"Expr3", ""};

    while (check(TokenType::NAME)) {
        node.children.push_back(makeLeaf("NAME", tokens[current].lexeme)); match(TokenType::NAME);
    }

    if (check(TokenType::DIGIT)) {
        node.children.push_back(parseExpr1());
    }
    else if (check(TokenType::REAL_OP) || check(TokenType::LOGIC_OP) || check(TokenType::ARTH_OP)) {
        node.children.push_back(parseExpr0());
    }

    return node;
}



ASTNode LLParser::parseExpr0() {
    ASTNode node{"Expr0", ""};

    if (check(TokenType::REAL_OP)) {
        node.children.push_back(makeLeaf("REAL_OP", tokens[current].lexeme)); match(TokenType::REAL_OP);
    }
    else if (check(TokenType::LOGIC_OP)) {
        node.children.push_back(makeLeaf("LOGIC_OP", tokens[current].lexeme)); match(TokenType::LOGIC_OP);
    }
    else if (check(TokenType::ARTH_OP)) {
        node.children.push_back(makeLeaf("ARTH_OP", tokens[current].lexeme)); match(TokenType::ARTH_OP);
    }
    else {
        throw std::runtime_error("Expected operator in Expr0");
    }

    if (check(TokenType::NAME)) {
        node.children.push_back(parseExpr3());
    }
    else if (check(TokenType::DIGIT) || check(TokenType::REAL)) {
        node.children.push_back(parseExpr1());
    }
    else {
        success=false;
        errors.push_back("Expected expression after operator");
    }

    return node;
}


ASTNode LLParser::parseExpr1() {
    ASTNode node{"Expr1", ""};

    if (check(TokenType::DIGIT)) {
        node.children.push_back(makeLeaf("DIGIT", tokens[current].lexeme));
        match(TokenType::DIGIT);
    }
    else if (check(TokenType::REAL)) {
        node.children.push_back(makeLeaf("REAL", tokens[current].lexeme));
        match(TokenType::REAL);
    }
    else if (check(TokenType::NAME)) {
        node.children.push_back(makeLeaf("NAME", tokens[current].lexeme));
        match(TokenType::NAME);
    }
    else {
        errors.push_back("Expected DIGIT, REAL, or NAME in Expr1 (got '" + tokens[current].lexeme + "')");
        success=false;
    }

    // Optionally handle operator and RHS
    if (check(TokenType::ARTH_OP) || check(TokenType::REAL_OP) || check(TokenType::LOGIC_OP)) {
        node.children.push_back(parseExpr0());
    }

    return node;
}






