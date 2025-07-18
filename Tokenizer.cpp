#include "Tokenizer.h"
#include <iostream>

vector<string> Tokenizer::tokenize(const string& filePath) {
    vector<string> tokens;
    ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        cerr << "Error opening file at: " << filePath << endl;
        cerr << "Check if:\n"
            << "1. The path is correct\n"
            << "2. The file exists\n"
            << "3. You have proper permissions" << endl;
        return tokens;
    }

    string line;
    while (getline(inputFile, line)) {
        string currentToken;
        linecounter++;

        for (size_t i = 0; i < line.size(); ++i) {
            if (MLC) {
                if (i + 1 < line.size() && line[i] == '*' && line[i + 1] == '/') {
                    MLC = false;
                    i++;
                }
                continue;
            }

            char c = line[i];

            switch (c) {
            
                case '\n':
                case '\t':
                case '\r':
                case ' ':
                    
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                break;

            case '=':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                if (i + 1 < line.size() && line[i + 1] == '=') {
                    tokens.push_back("==");
                    i++;
                }
                else {
                    tokens.push_back("=");
                }
                break;

            case '+':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                if (i + 1 < line.size() && line[i + 1] == '+') {
                    tokens.push_back("++");
                    i++;
                }
                else if (i + 1 < line.size() && line[i + 1] == '=') {
                    tokens.push_back("+=");
                    i++;
                }
                else
                {
                    tokens.push_back("+");
                }
                break;

            case '-':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                if (i + 1 < line.size() && line[i + 1] == '-') {
                    tokens.push_back("--");
                    i++;
                }
                else
                {
                    tokens.push_back("-");
                }
                break;

            case '*':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                if (i + 1 < line.size() && line[i + 1] == '=') {
                    tokens.push_back("*=");
                    i++;
                }
                else
                {
                    tokens.push_back("*");
                }
                break;

            case '/':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                if (i + 1 < line.size() && line[i + 1] == '=') {
                    tokens.push_back("/=");
                    i++;
                }
                else if (i + 1 < line.size() && line[i + 1] == '/') {
                    // Skip the rest of the line when we see //
                    i = line.size();  // This will break out of the for loop
                }
                else if (i + 1 < line.size() && line[i + 1] == '*') {
                    // Start of multi-line comment
                    MLC = true;
                    i++;  // Skip the '*'
                }
                else
                {
                    tokens.push_back("/");
                }
                break;

            case '%':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                if (i + 1 < line.size() && line[i + 1] == '=') {
                    tokens.push_back("%=");
                    i++;
                }
                else
                {
                    tokens.push_back("%");
                }
                break;

            case '&':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                if (i + 1 < line.size() && line[i + 1] == '&') {
                    tokens.push_back("&&");
                    i++;
                }
                else
                {
                    tokens.push_back("&");
                }
                break;

            case '|':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                if (i + 1 < line.size() && line[i + 1] == '|') {
                    tokens.push_back("||");
                    i++;
                }
                else
                {
                    tokens.push_back("|");
                }
                break;

            case '^': //XOR
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back("^");
                break;

            case '!':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }if (i + 1 < line.size() && line[i + 1] == '=') {
                    tokens.push_back("!=");
                    i++;
                }
                else
                {
                    tokens.push_back("!");
                }
                break;

            case '(':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back("(");
                break;

            case ')':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back(")");
                break;

            case '{':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back("{");
                break;

            case '}':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back("}");
                break;

            case '[':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back("[");
                break;

            case ']':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back("]");
                break;

            case '.':
                if (i > 0 && i + 1 < line.size() &&
                    (line[i - 1] >= '0' && line[i - 1] <= '9') &&
                    (line[i + 1] >= '0' && line[i + 1] <= '9'))
                {
                    currentToken += c;
                }
                else {
                    if (!currentToken.empty()) {
                        tokens.push_back(currentToken);
                        currentToken.clear();
                    }
                    tokens.push_back(".");
                }
                break;

            case '<':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }if (i + 1 < line.size() && line[i + 1] == '=') {
                    tokens.push_back("<=");
                    i++;
                }
                else if (i + 1 < line.size() && line[i + 1] == '<') {
                    tokens.push_back("<<"); //for cout or shift right
                    i++;
                }
                else
                {
                    tokens.push_back("<");
                }
                break;

            case '>':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }if (i + 1 < line.size() && line[i + 1] == '=') {
                    tokens.push_back(">=");
                    i++;
                }
                else if (i + 1 < line.size() && line[i + 1] == '>') {
                    tokens.push_back(">>"); //for cout or shift right
                    i++;
                }
                else
                {
                    tokens.push_back(">");
                }
                break;

            case ';':
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back(";");
                break;

            default:
                currentToken += c;
            }
        }

        if (!MLC && !currentToken.empty()) {
            tokens.push_back(currentToken);
        }
    }

    inputFile.close();
    return tokens;
}
