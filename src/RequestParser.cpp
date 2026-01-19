#include "RequestParser.hpp"
#include <sstream>
#include <algorithm>

namespace http
{

	RequestParser::RequestParser() : _state(PARSING), _buffer("") {}

	RequestParser::~RequestParser() {}

	RequestParser::State RequestParser::parse(const char *data, size_t size)
	{
		if (_state == COMPLETE || _state == ERROR)
			return _state;

		_buffer.append(data, size);

		// fin de ligne
		size_t endOfLine = _buffer.find("\r\n");
		if (endOfLine == std::string::npos) // npos == max value du size_t
			return _state;					// Incomplet, attente de plus de données

		if (_state == PARSING)
		{
			std::string requestLine = _buffer.substr(0, endOfLine);
			_buffer.erase(0, endOfLine + 2);

			if (!_parseRequestLine(requestLine))
			{
				_state = ERROR;
				return _state;
			}

			while (true)
			{
				endOfLine = _buffer.find("\r\n");
				if (endOfLine == std::string::npos)
					return PARSING;
				if (endOfLine == 0)
				{
					_buffer.erase(0, 2);
					_state = COMPLETE;
					return _state;
				}

				std::string headerLine = _buffer.substr(0, endOfLine);
				_buffer.erase(0, endOfLine + 2);

				if (!_parseHeader(headerLine))
				{
					_state = ERROR;
					return _state;
				}
			}
		}

		return _state;
	}

	/* example from rfc
	HTTP/1.1 200 OK
		 Date: Mon, 27 Jul 2009 12:28:53 GMT
		 Server: Apache
		 Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT
		 ETag: "34aa387-d-1568eb00"
		 Accept-Ranges: bytes
		 Content-Length: 51
		 Vary: Accept-Encoding
		 Content-Type: text/plain
	*/

	bool RequestParser::_parseRequestLine(const std::string &line)
	{
		std::istringstream iss(line); // en fait un stream, comme un std::cin
		std::string method, path, version;

		// check si tout les tokens sont rempli
		if (!(iss >> method >> path >> version))
			return false;

		if (version != "HTTP/1.1" && version != "HTTP/1.0")
			return false;
		_request.setMethod(method);
		_request.setPath(path);

		return true;
	}

	bool RequestParser::_parseHeader(const std::string &line)
	{
		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
			return false;
		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);

		// trim les espaces de la valeur
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);

		_request.setHeader(key, value);
		return true;
	}

	Request &RequestParser::getRequest()
	{
		return _request;
	}
}
