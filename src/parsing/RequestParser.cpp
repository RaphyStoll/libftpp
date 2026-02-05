#include "RequestParser.hpp"
#include "../include/StringUtils.hpp"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace http
{
	RequestParser::RequestParser()
		: _state(PARSING_REQUEST), _buffer(""), _errorCode(0), _contentLength(0),
		  _maxBodySize(1048576), _bodyBytesRemaining(0), _currentChunkSize(0),
		  _chunkBytesRemaining(0), _seenContentLength(0), _contentLengthHeaderValue(0),
		  _hasTransferEncoding(false), _headerCount(0) {}

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
		_httpVersion = version;

		return true;
	}

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
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);
		// libftpp::str::StringUtils::trimWhiteSpace(value);

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
		if (lowerKey == "connection"){
			_request.setHeader(key, libftpp::str::StringUtils::toLower(value));
			return true;
		}

		_request.setHeader(key, value);
		return true;
	}

	Request &RequestParser::getRequest() { return _request; }

	RequestParser::State RequestParser::_determineBodyParsing()
	{
		if (_hasTransferEncoding && _seenContentLength) // les 2 ne peuvent pas se retrouver en meme temps
		{
			_errorCode = 400;
			return ERROR;
			// ou alors enlever le content length mais plus risqué
			// _request.removeHeader("Content-Length");
		}

		if (_hasTransferEncoding)
		{
			_state = PARSING_CHUNK_SIZE;
			return _state;
		}

		std::string contentLengthStr = _request.getHeader("Content-Length");

		// body du content-length
		if (!contentLengthStr.empty())
		{
			char *end;
			_contentLength = std::strtoul(contentLengthStr.c_str(), &end, 10);
			if (*end != '\0')
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
			_buffer.erase(0, toRead); // todo replace by the compactBuffer
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

				if (_request.getBodySize() > _maxBodySize)
				{
					_errorCode = 413;
					return ERROR;
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
		if (host.find(' ') != std::string::npos || host.find('\t') != std::string::npos)
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
			if (_errorCode == 0)
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

	bool RequestParser::_isValidMethod(const std::string &method) const
	{
		if (method.empty())
			return false;

		for (size_t i = 0; i < method.size(); ++i)
		{
			char c = method[i];
			if (!(std::isalnum(c) || c == '!' || c == '#' || c == '$' || c == '%' ||
				  c == '&' || c == '\'' || c == '*' || c == '+' || c == '-' ||
				  c == '.' || c == '^' || c == '_' || c == '`' || c == '|' ||
				  c == '~'))
				return false;
		}

		return (method == "GET" || method == "POST" || method == "DELETE");
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

	bool RequestParser::_parseContentLengthHeader(const std::string &val)
	{
		std::vector<std::string> parts;
		std::string tmp = val;
		size_t pos = 0;

		while ((pos = tmp.find(',')) != std::string::npos)
		{
			parts.push_back(tmp.substr(0, pos));
			tmp.erase(0, pos + 1);
		}
		parts.push_back(tmp);

		std::string norm;
		for (size_t i = 0; i < parts.size(); ++i)
		{
			std::string p = parts[i];
			p.erase(0, p.find_first_not_of(" \t"));
			p.erase(p.find_last_not_of(" \t") + 1);

			if (p.empty())
			{
				_errorCode = 400;
				return false;
			}

			for (size_t j = 0; j < p.size(); ++j)
			{
				if (!std::isdigit(static_cast<unsigned char>(p[j]))) // cpp98 ?
				{
					_errorCode = 400;
					return false;
				}
			}

			if (norm.empty())
				norm = p;
			else if (norm != p)
			{
				_errorCode = 400;
				return false;
			}
		}

		if (_seenContentLength && _contentLengthHeaderValue != norm)
		{
			_errorCode = 400;
			return false;
		}

		_seenContentLength = true;
		_contentLengthHeaderValue = norm;
		_request.setHeader("Content-Length", norm);
		return true;
	}

	bool RequestParser::_parseTransferEncodingHeader(const std::string &val)
	{
		std::string v = libftpp::str::StringUtils::toLower(val);
		v.erase(0, v.find_first_not_of(" \t"));
		v.erase(v.find_last_not_of(" \t") + 1);

		if (v.empty())
		{
			_errorCode = 400;
			return false;
		}

		std::vector<std::string> parts;
		std::string tmp = v;
		size_t pos = 0;

		while ((pos = tmp.find(',')) != std::string::npos)
		{
			parts.push_back(tmp.substr(0, pos));
			tmp.erase(0, pos + 1);
		}
		parts.push_back(tmp);

		if (parts.size() != 1)
		{
			_errorCode = 400;
			return false;
		}
		parts[0].erase(0, parts[0].find_first_not_of(" \t"));
		parts[0].erase(parts[0].find_last_not_of(" \t") + 1);

		if (parts[0] != "chunked")
		{
			_errorCode = 501;
			return false;
		}

		_hasTransferEncoding = true;
		_request.setHeader("Transfer-Encoding", "chunked");
		return true;
	}

	void RequestParser::reset() // si besoin pour le keep alive
	{
		_state = PARSING_REQUEST;
		_buffer.clear();
		_errorCode = 0;
		_contentLength = 0;
		_bodyBytesRemaining = 0;
		_currentChunkSize = 0;
		_chunkBytesRemaining = 0;
		_seenContentLength = false;
		_contentLengthHeaderValue.clear();
		_hasTransferEncoding = false;
		_headerCount = 0;
		_request = Request();
	}
} // namespace http
