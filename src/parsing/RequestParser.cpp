#include "RequestParser.hpp"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string>

namespace http
{
	RequestParser::RequestParser()
		: _state(PARSING_REQUEST), _buffer(""), _errorCode(0), _contentLength(0),
		  _maxBodySize(1048576), _bodyBytesRemaining(0), _currentChunkSize(0),
		  _chunkBytesRemaining(0) {}

	RequestParser::~RequestParser() {}

	RequestParser::State RequestParser::parse(const char *data, size_t size)
	{
		if (_state == COMPLETE || _state == ERROR)
			return _state;

		_buffer.append(data, size);

		// fin de ligne
		if (_buffer.size() > MAX_REQUEST_SIZE)
		{
			_errorCode = 413;
			_state = ERROR;
			return _state;
		}

		while (true)
		{
			State prevState = _state;
			switch (_state)
			{
			case PARSING_REQUEST:
				_state = _parseRequestLineState();
				break;

			case PARSING_HEADERS:
				_state = _parseHeadersState();
				if (_state == PARSING_BODY_LENGTH || _state == PARSING_CHUNK_SIZE ||
					_state == COMPLETE)
				{
					if (!_validateHeaders())
					{
						_state = ERROR;
						return _state;
					}
				}
				break;

			case PARSING_BODY_LENGTH:
				_state = _parseBodyByLength();
				break;

			case PARSING_CHUNK_SIZE:
			case PARSING_CHUNK_DATA:
			case PARSING_TRAILER:
				_state = _parseChunkedBody();
				break;
			default:
				return _state;
			}
			if (_state == prevState)
				return _state;
			if (_state == COMPLETE || _state == ERROR)
				return _state;
		}
	}

	bool RequestParser::_parseRequestLine(const std::string &line)
	{
		std::istringstream iss(line); // en fait un stream, comme un std::cin
		std::string method, uri, version;

		// check si tout les tokens sont rempli
		if (!(iss >> method >> uri >> version))
			return false;

		if (version != "HTTP/1.1")
			return false;

		size_t queryPos = uri.find('?');

		if (queryPos != std::string::npos)
		{
			_request.setPath(uri.substr(0, queryPos));
			_request.setQueryString(uri.substr(queryPos + 1));
		}
		else
			_request.setPath(uri);

		_request.setMethod(method);
		_request.setHttpVersion(version);
		_httpVersion = version;

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

	Request &RequestParser::getRequest() { return _request; }

	RequestParser::State RequestParser::_determineBodyParsing()
	{
		std::string transferEncoding = _request.getHeader("Transfer-Encoding");
		std::string contentLengthStr = _request.getHeader("Content-Length");

		if (transferEncoding.find("chunked") != std::string::npos)
		{
			_state = PARSING_CHUNK_SIZE;
			return _state;
		}

		// body du content-length
		if (!contentLengthStr.empty())
		{
			char *end;
			_contentLength = std::strtoul(contentLengthStr.c_str(), &end, 10);
			if (*end != '\0' || contentLengthStr.empty())
			{
				_errorCode = 400; // Content-Length error
				return ERROR;
			}
			if (_contentLength > _maxBodySize)
			{
				_errorCode = 413; // Trop gros
				return ERROR;
			}
			if (_contentLength == 0)
				return COMPLETE;
			_state = PARSING_BODY_LENGTH;
			_bodyBytesRemaining = _contentLength;
			return _state;
		}

		// pas de body attendu pour get delete etc
		// si pas de context length pour post alors error
		if (_request.getMethod() == "POST")
		{
			_errorCode = 411;
			return ERROR;
		}
		return COMPLETE;
	}

	RequestParser::State RequestParser::_parseBodyByLength()
	{
		size_t available = _buffer.size();
		size_t toRead = std::min(available, _bodyBytesRemaining);

		if (toRead > 0)
		{
			_request.appendBody(_buffer.substr(0, toRead));
			_buffer.erase(0, toRead);
			_bodyBytesRemaining -= toRead;
		}

		if (_bodyBytesRemaining == 0)
			_state = COMPLETE;
		return _state;
	}

	RequestParser::State RequestParser::_parseChunkedBody()
	{
		while (true)
		{
			if (_state == PARSING_CHUNK_SIZE)
			{
				size_t lineEnd = _buffer.find("\r\n");
				if (lineEnd == std::string::npos)
					return _state; // pas assez de donnees

				std::string sizeLine = _buffer.substr(0, lineEnd);
				_buffer.erase(0, lineEnd + 2);

				size_t semicolon = sizeLine.find(';');
				if (semicolon != std::string::npos)
					sizeLine = sizeLine.substr(0, semicolon);

				char *end;
				_currentChunkSize = std::strtoul(sizeLine.c_str(), &end, 16);
				if (*end != '\0' && !std::isspace(*end))
				{
					_errorCode = 400;
					return ERROR;
				}

				if (_currentChunkSize == 0)
				{
					_state =
						PARSING_TRAILER; // dernier chunk du coup on lit les CRLF trailer
					continue;
				}

				_state = PARSING_CHUNK_DATA;
				_chunkBytesRemaining = _currentChunkSize;
			}

			if (_state == PARSING_CHUNK_DATA)
			{
				size_t available = _buffer.size();
				size_t toRead = std::min(available, _chunkBytesRemaining);

				if (toRead > 0)
				{
					_request.appendBody(_buffer.substr(0, toRead));
					_buffer.erase(0, toRead);
					_chunkBytesRemaining -= toRead;
				}

				if (_chunkBytesRemaining == 0)
				{
					if (_buffer.size() < 2)
						return _state;
					if (_buffer.substr(0, 2) != "\r\n")
					{
						_errorCode = 400;
						return ERROR;
					}
					_buffer.erase(0, 2);
					_state = PARSING_CHUNK_SIZE;
				}
				else
					return _state;
			}

			if (_state == PARSING_TRAILER)
			{
				size_t lineEnd = _buffer.find("\r\n");
				if (lineEnd == std::string::npos)
					return _state;

				if (lineEnd == 0)
				{
					_buffer.erase(0, 2);
					return COMPLETE;
				}
				_buffer.erase(0, lineEnd + 2);
			}
		}
	}

	bool RequestParser::_validateHeaders()
	{
		std::string host = _request.getHeader("Host");
		if (host.empty())
		{
			_errorCode = 400;
			return false;
		}
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
			_errorCode = 400;
			return ERROR;
		}
		return PARSING_HEADERS;
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

			if (!_parseHeader(headerLine))
			{
				_errorCode = 400;
				return ERROR;
			}
		}
	}

	std::string RequestParser::getErrorMessage() const
	{
		switch (_errorCode)
		{
		case 400:
			return "Bad Request";
		case 405:
			return "Method Not Allowed";
		case 411:
			return "Length Required";
		case 413:
			return "Payload Too Large";
		case 414:
			return "URI Too Long";
		case 431:
			return "Request Header Fields Too Large";
		case 501:
			return "Not Implemented";
		case 505:
			return "HTTP Version Not Supported";
		default:
			return "Unknown Error";
		}
	}

} // namespace http
