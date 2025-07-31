#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGenerator.h"

void runFile(const std::string& path) 
{
    std::ifstream file(path);

    if (!file.is_open()) 
    {
        std::cerr << "Error: Could not open file " << path << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    auto statements = parser.parse();
    
    CodeGenerator generator;
    try 
    {
        generator.generate(statements);
    } 
    catch (const std::runtime_error& e) 
    {
        std::cerr << "Runtime Error during code generation: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) 
{
    if (argc != 2) 
    {
        std::cout << "Usage: " << argv[0] << " <filename.lr>" << std::endl;
        return 1;
    }

    runFile(argv[1]);
    return 0;
}