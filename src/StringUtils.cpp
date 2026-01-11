#include "../include/StringUtils.hpp"

#include <cctype>
#include <limits>

using libftpp::str::StringUtils;

bool StringUtils::is_space(unsigned char c) {
	return std::isspace(c) != 0;
}

unsigned char StringUtils::to_lower(unsigned char c) {
	return (unsigned char)std::tolower(c);
}

std::string StringUtils::ltrim(const std::string& s) {
	size_t i = 0;
	while (i < s.size() && is_space((unsigned char)s[i]))
		++i;
	return s.substr(i);
}

std::string StringUtils::rtrim(const std::string& s) {
	if (s.empty())
		return s;

	size_t i = s.size();
	while (i > 0 && is_space((unsigned char)s[i - 1]))
		--i;
	return s.substr(0, i);
}

std::string StringUtils::trim(const std::string& s) {
	return rtrim(ltrim(s));
}

std::vector<std::string> StringUtils::split(const std::string& s, char delim, size_t limit) {
	std::vector<std::string> out;
	if (limit == 1) {
		out.push_back(s);
		return out;
	}

	size_t start = 0;
	size_t parts = 0;

	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] == delim) {
			if (limit != 0 && (parts + 1) >= (limit - 1)) {
				break;
			}
			out.push_back(s.substr(start, i - start));
			++parts;
			start = i + 1;
		}
	}

	out.push_back(s.substr(start));
	return out;
}

bool StringUtils::iequals(const std::string& a, const std::string& b) {
	if (a.size() != b.size())
		return false;

	for (size_t i = 0; i < a.size(); ++i) {
		unsigned char ca = (unsigned char)a[i];
		unsigned char cb = (unsigned char)b[i];
		if (to_lower(ca) != to_lower(cb))
			return false;
	}
	return true;
}

bool StringUtils::parse_uint(const std::string& s, unsigned int& out) {
	if (s.empty())
		return false;

	unsigned int value = 0;
	const unsigned int maxv = std::numeric_limits<unsigned int>::max();

	for (size_t i = 0; i < s.size(); ++i) {
		unsigned char c = (unsigned char)s[i];
		if (c < '0' || c > '9')
			return false;

		unsigned int digit = (unsigned int)(c - '0');

		if (value > (maxv - digit) / 10)
			return false;

		value = value * 10 + digit;
	}

	out = value;
	return true;
}

bool StringUtils::parse_size_t(const std::string& s, size_t& out) {
	if (s.empty())
		return false;

	size_t value = 0;
	const size_t maxv = std::numeric_limits<size_t>::max();

	for (size_t i = 0; i < s.size(); ++i) {
		unsigned char c = (unsigned char)s[i];
		if (c < '0' || c > '9')
			return false;

		size_t digit = (size_t)(c - '0');

		if (value > (maxv - digit) / 10)
			return false;

		value = value * 10 + digit;
	}

	out = value;
	return true;
}

std::string StringUtils::toLower(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); ++i) {
        unsigned char c = (unsigned char)out[i];
        out[i] = (char)std::tolower(c);
    }
    return out;
}

std::string StringUtils::toUpper(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); ++i) {
        unsigned char c = (unsigned char)out[i];
        out[i] = (char)std::toupper(c);
    }
    return out;
}
