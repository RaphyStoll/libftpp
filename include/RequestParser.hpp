#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Request.hpp"
#include <cstddef>
#include <string>

static const size_t MAX_REQUEST_SIZE = 8192;
namespace http {

class RequestParser {

public:
  enum State {
    PARSING_REQUEST,
    PARSING_HEADERS,
    PARSING_BODY_LENGTH,
    PARSING_CHUNK, // uniquement pour 1.1
    PARSING_CHUNK_SIZE,
    PARSING_CHUNK_DATA,
    PARSING_TRAILER,
    COMPLETE,
    ERROR
  };
  RequestParser();
  ~RequestParser();

  State parse(const char *data, size_t size);
  Request &getRequest();

  int getErrorCode() const { return _errorCode; }
  std::string getErrorMessage() const;

private:
  Request _request;
  State _state;
  std::string _buffer;
  int _errorCode;

  std::string _httpVersion;
  unsigned long _contentLength;
  unsigned long _maxBodySize;
  unsigned long _bodyBytesRemaining;
  unsigned long _currentChunkSize;
  unsigned long _chunkBytesRemaining;

  bool _parseRequestLine(const std::string &line);
  bool _parseHeader(const std::string &line);

  State _determineBodyParsing();
  State _parseBodyByLength();
  State _parseChunkedBody();
  bool _validateHeaders();
  State _parseRequestLineState();
  State _parseHeadersState();
};

} // namespace http

#endif
