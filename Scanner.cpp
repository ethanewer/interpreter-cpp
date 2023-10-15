#include "Scanner.hpp"

Scanner::Scanner(std::string source) : source(source), start(0), curr(0), line(1) {
	keywords["and"] = AND;
	keywords["class"] = CLASS;
	keywords["else"] = ELSE;
	keywords["false"] = FALSE;
	keywords["for"] = FOR;
	keywords["fn"] = FN;
	keywords["if"] = IF;
	keywords["nil"] = NIL;
	keywords["or"] = OR;
	keywords["print"] = PRINT;
	keywords["return"] = RETURN;
	keywords["super"] = SUPER;
	keywords["this"] = THIS;
	keywords["true"] = TRUE;
	keywords["let"] = LET;
	keywords["while"] = WHILE;
}

std::vector<std::shared_ptr<Token>> Scanner::scan_tokens() {
	std::vector<std::shared_ptr<Token>> tokens;
	while (!is_at_end()) {
		start = curr;
		scan_token(tokens);
	}
	tokens.push_back(std::make_shared<Token>(_EOF, "", nullptr, line));
	return tokens;
}

bool Scanner::is_at_end() {
	return curr >= source.size();
}

void Scanner::scan_token(std::vector<std::shared_ptr<Token>>& tokens) {
	char c = advance();
	switch (c) {
		case '(': add_token(tokens, LEFT_PAREN); break;
		case ')': add_token(tokens, RIGHT_PAREN); break;
		case '{': add_token(tokens, LEFT_BRACE); break;
		case '}': add_token(tokens, RIGHT_BRACE); break;
		case ',': add_token(tokens, COMMA); break;
		case '.': add_token(tokens, DOT); break;
		case '-': add_token(tokens, MINUS); break;
		case '+': add_token(tokens, PLUS); break;
		case ';': add_token(tokens, SEMICOLON); break;
		case '*': add_token(tokens, STAR); break; 
		case '!': add_token(tokens, match('=') ? BANG_EQUAL : BANG); break;
		case '=': add_token(tokens, match('=') ? EQUAL_EQUAL : EQUAL); break;
		case '<': add_token(tokens, match('=') ? LESS_EQUAL : LESS); break;
		case '>': add_token(tokens, match('=') ? GREATER_EQUAL : GREATER); break;
		case '/':
			if (match('/')) {
				while (peek() != '\n' && !is_at_end()) {
					advance();
				}
			} else {
				add_token(tokens, SLASH);
			}
			break;
		case ' ':
		case '\r':
		case '\t': break;
		case '\n': line++; break;
		case '"': string(tokens, '"'); break;
		case '\'': string(tokens, '\''); break;
		default: 
			if (is_digit(c)) number(tokens);
			else if (is_alpha(c)) identifier(tokens);
			else throw SyntaxError(line, "Unexpected character");
			break;
	}
}

char Scanner::advance() {
	return source[curr++];
}

void Scanner::add_token(std::vector<std::shared_ptr<Token>>& tokens, TokenType type) {
	add_token(tokens, type, nullptr);
}

void Scanner::add_token(std::vector<std::shared_ptr<Token>>& tokens, TokenType type, std::shared_ptr<Obj> literal) {
	std::string text = source.substr(start, curr - start);
	tokens.push_back(std::make_shared<Token>(type, text, literal, line));
}

bool Scanner::match(char expected) {
	if (is_at_end()) return false;
	if (source[curr] != expected) return false;
	curr++;
	return true;
}

char Scanner::peek() {
	if (is_at_end()) return '\0';
	return source[curr];
}

char Scanner::peek_next() {
	if (curr + 1 >= source.size()) return '\0';
	return source[curr + 1];
}

void Scanner::string(std::vector<std::shared_ptr<Token>>& tokens, char quote) {
	while (peek() != quote && !is_at_end()) {
		if (peek() == '\n') line++;
		advance();
	}

	if (is_at_end()) {
		throw SyntaxError(line, "Unterminated string");
		return;
	}

	advance();
	add_token(tokens, STRING, std::make_shared<StringObj>(source.substr(start + 1, (curr - 1) - (start + 1))));
}

bool Scanner::is_digit(char c) {
	return c >= '0' && c <= '9';
} 

bool Scanner::is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::is_alpha_numeric(char c) {
	return is_alpha(c) || is_digit(c);
}

void Scanner::number(std::vector<std::shared_ptr<Token>>& tokens) {
	while (is_digit(peek())) advance();
	if (peek() == '.' && is_digit(peek_next())) advance();
	while (is_digit(peek())) advance();
	add_token(tokens, NUMBER, std::make_shared<DoubleObj>(std::stod(source.substr(start, curr - start))));
}

void Scanner::identifier(std::vector<std::shared_ptr<Token>>& tokens) {
	while (is_alpha_numeric(peek())) advance();
	std::string text = source.substr(start, curr - start);
	if (keywords.count(text)) add_token(tokens, keywords[text]);
	else add_token(tokens, IDENTIFIER);
}