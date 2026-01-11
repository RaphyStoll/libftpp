

#include "../include/HttpRequest.hpp"
#include "../include/StringUtils.hpp"

using libftpp::HttpRequest::HttpRequest;

HttpRequest::HttpRequest(): _method(), _target(), _version(), _headers() {}

HttpRequest::~HttpRequest() {}

void HttpRequest::clear() {
	_method.clear();
	_target.clear();
	_version.clear();
	_headers.clear();
}

const std::string& HttpRequest::getMethod() const { return _method; }
const std::string& HttpRequest::getTarget() const { return _target; }
const std::string& HttpRequest::getVersion() const { return _version; }

std::string HttpRequest::normKey(const std::string& k) {
	return str::StringUtils::toLower(str::StringUtils::trim(k));
}

std::string HttpRequest::normalizeHeaderKey(const std::string& key) {
	return normKey(key);
}

bool HttpRequest::parseRequestLine(const std::string& line) {
	std::string s = str::StringUtils::trim(line);
	if (s.empty())
		return false;

	std::string method;
	std::string target;
	std::string version;

	size_t i = 0;
	size_t n = s.size();


	while (i < n && s[i] == ' ') ++i;
	size_t start = i;
	while (i < n && s[i] != ' ') ++i;
	if (i == start) return false;
	method = s.substr(start, i - start);

	while (i < n && s[i] == ' ') ++i;
	start = i;
	while (i < n && s[i] != ' ') ++i;
	if (i == start) return false;
	target = s.substr(start, i - start);

	while (i < n && s[i] == ' ') ++i;
	start = i;
	while (i < n && s[i] != ' ') ++i;
	if (i == start) return false;
	version = s.substr(start, i - start);

	while (i < n && s[i] == ' ') ++i;
	if (i != n)
		return false;

	if (method.empty() || target.empty() || version.empty())
		return false;
	if (version.size() < 5 || version.compare(0, 5, "HTTP/") != 0)
		return false;

	_method = method;
	_target = target;
	_version = version;
	return true;
}

bool HttpRequest::parseHeaderLine(const std::string& line) {
	size_t pos = line.find(':');
	if (pos == std::string::npos)
		return false;

	std::string raw_key = line.substr(0, pos);
	std::string raw_val = line.substr(pos + 1);

	std::string key = normalizeHeaderKey(raw_key);
	std::string val = str::StringUtils::trim(raw_val);

	if (key.empty())
		return false;

	_headers[key] = val;
	return true;
}

bool HttpRequest::hasHeader(const std::string& key) const {
	const std::string k = normKey(key);
	return _headers.find(k) != _headers.end();
}

std::string HttpRequest::getHeader(const std::string& key, const std::string& def) const {
	const std::string k = normKey(key);

	std::map<std::string, std::string>::const_iterator it = _headers.find(k);
	if (it == _headers.end())
		return def;
	return it->second;
}

bool HttpRequest::contentLength(size_t& out) const {
	if (!hasHeader("content-length"))
		return false;

	std::string v = getHeader("content-length", "");
	v = str::StringUtils::trim(v);

	size_t tmp = 0;
	if (!str::StringUtils::parse_size_t(v, tmp))
		return false;

	out = tmp;
	return true;
}
