#include "Post.hpp"

bool webserv::http::Post::_checkBodySize(size_t bodySize, size_t maxSize, int &httpCode)
{
	if (maxSize < 0 && bodySize > maxSize) {
		httpCode = 413;
		return false;
	}
	return true;
}