#pragma once
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>
#include <string>
#include <fstream>
#include "LLParser.h"

using namespace std;

class Tokenizer {
    public:
    vector<string> tokenize(const string& filePath);
    
    TokenType determineTokenType(const string& lexeme) {
        // Keywords
        if (lexeme == "main") return TokenType::MAIN;
        else if (lexeme == "while") return TokenType::WHILE;
        else if (lexeme == "return") return TokenType::RETURN;
        else if (lexeme == "break") return TokenType::BREAK;
        else if (lexeme == "continue") return TokenType::CONTINUE;
        
        // Types/identifiers
        else if (lexeme == "int" || lexeme == "float" || lexeme == "bool" ||
                 lexeme == "double" || lexeme == "char" || lexeme == "string") {
            return TokenType::IDENTIFIER;
        }
        else if(lexeme == "Int" || lexeme == "Float" || lexeme == "Bool" ||
                lexeme == "Double" || lexeme == "Char" || lexeme == "String")
        {
            return  TokenType:: NONVaild;
        }
        
        // Literals
        else if (isdigit(lexeme[0])) {
            for (int i = 0; i < lexeme.size(); i++) {
                if (isdigit(lexeme[i])&& lexeme[i] !='.') {
                    continue;
                }
                else{
                if (lexeme[i] == '.') {
                    
                    return TokenType::REAL;
                }
                    else
                    {
                        return TokenType::NONVaild;
                        
                    }
                }
            }
            return  TokenType::DIGIT;
        }
        else if (isalpha(lexeme[0])){
            for(int i=0; i<lexeme.size();i++)
            {
                
                if (!isalpha(lexeme[i]) && !isdigit(lexeme[i]) && lexeme[i] != '_')
                {
                    return TokenType::NONVaild;
                }
                
                return TokenType::NAME;
            }
        }
        
        // Operators
        else if (lexeme == "=" || lexeme == ">" || lexeme == "<" || lexeme == "<=" || lexeme == ">=" || lexeme == "==" || lexeme == "!=") {
            return TokenType::REAL_OP;
        }
        else if (lexeme == "&&" || lexeme == "||" || lexeme == "!") {
            return TokenType::LOGIC_OP;
        }
        else if (lexeme == "+" || lexeme == "-" || lexeme == "*" || lexeme == "/") {
            return TokenType::ARTH_OP;
        }
        
        // Punctuation
        else if (lexeme == "(") return TokenType::LPAREN;
        else if (lexeme == ")") return TokenType::RPAREN;
        else if (lexeme == "{") return TokenType::LBRACE;
        else if (lexeme == "}") return TokenType::RBRACE;
        else if (lexeme == "[") return TokenType::LBRACKET;
        else  if (lexeme == "]") return TokenType::RBRACKET;
        else  if (lexeme == ";") return TokenType::SEMICOLON;
        
        // Default case for variable names
         return TokenType::NONVaild;
        
    }
    
    int get_line()
    {
        return linecounter;
    }


private:
    bool MLC = false;// Multi-line comment flag
    int linecounter=0;
};

#endif // TOKENIZER_H
