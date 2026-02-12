#include "RequestParser.hpp"
#include <sstream>


using namespace webserv::http;

bool RequestParser::_parseRequestLine(const std::string &line)
{
	std::istringstream iss(line); // en fait un stream, comme un std::cin
	std::string method, uri, version;

	// check si tout les tokens sont rempli
	if (!(iss >> method >> uri >> version))
		return false;

	if (!_isValidMethod(method))
	{
		_errorCode = 501;
		return false;
	}

	if (version != "HTTP/1.1")
	{
		_errorCode = 505;
		return false;
	}

	if (!_isValidRequestTarget(uri))
		return false;

	std::string pathPart = uri;
	if (uri.find("http://") == 0 || uri.find("https://") == 0)
	{
		size_t schemeEnd = uri.find("://");
		size_t pathStart = uri.find('/', schemeEnd + 3);
		if (pathStart == std::string::npos)
			pathPart = "/";
		else
			pathPart = uri.substr(pathStart);
	}

	size_t queryPos = pathPart.find('?');

	if (queryPos != std::string::npos)
	{
		_request.setPath(pathPart.substr(0, queryPos));
		_request.setQueryString(pathPart.substr(queryPos + 1));
	}
	else
		_request.setPath(pathPart);

	if (_hasDotDotSegment(_request.getPath()))
	{
		_errorCode = 400;
		return false;
	}

	_request.setMethod(method);
	_request.setHttpVersion(version);

	return true;
}

RequestParser::State RequestParser::_parseRequestLineState()
{
	size_t endOfLine = _buffer.find("\r\n");
	if (endOfLine == std::string::npos)
		return PARSING_REQUEST;
	std::string requestLine = _buffer.substr(0, endOfLine);
	_buffer.erase(0, endOfLine + 2);
	if (!_parseRequestLine(requestLine))
	{
		if (_errorCode == 0)
			_errorCode = 400;
		return ERROR;
	}
	return PARSING_HEADERS;
}

bool RequestParser::_isValidMethod(const std::string &method) const
{
	return (method == "GET" || method == "POST" || method == "DELETE");
}

bool RequestParser::_isValidRequestTarget(const std::string &uri)
{
	if (uri.empty())
	{
		_errorCode = 400;
		return false;
	}

	if (uri.size() > MAX_URI_LENGTH)
	{
		_errorCode = 414;
		return false;
	}

	for (size_t i = 0; i < uri.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(uri[i]);
		if (c <= 0x20 || c == 0x7F) // virer tout ce qui est avant espace et le delete
		{
			_errorCode = 400;
			return false;
		}
	}

	if (uri == "*") // uniquement true si la method OPTIONS est utilisee
		return false;

	return true;
}

bool RequestParser::_hasDotDotSegment(const std::string &path) const
{
	if (path.find("/../") != std::string::npos)
		return true;
	if (path.size() >= 3 && path.compare(path.size() - 3, 3, "/..") == 0)
		return true;
	if (path.compare(0, 3, "../") == 0)
		return true;
	return false;
}
