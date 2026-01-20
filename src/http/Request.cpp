#include "../../include/Request.hpp"

namespace http {

Request::Request() {}

Request::~Request() {}

std::string Request::getHeader(const std::string &name) const {
    std::map<std::string, std::string>::const_iterator it = _headers.find(name);
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

std::string Request::getMethod() const {
    return _method;
}

std::string Request::getPath() const {
    return _path;
}

std::string Request::getBody() const {
    return _body;
}

void Request::setMethod(const std::string &method) {
    _method = method;
}

void Request::setPath(const std::string &path) {
    _path = path;
}

void Request::setHeader(const std::string &name, const std::string &value) {
    _headers[name] = value;
}

void Request::setBody(const std::string &body) {
    _body = body;
}

} // namespace http
