#include "../../include/RequestParser.hpp"
#include <sstream>
#include <iostream>

namespace http {

RequestParser::RequestParser() : _state(PARSING) {}

RequestParser::~RequestParser() {}

RequestParser::State RequestParser::parse(const char *data, size_t size) {
    _buffer.append(data, size);

    // Simple detection of double CRLF indicating end of headers
    size_t header_end = _buffer.find("\r\n\r\n");
    if (header_end != std::string::npos) {
        // Basic parsing of the first line
        std::istringstream stream(_buffer);
        std::string method, path, version;
        
        if (stream >> method >> path >> version) {
            _request.setMethod(method);
            _request.setPath(path);
            _state = COMPLETE;
        } else {
            _state = ERROR;
        }
    }

    return _state;
}

Request &RequestParser::getRequest() {
    return _request;
}

} // namespace http
