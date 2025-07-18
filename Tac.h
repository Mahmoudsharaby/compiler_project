#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "LLParser.h"
using namespace std;
class TACGenerator {
private:
    int tempVarCounter = 0;
    int labelCounter = 0;
    vector<string> code;

    string newTemp() {
        return "t" + to_string(++tempVarCounter);
    }

   string newLabel() {
        return "L" + to_string(++labelCounter);
    }

public:
    void generate(const ASTNode& root) {
        traverse(root);
    }

    void exportToFile(const string& filename = "tac.txt") {
        ofstream file(filename);
        for (const auto& line : code) {
            file << line << "\n";
        }
        file.close();
        cout << "TAC written to " << filename << endl;
    }

private:
    void traverse(const ASTNode& node) {
        if (node.type == "Statement" && !node.children.empty()) {
            // Handle while loop
            if (node.children[0].type == "WHILE") {
                string startLabel = newLabel();
                string endLabel = newLabel();
                code.push_back(startLabel + ":");

                const ASTNode& condition = node.children[1];
                string condResult = generateExpr(condition);
                code.push_back("if not " + condResult + " goto " + endLabel);

                // Loop body
                for (size_t i = 2; i < node.children.size(); ++i) {
                    traverse(node.children[i]);
                }

                code.push_back("goto " + startLabel);
                code.push_back(endLabel + ":");
                return;
            }

            //  Handle assignment: NAME = Expr
            for (const auto& child : node.children) {
                if (child.type == "Assignment" && child.children.size() == 3) {
                    const auto& lhs = child.children[0];
                    const auto& op = child.children[1];
                    const auto& rhs = child.children[2];

                    if ((lhs.type == "NAME" || lhs.type == "IDENTIFIER") && op.value == "=") {
                        string rhsResult = generateExpr(rhs);
                        code.push_back(lhs.value + " = " + rhsResult);
                        return;
                    }
                }
            }
        }

        // Recurse through all other children
        for (const auto& child : node.children) {
            traverse(child);
        }
    }

    string generateExpr(const ASTNode& node) {
        if (node.type == "DIGIT" || node.type == "REAL" || node.type == "NAME") {
            return node.value;
        }

        if (node.type == "BinaryExpr" && node.children.size() == 3) {
            string left = generateExpr(node.children[0]);
            string op   = node.children[1].value;
            string right = generateExpr(node.children[2]);

            string temp = newTemp();
            code.push_back(temp + " = " + left + " " + op + " " + right);
            return temp;
        }

        if (node.type == "Expr" || node.type == "SimpleExpr" || node.type == "Assignment") {
            return generateExpr(node.children.back());
        }

        if (!node.children.empty()) {
            return generateExpr(node.children[0]);
        }

        return node.value;
    }
};

#endif // TAC_H

