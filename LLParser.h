#pragma once
#ifndef LLPARSER_H
#define LLPARSER_H
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <map>
#include "json.hpp"// You need to add this header from
#include <fstream>

using namespace std;
using json = nlohmann::json;

enum class TokenType {
    MAIN,
    WHILE,
    RETURN,
    BREAK,
    CONTINUE,
    IDENTIFIER, // int, float, bool, double, char, string
    NAME,       // A-Z, a-z, _
    DIGIT,      // 0-9
    REAL_OP,    // =, >, <, <=, >=, +=, *=, -=, /=, !=, ==
    LOGIC_OP,   // &&, ||, !
    ARTH_OP,    // +, -, *, /
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    LBRACKET,   // [
    RBRACKET,   // ]
    SEMICOLON,  // ;
    NONVaild,
    REAL,
    END
    
};

struct Token {
    TokenType type;
    string lexeme;
};

struct ASTNode {
    string type;
    string value;
    string annotation;
    vector<ASTNode> children;

    json toJson() const {
        json j;
        j["type"] = type;
        j["value"] = value;
        j["children"] = json::array();
        if (!annotation.empty()) {
                j["annotation"] = annotation;  
            }
        for (const auto& child : children) {
            j["children"].push_back(child.toJson());
        }
        return j;
    }
};


class LLParser {
public:
  
    bool is_success()
    {
        return success;
    }
    vector<string> get_errors()
    {
        return errors;
    }
    ASTNode parseAST(const vector<Token>& inputTokens);  // Entry point for AST parsing
    void exportAST(const ASTNode& root, const string& filename = "ast.json");

   
    ASTNode makeLeaf(const string& type, const string& value);

    string tokenTypeToString(TokenType type) {
       switch (type) {
           case TokenType::MAIN: return "MAIN";
           case TokenType::WHILE: return "WHILE";
           case TokenType::RETURN: return "RETURN";
           case TokenType::BREAK: return "BREAK";
           case TokenType::CONTINUE: return "CONTINUE";
           case TokenType::IDENTIFIER: return "IDENTIFIER";
           case TokenType::NAME: return "NAME";
           case TokenType::DIGIT: return "DIGIT";
           case TokenType::REAL_OP: return "REAL_OP";
           case TokenType::LOGIC_OP: return "LOGIC_OP";
           case TokenType::ARTH_OP: return "ARTH_OP";
           case TokenType::LPAREN: return "LPAREN";
           case TokenType::RPAREN: return "RPAREN";
           case TokenType::LBRACE: return "LBRACE";
           case TokenType::RBRACE: return "RBRACE";
           case TokenType::LBRACKET: return "LBRACKET";
           case TokenType::RBRACKET: return "RBRACKET";
           case TokenType::SEMICOLON: return "SEMICOLON";
           case TokenType::END: return "END";
           case TokenType::NONVaild: return "NONVaild";
           case TokenType::REAL: return "Real";
           default: return "Unknown";
       }
   }
private:
    bool success=true;
    vector<string> errors;
    vector<Token> tokens;
    size_t current = 0;
    Token peek();
    void advance();
    void match(TokenType expected);
    bool check(TokenType type);
    ASTNode parseS();
    ASTNode parseStatement();
    bool canStartExpr();
    ASTNode parseExpr();
    ASTNode parseSimpleExpr();
    ASTNode parsePrimary();
    ASTNode parseExprArray();
    ASTNode parseExpr3();
    ASTNode parseExpr0();
    ASTNode parseExpr1();
};

#endif // LLPARSER_H
