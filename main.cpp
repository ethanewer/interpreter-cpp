#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string>
#include <vector>
#include <stdexcept>
#include "Error.hpp"
#include "Interpreter.hpp"
#include "Token.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"
#include "Resolver.hpp"

Interpreter interpreter;
bool had_error;
bool had_runtime_error;

void run(std::string source) {
	std::vector<std::shared_ptr<Stmt>> stmts;
	try {
		Scanner scanner(source);
		std::vector<std::shared_ptr<Token>> tokens = scanner.scan_tokens();
		
		Parser parser(tokens);
		stmts = parser.parse();
		Resolver resolver(&interpreter);
		resolver.resolve(stmts);
		interpreter.interpret(stmts);
	} catch (SyntaxError& e) {
		std::cout << "Syntax error: [line: " << e.line << "] " << e.what() << '\n';
		had_error = true;
	} catch (RuntimeError& e) {
		std::cout << "Runtime error: [line: " << e.token->line << "] " << e.what() << '\n';
		had_error = true;
		had_runtime_error = true;
	}
}

void run_prompt() {
	for (;;) {
		std::cout << "> ";
		std::string line;
		std::getline(std::cin, line);
		run(line);
		had_error = false;
	}
}

void run_file(const std::string& path) {
    std::ifstream file(path);
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        std::string file_content = ss.str();
        file.close();

        run(file_content);

        if (had_error) std::exit(65);
        if (had_runtime_error) std::exit(70);
    } else {
        std::cout << "Error: Could not open file " << path << std::endl;
        std::exit(1);
    }
}

int main(int argc, char* argv[]) {
	if (argc > 2) std::cout << "Usage: jlox [script]" << std::endl;
    else if (argc == 2) run_file(argv[1]);
    else run_prompt();
}