#pragma once

#include <string>
#include <vector>
#include <cstddef>

namespace libftpp {
namespace str {

class StringUtils {
private:
	// Classe statique : constructeurs privés
	StringUtils() {};
	StringUtils(const StringUtils& other) { (void)other; };
	StringUtils& operator=(const StringUtils& other) { (void)other; return *this; };
	~StringUtils() {};

	static bool is_space(unsigned char c);
	static unsigned char to_lower(unsigned char c);

public:
	static std::string toLower(const std::string& s);
	static std::string toUpper(const std::string& s);
	// Retire les espaces (isspace) au début de la chaîne
	static std::string ltrim(const std::string& s);
	
	// Retire les espaces (isspace) à la fin de la chaîne
	static std::string rtrim(const std::string& s);
	
	// Retire les espaces au début et à la fin
	static std::string trim(const std::string& s);

	// Découpe la chaîne 's' selon le délimiteur 'delim'.
	// limit = 0 : découpe tout (pas de limite).
	// limit > 0 : retourne au maximum 'limit' morceaux (le dernier contient le reste).
	static std::vector<std::string> split(const std::string& s, char delim, size_t limit = 0);

	// Comparaison insensible à la casse.
	// Retourne true si a == b (en ignorant la casse).
	static bool iequals(const std::string& a, const std::string& b);

	// Tente de parser un unsigned int depuis la chaîne s.
	// Retourne true si succès, false si erreur (caractère invalide ou overflow).
	static bool parse_uint(const std::string& s, unsigned int& out);
	
	// Idem pour size_t.
	static bool parse_size_t(const std::string& s, size_t& out);
};
}
}
