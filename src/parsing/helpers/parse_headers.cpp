#include "RequestParser.hpp"
#include "../include/StringUtils.hpp"

using namespace webserv::http;

bool RequestParser::_parseHeader(const std::string &line)
{
	if (line.size() > MAX_HEADER_LINE_LENGTH)
	{
		_errorCode = 431;
		return false;
	}

	size_t colonPos = line.find(':');
	if (colonPos == std::string::npos || colonPos == 0)
	{
		_errorCode = 400;
		return false;
	}

	std::string key = line.substr(0, colonPos);
	std::string value = line.substr(colonPos + 1);

	if (!_isValidHeaderName(key))
	{
		_errorCode = 400;
		return false;
	}

	// trim les espaces de la valeur
	value = libftpp::str::StringUtils::trim(value);

	for (size_t i = 0; i < value.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(value[i]);
		if ((c < 0x20 && c != '\t') || c == 0x7F)
		{
			_errorCode = 400;
			return false;
		}
	}

	std::string lowerKey = libftpp::str::StringUtils::toLower(key);
	if (lowerKey == "content-length")
		return _parseContentLengthHeader(value);
	if (lowerKey == "transfer-encoding")
		return _parseTransferEncodingHeader(value);
	if (lowerKey == "connection")
	{
		_request.setHeader(key, libftpp::str::StringUtils::toLower(value));
		return true;
	}

	_request.setHeader(key, value);
	return true;
}

RequestParser::State RequestParser::_parseHeadersState()
{
	while (true)
	{
		size_t endOfLine = _buffer.find("\r\n");
		if (endOfLine == std::string::npos)
			return PARSING_HEADERS;

		if (endOfLine == 0)
		{
			_buffer.erase(0, 2);
			return _determineBodyParsing();
		}

		std::string headerLine = _buffer.substr(0, endOfLine);
		_buffer.erase(0, endOfLine + 2);

		if (!headerLine.empty() && (headerLine[0] == ' ' || headerLine[0] == '\t'))
		{
			_errorCode = 400;
			return ERROR;
		}

		++_headerCount;
		if (_headerCount > MAX_HEADER_COUNT)
		{
			_errorCode = 431;
			return ERROR;
		}

		if (!_parseHeader(headerLine))
		{
			if (_errorCode == 0)
				_errorCode = 400;
			return ERROR;
		}
	}
}

bool RequestParser::_isValidHeaderName(const std::string &name) const
{
	if (name.empty())
		return false;
	for (size_t i = 0; i < name.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(name[i]);
		if (!(std::isalnum(c) || c == '!' || c == '#' || c == '$' ||
			  c == '%' || c == '&' || c == '\'' || c == '*' || c == '+' ||
			  c == '-' || c == '.' || c == '^' || c == '_' || c == '`' ||
			  c == '|' || c == '~'))
			return false;
	}
	return true;
}

bool RequestParser::_validateHeaders()
{
	std::string host = _request.getHeader("Host");
	if (host.empty())
	{
		_errorCode = 400;
		return false;
	}
	if (host.find(' ') != std::string::npos || host.find('\t') != std::string::npos)
	{
		_errorCode = 400;
		return false;
	}

	return true;
}
