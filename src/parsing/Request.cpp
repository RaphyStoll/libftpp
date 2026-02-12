#include "Request.hpp"
#include "../include/StringUtils.hpp"
#include <iostream>

using namespace webserv::http;

Request::Request()
	: _method(""), _path(""), _queryString(""), _httpVersion(""), _body("") {}

Request::~Request() {}

std::string Request::getMethod() const { return _method; }
std::string Request::getPath() const { return _path; }
std::string Request::getQueryString() const { return _queryString; }
std::string Request::getHttpVersion() const { return _httpVersion; }
std::string Request::getBody() const { return _body; }

std::string Request::getHeader(const std::string &name) const
{
	std::map<std::string, std::string>::const_iterator it =
		_headers.find(libftpp::str::StringUtils::toLower(name));
	if (it != _headers.end())
		return it->second;
	return "";
}

size_t Request::getBodySize() const { return _body.size(); }

void Request::setMethod(const std::string &method) { _method = method; }
void Request::setPath(const std::string &path) { _path = path; }
void Request::setQueryString(const std::string &query) { _queryString = query; }
void Request::setBody(const std::string &body) { _body = body; }

void Request::setHttpVersion(const std::string &version)
{
	_httpVersion = version;
}

void Request::setHeader(const std::string &name, const std::string &value)
{
	std::string lower = libftpp::str::StringUtils::toLower(name);
	if (lower == "content-length")
	{
		_headers[lower] = value;
		return;
	}

	std::map<std::string, std::string>::iterator it = _headers.find(lower);
	if (it == _headers.end())
	{
		_headers[lower] = value;
		return;
	}

	if (!it->second.empty() && !value.empty())
		it->second += ", " + value;
	else
		it->second += value;
}

	void Request::appendBody(const std::string &data) { _body.append(data); }

		void Request::print(void) const
	{
		std::cout << "Request : " << std::endl;
        std::cout << "  _method : " << _method << std::endl;
        std::cout << "  _path : " << _path << std::endl;
        std::cout << "  _queryString : " << _queryString << std::endl;
        std::cout << "  _httpVersion : " << _httpVersion << std::endl;
		std::cout << "  _body : " << _body << std::endl;
/*
        std::cout << "  methods : ";
        for(size_t i = 0 ; i < methods.size(); i++)
            std::cout << methods[i] << " ";
        std::cout << std::endl;
*/
        

 	}

} // namespace http
