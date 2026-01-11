#pragma once

#include <string>
#include <cstddef>

namespace libftpp {
namespace conf {

struct Token {
	enum Type {
		T_EOF = 0,

		T_IDENT,	// mots, chemins, valeurs non-quotées
		T_STRING,	// "quoted string"
		T_NUMBER,	// nombre quoi

		T_LBRACE,	// {
		T_RBRACE,	// }
		T_LPAREN,	// (
		T_RPAREN,	// )
		T_SEMI,		// ;
		T_COMMA		// ,
	};

	Type		type;
	std::string	lexeme;	// texte du token (ex: "server", "/var/www", "8080")
	size_t		line;	// 1-based
	size_t		col;	// 1-based

	Token();
	Token(Type t, const std::string& s, size_t ln, size_t cl);
};

// -------------------- Tokenizer --------------------
// Tokenizer simple pour fichiers de config style nginx (webserv)
// - ignore espaces / tabs / newlines (mais garde line/col)
// - gère strings entre guillemets " ... " avec échappement minimal (\\ et \")
// - commentaires: supposés déjà "strip" en amont (ou à gérer ailleurs)

class Tokenizer {
private:
	// --- Canonique C++98 ---
	Tokenizer(const Tokenizer& other);
	Tokenizer& operator=(const Tokenizer& other);

	// --- Source ---
	std::string _src;
	size_t	  _i;		// index courant dans _src
	size_t	  _line;	// 1-based
	size_t	  _col;		// 1-based

	// --- Helpers internes ---
	bool		eof() const;
	char		peek() const;	// '\0' si EOF
	char		get();			// consomme un char, update line/col
	void		skip_ws();		// skip espaces (inclut \r \n \t)
	bool		is_ident_char(char c) const;
	bool		is_digit(char c) const;

	Token	   read_string();	// lit "..."
	Token	   read_number();	// lit 123...
	Token	   read_ident();	// lit ident/path/value

public:
	Tokenizer(const std::string& src);
	~Tokenizer();

	// Retourne le prochain token (consomme).
	Token next();

	// Peek du prochain token sans consommer.
	// (Implementation typique: save state, next(), restore)
	Token peekToken();

	// Position actuelle (utile pour erreurs)
	size_t line() const;
	size_t col() const;

	// Reset (réutiliser le tokenizer)
	void reset(const std::string& src);
};

}
}
