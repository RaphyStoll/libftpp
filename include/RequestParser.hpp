#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Request.hpp"
#include <cstddef>
#include <string>

namespace http {

class RequestParser {
public:
	enum State { PARSING, COMPLETE, ERROR }; //autres

	RequestParser();
	~RequestParser();

	State parse(const char *data, size_t size);

	Request &getRequest();
private:
	Request _request;
	State _state;
	std::string _buffer;

	bool _parseRequestLine(const std::string &line);
	bool _parseHeader(const std::string &line);
};

} // namespace http

#endif
