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

};

} // namespace http

#endif
