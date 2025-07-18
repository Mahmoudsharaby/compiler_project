//
//   semantic.h
//  System_project
//
//  Created by Mahmoud Hany on 08/05/2025.
//

#ifndef _semantic_h
#define _semantic_h
#include <iostream>
#include <map>
#include <vector>
#include "LLParser.h"
#include "Tokenizer.h"
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
class semantic
{
private:
    map<string, string> symboltable;
    vector<string> errors;
    bool success=true;
public:
  
    void build_table(vector<Token> data)
    {
        for (size_t i = 0; i + 1 < data.size(); i++) {
            if (data[i].type == TokenType :: IDENTIFIER && data[i+1].type==TokenType::NAME) {
                string varType = data[i].lexeme;
                string varName = data[i + 1].lexeme;
                
                if (symboltable.find(varName) != symboltable.end()) {
                    errors.push_back("Error: double declaration for" + varName);
                    success=false;
                } else {
                    symboltable[varName] = varType;
                }
            }
        }
    }
    void check_variable_usage(vector<Token> tokens) {
        for (Token token : tokens) {
            if (token.type == TokenType::NAME) { // Variable usage
                if (symboltable.find(token.lexeme) == symboltable.end()) {
                    errors.push_back( "Error: Undeclared variable used: "+ token.lexeme);
                    success=false;
                }
            }
        }
    }

    void check_assignments_with_expression(vector<Token>& tokens) {
        for (size_t i = 0; i + 2 < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::NAME &&
                tokens[i + 1].lexeme == "=") {

                string lhsVar = tokens[i].lexeme;

                // Check if variable was declared
                if (symboltable.find(lhsVar) == symboltable.end()) {
                    errors.push_back( "Error: Undeclared variable '" +lhsVar +"'" );
                    success=false;
                    continue;
                }

                string lhsType = symboltable[lhsVar];

                // Deduce RHS expression type
                string rhsType = evaluate_expression_type(tokens, i + 2);

                // Type compatibility check
                if (!are_types_compatible(lhsType, rhsType)) {
                    errors.push_back("Error: Type mismatch in assignment to '" + lhsVar
                         + "'. Declared as " + lhsType + ", assigned expression of type " + rhsType);
                    success=false;
                }

                // Move i to end of statement
                while (i < tokens.size() && tokens[i].lexeme != ";") {
                    ++i;
                }
            }
        }
    }

    string evaluate_expression_type(vector<Token>& tokens, size_t start) {
        string currentType = "";

        for (size_t i = start; i < tokens.size(); ++i) {
            Token token = tokens[i];
            if (token.lexeme == ";") break;

            if (token.type == TokenType::NAME) {
                if (symboltable.find(token.lexeme) != symboltable.end()) {
                    string varType = symboltable[token.lexeme];
                    currentType = (currentType.empty())
                                  ? varType
                                  : dominant_type(currentType, varType);
                } else {
                    errors.push_back( "Error: Undeclared variable '" + token.lexeme + "' used in expression" );
                    success=false;
                    return "unknown";
                }
            }
            else if (token.type == TokenType::DIGIT) {
                currentType = dominant_type(currentType, "int");
            }
            else if (token.type == TokenType::REAL) {
                currentType = dominant_type(currentType, "float");
            }
            else if (token.type == TokenType::NAME) {
                currentType = dominant_type(currentType, "string");
            }
            else if (token.lexeme == "true" || token.lexeme == "false") {
                currentType = dominant_type(currentType, "bool");
            }
            else if (token.type == TokenType::REAL_OP ||
                     token.type == TokenType::ARTH_OP ||
                     token.type == TokenType::LOGIC_OP) {
                continue; // skip operators
            }
            else {
                break; // unknown or unhandled token
            }
        }

        return currentType.empty() ? "unknown" : currentType;
    }

    string dominant_type(const string& a, const string& b) {
        if (a == b) return a;

        // Numeric promotion
        if (a == "double" || b == "double") return "double";
        if (a == "float" || b == "float") return "float";
        if (a == "int" || b == "int") return "int";

        // String override
        if (a == "string" || b == "string") return "string";

        // Boolean logic
        if (a == "bool" || b == "bool") return "bool";

        return "unknown";
    }

    bool are_types_compatible(const string& declared, const string& exprType) {
        if (declared == exprType) return true;

        // Implicit promotion rules
        if ((declared == "float" || declared == "double") && (exprType == "int" || exprType=="float")) return true;

        // Allow bool = bool expressions
        if (declared == "bool" && exprType == "bool") return true;

        // Allow string = string
        if (declared == "string" && exprType == "string") return true;

        return false;
    }
    
    
    void annotate_ast(ASTNode& node, const map<string,string>& symboltable) {
        // Annotate NAMEs with type
        if ((node.type == "NAME" || node.type == "IDENTIFIER") && symboltable.count(node.value)) {
            node.annotation = symboltable.at(node.value);
        }

        for (auto& child : node.children) {
            annotate_ast(child, symboltable);
        }
    }
    
    void export_symbol_table(const string& filename = "symbol_table.txt") {
        ofstream file(filename);
        file << "Symbol\tType\n";
        for (const auto& [var, type] : symboltable) {
            file << var << "\t" << type << "\n";
        }
        file.close();
        //cout << "Symbol table written to " << filename << endl;
    }

    const map<string, string>& get_symboltable() const {
        return symboltable;
    }

    vector<string> get_errors()
    {
        return  errors;
    }
    bool is_success()
    {
        return success;
    }
    

};

#endif /* _semantic_h */
