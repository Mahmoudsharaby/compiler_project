#include <iostream>
#include <fstream>
#include <sstream>
#include "Tokenizer.h"
#include "LLParser.h"
#include " semantic.h"
#include "Tac.h"


 

int main() {
    
    string filepath="/Users/mahmoudhany/Desktop/System_project/System_project/File.txt";
        vector<string> sd;
        vector<Token> token;
        Tokenizer text;
    // STEP 1: Read source code from input.txt
        sd=text.tokenize(filepath);
    // STEP 2: Tokenize the source code
    for(int i=0;i<sd.size();i++)
    {
        
        Token temp;
        temp.type=text.determineTokenType(sd[i]);
        temp.lexeme=sd[i];
        token.push_back(temp);
   }
    

    // STEP 3: Parse tokens into AST
    LLParser parser;
    ASTNode ast = parser.parseAST(token);
    if(parser.is_success())
    {
        parser.exportAST(ast);}
    else
    {
        vector<string> errors=parser.get_errors();
        for(auto& error:errors)
        {
            cout << error << endl;
        }
    }

    // STEP 4: Perform semantic analysis
    semantic sem;
    sem.build_table(token);                       // Build symbol table
    sem.check_variable_usage(token);              // Check for undeclared uses
    sem.check_assignments_with_expression(token); // Check assignments and type compatibility
    if(sem.is_success()){
        sem.export_symbol_table("symbol_table.txt");   // Write symbol table to file
        // STEP 5: Annotate AST with types
        sem.annotate_ast(ast,sem.get_symboltable());                         // In-place annotation using symbol table
        parser.exportAST(ast, "annotated_ast.json");// Output the annotated AST as JSON
    }
    else
    {
        vector<string> errors=sem.get_errors();
        for(auto& error:errors)
        {
            cout << error << endl;
        }
    }

    if(parser.is_success() && sem.is_success()){
        
        TACGenerator tac;
        tac.generate(ast);              // Generate TAC from AST
        tac.exportToFile("tac.txt");
        
        
        cout << "✔ Compilation completed successfully.\n";
        cout << "  → AST saved to annotated_ast.json\n";
        cout << "  → Symbol table saved to symbol_table.txt\n";
    }

    return 0;
}

