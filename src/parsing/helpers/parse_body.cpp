#include "RequestParser.hpp"
#include <cstdlib>

using namespace webserv::http;

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
