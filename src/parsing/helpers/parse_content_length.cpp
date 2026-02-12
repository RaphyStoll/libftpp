#include "RequestParser.hpp"
#include <string>
#include <vector>

using namespace webserv::http;

bool RequestParser::_parseContentLengthHeader(const std::string &val)
{
	std::vector<std::string> parts = libftpp::str::StringUtils::split(val, ',');
	std::string norm;

	for (size_t i = 0; i < parts.size(); ++i)
	{
		std::string p = parts[i];
		p = libftpp::str::StringUtils::trim(p);

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
	v = libftpp::str::StringUtils::trim(v);

	if (v != "chunked")
	{
		_errorCode = (v.empty() || v.find(',') != std::string::npos) ? 400 : 501;
		return false;
	}

	_hasTransferEncoding = true;
	_request.setHeader("Transfer-Encoding", "chunked");
	return true;
}
