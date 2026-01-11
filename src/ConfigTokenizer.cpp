#include "../include/ConfigTokenizer.hpp"

#include <cctype>

using namespace libftpp::conf;

Token::Token()
: type(T_EOF), lexeme(), line(1), col(1) {}

Token::Token(Type t, const std::string& s, size_t ln, size_t cl)
: type(t), lexeme(s), line(ln), col(cl) {}

Tokenizer::Tokenizer(const std::string& src)
: _src(src), _i(0), _line(1), _col(1) {}

Tokenizer::~Tokenizer() {}

bool Tokenizer::eof() const {
	return _i >= _src.size();
}

char Tokenizer::peek() const {
	if (eof()) return '\0';
	return _src[_i];
}

char Tokenizer::get() {
	if (eof()) return '\0';

	char c = _src[_i++];
	if (c == '\n') {
		++_line;
		_col = 1;
	} else {
		++_col;
	}
	return c;
}

void Tokenizer::skip_ws() {
	while (!eof()) {
		unsigned char c = (unsigned char)peek();
		if (std::isspace(c) == 0)
			break;
		get();
	}
}

bool Tokenizer::is_digit(char c) const {
	return (c >= '0' && c <= '9');
}

	// Assez permissif pour config nginx-like:
	// lettres, chiffres, '_' '-' '.' '/' ':' '@' '+' '='
	// (permet: /var/www, 127.0.0.1:8080, index.html, on/off, 10m, etc.)
bool Tokenizer::is_ident_char(char c) const {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return true;
	if (is_digit(c))
		return true;
	if (c == '_' || c == '-' || c == '.' || c == '/' || c == ':' ||
		c == '@' || c == '+' || c == '=')
		return true;
	return false;
}

Token Tokenizer::read_string() {
	size_t start_line = _line;
	size_t start_col  = _col;

	get();

	std::string out;
	while (!eof()) {
		char c = get();
		if (c == '\0')
			break;

		if (c == '"') {
			return Token(Token::T_STRING, out, start_line, start_col);
		}
		if (c == '\\') {
			if (eof()) break;
			char n = get();
			if (n == '"' || n == '\\') {
				out.push_back(n);
			} else {
				out.push_back('\\');
				out.push_back(n);
			}
			continue;
		}

		out.push_back(c);
	}
	return Token(Token::T_STRING, out, start_line, start_col);
}

Token Tokenizer::read_number() {
	size_t start_line = _line;
	size_t start_col  = _col;

	size_t start = _i;
	while (!eof() && is_digit(peek()))
		get();

	return Token(Token::T_NUMBER, _src.substr(start, _i - start), start_line, start_col);
}

Token Tokenizer::read_ident() {
	size_t start_line = _line;
	size_t start_col  = _col;

	size_t start = _i;
	while (!eof() && is_ident_char(peek()))
		get();

	return Token(Token::T_IDENT, _src.substr(start, _i - start), start_line, start_col);
}

Token Tokenizer::next() {
	skip_ws();

	if (eof())
		return Token(Token::T_EOF, "", _line, _col);

	size_t ln = _line;
	size_t cl = _col;
	char c = peek();

	if (c == '{') { get(); return Token(Token::T_LBRACE, "{", ln, cl); }
	if (c == '}') { get(); return Token(Token::T_RBRACE, "}", ln, cl); }
	if (c == '(') { get(); return Token(Token::T_LPAREN, "(", ln, cl); }
	if (c == ')') { get(); return Token(Token::T_RPAREN, ")", ln, cl); }
	if (c == ';') { get(); return Token(Token::T_SEMI,   ";", ln, cl); }
	if (c == ',') { get(); return Token(Token::T_COMMA,  ",", ln, cl); }

	if (c == '"')
		return read_string();

	if (is_digit(c))
		return read_number();

	if (is_ident_char(c))
		return read_ident();

	get();
	return Token(Token::T_IDENT, std::string(1, c), ln, cl);
}

Token Tokenizer::peekToken() {

	size_t save_i	= _i;
	size_t save_line = _line;
	size_t save_col  = _col;

	Token t = next();

	_i	= save_i;
	_line = save_line;
	_col  = save_col;

	return t;
}

size_t Tokenizer::line() const { return _line; }
size_t Tokenizer::col() const { return _col; }

void Tokenizer::reset(const std::string& src) {
	_src  = src;
	_i	= 0;
	_line = 1;
	_col  = 1;
}
